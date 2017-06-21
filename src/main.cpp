#include "MainWindow.h"
#include "glwidget.h"
#include <QApplication>
#include <unistd.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

//    //Used to show as QtWidgetWindow
//    MainWindow w;

//    w.resize(720, 720);
//    w.show();

//    return a.exec();

    //Used to run as exe
    if(argc < 2)
    {
        std::cout<<"Wrong number of parameters entered, needs settings file"<<std::endl;
        exit(1);
    }

    char cwd[1024];
    const char* filepath = strdup(getcwd(cwd, sizeof(cwd)));
    std::string filenameStr(filepath);
    filenameStr += "/";

    //HARDCODE
    filenameStr = "/local/vince/maya/projects/MattTraynar_demo/scripts/FieldTool/";

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

        std::cout<<"##Complete"<<std::endl;
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
