#ifndef VORONOIFACE_H
#define VORONOIFACE_H

#include <QVector3D>

#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>

#include <algorithm>

#include <CGAL/Polygon_2_algorithms.h>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef K::Point_2 CGALPoint;

#include "voronoiedge.h"
#include "exportscene.h"

enum positionCase
{
    inside,
    edge,
    outside
};

class VoronoiFace
{
public:
    VoronoiFace(std::vector<QVector3D> _edgeVerts);
    VoronoiFace(std::vector<VoronoiEdge *> _edgeList);

    VoronoiFace(std::vector<uint> _indices);

    ~VoronoiFace();
    VoronoiFace(const VoronoiFace & _toCopy);

    void loadVerts(std::vector<VoronoiEdge*> &_edges);
    void removeDuplicates();

    inline int getEdgeCount() const                                    { return m_edgeCount; }
    inline int getOriginalEdgeCount() const                         { return m_originalEdges.size(); }
    inline uint getEdgeID(int index) const                           { if(index < m_indices.size()) { return m_indices[index];}
                                                                                      else { return 10000000; }}
    inline VoronoiEdge* getOriginalEdge(int index) const     { return m_originalEdges[index]; }

    inline void updateEdgeCount() { m_edgeCount = m_indices.size(); }

    bool usesEdge(uint ID);
    void replaceEdge(uint ID, std::vector<uint> newIDs);
    void removeEdge(uint ID);

    void storeOriginalEdges(std::vector<VoronoiEdge*> &_edges);

    std::vector<uint> getEdgeIDsInRange(uint start, uint end);
    uint getNextEdge(uint index);
    uint getNextUnnconnectedEdge(uint index, bool up);
    bool m_reversedEdge;

    void organiseEdgeIDs();

    positionCase containsPoint(QVector3D _pos);

    int m_edgeCount;

    //------------------------------------------------
    void operator = (const VoronoiFace &_toCopy);

    void updateVerts();
    void updateEdge(int index, VoronoiEdge* _e1, VoronoiEdge* _e2);

    void passVBOToShader(QOpenGLShaderProgram &_pgm);
    void draw();

    void exportRegion(std::string filepath);

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

    void setSegments(std::vector< std::vector<uint> > _segments) { m_segmentIndices = _segments; }
    void makeSkips(std::vector<int> _skips) { m_skips = _skips; }

    std::vector<QVector3D> createTreePositions();

    bool m_isUsable;

private:
    std::vector<uint> m_indices;
    std::vector< std::vector<uint> > m_segmentIndices;
    std::vector<int> m_skips;
    std::vector<VoronoiEdge*> m_originalEdges;

    std::vector<VoronoiEdge*> m_edges;
    std::vector<QVector3D> m_edgeVerts;
    QVector3D m_midPoint;
    bool m_midPointIsCalculated;

    QOpenGLVertexArrayObject m_vao;
    QOpenGLBuffer m_verts;


//    std::vector<VoronoiFace*> m_neighbours;

};

#endif // VORONOIFACE_H
