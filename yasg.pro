#-------------------------------------------------
#
# Project created by QtCreator 2013-06-09T14:01:01
#
#-------------------------------------------------

QT       += core gui widgets opengl

#QMAKE_CXXFLAGS += -std=c++11

TARGET = yasg
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    playground.cpp \
    statistics.cpp \
    about.cpp

HEADERS  += mainwindow.h \
    playground.h \
    statistics.h \
    about.h

FORMS    += mainwindow.ui \
    statistics.ui \
    about.ui

OTHER_FILES += img/blue.png \
    img/green.png \
    img/pink.png \
    img/red.png \
    img/yellow.png
