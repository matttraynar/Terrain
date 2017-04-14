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

class EnglishFields
{
public:
    EnglishFields();
    EnglishFields(double _width);
    EnglishFields(double _width, std::vector<QVector3D> _sites);

    ~EnglishFields();

    void operator = (EnglishFields &toCopy);

    void makeVoronoiDiagram(int _seed);

    void subdivideEdge(QVector3D _start, QVector3D _end, std::vector< std::pair< QVector3D, QVector3D > > & edgeLis);
    void subdivideEdge(VoronoiEdge* edge, std::vector<VoronoiEdge *> &_edges);

    inline std::vector<VoronoiFace> getRegions() const { return m_regions; }


private:
    void subdivide();
    void editEdges();
    void displaceEdge(VoronoiFace &_face);
    void midPointEdge(VoronoiEdge *edge, int iteration, std::vector<uint> &_newIDs);
    void updateEdge(uint _oldID, std::vector<uint> _newIDs);
    void removeEdge(uint ID);

    bool checkContains(uint ID, std::vector<uint> IDs);
    bool isBoundaryEdge(VoronoiEdge* _edge);

    int m_maxDisplacementIterations;
    std::vector<uint> m_editedEdgeIDs;



    //--------------------------------------------------------------------------------------//
    void ridgeAndFurrow(VoronoiFace face, std::vector<VoronoiFace> &_facesToUpdate);

    void threeField(VoronoiFace face, std::vector<VoronoiFace> &_facesToUpdate);

    void makeOrganic(VoronoiFace &_face);

    void subdivideRegions();

    void makeFieldFeatures();

    void midPointDisplace(VoronoiEdge* edge, int iteration, std::vector<VoronoiEdge*> &_edges);

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
