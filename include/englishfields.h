#ifndef ENGLISHFIELDS_H
#define ENGLISHFIELDS_H

//INCLUDES FOR CROPPED VORONOI FUNCTIONS
//      (See makeVoronoiDiagram() )
#include <CroppedVoronoi/hds_type.h>
#include <CroppedVoronoi/voronoi_cropping.h>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/point_generators_2.h>

typedef CGAL::Simple_cartesian< CGAL::Lazy_exact_nt< CGAL::Gmpq > > Exact_kernel;
typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef CGAL::HalfedgeDS_default< K, HDS_Item_extra > HDS;
//INCLUDES AND TYPEDEFS END

#include <iterator>

#include <QVector3D>

#include <vector>
#include <memory>
#include <math.h>
#include <time.h>
#include <unordered_map>

#include "voronoiface.h"
#include "perlinnoise.h"

struct EdgeToUpdate
{
    EdgeToUpdate(VoronoiEdge* _old,
                          VoronoiEdge* _n1,
                          VoronoiEdge* _n2)
    {
        oldEdge = _old;
        newEdge_1 = _n1;
        newEdge_2 = _n2;
    }

    VoronoiEdge* oldEdge;
    VoronoiEdge* newEdge_1;
    VoronoiEdge* newEdge_2;
};

struct Intersection
{
    Intersection(float _length,
                        VoronoiEdge* _edge,
                        QVector3D _point)
    {
        length = _length;
        edge = _edge;
        point = _point;
    }

    float length;
    VoronoiEdge* edge;
    QVector3D point;
};

struct SortIntersection
{
    bool operator () (Intersection a, Intersection b)
    {
        return(a.length < b.length);
    }
};

struct SortAnglePair
{
    bool operator () (const std::pair<float, uint> &a, const std::pair<float, uint> &b)
    {
        return(a.first < b.first);
    }
};

class EnglishFields
{
public:
    EnglishFields();
    EnglishFields(double _width, bool _hasSeed, int _seed, bool _hasPos, int _numPoints, QVector3D _farmPos);
    EnglishFields(double _width, std::vector<QVector3D> _sites);

    ~EnglishFields();

    void exportFields(std::string _exportPath);

    void operator = (EnglishFields &toCopy);

    void makeVoronoiDiagram(int _seed);

    void subdivideEdge(QVector3D _start, QVector3D _end, std::vector< std::pair< QVector3D, QVector3D > > & edgeLis);
    void subdivideEdge(VoronoiEdge* edge, std::vector<VoronoiEdge *> &_edges);

    inline std::vector<VoronoiFace> getRegions() const { return m_regions; }

    inline std::vector<VoronoiEdge*> getEdges() const{ return m_allEdges; }
    inline VoronoiEdge* getEdge(int ID) const { return m_allEdges[ID]; }

    inline std::vector<QVector3D* > getVerts() const { return m_allVerts; }
    inline QVector3D* getVert(int ID) const { return m_allVerts[ID]; }

    void createWalls(QOpenGLShaderProgram &_pgm);
    void drawWalls(QOpenGLShaderProgram &_pgm);

    inline std::vector<QVector3D> getTreePositions() const { return m_treePositions; }

    QVector3D getFarmPosition() { return m_regions[m_farmRegion].getMiddle(); }

private:
    void subdivide();
    void editEdges();

    void displaceEdge(VoronoiFace &_face);
    void midPointEdge(VoronoiEdge *edge, int iteration, std::vector<uint> &_newIDs, bool _displace);

    void threeField(VoronoiFace &_face);

    void straightField(VoronoiFace &_face);

    void makeEdgesUsable();
    void subdivideEdge(uint ID);

    void updateEdge(uint _oldID, std::vector<uint> _newIDs);
    void removeEdge(uint ID);

    bool checkContains(uint ID, std::vector<uint> IDs);
    bool isBoundaryEdge(VoronoiEdge* _edge);

    bool m_hasFarm;
    QVector3D m_farmPos;
    int m_numPoints;

    uint findFarmRegion(QVector3D _pos);
    void farmFieldEdges();
    uint m_farmRegion;

    int m_maxDisplacementIterations;
    std::vector<uint> m_editedEdgeIDs;
    std::unordered_map<uint, std::pair< uint, uint> > m_newEdges;

    bool isVertex(QVector3D* _point);
    void getSegments(VoronoiFace &_face);

    std::vector<std::pair<uint, bool>> m_vertexInformation;

    std::vector<QVector3D> m_treePositions;
    std::vector<int> m_colours;


    //--------------------------------------------------------------------------------------//
//    void ridgeAndFurrow(VoronoiFace face, std::vector<VoronoiFace> &_facesToUpdate);

//    void threeField(VoronoiFace face, std::vector<VoronoiFace> &_facesToUpdate);

//    void makeOrganic(VoronoiFace &_face);

//    void subdivideRegions();

//    void makeFieldFeatures();

//    void midPointDisplace(VoronoiEdge* edge, int iteration, std::vector<VoronoiEdge*> &_edges);

    int clampIndex(int index, int numVertices);

    int edgeExists(VoronoiEdge* _edge);
    int edgeExists(VoronoiEdge *_edge, std::vector<VoronoiEdge*> _edges);
    int vertExists(QVector3D* _vert);

    void updateFaces(VoronoiEdge* _prev, VoronoiEdge* _e1, VoronoiEdge* _e2);

    double m_width;

    std::vector<QVector3D> m_sites;

    std::vector<VoronoiFace> m_regions;

    std::vector<VoronoiEdge*> m_allEdges;
    std::vector<QVector3D*> m_allVerts;

    std::vector<EdgeToUpdate> m_updaterEdges;
};

#endif // ENGLISHFIELDS_H
