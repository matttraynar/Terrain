#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QGLWidget>

#include <QTimerEvent>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QKeyEvent>

#include <QOpenGLShaderProgram>

#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>

#include <QOpenGLTexture>

#include <vector>
#include <math.h>
#include <random>

#include "perlinnoise.h"
#include "englishfields.h"
#include "mesh.h"
#include "exportscene.h"

#define PI 3.14159265

struct SortVector
{
    bool operator() (const QVector3D &a, const QVector3D &b)
    {
        return(a.y() < b.y());
    }
};

class GLWidget : public QGLWidget
{
    Q_OBJECT

public:
    GLWidget(std::string _filepath = "", QWidget* parent = 0);
    ~GLWidget();

    bool m_ortho;
    bool doOnce;

    bool shading;

    void changeOrtho();

protected:
     //-------------------------------------------------------
    //Drawing and painting functions
    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();

    void timerEvent(QTimerEvent* e);

    //-------------------------------------------------------
    //Camera movement functions
    void wheelEvent(QWheelEvent* e);
    void mousePressEvent(QMouseEvent* e);
    void mouseMoveEvent(QMouseEvent* e);
    void mouseReleaseEvent(QMouseEvent* e);

    static void qNormalizeAngle(int &angle);

    void setXTranslation(int x);
    void setYTranslation(int y);
    void setZTranslation(int z);

    void setXRotation(int angle);
    void setYRotation(int angle);
    void setZRotation(int angle);
     //-------------------------------------------------------

private:
    std::string m_workingPath;

     //-------------------------------------------------------
    //Shader functions
    bool prepareShaderProgram(const QString& vertexShaderPath, const QString& fragmentShaderPath);
    bool prepareShaderProgram(const std::string &vertexShaderPath, const std::string &fragmentShaderPath);
    void loadMatricesToShader(QVector3D position);

    QOpenGLShaderProgram m_pgm;

    EnglishFields m_fieldGenerator;

    std::vector<std::shared_ptr<Mesh>> m_treeMeshes;
    std::vector<int> m_treeMeshesToUse;

     //-------------------------------------------------------
    //Terrain generation functions
    void generateHeightMap(int iterations, float roughness);
    void drawTerrain();

    void diamond(int x, int y, int sideLength, float scale);
    void square(int x, int y, int sideLength, float scale);

    float getHeight(int x, int y);
    QVector3D getNormal(int x, int y);

    void prepareTerrain();
    void prepareWater();
    void prepareTrees();

    QOpenGLTexture *addNewTexture(QString &filename);
    QOpenGLTexture *addNewTexture(std::string &filename);

    std::vector<std::shared_ptr<Mesh>> m_farmMeshes;
    QVector3D farmPosition;
    int m_divisions;
    float m_waterLevel;

    float m_terrainMin;
    float m_terrainMax;

    QOpenGLVertexArrayObject vao_fields;
    QOpenGLBuffer vbo_fields;

    QOpenGLVertexArrayObject vao_terrain;
    QOpenGLBuffer vbo_terrain;
    QOpenGLBuffer nbo_terrain;
    QOpenGLBuffer uvbo_terrain;

    std::vector<QVector3D> m_verts;
    std::vector<QVector3D> m_norms;
    std::vector<QVector2D> m_uvs;

    std::vector< std::vector<float> > m_heights;
    std::vector< std::vector<QVector3D> > m_normalMap;

    QOpenGLVertexArrayObject vao_water;
    QOpenGLBuffer vbo_water;
    QOpenGLBuffer nbo_water;

    std::vector<QVector3D> m_waterVerts;
    std::vector<QVector3D> m_waterNorms;

    QOpenGLVertexArrayObject vao_trees;
    QOpenGLBuffer vbo_trees;
    QOpenGLBuffer ibo_trees;

    std::vector<QVector3D> m_treeVerts;
    std::vector<uint> m_treeIndices;
    std::vector<QVector3D> m_treePositions;

    std::vector<QVector3D> m_sitePoints;

    std::vector<VoronoiFace> m_vRegions;

     //-------------------------------------------------------
    //Camera movement members
    QMatrix4x4 m_model;
    QMatrix4x4 m_view;
    QMatrix4x4 m_proj;
    QMatrix4x4 m_mvp;

    QVector3D m_cameraPos;
    QVector3D m_dir;

    float m_x;
    bool moveDown;

    int m_mouseDelta;
    int m_xRot;
    int m_yRot;
    int m_zRot;
    int m_xDis;
    int m_yDis;
    int m_zDis;

    QPoint m_lastPos;
     //-------------------------------------------------------

    bool m_wireframe;

    int index;
};

#endif // GLWIDGET_H
