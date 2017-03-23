#ifndef VORONOIFACE_H
#define VORONOIFACE_H

#include <QVector3D>

#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>

class VoronoiFace
{
public:
    VoronoiFace(std::vector<QVector3D> _edgeVerts);
    ~VoronoiFace();
    VoronoiFace(const VoronoiFace & _toCopy);

    void operator = (const VoronoiFace &_toCopy);


    void passVBOToShader(QOpenGLShaderProgram &_pgm);
    void draw();

    void print();

private:
    std::vector<QVector3D> m_edgeVerts;

    QOpenGLVertexArrayObject m_vao;
    QOpenGLBuffer m_verts;

};

#endif // VORONOIFACE_H
