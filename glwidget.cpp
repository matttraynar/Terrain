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

    prepareTerrain(5);

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

    m_pgm.setUniformValue("mCol",QVector4D(1.0f,1.0f,1.0f,0.0f));

    loadMatricesToShader();

    drawTerrain();

    vao_water.bind();
    m_pgm.setUniformValue("mCol",QVector4D(0.0f,0.0f,1.0f,1.0f));

    glDrawArrays(GL_QUADS, 0, (int)m_waterVerts.size());

    vao_water.release();

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

void GLWidget::loadMatricesToShader()
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
    float dimensions = pow(2, iterations) + 1;

    m_heights.resize(dimensions, std::vector<float>(dimensions, 0.0f));

    diamondSquare(0, 0, dimensions, dimensions, 3.0f, iterations);

    float start = -10.0f;

    m_norms.resize(dimensions * dimensions * 4, QVector3D(0.0f, 0.0f, 1.0f));

    for(uint i = 0; i < dimensions - 1; ++i)
    {
        for(uint j = 0; j < dimensions - 1; ++j)
        {


            QVector3D newVertex( (((float)i / (dimensions - 1) ) * 20.0f) + start,
                                            m_heights[i][j],
                                            (((float)j / (dimensions - 1) ) * 20.0f) + start);

            m_verts.push_back(newVertex);

            QVector3D newVertex2( ((((float)i + 1) / (dimensions - 1) ) * 20.0f) + start,
                                            m_heights[i + 1][j],
                                            (((float)j / (dimensions - 1) ) * 20.0f) + start);

            m_verts.push_back(newVertex2);

            QVector3D newVertex3( ((((float)i + 1) / (dimensions - 1) ) * 20.0f) + start,
                                            m_heights[i + 1][j + 1],
                                            ((((float)j + 1) / (dimensions - 1) ) * 20.0f) + start);

            m_verts.push_back(newVertex3);

            QVector3D newVertex4( (((float)i / (dimensions - 1) ) * 20.0f) + start,
                                            m_heights[i][j + 1],
                                            ((((float)j + 1) / (dimensions - 1) ) * 20.0f) + start);

            m_verts.push_back(newVertex4);
        }
    }

    int count = 0;

    for(int i = 0; i < dimensions - 1; ++i)
    {
        for(int j = 0; j < dimensions - 1; ++j)
        {
            float heightToLeft = 0.0f;
            float heightToRight = 0.0f;
            float heightToFront = 0.0f;
            float heightToBack = 0.0f;

            if(count < m_norms.size()){

                ((i - 1) > 0) ? heightToLeft = m_heights[i - 1][j] : heightToLeft = 0.0f;
                ((i + 1) < dimensions) ? heightToRight = m_heights[i + 1][j] : heightToRight = 0.0f;
                ((j + 1) < dimensions) ? heightToFront = m_heights[i][j + 1] : heightToFront = 0.0f;
                ((j - 1) > 0) ? heightToBack = m_heights[i][j - 1] : heightToBack = 0.0f;

                m_norms[count].setX(heightToLeft - heightToRight);
                m_norms[count].setY(heightToBack - heightToFront);
                m_norms[count].setZ(2.0f);

                m_norms[count].normalize();


                count++;

                if(count < m_norms.size())
                {
                    ((i + 1 - 1) > 0) ? heightToLeft = m_heights[i + 1 - 1][j] : heightToLeft = 0.0f;
                    ((i + 1 + 1) < dimensions) ? heightToRight = m_heights[i + 1 + 1][j] : heightToRight = 0.0f;
                    ((j + 1) < dimensions && (i + 1) < dimensions) ? heightToFront = m_heights[i + 1][j + 1] : heightToFront = 0.0f;
                    ((j - 1) > 0 && (i + 1) < dimensions) ? heightToBack = m_heights[i + 1][j - 1] : heightToBack = 0.0f;

                    m_norms[count].setX(heightToLeft - heightToRight);
                    m_norms[count].setY(heightToBack - heightToFront);
                    m_norms[count].setZ(2.0f);

                    m_norms[count].normalize();


                    count++;

                    if(count < m_norms.size())
                    {
                        ((i + 1 - 1) > 0 && (j + 1) < dimensions) ? heightToLeft = m_heights[i + 1 - 1][j + 1] : heightToLeft = 0.0f;
                        ((i + 1 + 1) < dimensions && (j + 1) < dimensions) ? heightToRight = m_heights[i + 1 + 1][j + 1] : heightToRight = 0.0f;
                        ((j + 1 + 1) < dimensions && (i + 1) < dimensions) ? heightToFront = m_heights[i + 1][j + 1 + 1] : heightToFront = 0.0f;
                        ((j + 1 - 1) > 0 && (i + 1) < dimensions) ? heightToBack = m_heights[i + 1][j - 1 + 1] : heightToBack = 0.0f;

                        m_norms[count].setX(heightToLeft - heightToRight);
                        m_norms[count].setY(heightToBack - heightToFront);
                        m_norms[count].setZ(2.0f);

                        m_norms[count].normalize();


                        count++;

                        if(count < m_norms.size())
                        {
                            ((i - 1) > 0 && (j + 1) < dimensions) ? heightToLeft = m_heights[i - 1][j + 1] : heightToLeft = 0.0f;
                            ((i + 1) < dimensions && (j + 1) < dimensions) ? heightToRight = m_heights[i + 1][j + 1] : heightToRight = 0.0f;
                            ((j + 1 + 1) < dimensions) ? heightToFront = m_heights[i][j + 1 + 1] : heightToFront = 0.0f;
                            ((j + 1 - 1) > 0) ? heightToBack = m_heights[i][j - 1 + 1] : heightToBack = 0.0f;

                            m_norms[count].setX(heightToLeft - heightToRight);
                            m_norms[count].setY(heightToBack - heightToFront);
                            m_norms[count].setZ(2.0f);

                            m_norms[count].normalize();


                            count++;
                        }
                    }
                }
            }
        }
    }

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
}

void GLWidget::drawTerrain()
{
    vao_terrain.bind();

    (m_wireframe) ?  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE) :  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

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
