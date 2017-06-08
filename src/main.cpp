#include "MainWindow.h"
#include "glwidget.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

//    MainWindow w;

//    w.resize(720, 720);
//    w.show();

//    return a.exec();

    if(argc < 2)
    {
        std::cout<<"Wrong number of parameters entered, needs settings file"<<std::endl;
        exit(1);
    }

    const char* filepath = strdup(argv[0]);
    std::string filenameStr(filepath);

    for(uint i = 0; i < 11; ++i)
    {
        filenameStr.pop_back();
    }

    const char* settings = strdup(argv[1]);
    std::string settingsStr(settings);

    GLWidget window(filenameStr, settingsStr);
    std::cout<<"Setup complete, rendering";

    window.renderOrtho();

    window.render3D();

    window.renderTexture();

    window.renderHeightmap();

    return 0;
}
