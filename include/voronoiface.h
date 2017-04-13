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

    VoronoiFace(std::vector<uint> _indices);

    ~VoronoiFace();
    VoronoiFace(const VoronoiFace & _toCopy);

    void loadVerts(std::vector<VoronoiEdge*> &_edges);

    inline int getEdgeCount() const          { return m_indices.size(); }
    inline uint getEdgeID(int index) const { return m_indices[index]; }

    //------------------------------------------------
    void operator = (const VoronoiFace &_toCopy);

    void updateVerts();
    void updateEdge(int index, VoronoiEdge* _e1, VoronoiEdge* _e2);

    void passVBOToShader(QOpenGLShaderProgram &_pgm);
    void draw();

    void checkUsable();

    void print();

    void adjustHeight(int index, float newHeight);

    inline void addEdge(VoronoiEdge* _edge) { m_edges.push_back(_edge); }
//    inline void addNeighbour(VoronoiFace &_face) { m_neighbours.push_back(_face); }

    inline std::vector<QVector3D> getEdges() const { return m_edgeVerts; }
    inline QVector3D getVertex(int index) const { return m_edgeVerts[index]; }
//    inline int getNumEdges() const  { return m_edgeVerts.size(); }
    inline float getNumVerts() const { return m_edgeVerts.size() / 2.0f; }

    QVector3D getMiddle();
    QVector3D getWeightedMiddle(int vert, float weight);
    QVector3D getWeightedMiddle2(int vert, float weight);

    std::vector<VoronoiEdge*> getEdges() { return m_edges; }
    inline VoronoiEdge* getEdge(int index) const { return m_edges[index]; }
    inline int getNumEdges() const {return m_edges.size(); }

    bool m_isUsable;

private:
    std::vector<uint> m_indices;
    std::vector<VoronoiEdge*> m_edges;
    std::vector<QVector3D> m_edgeVerts;
    QVector3D m_midPoint;
    bool m_midPointIsCalculated;

    QOpenGLVertexArrayObject m_vao;
    QOpenGLBuffer m_verts;

//    std::vector<VoronoiFace*> m_neighbours;

};

#endif // VORONOIFACE_H
