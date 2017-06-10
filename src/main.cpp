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

    if(argc == 2)
    {
        GLWidget window(filenameStr, settingsStr);

        window.render3D();
        std::cout<<"85"<<std::endl;

        window.renderOrtho();
        std::cout<<"90"<<std::endl;

        window.renderTexture();
        std::cout<<"95"<<std::endl;

        window.renderHeightmap();
        std::cout<<"100"<<std::endl;
    }
    if(argc == 5)
    {
        const char* xChar = strdup(argv[2]);
        const char* yChar = strdup(argv[3]);
        const char* zChar = strdup(argv[4]);

        QVector3D farmPosition(std::atof(xChar) - 25.0f, std::atof(yChar), std::atof(zChar) - 25.0f);

        GLWidget window(true, filenameStr, settingsStr, farmPosition);

        window.renderOrtho();
        std::cout<<"100"<<std::endl;
    }
    if(argc == 6)
    {
        const char* xChar = strdup(argv[2]);
        const char* yChar = strdup(argv[3]);
        const char* zChar = strdup(argv[4]);

        QVector3D farmPosition(std::atof(xChar) - 25.0f, std::atof(yChar), std::atof(zChar) - 25.0f);

        GLWidget window(true, true, filenameStr, settingsStr, farmPosition);

        window.render3D();

        window.renderOrtho();

        window.renderTexture();

        window.renderHeightmap();
        std::cout<<"100"<<std::endl;
    }

    return 0;
}
