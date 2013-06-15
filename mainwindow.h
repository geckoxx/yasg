#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QLabel>
#include "playground.h"
#include "statistics.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void newGame();
    void showStats();
    void showAbout();
    void newScore(int score);
    
private:
    Ui::MainWindow* ui;
    QStackedWidget* contentWidget;
    Playground* playground;
    Statistics* statistics;

    QLabel* scoreLabel;
};

#endif // MAINWINDOW_H
