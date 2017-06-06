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

    window.m_ortho = true;
    window.heightmap = false;
    std::cout<<'\n';
    QPixmap picture = window.renderPixmap(720, 720, false);

    std::cout<<"Saving ortho"<<std::endl;
    std::cout<<'\n';

    std::string output = filenameStr + "orthoImage.png";

    picture.save(output.c_str(), "PNG", 100);

    window.shading = true;
    window.m_ortho = false;

    QPixmap orthoPicture = window.renderPixmap(720, 720, false);

    std::cout<<"Saving persp"<<std::endl;
    std::cout<<'\n';

    output = filenameStr + "perspImage.png";
    orthoPicture.save(output.c_str(), "PNG", 100);

    window.shading = false;
    window.m_ortho =  true;

    QPixmap texture = window.renderPixmap(720, 720, false);

    std::cout<<"Saving texture"<<std::endl;

    output = filenameStr + "terrainTexture.png";
    texture.save(output.c_str(), "PNG", 100);

    window.m_ortho = false;
    window.heightmap = true;

    QPixmap heightmap = window.renderPixmap(720, 720, false);

    std::cout<<"Saving texture"<<std::endl;

    output = filenameStr + "heightmap.png";
    heightmap.save(output.c_str(), "PNG", 100);

    return 0;
}
