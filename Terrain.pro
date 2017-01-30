QT       += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Terrain
TEMPLATE = app

SOURCES += main.cpp\
        MainWindow.cpp \
    glwidget.cpp

HEADERS  += MainWindow.h \
    glwidget.h

FORMS    += MainWindow.ui

DISTFILES += shaders/vert.glsl \
					shaders/frag.glsl

win32::LIBS += -lopengl32
