QT       += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Terrain
TEMPLATE = app

SOURCES += src/main.cpp \
					src/MainWindow.cpp \
					src/glwidget.cpp \
					src/perlinnoise.cpp \
					src/englishfields.cpp

HEADERS  += include/MainWindow.h \
					include/glwidget.h \
					include/perlinnoise.h \
					include/englishfields.h

FORMS    += MainWindow.ui

DISTFILES += shaders/vert.glsl \
					shaders/frag.glsl

win32::LIBS += -lopengl32

INCLUDEPATH += include/
