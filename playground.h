#ifndef PLAYGROUND_H
#define PLAYGROUND_H

#include <QGLWidget>

#include <vector>
#include <map>
#include <stack>

struct Screencfg {
    int width;
    int height;
    int bpp;
};

struct position {
    int x;
    int y;
};

enum Gamestatus {
    UNSTARTED, RUNNING, PAUSED, GAMEOVER
};

struct click {
    int type;
    std::vector<position> bubbles;
    std::map<int, std::map<int, position> > prevPos;
    int points;
};

class Playground : public QGLWidget
{
    Q_OBJECT
private:
    Screencfg scrcfg;
    Gamestatus status;
    //std::default_random_engine generator;
    //std::uniform_int_distribution<int> distribution;

    int colors;
    GLuint* colorIndeces;
    int* colorCounts;
    int rows, columns;
    int** bubbles;

    int bubbleSize;
    int playgroundWidth, playgroundHeight;

    int score;
    bool gameOver;

    int selectedType;
    std::vector<position> selectedBubbles;

    std::stack<click> moves;
    click lastMove;

public:
    explicit Playground(QWidget *parent = 0, int colorCount = 5, int columns = 20, int rows = 10);
    virtual ~Playground();
    
protected:
    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();
    void mouseReleaseEvent(QMouseEvent *me);

private:
    GLuint loadImage(QString path);
    void calcBubbleSize();
    void renderBubble(int x, int y, GLuint color);
    void handleClick(int x, int y);
    void fillBubbles();
    void createBlock(int column, int row);
    void checkBubble(int column, int row);
    void refresh();
    bool floodMoveCheck(int column, int row, int type);

signals:
    void newScore(int score);
    
public slots:
    void startNewGame();
    void oneStepBack();
    
};

#endif // PLAYGROUND_H
