#ifndef VORONOI_H
#define VORONOI_H

#include "voronoitypes.h"

#include <vector>
#include <set>
#include <queue>

typedef std::shared_ptr<VoronoiPoint> sPoint;
typedef std::shared_ptr<VoronoiEdge> sEdge;
typedef std::shared_ptr<VoronoiParabola> sParab;
typedef std::shared_ptr<VoronoiEvent> sEvent;

class Voronoi
{
public:
    Voronoi();

    std::shared_ptr<std::vector<sEdge>> makeVoronoiEdges(std::shared_ptr< std::vector<sPoint> > _verts, int _width, int _height);

private:
    void insertParabola(VoronoiPoint* _p);
    void removeParabola(sEvent _e);

    double getXOfEdge(sParab _par, double _y);
    sParab getParabFromX(double _x);

    double getY(sPoint _p, double _x);

    void finishEdge(sParab _par);
    void checkCircleEvent(sParab _par);

    sPoint getEdgeIntersection(sEdge _a, sEdge _b);

    std::shared_ptr< std::vector< sPoint > > m_sites;
    std::shared_ptr< std::vector< sEdge > > m_edges;

    float m_width;
    float m_height;

    sParab m_root;
    double m_sweepPosition;

    std::set<sEvent> m_deleted;
    std::vector<sPoint> m_voronoiPoints;
    std::priority_queue< sEvent, std::vector<sEvent>, VoronoiEvent::CompareEvent> m_queue;
};

#endif // VORONOI_H
