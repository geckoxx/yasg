#include <functional>
#include <cstdlib>
#include <stdexcept>
#include <ctime>
#include <QMouseEvent>
#include <QDateTime>

#include "playground.h"

Playground::Playground(QWidget *parent, int colorCount, int columns, int rows) :
    QGLWidget(parent)
{
    this->scrcfg.width = this->width();
    this->scrcfg.height = this->height();
    this->scrcfg.bpp = 32;

    qsrand(QDateTime::currentDateTime().toTime_t());

    colors = colorCount;
    colorIndeces = new GLuint[colors];
    colorIndeces[0] = bindTexture(QPixmap("yasg/img/blue.png"));
    colorIndeces[1] = bindTexture(QPixmap("yasg/img/green.png"));
    colorIndeces[2] = bindTexture(QPixmap("yasg/img/pink.png"));
    colorIndeces[3] = bindTexture(QPixmap("yasg/img/red.png"));
    colorIndeces[4] = bindTexture(QPixmap("img/yellow.png"));
    colorCounts = new int[colors];
    this->rows = rows;
    this->columns = columns;
    bubbles = new int*[columns];
    for(int i = 0; i < columns; ++i)
        bubbles[i] = new int[rows];
    calcBubbleSize();
    score = 0;
    lastMove.type = -1;
    status = UNSTARTED;
    startNewGame();
}

Playground::~Playground()
{
    for(int i=0; i<colors;i++) {
        deleteTexture(colorIndeces[i]);
    }
}

GLuint Playground::loadImage(QString path)
{
    QPixmap pixmap = QPixmap(path);
    GLuint textureID;

    QImage img = pixmap.toImage();
    int numOfColors = img.format();

    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, numOfColors, img.width(), img.height(), 0, GL_BGRA, GL_UNSIGNED_BYTE, img.bits());
    return textureID;
}

void Playground::calcBubbleSize()
{
    int rowSize = (int) (scrcfg.height / rows);
    int colSize = (int) (scrcfg.width / columns);
    bubbleSize = rowSize < colSize ? rowSize : colSize;
}

void Playground::initializeGL()
{
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glEnable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT);
    glViewport(0, 0, scrcfg.width, scrcfg.height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, scrcfg.width, scrcfg.height, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
}

void Playground::resizeGL(int w, int h)
{
    scrcfg.width = w;
    scrcfg.height = h;
    glViewport(0, 0, scrcfg.width, scrcfg.height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, scrcfg.width, scrcfg.height, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    calcBubbleSize();
}

void Playground::paintGL()
{
    glClearColor( 0.5f, 0.5f, 0.5f, 0.f );
    glClear(GL_COLOR_BUFFER_BIT);

    for(int x = 0; x < columns; x++)
        for(int y = 0; y < rows; y++)
            if(bubbles[x][y] != -1) {
                int colorId = bubbles[x][y];
                renderBubble(x * bubbleSize, y * bubbleSize, colorIndeces[colorId]);
            }

    //glLoadIdentity();
}

void Playground::mouseReleaseEvent(QMouseEvent *me)
{
    handleClick(me->x(), me->y());
}

void Playground::renderBubble(int x, int y, GLuint color)
{
    QRectF rect;
    rect.setX(x);
    rect.setY(y);
    rect.setHeight(bubbleSize);
    rect.setWidth(bubbleSize);
    drawTexture(rect, color);
    /*glBindTexture( GL_TEXTURE_2D, color);

    glBegin( GL_QUADS );

    glTexCoord2i( 0, 0 );
    glVertex3f( x, y, 0);

    glTexCoord2i( 1, 0 );
    glVertex3f( x + bubbleSize, y, 0);

    glTexCoord2i( 1, 1 );
    glVertex3f( x + bubbleSize, y + bubbleSize, 0);

    glTexCoord2i( 0, 1 );
    glVertex3f( x, y + bubbleSize, 0);
    glEnd();*/
}

void Playground::startNewGame()
{
    qsrand(QDateTime::currentDateTime().toTime_t());
    score = 0;
    for(int i = 0; i < colors; i++)
        colorCounts[i] = 0;
    fillBubbles();
    gameOver = false;
    lastMove.type = -1;
    while (!moves.empty()) {
        moves.pop();
    }

    status = RUNNING;
    repaint();
    emit newScore(score);
}

void Playground::handleClick ( int x, int y )
{
    if(x < 0 || y < 0)
        return;
    if(gameOver)
        return;
    int column = x / bubbleSize;
    int row = y / bubbleSize;
    if(column >= columns || row >= rows)
        return;
    int typ = bubbles[column][row];
    if(typ == -1)
        return;
    selectedType = typ;
    selectedBubbles.clear();
    checkBubble(column, row);
    int selCount = selectedBubbles.size();
    if(selCount < 2)
        return;
    click newMove;
    newMove.type = selectedType;
    newMove.bubbles = selectedBubbles;
    newMove.points = (selCount * selCount) - selCount;
    moves.push(newMove);
    lastMove = moves.top();
    colorCounts[selectedType] -= selCount;
    score += newMove.points;
    for(int i = 0; i < selCount; i++)
        bubbles[selectedBubbles[i].x][selectedBubbles[i].y] = -1;
    refresh();
    if(!floodMoveCheck(0, rows - 1, -1)) {
        gameOver = true;
        status = GAMEOVER;
    }
}

void Playground::oneStepBack()
{
    if(lastMove.type == -1)
        return;
    for(std::map<int, std::map<int, position> >::iterator iter1 = lastMove.prevPos.begin(); iter1 != lastMove.prevPos.end(); iter1++) {
        for(std::map<int, position>::iterator iter2 = iter1->second.begin(); iter2 != iter1->second.end(); iter2++) {
            //colorCounts[bubbles[iter2->second.x][iter2->second.y]]--;
            bubbles[iter2->second.x][iter2->second.y] = bubbles[iter1->first][iter2->first];
            bubbles[iter1->first][iter2->first] = -1;
        }
    }
    for(int i = 0; i < lastMove.bubbles.size(); i++) {
        bubbles[lastMove.bubbles[i].x][lastMove.bubbles[i].y] = lastMove.type;
    }
    colorCounts[lastMove.type] += lastMove.bubbles.size();
    score -= lastMove.points;
    moves.pop();
    if(!moves.empty())
        lastMove = moves.top();
    else
        lastMove.type = -1;
    repaint();
    emit newScore(score);
}

void Playground::fillBubbles()
{
    for(int x = 0; x < columns; x++)
        for(int y = 0; y < rows; y++)
            createBlock(x, y);
}

void Playground::createBlock ( int column, int row )
{
    int typ = qrand() % colors;
    colorCounts[typ]++;
    bubbles[column][row] = typ;
}

void Playground::checkBubble ( int column, int row )
{
    if(column < 0 || column >= columns || row < 0 || row >= rows)
        return;
    if(bubbles[column][row] != selectedType)
        return;
    else {
        position bubblePos;
        bubblePos.x = column;
        bubblePos.y = row;
        for(int i = 0; i < selectedBubbles.size(); i++) {
            if(selectedBubbles[i].x == bubblePos.x && selectedBubbles[i].y == bubblePos.y)
                return;
        }
        selectedBubbles.push_back(bubblePos);
        checkBubble(column - 1, row);
        checkBubble(column + 1, row);
        checkBubble(column, row - 1);
        checkBubble(column, row + 1);
    }
}

void Playground::refresh()
{
    lastMove.prevPos.clear();
    int emptyColumn = 0;
    for(int x = columns - 1; x >= 0; x--) {
        int emptyElement = 0;
        for(int y = rows - 1; y >= 0; y--) {
            if(bubbles[x][y] == -1)
                emptyElement++;
            else if(emptyElement > 0) {
                position oldPos;
                oldPos.x = x;
                oldPos.y = y;
                lastMove.prevPos[x][y + emptyElement] = oldPos;
                bubbles[x][y + emptyElement] = bubbles[x][y];
                bubbles[x][y] = -1;
            }
        }
        if(bubbles[x][rows - 1] == -1)
            emptyColumn++;
        else if(emptyColumn > 0) {
            for(int y = rows - 1; y >= 0; y--) {
                position oldPos;
                try {
                    oldPos = lastMove.prevPos.at(x).at(y);
                    lastMove.prevPos.at(x).erase(y);
                } catch (const std::out_of_range& oor) {
                    oldPos.x = x;
                    oldPos.y = y;
                }
                lastMove.prevPos[x + emptyColumn][y] = oldPos;
                bubbles[x + emptyColumn][y] = bubbles[x][y];
                bubbles[x][y] = -1;
            }
        }
    }
    for(int x = emptyColumn - 1; x > -1; x--)
        for(int y = 0; y < rows; y++)
            createBlock(x, y);
    moves.top().prevPos = lastMove.prevPos;
    repaint();
    emit newScore(score);
}

bool Playground::floodMoveCheck ( int column, int row, int type )
{
    if(column < 0 || column >= columns || row < 0 || row >= rows)
        return false;
    int bubbleType = bubbles[column][row];
    if(bubbleType == -1)
        return false;
    if(bubbleType == type)
        return true;
    return floodMoveCheck(column + 1, row, bubbleType) || floodMoveCheck(column, row - 1, bubbleType);
}
