#include "glwidget.h"

#include <QDebug>

GLWidget::GLWidget(std::string _filepath, std::string _settings, QWidget* parent ) :
    QGLWidget(parent)
{
    loadThese(_settings);
    m_live = false;

    QSurfaceFormat glFormat;
    glFormat.setVersion(3, 3);
    glFormat.setProfile(QSurfaceFormat::CoreProfile);

    m_workingPath = _filepath;

    m_xRot = 0;
    m_yRot = 0;
    m_zRot = 0;
    m_zDis = 0;
    m_mouseDelta = 0;
    m_cameraPos = QVector3D(20.0f, 50.0f, 20.0f);

    heightmap = false;
    m_ortho = false;
    doOnce = true;
    shading = true;

    m_x = -0;
    moveDown = false;

    m_wireframe = false;

    m_view.setToIdentity();
    m_view.lookAt(m_cameraPos,
                          QVector3D(0.0f, 0.0f, 0.0f),
                          QVector3D(0.0f,1.0f, 0.0f));


    startTimer(1);

    generateHeightMap(s_heightmapIters, s_roughness);
    m_waterLevel = ((m_terrainMax - m_terrainMin) / 4.0f) + m_terrainMin;
    std::cout<<"20"<<std::endl;

    double width = 50.0;

    m_fieldGenerator = EnglishFields(s_terrainSize, s_hasSeed, s_seed, s_hasFarm, s_numPoints, s_farmPos, m_live);

    m_vRegions = m_fieldGenerator.getRegions();

    bool adjustHeights = true;

    if(adjustHeights)
    {
        qInfo()<<"Adjusting heights";

        float minDistance = 1000000;
        float yValue = 0.0f;

        for(uint i = 0; i < m_fieldGenerator.getVerts().size(); ++i)
        {
            minDistance = 1000000;
            yValue = 0.0f;

            for(int k = 0; k < m_verts.size(); ++k)
            {
                QVector3D flatVector1(m_verts[k].x(), 0, m_verts[k].z());
                QVector3D flatVector2(m_fieldGenerator.getVert(i)->x(), 0.0f, m_fieldGenerator.getVert(i)->z());

                if(((flatVector1.x() - flatVector2.x()) < 0.25f) || ((flatVector1.z() - flatVector2.z()) < 0.25f))
                {
                    if((flatVector1 - flatVector2).length() < minDistance)
                    {
                        minDistance = (flatVector1 - flatVector2).length();
                        yValue = m_verts[k].y();
                    }
                }
            }

            m_fieldGenerator.getVert(i)->setY(yValue);
        }

        minDistance = 1000000;
        yValue = 0.0f;

        if(s_hasFarm)
        {
            farmPosition = m_fieldGenerator.getFarmPosition();
            qInfo()<<m_verts.size();
            for(int k = 0; k < m_verts.size(); ++k)
            {
                qInfo()<<k;
                QVector3D flatVector1(m_verts[k].x(), 0, m_verts[k].z());
                QVector3D flatVector2(farmPosition.x(), 0.0f, farmPosition.z());

                if(((flatVector1.x() - flatVector2.x()) < 0.25f) || ((flatVector1.z() - flatVector2.z()) < 0.25f))
                {
                    if((flatVector1 - flatVector2).length() < minDistance)
                    {
                        minDistance = (flatVector1 - flatVector2).length();
                        yValue = m_verts[k].y();
                    }
                }
            }

            farmPosition.setY(yValue);
            qInfo()<<"Position: "<<farmPosition;
        }


    }

    bool adjustTreeHeights = true;

    if(adjustTreeHeights)
    {
        for(uint i = 0; i < m_treePositions.size(); ++i)
        {
            float minDistance = 1000000;
            float yValue = 0.0f;

            for(int k = 0; k < m_verts.size(); ++k)
            {
                QVector3D flatVector1(m_verts[k].x(), 0, m_verts[k].z());
                QVector3D flatVector2(m_treePositions[i].x(), 0.0f, m_treePositions[i].z());

                if(((flatVector1.x() - flatVector2.x()) < 0.25f) || ((flatVector1.z() - flatVector2.z()) < 0.25f))
                {
                    if((flatVector1 - flatVector2).length() < minDistance)
                    {
                        minDistance = (flatVector1 - flatVector2).length();
                        yValue = m_verts[k].y();
                    }
                }
            }

            m_treePositions[i].setY(yValue);
        }
    }
}

GLWidget::GLWidget(bool _liveUpdate, std::string _filepath, std::string _settings, QVector3D _farmPos, QWidget *parent) :
    QGLWidget(parent)
{
    loadThese(_settings);

    m_live = _liveUpdate;

    QSurfaceFormat glFormat;
    glFormat.setVersion(3, 3);
    glFormat.setProfile(QSurfaceFormat::CoreProfile);

    m_workingPath = _filepath;

    m_xRot = 0;
    m_yRot = 0;
    m_zRot = 0;
    m_zDis = 0;
    m_mouseDelta = 0;
    m_cameraPos = QVector3D(20.0f, 50.0f, 20.0f);

    m_ortho = true;
    doOnce = true;
    shading = true;

    m_x = -0;
    moveDown = false;

    m_wireframe = false;

    m_view.setToIdentity();
    m_view.lookAt(m_cameraPos,
                          QVector3D(0.0f, 0.0f, 0.0f),
                          QVector3D(0.0f,1.0f, 0.0f));


    startTimer(1);

    std::cout<<"20"<<std::endl;

    farmPosition = _farmPos;
    m_fieldGenerator = EnglishFields(s_terrainSize, s_hasSeed, s_seed, s_hasFarm, s_numPoints, _farmPos, m_live);

    m_vRegions = m_fieldGenerator.getRegions();

    m_terrainMin = 1000000;
    m_terrainMax = -100000;

    if(s_exportTerrain)
    {
        ExportScene::getFrom(s_terrainPath + "/Terrain.obj", m_verts, m_norms, m_terrainMin, m_terrainMax);
    }
    else
    {
        ExportScene::getFrom(m_workingPath + "/Output/Terrain.obj", m_verts, m_norms, m_terrainMin, m_terrainMax);
    }
    m_waterLevel = ((m_terrainMax - m_terrainMin) / 4.0f) + m_terrainMin;

    for(int i = 0; i < m_verts.size(); i += 4)
    {
        m_uvs.push_back(QVector2D(0, 0));
        m_uvs.push_back(QVector2D(0.5f, 0));
        m_uvs.push_back(QVector2D(0.5f, 0.5f));
        m_uvs.push_back(QVector2D(0, 0.5f));
    }
}

GLWidget::~GLWidget()
{

}

void GLWidget::loadThese(std::string _settingsPath)
{
    std::string line;
    std::ifstream f(_settingsPath);

    if(f)
    {
        getline(f, line);
        s_seed = std::stoi(line);
        (s_seed < 0) ? s_hasSeed = false : s_hasSeed = true;

        if(!s_hasSeed)
        {
            s_seed = time(NULL);
        }

        getline(f, line);
        s_terrainSize = std::stoi(line);

        getline(f, line);
        s_numPoints = std::stoi(line);

        getline(f, line);
        s_roughness = std::strtof(line.c_str(), 0);

        getline(f, line);
        s_heightmapIters = std::stoi(line);

        getline(f,line);

        getline(f, line);
        (line.find("True") != std::string::npos) ? s_hasFarm = true : s_hasFarm = false;

        if(s_hasFarm)
        {
            getline(f, line);
            s_farmPos.setX(strtof(line.c_str(), NULL));

            getline(f, line);
            s_farmPos.setY(strtof(line.c_str(), NULL));

            getline(f, line);
            s_farmPos.setZ(strtof(line.c_str(), NULL));

            getline(f, line);

            getline(f, line);

            while(line.find(".obj") != std::string::npos)
            {
                s_farmMeshes.push_back(line);
                getline(f, line);
            }
        }

        getline(f, line);

        getline(f, line);
        (line.find("True") != std::string::npos) ? s_hasTrees = true : s_hasTrees = false;

        if(s_hasTrees)
        {
            getline(f, line);
            (line.find("True") != std::string::npos) ? s_normTrees = true : s_normTrees = false;

            getline(f, line);
            (line.find("True") != std::string::npos) ? s_clusterTrees = true : s_clusterTrees = false;

            getline(f, line);

            getline(f, line);

            while(line.find(".obj") != std::string::npos)
            {
                s_treeMeshes.push_back(line);
                getline(f, line);
            }
        }
        else
        {
            s_normTrees = false;
            s_clusterTrees = false;
        }

        getline(f, line);
        getline(f, line);
        getline(f, line);

        getline(f, line);
        (line.find("True") != std::string::npos) ? s_exportTerrain = true : s_exportTerrain = false;

        if(s_exportTerrain)
        {
            getline(f, line);
            s_terrainPath = line;

            getline(f, line);
            (line.find("True") != std::string::npos) ? s_triangulateTerrain = true : s_triangulateTerrain = false;
        }
        else
        {
            s_triangulateTerrain = false;
        }

        getline(f, line);

        getline(f, line);
        (line.find("True") != std::string::npos) ? s_exportTexture = true : s_exportTexture = false;

        if(s_exportTexture)
        {
            getline(f, line);
            s_texturePath = line;
        }

        getline(f, line);

        getline(f, line);
        (line.find("True") != std::string::npos) ? s_exportHeightmap = true : s_exportHeightmap = false;

        if(s_exportHeightmap)
        {
            getline(f, line);
            s_heightmapPath = line;
        }

        getline(f, line);

        getline(f, line);
        (line.find("True") != std::string::npos) ? s_exportWalls = true : s_exportWalls = false;

        if(s_exportWalls)
        {
            getline(f, line);
            s_wallsPath = line;
        }

        getline(f, line);

        getline(f, line);
        (line.find("True") != std::string::npos) ? s_exportLocations = true : s_exportLocations = false;

        if(s_exportLocations)
        {
            getline(f, line);
            s_locationsPath = line;
        }

        f.close();
    }
    else
    {
        std::cout<<"INVALID SETTINGS FILE";
    }


//    std::cout<<s_seed<<std::endl;
//    std::cout<<s_terrainSize<<std::endl;
//    std::cout<<s_numPoints<<std::endl;
//    std::cout<<s_roughness<<std::endl;
//    std::cout<<s_heightmapIters<<std::endl;

//    std::cout<<"Farms\n";
//    std::cout<<s_hasFarm<<std::endl;
//    std::cout<<s_farmPos.x()<<" "<<s_farmPos.y()<<" "<<s_farmPos.z()<<std::endl;

//    for(size_t i = 0; i < s_farmMeshes.size(); ++i)
//    {
//        std::cout<<s_farmMeshes[i]<<std::endl;
//    }

//    std::cout<<"Trees\n";
//    std::cout<<s_hasTrees<<std::endl;
//    std::cout<<s_normTrees<<std::endl;
//    std::cout<<s_clusterTrees<<std::endl;

//    for(size_t i = 0; i < s_treeMeshes.size(); ++i)
//    {
//        std::cout<<s_treeMeshes[i]<<std::endl;
//    }

//    std::cout<<s_exportTerrain<<std::endl;
//    std::cout<<s_triangulateTerrain<<std::endl;
//    std::cout<<s_terrainPath<<std::endl;
//    std::cout<<s_exportTexture<<std::endl;
//    std::cout<<s_texturePath<<std::endl;
//    std::cout<<s_exportHeightmap<<std::endl;
//    std::cout<<s_heightmapPath<<std::endl;
//    std::cout<<s_exportWalls<<std::endl;
//    std::cout<<s_wallsPath<<std::endl;
//    std::cout<<s_exportLocations<<std::endl;
//    std::cout<<s_locationsPath<<std::endl;
}

void GLWidget::initializeGL()
{
    if(doOnce)
    {
        glEnable(GL_DEPTH_TEST);


        if(!prepareShaderProgram(m_workingPath + "shaders/vert.glsl", m_workingPath + "shaders/frag.glsl") )
        {
            exit(1);
        }

        m_pgm.bind();

        //Next pass the minimum and height range values to the shader

        if(s_hasTrees)
        {
            for(size_t i = 0; i < s_treeMeshes.size(); ++i)
            {
                m_treeMeshes.push_back(std::shared_ptr<Mesh>(new Mesh(s_treeMeshes[i])));
                m_treeMeshes[i]->prepareMesh(m_pgm);
            }
        }

        //Load the farm meshes
        if(s_hasFarm)
        {
            for(size_t i = 0; i < s_farmMeshes.size(); ++i)
            {
                m_farmMeshes.push_back(std::shared_ptr<Mesh>(new Mesh(s_farmMeshes[i])));
                m_farmMeshes[i]->prepareMesh(m_pgm);
            }
        }

        if(m_live)
        {
            std::cout<<"78"<<std::endl;
        }
        else
        {
            std::cout<<"60"<<std::endl;
        }

        m_fieldGenerator.createWalls(m_pgm);

        m_vRegions = m_fieldGenerator.getRegions();
        if(m_live)
        {
            std::cout<<"85"<<std::endl;
        }
        else
        {
            std::cout<<"70"<<std::endl;
        }

        for(uint i = 0; i < m_vRegions.size(); ++i)
        {
            m_vRegions[i].passVBOToShader(m_pgm);
        }


        if(s_clusterTrees)
        {
            m_treePositions = m_fieldGenerator.getTreePositions();
        }

        prepareTerrain();

        if(!m_live)
        {
            prepareWater();
            prepareTrees();
        }
        m_waterLevel = ((m_terrainMax - m_terrainMin) / 4.0f) + m_terrainMin;

        for(uint i = 0; i < m_treePositions.size(); ++i)
        {
            int treeIndex = m_treeMeshes.size() * (float)rand()/(float)RAND_MAX;

            m_treeMeshesToUse.push_back(treeIndex);
        }


        if(m_live)
        {
            std::cout<<"95"<<std::endl;
        }
        else
        {
            std::cout<<"80"<<std::endl;
        }

        //Finished creating regions

        m_pgm.bind();

        QOpenGLTexture* sand = addNewTexture(m_workingPath + "textures/sand.png");
        sand->bind(0);
        m_pgm.setUniformValue("sandTexture", 0);

        QOpenGLTexture* grass = addNewTexture(m_workingPath + "textures/grass.png");
        grass->bind(1);
        m_pgm.setUniformValue("grassTexture", 1);

        QOpenGLTexture* rock = addNewTexture(m_workingPath + "textures/rock.png");
        rock->bind(2);
        m_pgm.setUniformValue("rockTexture", 2);

        QOpenGLTexture* snow = addNewTexture(m_workingPath + "textures/snow.png");
        snow->bind(3);
        m_pgm.setUniformValue("snowTexture", 3);

        m_pgm.release();

        if(s_exportTerrain)
        {
            ExportScene::sendTo("obj", s_terrainPath + "/Terrain.obj", m_verts, m_norms, m_uvs, s_terrainSize, s_triangulateTerrain, true);
        }
        else
        {
            ExportScene::sendTo("obj", m_workingPath + "/Output/Terrain.obj", m_verts, m_norms, m_uvs, s_terrainSize, s_triangulateTerrain, true);
        }

        if(s_exportWalls)
        {
            m_fieldGenerator.exportFields(s_wallsPath, false);
        }
        else
        {
            m_fieldGenerator.exportFields(m_workingPath, true);
        }

        if(s_hasTrees)
        {
            exportLocations();
        }

        doOnce = false;
    }
    else
    {
        m_pgm.bind();

        QOpenGLTexture* sand = addNewTexture(m_workingPath + "textures/sand.png");
        sand->bind(0);
        m_pgm.setUniformValue("sandTexture", 0);

        QOpenGLTexture* grass = addNewTexture(m_workingPath + "textures/grass.png");
        grass->bind(1);
        m_pgm.setUniformValue("grassTexture", 1);

        QOpenGLTexture* rock = addNewTexture(m_workingPath + "textures/rock.png");
        rock->bind(2);
        m_pgm.setUniformValue("rockTexture", 2);

        QOpenGLTexture* snow = addNewTexture(m_workingPath + "textures/snow.png");
        snow->bind(3);
        m_pgm.setUniformValue("snowTexture", 3);

        m_pgm.release();
    }
}

void GLWidget::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);

    update();
}

void GLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_pgm.bind();
    m_pgm.setUniformValue("min", m_terrainMin);
    m_pgm.setUniformValue("range", m_terrainMax - m_terrainMin);
    m_pgm.setUniformValue("waterLevel", m_waterLevel);

    m_pgm.setUniformValue("useShading", shading);
    m_pgm.setUniformValue("useHeightmap", heightmap);

    m_pgm.release();

    m_pgm.bind();

    m_pgm.setUniformValue("lightPos",QVector3D(m_x, 10.0f, 0.0f));

    loadMatricesToShader(QVector3D(0,0,0));

    m_pgm.setUniformValue("mCol",QVector4D(0.0f,0.0f,0.0f,0.0f));

    if(heightmap)
    {
        m_pgm.setUniformValue("mCol",QVector4D(1.0f,1.0f,1.0f,1.0f));
    }
    drawTerrain();

    if(shading)
    {
        if(!m_ortho && s_hasTrees)
        {
            vao_trees.bind();

            m_pgm.setUniformValue("mCol",QVector4D(0.05f, 0.24f,0.01f,0.5f));

            qInfo()<<m_treePositions.size();
            qInfo()<<m_treeMeshesToUse.size();
            for(uint i = 0; i < m_treePositions.size(); ++i)
            {
                loadMatricesToShader(m_treePositions[i]);
                m_treeMeshes[m_treeMeshesToUse[i]]->draw();
            }

            vao_trees.release();
        }

        if(s_hasFarm)
        {
            m_pgm.setUniformValue("mCol",QVector4D(0.2f, 0.17f, 0.0f, 1.0f));

            loadMatricesToShader(farmPosition);

            for(uint i = 0; i < m_farmMeshes.size(); ++i)
            {
                qInfo()<<"Farm building "<<i;
                m_farmMeshes[i]->draw();
            }
        }

        m_pgm.setUniformValue("mCol",QVector4D(0.25f, 0.1f ,0.1f, 1.0f));

        loadMatricesToShader(QVector3D(0,0,0));

        qInfo()<<"Walls";
        m_fieldGenerator.drawWalls(m_pgm);
    }

    m_pgm.release();
}

void GLWidget::renderTexture()
{
    shading = false;
    m_ortho =  true;
    m_pgm.setUniformValue("mCol",QVector4D(0.2f,0.95f,0.2f,0.0f));

    QPixmap texture = renderPixmap(720, 720, false);

    std::string output = s_texturePath + "/terrainTexture.png";

    if(!s_exportTexture)
    {
        output = m_workingPath + "/Output/terrainTexture.png";
    }


    texture.save(output.c_str(), "PNG", 100);
}

void GLWidget::renderHeightmap()
{
    m_pgm.setUniformValue("mCol",QVector4D(0.2f,0.95f,0.2f,0.0f));
    heightmap = true;

    QPixmap heightmapImage = renderPixmap(720, 720, false);

    std::string output = s_heightmapPath + "/heightmap.png";

    if(!s_exportHeightmap)
    {
        output = m_workingPath + "/Output/heightmap.png";
    }

    heightmapImage.save(output.c_str(), "PNG", 100);
}

void GLWidget::renderOrtho()
{
    m_ortho = true;
    heightmap = false;
    glClearColor(0.0f, 0.0f, 0.2f, 0.0f);

    m_pgm.setUniformValue("mCol",QVector4D(0.2f,0.95f,0.2f,0.0f));
    changeOrtho();

    QPixmap picture = renderPixmap(720, 720, false);

    std::string output = s_wallsPath + "/orthoImage.png";

    if(!s_exportWalls)
    {
        output = m_workingPath + "/Output/orthoImage.png";
    }

    picture.save(output.c_str(), "PNG", 100);
}

void GLWidget::render3D()
{
    shading = true;
    m_ortho = false;

    glClearColor(0.0f, 0.0f, 0.2f, 0.0f);

    m_yRot -= 10 * 10.0f * 16.0f;
    for(int i = 0; i < 21; ++i)
    {
        m_yRot += 10.0f * 16.0f;
        m_pgm.setUniformValue("mCol",QVector4D(1.0f,0.0f,0.0f,0.0f));
        QPixmap orthoPicture = renderPixmap(720, 720, false);


        std::string output = m_workingPath + "Output/perspImage" + std::to_string(i) + ".png";
        orthoPicture.save(output.c_str(), "PNG", 100);
    }
}

void GLWidget::exportLocations()
{
    std::ofstream outputFile;
    outputFile.open(s_locationsPath + "/locationData.txt");

    for(size_t i = 0; i < m_treePositions.size(); ++i)
    {
        outputFile << m_treePositions[i].x() << "/" << m_treePositions[i].y() << "/" << m_treePositions[i].z() << "/" << m_treeMeshesToUse[i] << '\n';
    }
    outputFile << "######";

    outputFile.close();
}

void GLWidget::timerEvent(QTimerEvent *e)
{
    e;


    //Pulse background
//    static float colour = 0.0f;

//    if(colour <= 0.25f)
//    {
//        glClearColor(0.0f, 0.0f, colour, 1.0f);
//        colour += 0.0001f;
//    }
//    else if(colour <= 0.5f)
//    {
//        glClearColor(0.0f, 0.0f, 0.5f - colour, 1.0f);
//        colour += 0.0001f;
//    }
//    else
//    {
//        colour = 0.0f;
//    }

    if(m_x > 20)
    {
        moveDown = true;
    }
    else if(m_x < -20)
    {
        moveDown = false;
    }

    if(moveDown)
    {
        m_x -= 0.01f;
    }
    else
    {
        m_x += 0.01f;
    }

    update();
}

void GLWidget::wheelEvent(QWheelEvent *e)
{
    //Get the number of degrees the wheel has turned
    int numDegrees = e->delta() / 8;
    int numSteps = numDegrees / 15;

    //Add the steps to a member variable used in paintGL
    m_mouseDelta += numSteps;


    index++;

    if(index > m_vRegions.size() - 1)
    {
        index = 0;
    }

    update();
}

void GLWidget::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::RightButton)
    {
        m_ortho = !m_ortho;

        changeOrtho();
    }

    //Store the position the mouse was pressed in
    m_lastPos = e->pos();
    m_wireframe = true;
}

void GLWidget::mouseMoveEvent(QMouseEvent *e)
{
    //Get the change in x & y since the last click
    int dx = e->x() - m_lastPos.x();
    int dy = e->y() - m_lastPos.y();

    //See if the click was a left or a right click, change position accordingly
    if (e->buttons() & Qt::LeftButton)
    {
        setXRotation(m_xRot + 8 * dy);
        setYRotation(m_yRot + 8 * dx);
    }

    //Update the last position
    m_lastPos = e->pos();

    update();
}

void GLWidget::mouseReleaseEvent(QMouseEvent *e)
{
    e;
    m_wireframe = false;
}

void GLWidget::qNormalizeAngle(int &angle)
{
    //This uses the mouse/camera movement example on Qt's website:
    //http://doc.qt.io/qt-4.8/qt-opengl-hellogl-example.html

    //Make sure the angle is adjusted accordingly
    while (angle < 0) angle += 360 * 16;
    while (angle > 360 * 16) angle -= 360 * 16;
}

void GLWidget::setXTranslation(int x)
{
    //Set the new x position and call update on the window
    qNormalizeAngle(x);

    if (x != m_xDis)
    {
        m_xDis = x;
        update();
    }
}

void GLWidget::setYTranslation(int y)
{
    //Set the new y position and call update on the window
    qNormalizeAngle(y);

    if (y != m_yDis)
    {
        m_yDis = y;
        update();
    }
}

void GLWidget::setZTranslation(int z)
{
    //Set the new z position and call update on the window
    qNormalizeAngle(z);

    if (z != m_zDis)
    {
        m_zDis= z;
        update();
    }
}

void GLWidget::setXRotation(int angle)
{
    //Set the new x rotation and call update on the window

    qNormalizeAngle(angle);

    if (angle != m_xRot)
    {
        m_xRot = angle;
        update();
    }
}

void GLWidget::setYRotation(int angle)
{
    //Set the new y rotation and call update on the window
    qNormalizeAngle(angle);

    if (angle != m_yRot)
    {
        m_yRot = angle;
        update();
    }
}

void GLWidget::setZRotation(int angle)
{
    //Set the new z rotation and call update on the window
    qNormalizeAngle(angle);

    if (angle != m_zRot)
    {
        m_zRot = angle;
        update();
    }
}

 //----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

bool GLWidget::prepareShaderProgram(const std::string &vertexShaderPath, const std::string &fragmentShaderPath)
{
    // Load the vertex shader
    bool result = m_pgm.addShaderFromSourceFile(QOpenGLShader::Vertex, vertexShaderPath.c_str() );

    //Check if the function succeeded, if not print why and return
    if ( !result )
    {
        qWarning() << m_pgm.log();
        return result;
    }

   //Do the same with the fragment shader
    result = m_pgm.addShaderFromSourceFile(QOpenGLShader::Fragment, fragmentShaderPath.c_str() );

    if ( !result )
    {
       qWarning() << m_pgm.log();
       return result;
    }

   //Finally link the shaders to the program
    result = m_pgm.link();

    if ( !result )
    {
       qWarning() << m_pgm.log();
       return result;
    }

    return result;
}

bool GLWidget::prepareShaderProgram( const QString& vertexShaderPath, const QString& fragmentShaderPath )
{
     // Load the vertex shader
     bool result = m_pgm.addShaderFromSourceFile(QOpenGLShader::Vertex, vertexShaderPath );

     //Check if the function succeeded, if not print why and return
     if ( !result )
     {
         qWarning() << m_pgm.log();
         return result;
     }

    //Do the same with the fragment shader
     result = m_pgm.addShaderFromSourceFile(QOpenGLShader::Fragment, fragmentShaderPath );

     if ( !result )
     {
        qWarning() << m_pgm.log();
        return result;
     }

    //Finally link the shaders to the program
     result = m_pgm.link();

     if ( !result )
     {
        qWarning() << m_pgm.log();
        return result;
     }

     return result;
}

void GLWidget::loadMatricesToShader(QVector3D position)
{
    //Get vector from camera to the origin
    QVector3D o(10,0,10);
    QVector3D camToOrig = o - m_cameraPos;

    //Normalize and calculate a new position somewhere along this vector
    camToOrig.normalize();
    QVector3D newCamPos = m_cameraPos + (m_mouseDelta * camToOrig);

    //Reset the view matrix and set to look at origin from the new position
    m_view.setToIdentity();

    if(m_ortho)
    {
        m_view.ortho(-25, 25, 25, -25, 0.01f, 1000.0f);
    }
    else
    {
        m_view.lookAt(newCamPos + m_dir, QVector3D(0,(m_terrainMax + m_terrainMin) / 4.0f,0), QVector3D(0,1,0));
    }

    //Reset the projection matrix and set to the right perspective
    m_proj.setToIdentity();

    if(!m_ortho)
    {
        m_proj.perspective(45.0f, float(width())/height(), 0.1f, 1000.0f);
    }

    //Reset the model matrix and set to the right matrix taking into account mouse movement

    if(m_ortho)
    {
        m_model.setToIdentity();

        m_model.rotate(4324.0f / 16.0f, QVector3D(1, 0, 0));

        m_model.translate(QVector3D(0,2,0));

        m_model.translate(position);
    }
    else
    {
        m_model.setToIdentity();

//        m_model.rotate(m_xRot / 16.0f, QVector3D(1, 0, 0));
        m_model.rotate(m_yRot / 16.0f, QVector3D(0, 1, 0));

        m_model.translate(position);
    }

    //Calculate the MVP
    m_mvp = m_proj * m_view * m_model;


    //Pass the MVP into the shader
    m_pgm.setUniformValue("M",m_model);
    m_pgm.setUniformValue("MVP",m_mvp);
    m_pgm.setUniformValue("camPos", newCamPos);
}

void GLWidget::changeOrtho()
{
    if(m_ortho)
    {
        glClearColor(0.8, 0.8f, 0.8f, 1.0f);
        for(uint i = 0; i < m_verts.size(); ++i)
        {
            m_verts[i].setY(m_verts[i].y() + 1.0f);
        }

        vbo_terrain.bind();
        vbo_terrain.allocate(&m_verts[0], (int)m_verts.size() * sizeof(GLfloat) * 3);
        vbo_terrain.release();
    }
    else
    {
        glClearColor(0.0f, 0.0f, 0.2f, 0.0f);
        for(uint i = 0; i < m_verts.size(); ++i)
        {
            m_verts[i].setY(m_verts[i].y() - 1.0f);
        }
        vbo_terrain.bind();
        vbo_terrain.allocate(&m_verts[0], (int)m_verts.size() * sizeof(GLfloat) * 3);
        vbo_terrain.release();
    }
}
 //----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void GLWidget::generateHeightMap(int iterations, float roughness)
{
    //We can use the bitwise shift to get 2^iterations
    m_divisions = 1 << iterations;

    //Setup the height map container, initialise at 0.0f
    m_heights.resize(m_divisions + 1, std::vector<float>(m_divisions + 1, 0.0f));

    //Create a new random generator
    std::random_device rnd;
    std::mt19937 eng(rnd());

//    if(s_hasSeed)
//    {
//        eng.seed(s_seed);
//    }
//    else
//    {
//        eng.seed(time(NULL));
//    }

    std::uniform_real_distribution<> random(-1.0f, 1.0f);

    //Set the four corners of the height map to random values between -1 and 1
    m_heights[0][0] = random(eng);
    m_heights[0][m_divisions] = random(eng);
    m_heights[m_divisions][m_divisions] = random(eng);
    m_heights[m_divisions][0] = random(eng);

    float roughStore = roughness;

    //Iterate through the entire height map for the given number of times
    for(int i = 0; i < iterations; ++i)
    {
        //Calculate the current diamond/square slice (this value will go down
        //as the iterations become higher)
        int currentSide = 1 << (iterations - i);

        //And the length of square side
        int sideLength = currentSide >> 1;

        //Iterate through the height map using the slice as the step and
        //do the diamond calculation
        for(int j = 0; j < m_divisions; j += currentSide)
        {
            for(int k = 0; k < m_divisions; k += currentSide)
            {
                diamond(j, k , currentSide, roughness);
            }
        }

        //If the side length is bigger than 0 we can do the square step
        if(sideLength > 0)
        {
            //Iterate through the height map again doing the square step this time
            for(int j = 0; j <= m_divisions; j += sideLength)
            {
                for(int k = (j + sideLength) % currentSide; k <= m_divisions; k += currentSide)
                {
                    square(j - sideLength, k - sideLength, currentSide, roughness);
                }
            }
        }

        //Finally reduce the roughness by a half for the next iteration
        roughness *= 0.5;
    }

    //Now combining perlin noise with this height map
    PerlinNoise noise;

    std::uniform_real_distribution<> randomValue(0.0f, 1.0f);

    //Choose a random value to enter the 3D perlin noise with
    double zValue = randomValue(eng);

    for(double i = 0; i< m_heights.size(); ++i)
    {
        for(double j = 0; j < m_heights[i].size(); ++j)
        {
            //Iterate through the height field, adding a perlin noise value and then
            //taking the average
//            m_heights[i][j] = noise.octaveNoise(i/(double)m_heights.size(), 0.5,  j/(double)m_heights[j].size(), 2, 0.25) * 10.0;
            m_heights[i][j] += noise.noise(i / m_heights.size(), j / m_heights[i].size(), zValue) * (roughStore * zValue * 10.0f);
            m_heights[i][j] /= 2.0f;
        }
    }

    //Set the minimum and maximum values of the terrain to the value at [0][0]
    //This makes sure when boolean operations are done on them there will be
    //an actual value there and not one randomly assigned during creation
    m_terrainMin = m_terrainMax = m_heights[0][0];

    //Iterate through the height map and check for the smallest and largest values
    for(int i = 0; i< m_heights.size(); ++i)
    {
        for(int j = 0; j < m_heights[i].size(); ++j)
        {
            if(m_heights[i][j] < m_terrainMin)
            {
                m_terrainMin = m_heights[i][j];
            }
            else if(m_heights[i][j] > m_terrainMax)
            {
                m_terrainMax = m_heights[i][j];
            }
        }
    }

    //The next bit creates the flat parts of terrain. The following variables
    //represent the range in which the cut takes place
    float minCut = ((m_terrainMax - m_terrainMin) * (5.0f/8.0f)) + m_terrainMin;
    float maxCut = ((m_terrainMax - m_terrainMin) * (7.0f/8.0f)) + m_terrainMin;

    float minCut2 = ((m_terrainMax - m_terrainMin) * (2.0f/8.0f)) + m_terrainMin;
    float maxCut2 = ((m_terrainMax - m_terrainMin) * (4.0f/8.0f)) + m_terrainMin;

    //Iterate through the height map and check for values within the given
    //slice ranges. If the value is within the margin then set it to the average
    //of the min -> max slice
    for(int i = 0; i< m_heights.size(); ++i)
    {
        for(int j = 0; j < m_heights[i].size(); ++j)
        {
            if((m_heights[i][j] <= maxCut) && (m_heights[i][j] >= minCut))
            {
//                m_heights[i][j] = (maxCut + minCut) / 2.0f;
            }
            else if((m_heights[i][j] <= maxCut2) && (m_heights[i][j] >= minCut2))
            {
//                m_heights[i][j] = (maxCut2 + minCut2) / 2.0f;
            }
        }
    }

    //Finally we need to convert the height map into vertex positions

    //Set the range (width and height(of the terrain)
    float range = s_terrainSize;

    //Centering around the origin. E.G. a range of 20 starts at -10
    float start = - (range/2.0f);

    //This will be used to update the camera position
    QVector3D terrainMiddle(0,0,0);

    //Iterate through the height map and make the terrain verts
    for(int i = 0; i < m_divisions; ++i)
    {
        std::vector<QVector3D> tmpNormals;

        for(int j = 0; j < m_divisions; ++j)
        {
            //Add the normal at the current vertex to the container
            tmpNormals.push_back(getNormal(i, j));

            //Create four vertices. Whilst some will be repeated this is just easier
            //to do than trying to create an index list


            //The face is assigned anti-clockwise starting at (i, j):
            //  (i, j + 1) -------------------------- (i + 1, j + 1)
            //       |                                          |
            //       |                                          |
            //       |                                          |
            //       |                                          |
            //       |                                          |
            //    (i , j) --------------------------------- (i + 1, j)

            QVector3D v1((((float)(i)/m_divisions) * range) + start,
                                getHeight(i, j),
                                (((float)(j)/m_divisions) * range) + start);

            m_verts.push_back(v1);

            //Also ensure the normal for that vertex has been calculated
            m_norms.push_back(getNormal(i, j));

            //And then the UV coordinates;
            m_uvs.push_back(QVector2D(0, 0));

            QVector3D v2((((float)(i + 1)/m_divisions) * range) + start,
                                getHeight(i + 1, j),
                                (((float)(j)/m_divisions) * range) + start);

            m_verts.push_back(v2);

            m_norms.push_back(getNormal(i + 1, j));

            m_uvs.push_back(QVector2D(0.5f, 0));


            QVector3D v3((((float)(i + 1)/m_divisions) * range) + start,
                                getHeight(i + 1, j + 1),
                                (((float)(j + 1)/m_divisions) * range) + start);

            m_verts.push_back(v3);

            m_norms.push_back(getNormal(i + 1, j + 1));

            m_uvs.push_back(QVector2D(0.5f, 0.5f));


            QVector3D v4((((float)(i)/m_divisions) * range) + start,
                                getHeight(i, j + 1),
                                (((float)(j + 1)/m_divisions) * range) + start);

            m_verts.push_back(v4);

            m_norms.push_back(getNormal(i, j + 1));

            m_uvs.push_back(QVector2D(0, 0.5f));

            terrainMiddle += ((v1 + v2 + v3 + v4) / 4.0f);
            terrainMiddle /= 2.0f;
        }

        m_normalMap.push_back(tmpNormals);
    }

    //Update the camera position so that is centred (pretty much) on the middle
    //of the terrain an always has the terrain visible
    terrainMiddle.setY(((m_terrainMax - m_terrainMin) * 0.75f) + m_terrainMin);

    m_cameraPos += terrainMiddle;

    m_view.lookAt(m_cameraPos,
                            terrainMiddle,
                            QVector3D(0,1,0));
}

void GLWidget::drawTerrain()
{
    vao_terrain.bind();

    (m_wireframe) ?  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE) :  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glDrawArrays(GL_QUADS, 0, (int)m_verts.size());
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    vao_terrain.release();
}

float GLWidget::getHeight(int x, int y)
{
    if( x < 0 || x >= m_heights.size() || y < 0)
    {
        return 0.0f;
    }
    else if(y >= m_heights[x].size())
    {
        return 0.0f;
    }
    else
    {
        return m_heights[x][y];
    }
}

QVector3D GLWidget::getNormal(int x, int y)
{
    QVector3D normal(0,0,0);

    //Do a quick check to see if we're on any of the edges of
    //the terrain. If we are then just set the normal to the y axis
    if((x == 0) || (x >= (m_divisions - 1)) ||
            (y == 0) || (y >= (m_divisions - 1)))
    {
        normal = QVector3D(0,1,0);
    }
    //Otherwise take the average of the heights around the point
    else
    {
        float heightToLeft = getHeight(x - 1, y);
        float heightToRight = getHeight(x + 1, y);
        float heightToFront = getHeight(x, y + 1);
        float heightToBack = getHeight(x, y - 1);

        normal = QVector3D(heightToLeft - heightToRight, 2.0f, heightToBack - heightToFront);
    }

    //Finally normalize
    return normal.normalized();
}

void GLWidget::diamond(int x, int y, int sideLength, float scale)
{
    if(sideLength > 1)
    {
        std::random_device rnd;
        std::mt19937 eng(rnd());

//        if(s_hasSeed)
//        {
//            eng.seed(s_seed + 1);
//        }

        std::uniform_real_distribution<> diamondValue(-1.0f, 1.0f);

        int midPoint = sideLength / 2;

        float squareAverage = (getHeight(x, y) + getHeight(x + sideLength, y) + getHeight(x + sideLength, y + sideLength) + getHeight(x, y + sideLength))/4.0f;

        m_heights[x + midPoint][y + midPoint] = squareAverage + (diamondValue(eng) * scale);
    }
}

void GLWidget::square(int x, int y, int sideLength, float scale)
{
    std::random_device rnd;
    std::mt19937 eng(rnd());

//    if(s_hasSeed)
//    {
//        eng.seed(s_seed - 1);
//    }

    std::uniform_real_distribution<> squareValue(-1.0f, 1.0f);

    int midPoint = sideLength / 2;

    float heightAverage = 0.0f;
    float heightTotal = 0.0f;

    if(x >= 0)
    {
        heightAverage += getHeight(x, y + midPoint);
        heightTotal++;
    }

    if(y >= 0)
    {
        heightAverage += getHeight(x + midPoint, y);
        heightTotal++;
    }

    if(x + sideLength <= m_divisions)
    {
        heightAverage += getHeight(x + sideLength, y + midPoint);
        heightTotal++;
    }

    if(y + sideLength <= m_divisions)
    {
        heightAverage += getHeight(x + midPoint, y + sideLength);
        heightTotal++;
    }

    m_heights[x + midPoint][y + midPoint] = (heightAverage / heightTotal) + (squareValue(eng) * scale);
}

void GLWidget::prepareTerrain()
{
    //Create a VAO to store terrain drawing data in
    vao_terrain.create();
    vao_terrain.bind();

    //Create a vertex buffer object, set it to static (the vertex
    //data doesn't change), and then pass in the list of vertices
    vbo_terrain.create();
    vbo_terrain.setUsagePattern(QOpenGLBuffer::StaticDraw);
    vbo_terrain.bind();
    vbo_terrain.allocate(&m_verts[0], (int)m_verts.size() * sizeof(GLfloat) * 3);

    //Make sure the vertices will be passed to the right place
    //in the shader
    m_pgm.enableAttributeArray("vertexPos");
    m_pgm.setAttributeArray("vertexPos", GL_FLOAT, 0, 3);

    vbo_terrain.release();

    //Now create a normals buffer object and do the same process
    nbo_terrain.create();
    nbo_terrain.setUsagePattern(QOpenGLBuffer::StaticDraw);
    nbo_terrain.bind();
    nbo_terrain.allocate(&m_norms[0], (int)m_norms.size() * sizeof(GLfloat) * 3);

    m_pgm.enableAttributeArray("vertexNorm");
    m_pgm.setAttributeArray("vertexNorm", GL_FLOAT, 0, 3);

    nbo_terrain.release();

    uvbo_terrain.create();
    uvbo_terrain.setUsagePattern(QOpenGLBuffer::StaticDraw);
    uvbo_terrain.bind();
    uvbo_terrain.allocate(&m_uvs[0], (int)m_uvs.size() * sizeof(GLfloat) * 2);

    m_pgm.enableAttributeArray("vertexUV");
    m_pgm.setAttributeArray("vertexUV", GL_FLOAT, 0, 2);

    uvbo_terrain.release();

    //Finally release the VAO
    vao_terrain.release();
}

void GLWidget::prepareWater()
{
    //Set the water level to 1/4 of the terrains height range
    m_waterLevel = ((m_terrainMax - m_terrainMin) / 4.0f) + m_terrainMin;

    //Pass this value into the shader
    m_pgm.setUniformValue("waterLevel", m_waterLevel);

    //Hard code a plane at the specific level in the Y axis
    m_waterVerts.push_back(QVector3D(-100.0f, m_waterLevel, -100.0f));
    m_waterVerts.push_back(QVector3D(-100.0f, m_waterLevel, 100.0f));
    m_waterVerts.push_back(QVector3D(100.0f, m_waterLevel, 100.0f));
    m_waterVerts.push_back(QVector3D(100.0f, m_waterLevel, -100.0f));

    //Set the vertex norms
    m_waterNorms.push_back(QVector3D(0.0f, 1.0f, 0.0f));
    m_waterNorms.push_back(QVector3D(0.0f, 1.0f, 0.0f));
    m_waterNorms.push_back(QVector3D(0.0f, 1.0f, 0.0f));
    m_waterNorms.push_back(QVector3D(0.0f, 1.0f, 0.0f));

    //Create a VAO for the water plane
    vao_water.create();
    vao_water.bind();

    //Same process as for the terrain, create a vbo with the vertex data
    //and then create an nbo with the normal data
    vbo_water.create();
    vbo_water.bind();
    vbo_water.allocate(&m_waterVerts[0], (int)m_waterVerts.size() * sizeof(GLfloat) * 3);

     m_pgm.enableAttributeArray("vertexPos");
     m_pgm.setAttributeArray("vertexPos", GL_FLOAT, 0, 3);

     vbo_water.release();

     nbo_water.create();
     nbo_water.bind();
     nbo_water.allocate(&m_waterNorms[0], (int)m_waterNorms.size() * sizeof(GLfloat) * 3);

    m_pgm.enableAttributeArray("vertexNorm");
    m_pgm.setAttributeArray("vertexNorm", GL_FLOAT, 0, 3);

    nbo_water.release();

    //And then release the water VAO
    vao_water.release();
}

void GLWidget::prepareTrees()
{
    //Hard code some vertices which represent a 'tree'
    m_treeVerts.push_back(QVector3D(0.2f, 0.0f, 0.2f));   //0
    m_treeVerts.push_back(QVector3D(-0.2f, 0.0f, 0.2f));  //1
    m_treeVerts.push_back(QVector3D(-0.2f, 0.0f, -0.2f)); //2
    m_treeVerts.push_back(QVector3D(0.2f, 0.0f, -0.2f));  //3

    m_treeVerts.push_back(QVector3D(0.01f, 0.75f, 0.01f));   //4
    m_treeVerts.push_back(QVector3D(-0.01f, 0.75f, 0.01f));  //5
    m_treeVerts.push_back(QVector3D(-0.01f, 0.75f, -0.01f)); //6
    m_treeVerts.push_back(QVector3D(0.01f, 0.75f, -0.01f));  //7

    //Hard code the face indices
    //Bottom
    m_treeIndices.push_back(0);
    m_treeIndices.push_back(1);
    m_treeIndices.push_back(2);
    m_treeIndices.push_back(3);

    //Sides
    m_treeIndices.push_back(0);
    m_treeIndices.push_back(1);
    m_treeIndices.push_back(5);
    m_treeIndices.push_back(4);

    m_treeIndices.push_back(1);
    m_treeIndices.push_back(2);
    m_treeIndices.push_back(6);
    m_treeIndices.push_back(5);

    m_treeIndices.push_back(2);
    m_treeIndices.push_back(3);
    m_treeIndices.push_back(7);
    m_treeIndices.push_back(6);

    m_treeIndices.push_back(3);
    m_treeIndices.push_back(0);
    m_treeIndices.push_back(4);
    m_treeIndices.push_back(7);

    //Top
    m_treeIndices.push_back(4);
    m_treeIndices.push_back(5);
    m_treeIndices.push_back(6);
    m_treeIndices.push_back(7);

    //Now execute the same process as with the terrain and water, only
    //this time there are no normals (but there is an index buffer)
    vao_trees.create();
    vao_trees.bind();

    vbo_trees.create();
    vbo_trees.bind();
    vbo_trees.allocate(&m_treeVerts[0], (int)m_treeVerts.size() * sizeof(GLfloat) * 3);

    m_pgm.enableAttributeArray("vertexPos");
    m_pgm.setAttributeArray("vertexPos", GL_FLOAT, 0, 3);

    vbo_trees.release();

    ibo_trees.create();
    ibo_trees.bind();
    ibo_trees.allocate(&m_treeIndices[0], (int)m_treeIndices.size() * sizeof(uint));

    vao_trees.release();

    if(s_normTrees)
    {
        //Finally we need to place the trees so iterate through the list of normals
        int switchCounter = 0;

        for(int i = 0; i < m_norms.size(); i += 4)
        {
            //Get the normal of the current face
            QVector3D faceNorm = m_norms[i] + m_norms[i + 1] + m_norms[i + 2] + m_norms[i + 3];
            faceNorm /= 4.0f;
            faceNorm.normalize();

            //        if(faceNorm.y() > 0.8f)
            //        {
            //            QVector3D middle = (m_verts[i] + m_verts[i + 1] + m_verts[i + 2] + m_verts[i + 3]) / 4.0f;

            //            bool addSite = true;

            //            if(middle.y() < m_waterLevel * 1.25)
            //            {
            //                addSite = false;
            //            }
            //            else
            //            {
            //                for(uint j = 0; j < m_sitePoints.size(); ++j)
            //                {
            //                    if((m_sitePoints[j] - middle).length() < 10.0)
            //                    {
            //                        addSite = false;
            //                        break;
            //                    }
            //                }
            //            }

            //            if(addSite)
            //            {
            //                if(m_sitePoints.size() > 19)
            //                {
            //                    m_sitePoints.push_back(middle);

            //                    std::sort(m_sitePoints.begin(), m_sitePoints.end(), SortVector());

            //                    m_sitePoints.pop_back();
            //                }
            //                else
            //                {
            //                    m_sitePoints.push_back(middle);
            //                }
            //            }
            //        }

            //We can now just see what the y value of the face normal is. If it is large we know
            //the face points mostly upward, making it relatively flat. A check is also done to see
            //if the current vert is above the water level or not
            if((faceNorm.y() < 0.95f) && (m_verts[i].y() > (m_waterLevel + ((m_terrainMax - m_terrainMin) * 0.1f))))
            {
                //Create a variable for storing the middle of the face
                QVector3D midFace;

                //To introduce some pseudo-random placement (to ensure the trees aren't all added in a uniform
                //grid) use various methods to calulate the new position.
                switch(switchCounter)
                {
                case 0:
                    midFace = m_verts[i] + m_verts[i + 1] + m_verts[i + 2] + m_verts[i + 3];
                    m_treePositions.push_back(midFace/4.0f);
                    break;

                case 1:
                    midFace = m_verts[i] + m_verts[i + 1] + m_verts[i + 2];
                    m_treePositions.push_back(midFace/3.0f);
                    break;

                case 2:
                    midFace = m_verts[i] + m_verts[i + 1] + m_verts[i + 3];
                    m_treePositions.push_back(midFace/3.0f);
                    break;

                case 3:
                    midFace = m_verts[i] + m_verts[i + 3] + m_verts[i + 2];
                    m_treePositions.push_back(midFace/3.0f);
                    break;

                case 4:
                    midFace = m_verts[i + 3] + m_verts[i + 1] + m_verts[i + 2];
                    m_treePositions.push_back(midFace/3.0f);
                    break;

                case 5:
                    midFace = m_verts[i] + m_verts[i + 1];
                    m_treePositions.push_back(midFace/2.0f);
                    break;

                case 6:
                    midFace = m_verts[i + 1] + m_verts[i + 2];
                    m_treePositions.push_back(midFace/2.0f);
                    switchCounter = 0;
                    break;
                }

                //This ensures that the trees are positioned in different places.
                //Not the perfect way of doing it but introduces enough randominity
                //to create visual interest
                switchCounter++;

            }
        }
    }
}

QOpenGLTexture* GLWidget::addNewTexture(QString &filename)
{
    QOpenGLTexture* texture = new QOpenGLTexture(QImage(filename));

    texture->setWrapMode(QOpenGLTexture::Repeat);

    texture->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);

    texture->setMagnificationFilter(QOpenGLTexture::Linear);

    return texture;
}

QOpenGLTexture* GLWidget::addNewTexture(std::string &filename)
{
    QOpenGLTexture* texture = new QOpenGLTexture(QImage(filename.c_str()));

    texture->setWrapMode(QOpenGLTexture::Repeat);

    texture->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);

    texture->setMagnificationFilter(QOpenGLTexture::Linear);

    return texture;
}

 //----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
