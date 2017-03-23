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

    void adjustHeight(int index, float newHeight);

    std::vector<QVector3D> getEdges() const { return m_edgeVerts; }
    QVector3D getVertex(int index) const { return m_edgeVerts[index]; }
    QVector3D getMiddle();
    QVector3D getWeightedMiddle(int vert, float weight);

    int getNumEdges() const  { return m_edgeVerts.size(); }
    float getNumVerts() const { return m_edgeVerts.size() / 2.0f; }

private:
    std::vector<QVector3D> m_edgeVerts;
    QVector3D m_midPoint;
    bool m_midPointIsCalculated;

    QOpenGLVertexArrayObject m_vao;
    QOpenGLBuffer m_verts;

};

#endif // VORONOIFACE_H
