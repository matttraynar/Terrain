#ifndef VORONOIFACE_H
#define VORONOIFACE_H

#include <QVector3D>

#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>

#include "voronoiedge.h"

class VoronoiFace
{
public:
    VoronoiFace(std::vector<QVector3D> _edgeVerts);
    VoronoiFace(std::vector<VoronoiEdge *> _edgeList);
    ~VoronoiFace();
    VoronoiFace(const VoronoiFace & _toCopy);

    void operator = (const VoronoiFace &_toCopy);

    void updateVerts();
    void passVBOToShader(QOpenGLShaderProgram &_pgm);
    void draw();

    void print();

    void adjustHeight(int index, float newHeight);

    inline void addEdge(VoronoiEdge* _edge) { m_edges.push_back(_edge); }
//    inline void addNeighbour(VoronoiFace &_face) { m_neighbours.push_back(_face); }

    inline std::vector<QVector3D> getEdges() const { return m_edgeVerts; }
    inline QVector3D getVertex(int index) const { return m_edgeVerts[index]; }
    inline int getNumEdges() const  { return m_edgeVerts.size(); }
    inline float getNumVerts() const { return m_edgeVerts.size() / 2.0f; }

    QVector3D getMiddle();
    QVector3D getWeightedMiddle(int vert, float weight);

    std::vector<VoronoiEdge*> m_edges;

private:

    std::vector<QVector3D> m_edgeVerts;
    QVector3D m_midPoint;
    bool m_midPointIsCalculated;

    QOpenGLVertexArrayObject m_vao;
    QOpenGLBuffer m_verts;

//    std::vector<VoronoiFace*> m_neighbours;

};

#endif // VORONOIFACE_H