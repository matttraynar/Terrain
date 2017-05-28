#include "MainWindow.h"
#include "glwidget.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

//    MainWindow w;

//    w.resize(720, 720);
//    w.show();

//    a.exec();

    GLWidget window;
    window.m_ortho = true;
    QPixmap picture = window.renderPixmap(720, 720, false);
    picture.save("orthoImage.png", "PNG", 100);

    window.shading = true;
    window.m_ortho = false;

    QPixmap orthoPicture = window.renderPixmap(720, 720, false);
    orthoPicture.save("perspImage.png", "PNG", 100);

    window.shading = false;
    window.m_ortho =  true;

    QPixmap texture = window.renderPixmap(720, 720, false);
    texture.save("terrainTexture.png", "PNG", 100);

    return 0;
}
