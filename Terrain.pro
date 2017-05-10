QT       += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Terrain
TEMPLATE = app

SOURCES += src/main.cpp \
					src/MainWindow.cpp \
					src/glwidget.cpp \
					src/perlinnoise.cpp \
					src/englishfields.cpp \
    src/voronoiface.cpp \
    src/voronoiedge.cpp \
    src/mesh.cpp


HEADERS  += include/MainWindow.h \
					include/glwidget.h \
					include/perlinnoise.h \
					include/englishfields.h \
    include/voronoiface.h \
    include/voronoiedge.h \
    include/mesh.h


FORMS    += MainWindow.ui

DISTFILES += shaders/vert.glsl \
					shaders/frag.glsl

win32::LIBS += -lopengl32

INCLUDEPATH += include/
INCLUDEPATH += borrowed/

## CGAL STUFF
INCLUDEPATH += C:/CGAL-4.9_64/include
INCLUDEPATH += C:/CGAL-4.9_64/auxiliary/gmp/include
INCLUDEPATH += C:/boost

LIBS += C:/CGAL-4.9_64/auxiliary/gmp/lib/libgmp-10.lib
LIBS += C:/CGAL-4.9_64/auxiliary/gmp/lib/libmpfr-4.lib
LIBS += C:/CGAL-4.9_64/lib/CGAL-vc140-mt-gd-4.9.lib

win32::LIBS += -L"E:\mattt\Documents\MPCLibraries\assimp-3.3.1\lib\Debug" -lassimp-vc140-mt

INCLUDEPATH += $$PWD/libs/assimp/include/
