#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "about.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    contentWidget = new QStackedWidget();
    playground = new Playground();
    statistics = new Statistics();
    contentWidget->addWidget(playground);
    contentWidget->addWidget(statistics);
    contentWidget->addWidget(new About());
    contentWidget->setCurrentIndex(0);
    setCentralWidget(contentWidget);

    QWidget *separator = new QWidget();
    separator->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui->mainToolBar->addWidget(separator);
    scoreLabel = new QLabel();
    ui->mainToolBar->addWidget(scoreLabel);

    connect(ui->actionNew, SIGNAL(triggered()), this, SLOT(newGame()));
    connect(ui->actionBack, SIGNAL(triggered()), playground, SLOT(oneStepBack()));
    connect(ui->actionScore, SIGNAL(triggered()), this, SLOT(showStats()));
    connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(showAbout()));
    connect(playground, SIGNAL(newScore(int)), this, SLOT(newScore(int)));
}

MainWindow::~MainWindow()
{
    delete ui;
    delete contentWidget;
    //delete playground;
    //delete statistics;
    delete scoreLabel;
}

void MainWindow::newGame()
{
    contentWidget->setCurrentIndex(0);
    playground->startNewGame();
}

void MainWindow::showStats()
{
    if(1 != contentWidget->currentIndex())
        contentWidget->setCurrentIndex(1);
    else
        contentWidget->setCurrentIndex(0);
}

void MainWindow::showAbout()
{
    if(2 != contentWidget->currentIndex())
        contentWidget->setCurrentIndex(2);
    else
        contentWidget->setCurrentIndex(0);
}

void MainWindow::newScore(int score)
{
    scoreLabel->setText(QString::number(score).append(" Punkte"));
}
