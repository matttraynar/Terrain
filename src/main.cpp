#include "MainWindow.h"
#include "glwidget.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    w.resize(1080, 720);
    w.show();

//    GLWidget window;
//    QPixmap picture = window.renderPixmap(1080, 720, false);
//    picture.save("newImage.png", "PNG", 100);

    a.exec();

//    return 0;
}
