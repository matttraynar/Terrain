#include "glwidget.h"

#include <QDebug>

GLWidget::GLWidget( QWidget* parent ) :
    QGLWidget(parent)
{
    QSurfaceFormat glFormat;
    glFormat.setVersion(3, 3);
    glFormat.setProfile(QSurfaceFormat::CoreProfile);

    m_xRot = 0;
    m_yRot = 0;
    m_zRot = 0;
    m_zDis = 0;
    m_mouseDelta = 0;
    m_cameraPos = QVector3D(5.0f, 5.0f, 5.0f);

    m_x = -20;
    moveDown = false;

    m_wireframe = false;
}

GLWidget::~GLWidget()
{

}


void GLWidget::initializeGL()
{
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    glEnable(GL_DEPTH_TEST);

    if(!prepareShaderProgram("shaders/vert.glsl", "shaders/frag.glsl") )
    {
        exit(1);
    }

    m_pgm.bind();

    m_view.setToIdentity();
    m_view.lookAt(m_cameraPos,
                          QVector3D(0.0f, 0.0f, 0.0f),
                          QVector3D(0.0f, 1.0f, 0.0f));

    startTimer(1);

    prepareTerrain(3);
    qInfo()<<"Terrain prepared";

    m_pgm.release();

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

    m_pgm.setUniformValue("mCol",QVector4D(0.2f,0.95f,0.2f,0.0f));
    m_pgm.setUniformValue("lightPos",QVector3D(m_x, 10.0f, 0.0f));

    loadMatricesToShader(QVector3D(0,0,0));

    drawTerrain();

    vao_water.bind();
    m_pgm.setUniformValue("mCol",QVector4D(0.0f,0.0f,1.0f,1.0f));

//    glDrawArrays(GL_QUADS, 0, (int)m_waterVerts.size());

    vao_water.release();

    vao_trees.bind();
    m_pgm.setUniformValue("mCol",QVector4D(1.0f,0.0f,0.0f,1.0f));

    for(uint i = 0; i < m_treePositions.size(); ++i)
    {
        loadMatricesToShader(m_treePositions[i]);

        glDrawElements(GL_QUADS, (int)m_treeIndices.size(), GL_UNSIGNED_INT, &m_treeIndices[0]);
    }

    vao_trees.release();

    m_pgm.release();
}

void GLWidget::timerEvent(QTimerEvent *e)
{
    e;

    static float colour = 0.0f;

    if(colour <= 0.25f)
    {
        glClearColor(0.0f, 0.0f, colour, 1.0f);
        colour += 0.0001f;
    }
    else if(colour <= 0.5f)
    {
        glClearColor(0.0f, 0.0f, 0.5f - colour, 1.0f);
        colour += 0.0001f;
    }
    else
    {
        colour = 0.0f;
    }

    if(m_x > 20)
    {
        moveDown = true;
    }
    else if(m_x < -20)
    {
        moveDown = false;
    }

    (moveDown) ? m_x -= 0.05f : m_x += 0.05f;

    update();
}

void GLWidget::wheelEvent(QWheelEvent *e)
{
    //Get the number of degrees the wheel has turned
    int numDegrees = e->delta() / 8;
    int numSteps = numDegrees / 15;

    //Add the steps to a member variable used in paintGL
    m_mouseDelta += numSteps;

    update();
}

void GLWidget::mousePressEvent(QMouseEvent *e)
{
    //Store the position the mouse was pressed in
    m_lastPos = e->pos();
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
    QVector3D o(0,0,0);
    QVector3D camToOrig = o - m_cameraPos;

    //Normalize and calculate a new position somewhere along this vector
    camToOrig.normalize();
    QVector3D newCamPos = m_cameraPos + (m_mouseDelta * camToOrig);

    //Reset the view matrix and set to look at origin from the new position
    m_view.setToIdentity();
    m_view.lookAt(newCamPos + m_dir, QVector3D(0,0,0), QVector3D(0,1,0));

    //Reset the projection matrix and set to the right perspective
    m_proj.setToIdentity();
    m_proj.perspective(70.0f, float(width())/height(), 0.1f, 1000.0f);

    //Reset the model matrix and set to the right matrix taking into account mouse movement
    m_model.setToIdentity();

    m_model.rotate(m_xRot / 16.0f, QVector3D(1, 0, 0));
    m_model.rotate(m_yRot / 16.0f, QVector3D(0, 1, 0));

    m_model.translate(position);

    //Calculate the MVP
    m_mvp = m_proj * m_view * m_model;

    //Pass the MVP into the shader
    m_pgm.setUniformValue("M",m_model);
    m_pgm.setUniformValue("MVP",m_mvp);
    m_pgm.setUniformValue("camPos", newCamPos);
}

 //----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void GLWidget::prepareTerrain(int iterations)
{
    float dimensions = pow(2, iterations);

//    m_heights.resize(dimensions + 1, std::vector<float>(dimensions + 1, 0.0f));

//    diamondSquare(0, 0, dimensions + 1, dimensions + 1, 3.0f, iterations);
//    newDiamondSquare(0,0,dimensions - 1, dimensions - 1, 3.0f, 0, iterations, dimensions);

//    d2(0,dimensions,0, dimensions, 5.0f, 0, iterations);

    std::vector<float> heightMap = {0.0f, 0.0f, 0.0f, 0.0f};

    float range = 50.0f;

    float start = (0 - (range/2.0f));

    heightMap = d3(heightMap, 5.0f, 0, iterations);

    qInfo()<<"Verts";
    qInfo()<<heightMap.size();

    int rowCount = 0;
    int columnCount = 0;

    //PROBLEM WITH D£ returning height maps too small?>

//    !((columnCount == (pow(2, iterations) - 1) || (rowCount == (pow(2, iterations) - 1))))

    while((columnCount + ((rowCount + 1)* (pow(2, iterations)))) != heightMap.size())
    {
        qInfo()<<(columnCount == pow(2, iterations))<<(rowCount == (pow(2, iterations)));
        qInfo()<<columnCount + 1 + ((rowCount + 1) * (pow(2, iterations) + 1));
        qInfo()<<columnCount<<rowCount;

        QVector3D newVertex( (((float)columnCount / (pow(2,
                                                         iterations) + 1) ) * range) + start,
                                        heightMap[columnCount + (rowCount * (pow(2, iterations) + 1))],
                                        (((float)rowCount / (pow(2, iterations) + 1) ) * range) + start);

        m_verts.push_back(newVertex);
//            m_norms.push_back(getNormal(i, j));
        m_norms.push_back(QVector3D(0,1,0));

        QVector3D newVertex2( ((((float)columnCount + 1) / (pow(2, iterations) + 1) ) * range) + start,
                                        heightMap[columnCount + 1 + (rowCount * (pow(2, iterations) + 1))],
                                        (((float)rowCount / (pow(2, iterations) + 1) ) * range) + start);

        m_verts.push_back(newVertex2);
//            m_norms.push_back(getNormal(i + 1, j));
        m_norms.push_back(QVector3D(0,1,0));

        QVector3D newVertex3( ((((float)columnCount + 1) / (pow(2, iterations) + 1) ) * range) + start,
                                        heightMap[columnCount + 1 + ((rowCount + 1) * (pow(2, iterations) + 1))],
                                        ((((float)rowCount + 1) / (pow(2, iterations) + 1) ) * range) + start);

        m_verts.push_back(newVertex3);
//            m_norms.push_back(getNormal(i + 1, j + 1));
        m_norms.push_back(QVector3D(0,1,0));

        QVector3D newVertex4( (((float)columnCount / (pow(2, iterations) + 1) ) * range) + start,
                                        heightMap[columnCount + ((rowCount + 1) * (pow(2, iterations) + 1))],
                                        ((((float)rowCount + 1) / (pow(2, iterations) + 1) ) * range) + start);

        m_verts.push_back(newVertex4);
//            m_norms.push_back(getNormal(i, j + 1));
        m_norms.push_back(QVector3D(0,1,0));

        if(columnCount == (pow(2, iterations) - 1))
        {
            rowCount++;
            columnCount = 0;
        }
        else
        {
            columnCount++;
        }
    }

    qInfo()<<(!((columnCount == (pow(2, iterations)) && (rowCount == (pow(2, iterations) - 1)))));

//    for(uint i = 0; i < iterations - 1; ++i)
//    {
//        for(uint j = 0; j < iterations - 1; ++j)
//        {

//            qInfo()<<"v1";
//            QVector3D newVertex( (((float)i / (iterations) ) * range) + start,
//                                            heightMap[(i * iterations) + j],
//                                            (((float)j / (iterations) ) * range) + start);

//            m_verts.push_back(newVertex);
////            m_norms.push_back(getNormal(i, j));
//            m_norms.push_back(QVector3D(0,1,0));

//            qInfo()<<"v2";
//            QVector3D newVertex2( ((((float)i + 1) / (iterations) ) * range) + start,
//                                            heightMap[((i + 1) * iterations) + j],
//                                            (((float)j / (iterations) ) * range) + start);

//            m_verts.push_back(newVertex2);
////            m_norms.push_back(getNormal(i + 1, j));
//            m_norms.push_back(QVector3D(0,1,0));

//            qInfo()<<"v3";
//            QVector3D newVertex3( ((((float)i + 1) / (iterations) ) * range) + start,
//                                            heightMap[((i + 1) * iterations) + j + 1],
//                                            ((((float)j + 1) / (iterations) ) * range) + start);

//            m_verts.push_back(newVertex3);
////            m_norms.push_back(getNormal(i + 1, j + 1));
//            m_norms.push_back(QVector3D(0,1,0));

//            qInfo()<<"v4";
//            QVector3D newVertex4( (((float)i / (iterations) ) * range) + start,
//                                            heightMap[(i * iterations) + j + 1],
//                                            ((((float)j + 1) / (iterations ) ) * range) + start);

//            m_verts.push_back(newVertex4);
////            m_norms.push_back(getNormal(i, j + 1));
//            m_norms.push_back(QVector3D(0,1,0));
//        }
//    }




//    for(uint i = 0; i < dimensions; ++i)
//    {
//        for(uint j = 0; j < dimensions; ++j)
//        {
//            QVector3D newVertex( (((float)i / (dimensions) ) * range) + start,
//                                            m_heights[i][j],
//                                            (((float)j / (dimensions) ) * range) + start);

//            m_verts.push_back(newVertex);
//            m_norms.push_back(getNormal(i, j));

//            QVector3D newVertex2( ((((float)i + 1) / (dimensions) ) * range) + start,
//                                            m_heights[i + 1][j],
//                                            (((float)j / (dimensions) ) * range) + start);

//            m_verts.push_back(newVertex2);
//            m_norms.push_back(getNormal(i + 1, j));

//            QVector3D newVertex3( ((((float)i + 1) / (dimensions) ) * range) + start,
//                                            m_heights[i + 1][j + 1],
//                                            ((((float)j + 1) / (dimensions) ) * range) + start);

//            m_verts.push_back(newVertex3);
//            m_norms.push_back(getNormal(i + 1, j + 1));

//            QVector3D newVertex4( (((float)i / (dimensions) ) * range) + start,
//                                            m_heights[i][j + 1],
//                                            ((((float)j + 1) / (dimensions ) ) * range) + start);

//            m_verts.push_back(newVertex4);
//            m_norms.push_back(getNormal(i, j + 1));
//        }
//    }


//    m_norms.resize(m_verts.size(), QVector3D(0.5f, 0.0f, 0.0f));

    int count = 0;

    qInfo()<<"Norms";


//    for(int i = 0; i < dimensions; ++i)
//    {
//        for(int j = 0; j < dimensions; ++j)
//        {
//            float heightToLeft = 0.0f;
//            float heightToRight = 0.0f;
//            float heightToFront = 0.0f;
//            float heightToBack = 0.0f;

//            ((i - 1) > 0) ? heightToLeft = m_heights[i - 1][j] : heightToLeft = 0.0f;
//            ((i + 1) < dimensions) ? heightToRight = m_heights[i + 1][j] : heightToRight = 0.0f;
//            ((j + 1) < dimensions) ? heightToFront = m_heights[i][j + 1] : heightToFront = 0.0f;
//            ((j - 1) > 0) ? heightToBack = m_heights[i][j - 1] : heightToBack = 0.0f;

//            m_norms[count].setX(heightToLeft - heightToRight);
//            m_norms[count].setY(heightToBack - heightToFront);
//            m_norms[count].setZ(2.0f);

//            m_norms[count].normalize();
//            count++;

//            ((i + 1 - 1) > 0) ? heightToLeft = m_heights[i + 1 - 1][j] : heightToLeft = 0.0f;
//            ((i + 1 + 1) < dimensions) ? heightToRight = m_heights[i + 1 + 1][j] : heightToRight = 0.0f;
//            ((j + 1) < dimensions && (i + 1) < dimensions) ? heightToFront = m_heights[i + 1][j + 1] : heightToFront = 0.0f;
//            ((j - 1) > 0 && (i + 1) < dimensions) ? heightToBack = m_heights[i + 1][j - 1] : heightToBack = 0.0f;

//            m_norms[count].setX(heightToLeft - heightToRight);
//            m_norms[count].setY(heightToBack - heightToFront);
//            m_norms[count].setZ(2.0f);

//            m_norms[count].normalize();

//            count++;

//            ((i + 1 - 1) > 0 && (j + 1) < dimensions) ? heightToLeft = m_heights[i + 1 - 1][j + 1] : heightToLeft = 0.0f;
//            ((i + 1 + 1) < dimensions && (j + 1) < dimensions) ? heightToRight = m_heights[i + 1 + 1][j + 1] : heightToRight = 0.0f;
//            ((j + 1 + 1) < dimensions && (i + 1) < dimensions) ? heightToFront = m_heights[i + 1][j + 1 + 1] : heightToFront = 0.0f;
//            ((j + 1 - 1) > 0 && (i + 1) < dimensions) ? heightToBack = m_heights[i + 1][j - 1 + 1] : heightToBack = 0.0f;

//            m_norms[count].setX(heightToLeft - heightToRight);
//            m_norms[count].setY(heightToBack - heightToFront);
//            m_norms[count].setZ(2.0f);

//            m_norms[count].normalize();


//            count++;

//            ((i - 1) > 0 && (j + 1) < dimensions) ? heightToLeft = m_heights[i - 1][j + 1] : heightToLeft = 0.0f;
//            ((i + 1) < dimensions && (j + 1) < dimensions) ? heightToRight = m_heights[i + 1][j + 1] : heightToRight = 0.0f;
//            ((j + 1 + 1) < dimensions) ? heightToFront = m_heights[i][j + 1 + 1] : heightToFront = 0.0f;
//            ((j + 1 - 1) > 0) ? heightToBack = m_heights[i][j - 1 + 1] : heightToBack = 0.0f;

//            m_norms[count].setX(heightToLeft - heightToRight);
//            m_norms[count].setY(heightToBack - heightToFront);
//            m_norms[count].setZ(2.0f);

//            m_norms[count].normalize();

//            count++;
//        }
//    }

    vao_terrain.create();
    vao_terrain.bind();

    vbo_terrain.create();
    vbo_terrain.setUsagePattern(QOpenGLBuffer::StaticDraw);
    vbo_terrain.bind();
    vbo_terrain.allocate(&m_verts[0], (int)m_verts.size() * sizeof(GLfloat) * 3);

    m_pgm.enableAttributeArray("vertexPos");
    m_pgm.setAttributeArray("vertexPos", GL_FLOAT, 0, 3);

    vbo_terrain.release();

    nbo_terrain.create();
    nbo_terrain.setUsagePattern(QOpenGLBuffer::StaticDraw);
    nbo_terrain.bind();
    nbo_terrain.allocate(&m_norms[0], (int)m_norms.size() * sizeof(GLfloat) * 3);

    m_pgm.enableAttributeArray("vertexNorm");
    m_pgm.setAttributeArray("vertexNorm", GL_FLOAT, 0, 3);

    nbo_terrain.release();

    vao_terrain.release();

    float averageHeight = 0.0f;

    for(uint i = 0; i < m_verts.size(); ++i)
    {
        averageHeight += m_verts[i].y();
        averageHeight /= 2.0f;
    }

    float tmpHeight = averageHeight;
    for(uint i = 0; i < m_verts.size(); ++i)
    {
        if(m_verts[i].y() > tmpHeight)
        {
            averageHeight += m_verts[i].y();
            averageHeight /= 2.0f;
        }
    }

    tmpHeight = averageHeight;
    for(uint i = 0; i < m_verts.size(); ++i)
    {
        if(m_verts[i].y() > tmpHeight)
        {
            averageHeight += m_verts[i].y();
            averageHeight /= 2.0f;
        }
    }

    m_waterVerts.push_back(QVector3D(-100.0f, averageHeight, -100.0f));
    m_waterVerts.push_back(QVector3D(-100.0f, averageHeight, 100.0f));
    m_waterVerts.push_back(QVector3D(100.0f, averageHeight, 100.0f));
    m_waterVerts.push_back(QVector3D(100.0f, averageHeight, -100.0f));

    m_waterNorms.push_back(QVector3D(0.0f, 0.0f, 1.0f));
    m_waterNorms.push_back(QVector3D(0.0f, 0.0f, 1.0f));
    m_waterNorms.push_back(QVector3D(0.0f, 0.0f, 1.0f));
    m_waterNorms.push_back(QVector3D(0.0f, 0.0f, 1.0f));

    vao_water.create();
    vao_water.bind();

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

    vao_water.release();

    m_treeVerts.push_back(QVector3D(0.1f, 0.0f, 0.1f));   //0
    m_treeVerts.push_back(QVector3D(-0.1f, 0.0f, 0.1f));  //1
    m_treeVerts.push_back(QVector3D(-0.1f, 0.0f, -0.1f)); //2
    m_treeVerts.push_back(QVector3D(0.1f, 0.0f, -0.1f));  //3

    m_treeVerts.push_back(QVector3D(0.1f, 0.5f, 0.1f));   //4
    m_treeVerts.push_back(QVector3D(-0.1f, 0.5f, 0.1f));  //5
    m_treeVerts.push_back(QVector3D(-0.1f, 0.5f, -0.1f)); //6
    m_treeVerts.push_back(QVector3D(0.1f, 0.5f, -0.1f));  //7

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

    int switchCounter = 0;

    for(int i = 0; i < m_norms.size(); i += 4)
    {
        QVector3D faceNorm = m_norms[i] + m_norms[i + 1] + m_norms[i + 2] + m_norms[i + 3];
        faceNorm /= 4.0f;

        float angle = (acos(QVector3D::dotProduct(faceNorm.normalized(), QVector3D(0,1,0)) / (faceNorm.length())) * 180.0) / PI;

        if(angle > 125)
        {
            QVector3D midFace;

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

            switchCounter++;

        }
    }
}

void GLWidget::drawTerrain()
{
    vao_terrain.bind();

    (m_wireframe) ?  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE) :  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    glDrawArrays(GL_QUADS, 0, (int)m_verts.size());
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    vao_terrain.release();
}

void GLWidget::diamondSquare(uint x1, uint y1, uint x2, uint y2, float height, float iteration)
{
    if(iteration < 1) return;

    for(uint i = x1 + iteration; i < x2; i += iteration)
    {
        for(uint j = y1 + iteration; j < y2; j += iteration)
        {
            float v1 = m_heights[i - iteration][j - iteration];
            float v2 = m_heights[i][j - iteration];
            float v3 = m_heights[i - iteration][j];
            float v4 = m_heights[i][j];

            float center = ((v1 + v2 + v3 + v4)/4.0f) + ((float)rand() / RAND_MAX) * height;

            m_heights[(i - iteration) / 2][(j - iteration) / 2] = center;
        }
    }

    for(uint i = x1 + 2 * iteration; i < x2; i += iteration)
    {
        for(uint j = y1 + 2 * iteration; j < y2; j += iteration)
        {
            float v1 = m_heights[i - iteration][j - iteration];
            float v2 = m_heights[i][j - iteration];
            float v3 = m_heights[i - iteration][j];
            float v5 = m_heights[(i - iteration) / 2][(j - iteration) / 2];

            float newPoint = ((v1 + v3 + v5 + m_heights[i - 3 * iteration / 2][j - iteration / 2]) / 4.0f)  + ((float)rand() / RAND_MAX) * height;

            m_heights[i - iteration][j - iteration / 2] = newPoint;

            newPoint = ((v1 + v2 + v5 + m_heights[i - iteration / 2][j - 3 * iteration / 2]) / 4.0f)  + ((float)rand() / RAND_MAX) * height;

            m_heights[i - iteration / 2][j - iteration] = newPoint;

//            newPoint = ((v1 + v2 + v5 + m_heights[i - iteration / 2][j - iteration / 2]) / 4.0f)  + ((float)rand() / RAND_MAX) * height;

//            m_heights[i - iteration / 2][j - iteration / 2] = newPoint;

//            newPoint = ((v1 + v2 + v5 + m_heights[i - 3 * iteration / 2][j - 3 * iteration / 2]) / 4.0f)  + ((float)rand() / RAND_MAX) * height;

//            m_heights[i - iteration][j - iteration] = newPoint;
        }
    }

    diamondSquare(x1, y1, x2, y2, height / 2.0f, iteration / 2.0f);
}

void GLWidget::newDiamondSquare(uint x1, uint y1, uint x2, uint y2, float height, int iteration, int maxIterations, int dimensions)
{
    //INTERPOLATE VALUES

    if(iteration == maxIterations)
    {
        return;
    }

    // j
    // |
    // |
    // |
    // x--------------i

    qInfo()<<"Checking iter";
    if(iteration == 0)
    {
        qInfo()<<"First";
        m_heights.resize(dimensions, std::vector<float>(dimensions, 0.0f));

        m_heights[0]                       [0]                      = ((float)rand() / RAND_MAX) * height;
        m_heights[0]                       [dimensions - 1]  = ((float)rand() / RAND_MAX) * height;
        m_heights[dimensions - 1]   [0]                      = ((float)rand() / RAND_MAX) * height;
        m_heights[dimensions - 1]   [dimensions - 1]   = ((float)rand() / RAND_MAX) * height;

        uint xMid = x1 + (dimensions/(pow(2, iteration))) - 1;
        uint yMid = y1 + (dimensions/(pow(2, iteration))) - 1;

        newDiamondSquare(x1, y1, x2, y2, height, iteration + 1, maxIterations, dimensions);

//        newDiamondSquare(x1, y1, xMid, yMid, height, iteration + 1, maxIterations, dimensions);

//        newDiamondSquare(xMid, y1, x2, yMid, height, iteration + 1, maxIterations, dimensions);

//        newDiamondSquare(x1, yMid, xMid, y2, height, iteration + 1, maxIterations, dimensions);

//        newDiamondSquare(xMid, yMid,  x2, y2, height, iteration + 1, maxIterations, dimensions);
    }
    else
    {
        qInfo()<<"Corner step";

        float h1 = m_heights[x1][y1];
        float h2 = m_heights[x2][y1];
        float h3 = m_heights[x1][y2];
        float h4 = m_heights[x2][y2];

        float newHeight_Center = ((h1 + h2 + h3 + h4) / 4.0f) + ((float)rand() / RAND_MAX) * height;

        uint xMid = ((x2 - x1) / 2) + x1;
        uint yMid = ((y2 - y1) / 2) + y1;

        qInfo()<<"Diamond";
        m_heights[xMid][yMid] += newHeight_Center;


        float newHeight_Bottom = ((h2 - h1)/2.0f);// + ((float)rand() / RAND_MAX) * height;
        float newHeight_Top = ((h4 - h3)/2.0f);// + ((float)rand() / RAND_MAX) * height;
        float newHeight_Left = ((h3 - h1)/2.0f);// + ((float)rand() / RAND_MAX) * height;
        float newHeight_Right = ((h4 - h2)/2.0f);// + ((float)rand() / RAND_MAX) * height;

        qInfo()<<"Square";
        m_heights[xMid][y1] = newHeight_Bottom;
        m_heights[xMid][y2] = newHeight_Top;
        m_heights[x1][yMid] = newHeight_Left;
        m_heights[x2][yMid] = newHeight_Right;


        qInfo()<<"Next iter";
        newDiamondSquare(x1, y1, xMid, yMid, height, iteration + 1, maxIterations, dimensions);

        newDiamondSquare(xMid, y1, x2, yMid, height, iteration + 1, maxIterations, dimensions);

        newDiamondSquare(x1, yMid, xMid, y2, height, iteration + 1, maxIterations, dimensions);

        newDiamondSquare(xMid, yMid,  x2, y2, height, iteration + 1, maxIterations, dimensions);
    }

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
    float heightToLeft = getHeight(x - 1, y);
    float heightToRight = getHeight(x + 1, y);
    float heightToFront = getHeight(x, y + 1);
    float heightToBack = getHeight(x, y - 1);

    QVector3D normal(heightToLeft - heightToRight, 2.0f, heightToBack - heightToFront);
    return normal.normalized();
}

float GLWidget::getNoise(int x, int y)
{
    srand((x + 42) * (y + 19));
    return (((float)rand() / RAND_MAX));
}

void GLWidget::d2(int x1, int x2, int y1, int y2, float range, int iteration, int maxIterations)
{
    if(iteration == maxIterations)
    {
        return;
    }
    else if(iteration == 0)
    {
        qInfo()<<"Intial setup";
        std::random_device rnd;
        std::mt19937 eng(rnd());
        std::uniform_real_distribution<> diamondValue((-1 * (range/2.0f)), range/2.0f);

        m_heights.resize(pow(2, maxIterations) + 1, std::vector<float>(pow(2, maxIterations) + 1, 0.0f));

        m_heights[x1][y1] = diamondValue(eng);
        m_heights[x2][y1] = diamondValue(eng);
        m_heights[x1][y2] = diamondValue(eng);
        m_heights[x2][y2] = diamondValue(eng);

        d2(x1, x2, y1, y2, range, iteration + 1, maxIterations);
    }
    else
    {
        qInfo()<<"Iteration "<<iteration;

        std::random_device rnd;
        std::mt19937 eng(rnd());
        std::uniform_real_distribution<> diamondValue((-1 * (range/2.0f)), range/2.0f);
        std::uniform_real_distribution<> squareValue((-1 * (range/3.0f)), range/5.0f);

        float h1 = m_heights[x1][y1];
        float h2 = m_heights[x2][y1];
        float h3 = m_heights[x1][y2];
        float h4 = m_heights[x2][y2];

        int xMid = x1 + ((pow(2, maxIterations))/(pow(2, iteration)));
        int yMid = y1 + ((pow(2, maxIterations))/(pow(2, iteration)));

        m_heights[xMid][yMid] += (h1 + h2 + h3 + h4)/4.0f + diamondValue(eng);

        m_heights[xMid][y1] += (h1 + h2)/2.0f + squareValue(eng);
        m_heights[xMid][y2] += (h4 + h3)/2.0f + squareValue(eng);
        m_heights[x1][yMid] += (h3 + h1)/2.0f + squareValue(eng);
        m_heights[x2][yMid] += (h4 + h2)/2.0f + squareValue(eng);

        d2(x1, xMid, y1, yMid, range, iteration + 1, maxIterations);
        d2(x1, xMid, yMid, y2, range, iteration + 1, maxIterations);
        d2(xMid, x2, y1, yMid, range, iteration + 1, maxIterations);
        d2(xMid, x2, yMid, y2, range, iteration + 1, maxIterations);
    }
}

std::vector<float> GLWidget::d3(std::vector<float> heightMap, float range, int iteration, int maxIterations)
{
    qInfo()<<heightMap.size();

    if(iteration == maxIterations)
    {
        return heightMap;
    }
    else
    {
        qInfo()<<"Iteration "<<iteration;

        std::random_device rnd;
        std::mt19937 eng(rnd());
        std::uniform_real_distribution<> diamondValue((-1 * (range/2.0f)), range/2.0f);
        std::uniform_real_distribution<> squareValue((-1 * (range/3.0f)), range/5.0f);

        float h1 = heightMap[0];
        float h2 = heightMap[1];
        float h3 = heightMap[2];
        float h4 = heightMap[3];

        heightMap.insert(heightMap.begin() + 2, (h1 + h2 + h3 + h4)/4.0f + diamondValue(eng));

        heightMap.insert(heightMap.begin() + 1, (h1 + h2)/2.0f + squareValue(eng));
        heightMap.insert(heightMap.begin() + 3, (h4 + h3)/2.0f + squareValue(eng));
        heightMap.insert(heightMap.begin() + 5, (h3 + h1)/2.0f + squareValue(eng));
        heightMap.insert(heightMap.begin() + 7, (h4 + h2)/2.0f + squareValue(eng));

        std::vector<float> heights1{heightMap[0], heightMap[2], heightMap[4], heightMap[5]};

        heights1 = d3(heights1, range, iteration + 1, maxIterations);

        if(heights1.size() != 4)
        {
            heightMap[0] = heights1[0];
            heightMap[1] = heights1[2];
            heightMap[3] = heights1[6];
            heightMap[4] = heights1[8];

            //Add diamond
            heightMap.insert(heightMap.begin() + 3, heights1[4]);

            //Add square
            heightMap.insert(heightMap.begin() + 1, heights1[1]);
            heightMap.insert(heightMap.begin() + 4, heights1[3]);
            heightMap.insert(heightMap.begin() + 6, heights1[5]);
            heightMap.insert(heightMap.begin() + 8, heights1[7]);

            std::vector<float> heights2{heightMap[2], heightMap[3], heightMap[9], heightMap[10]};

            heights2 = d3(heights2, range, iteration + 1, maxIterations);

            heightMap[2] = heights2[0];
            heightMap[3] = heights2[2];
            heightMap[9] = heights2[6];
            heightMap[10] = heights2[8];

            heightMap.insert(heightMap.begin() + 7, heights2[4]);

            heightMap.insert(heightMap.begin() + 3, heights2[1]);
            heightMap[7] = heights2[3];
            heightMap.insert(heightMap.begin() + 9, heights2[5]);
            heightMap.insert(heightMap.begin() + 13, heights2[7]);

            std::vector<float> heights3{heightMap[10], heightMap[12], heightMap[15], heightMap[16]};

            heights3 = d3(heights3, range, iteration + 1, maxIterations);

            heightMap[10] = heights3[0];
            heightMap[12] = heights3[2];
            heightMap[15] = heights3[6];
            heightMap[16] = heights3[8];

            heightMap.insert(heightMap.begin() + 15, heights3[4]);

            heightMap[11] = heights3[1];
            heightMap.insert(heightMap.begin() +15, heights3[3]);
            heightMap.insert(heightMap.begin() + 17, heights3[5]);
            heightMap.insert(heightMap.begin() + 19, heights3[7]);

            std::vector<float> heights4{heightMap[12], heightMap[14], heightMap[20], heightMap[21]};

            heights4 = d3(heights4, range, iteration + 1, maxIterations);

            heightMap[12] = heights4[0];
            heightMap[14] = heights4[2];
            heightMap[20] = heights4[6];
            heightMap[21] = heights4[8];

            heightMap.insert(heightMap.begin() + 18, heights4[4]);

            heightMap[13] = heights4[1];
            heightMap[17] = heights4[3];
            heightMap.insert(heightMap.begin() + 19, heights4[5]);
            heightMap.insert(heightMap.begin() + 23, heights4[7]);
        }

        return heightMap;
    }
}
