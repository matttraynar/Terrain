#include "voronoi.h"

Voronoi::Voronoi()
{
    m_edges = 0;
}

std::vector<sEdge> Voronoi::makeVoronoiEdges(std::shared_ptr<std::vector<sPoint> > _verts, int _width, int _height)
{
    m_sites = _verts;
    m_width = _width;
    m_height = _height;
    m_root = 0;

    if(!m_edges)
    {
        m_edges.reset(new std::vector<sEdge>);
    }
    else
    {
        m_voronoiPoints.clear();
        m_edges->clear();
    }

    for(auto i = m_sites->begin(); i != m_sites->end(); ++i)
    {
        m_queue.push(new VoronoiEvent(*i, true));
    }

    sEvent currentEvent;

    while(!m_queue.empty())
    {
        currentEvent.reset(m_queue.top());
        m_queue.pop();

        m_sweepPosition = currentEvent->m_y;

        if(m_deleted.find(currentEvent) != m_deleted.end())
        {
            m_deleted.erase(currentEvent);
            continue;
        }

        if(currentEvent->m_isSiteEvent)
        {
            insertParabola(currentEvent->m_point);
        }
        else
        {
            removeParabola(currentEvent);
        }
    }

    finishEdge(m_root);

    for(auto i = m_edges->begin(); i != m_edges->end(); ++i)
    {
        if( (*i)->m_neighbour)
        {
            (*i)->m_start = (*i)->m_neighbour->m_end;
            delete (*i)->m_neighbour;
        }
    }

    return m_edges;
}

void Voronoi::insertParabola(sPoint _p)
{
    if(!m_root)
    {
        m_root.reset(new VoronoiParabola(_p));
        return;
    }

    if(m_root->m_isLeaf && (m_root->m_site->y - _p->y) < 1)
    {
        sPoint *focus = m_root->m_site;

        m_root->m_isLeaf = false;
        m_root->setLeft(new VoronoiParabola(focus));
        m_root->setRight(new VoronoiParabola(_p));

        sPoint voronoiPoint = new VoronoiPoint((_p->x + focus->x)/2, m_height);

        m_voronoiPoints.push_back(voronoiPoint);

        if(_p->x > focus->x)
        {
            m_root->m_edge.reset(new VoronoiEdge(voronoiPoint, _p, focus));
        }
        else
        {
            m_edges->push_back(m_root->m_edge);
        }
    }
    else
    {
        sParab newParabola = getParabFromX(_p->x);

        if(newParabola->m_circleEvent)
        {
            m_deleted.insert(newParabola->m_circleEvent);
            newParabola->m_circleEvent = 0;
        }

        sPoint start = new VoronoiPoint(_p->x, getY(newParabola->m_site, _p->x));
        m_voronoiPoints.push_back(start);

        sEdge eLeft = new VoronoiEdge(start, newParabola->m_site, _p);
        sEdge eRight = new VoronoiEdge(start, _p, newParabola->m_site);

        eLeft->m_neighbour.reset(eRight);
        m_edges->push_back(eLeft);

        newParabola->m_edge.reset(eRight);
        newParabola->m_isLeaf = false;

        sParab p0 = new VoronoiParabola(newParabola->m_site);
        sParab p1 = new VoronoiParabola(_p);
        sParab p2 = new VoronoiParabola(newParabola->m_site);

        newParabola->setRight(p2.get());
        newParabola->setLeft(new VoronoiParabola());
        newParabola->left()->m_edge = eLeft;

        newParabola->left()->setLeft(p0.get());
        newParabola->left()->setRight(p1.get());

        checkCircleEvent(p0);
        checkCircleEvent(p2);

    }

}
