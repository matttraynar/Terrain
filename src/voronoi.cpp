#include "voronoi.h"

#include <QDebug>

Voronoi::Voronoi()
{
    m_edges = 0;
}

std::shared_ptr<std::vector<sEdge>> Voronoi::makeVoronoiEdges(std::shared_ptr<std::vector<sPoint> > _verts, int _width, int _height)
{
    m_sites.reset(_verts.get());
    m_width = (float)_width;
    m_height = (float)_height;
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

    for(int i = 0; i < m_sites->size(); ++i)
    {
        sEvent newEvent(new VoronoiEvent((*m_sites)[i], true));
        m_queue.push(newEvent);
        m_queue.top()->print();
        qInfo()<<m_queue.size();
    }

    int count = 0;

    sEvent currentEvent;

//    VoronoiEvent* currentEvent;




    while(!m_queue.empty())
    {
        qInfo()<<"                    ";
        count++;

//        VoronoiEvent  tmp = *m_queue.top().get();

        currentEvent = m_queue.top();
//        sEvent currentEvent(&tmp);

        currentEvent->print();

        qInfo()<<m_queue.size();

//        currentEvent = m_queue.top().get();

        m_queue.pop();

//        sEvent currentEvent(m_queue.top().get());

        m_sweepPosition = currentEvent->m_y;

         if(m_deleted.find(currentEvent) != m_deleted.end())
        {
            m_deleted.erase(currentEvent);
            continue;
        }
        else if(currentEvent->m_isSiteEvent)
        {
             if(currentEvent->m_point.get() == NULL)
             {
                 qInfo()<<"NULL";
             }

            insertParabola(currentEvent->m_point);
        }
        else
        {
            removeParabola(sEvent(currentEvent));
        }
    }

    finishEdge(m_root);

    for(int i = 0; i < m_edges->size(); ++i)
    {
        if(m_edges->data()[i].get()->m_neighbour)
        {
            m_edges->data()[i].get()->m_start = m_edges->data()[i].get()->m_neighbour->m_end;
        }
    }

    return m_edges;
}

void Voronoi::insertParabola(sPoint _p)
{
    if(!m_root)
    {
//        m_root.reset(new VoronoiParabola(_p.get()));
        m_root.reset(new VoronoiParabola(_p));
        return;
    }

    if(m_root->m_isLeaf && (m_root->m_site->y - _p->y) < 1)
    {
        sPoint focus = m_root->m_site;

        m_root->m_isLeaf = false;
//        m_root->setLeft(new VoronoiParabola(focus.get()));
//        m_root->setRight(new VoronoiParabola(_p.get()));

        m_root->setLeft(sParab(new VoronoiParabola(focus)));
        m_root->setRight(sParab(new VoronoiParabola(_p)));

        sPoint voronoiPoint(new VoronoiPoint((_p->x + focus->x)/2, m_height));

        m_voronoiPoints.push_back(voronoiPoint);

        if(_p->x > focus->x)
        {
//            m_root->m_edge.reset(new VoronoiEdge(voronoiPoint.get(), _p.get(), focus.get()));
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

        sPoint start(new VoronoiPoint(_p->x, getY(newParabola->m_site, _p->x)));
        m_voronoiPoints.push_back(start);

//        sEdge eLeft(new VoronoiEdge(start.get(), newParabola->m_site.get(), _p.get()));
//        sEdge eRight(new VoronoiEdge(start.get(), _p.get(), newParabola->m_site.get()));

        sEdge eLeft(new VoronoiEdge(start, newParabola->m_site, _p));
        sEdge eRight(new VoronoiEdge(start, _p, newParabola->m_site));

        eLeft->m_neighbour.reset(eRight.get());
        m_edges->push_back(eLeft);

        newParabola->m_edge.reset(eRight.get());
        newParabola->m_isLeaf = false;

//        sParab p0(new VoronoiParabola(newParabola->m_site.get()));
//        sParab p1(new VoronoiParabola(_p.get()));
//        sParab p2(new VoronoiParabola(newParabola->m_site.get()));

        sParab p0(new VoronoiParabola(newParabola->m_site));
        sParab p1(new VoronoiParabola(_p));
        sParab p2(new VoronoiParabola(newParabola->m_site));

//        newParabola->setRight(p2.get());
//        newParabola->setLeft(new VoronoiParabola());

        newParabola->setRight(p2);
        newParabola->setLeft(sParab(new VoronoiParabola()));
        newParabola->left()->m_edge = eLeft;

//        newParabola->left()->setLeft(p0.get());
//        newParabola->left()->setRight(p1.get());

        newParabola->left()->setLeft(p0);
        newParabola->left()->setRight(p1);

        checkCircleEvent(p0);
        checkCircleEvent(p2);
    }
}

void Voronoi::removeParabola(sEvent _e)
{
    sParab p1 = _e->m_arc;

    sParab xLeft(VoronoiParabola::getLeftParent(p1));
    sParab xRight(VoronoiParabola::getRightParent(p1));

    sParab p0(VoronoiParabola::getLeftChild(xLeft));
    sParab p2(VoronoiParabola::getRightChild(xRight));

    if(p0 == p2)
    {
        qInfo()<<"Missing right and left parabolas";
    }

    if(p0->m_circleEvent)
    {
        m_deleted.insert(p0->m_circleEvent);
        p0->m_circleEvent = 0;
    }

    if(p2->m_circleEvent)
    {
        m_deleted.insert(p2->m_circleEvent);
        p2->m_circleEvent = 0;
    }

    sPoint p(new VoronoiPoint(_e->m_point->x, getY(p1->m_site, _e->m_point->x)));
    m_voronoiPoints.push_back(p);

    xLeft->m_edge->m_end.reset(p.get());
    xRight->m_edge->m_end.reset(p.get());

    sParab upperParabola;
    sParab currentParabola = p1;

    while(currentParabola != m_root)
    {
        currentParabola = currentParabola->m_parent;

        if(currentParabola == xLeft)
        {
            upperParabola = xLeft;
        }

        if(currentParabola == xRight)
        {
            upperParabola = xRight;
        }
    }

//    upperParabola->m_edge.reset(new VoronoiEdge(p.get(), p0->m_site.get(), p2->m_site.get()));
    upperParabola->m_edge.reset(new VoronoiEdge(p, p0->m_site, p2->m_site));

    m_edges->push_back(upperParabola->m_edge);

    sParab grandParent = p1->m_parent->m_parent;

    if(p1->m_parent->left() == p1)
    {
        if(grandParent->left() == p1->m_parent)
        {
//            grandParent->setLeft(p1->m_parent->right().get());
            grandParent->setLeft(p1->m_parent->right());
        }

        if(grandParent->right() == p1->m_parent)
        {
//            grandParent->setRight(p1->m_parent->right().get());
            grandParent->setRight(p1->m_parent->right());
        }
    }
    else
    {
        if(grandParent->left() == p1->m_parent)
        {
//            grandParent->setLeft(p1->m_parent->left().get());
            grandParent->setLeft(p1->m_parent->left());
        }

        if(grandParent->right() == p1->m_parent)
        {
//            grandParent->setRight(p1->m_parent->left().get());
            grandParent->setRight(p1->m_parent->left());
        }
    }

    checkCircleEvent(p0);
    checkCircleEvent(p2);
}

double Voronoi::getXOfEdge(sParab _par, double _y)
{
    //Need to find the x coordinate of the point on parabola _par
    //which has the y coordinate _y

    //First get the parabolas to the left and right of the current one
    //This function is only called when a new parabola is added.
    //Because of that the new parabola is basically a line
    //perpendicular to the sweep line (so left and right are just the
    //parabolas either side of the perpendicular line)
    sParab left(VoronoiParabola::getLeftChild(_par));
    sParab right(VoronoiParabola::getRightChild(_par));

    //Get the points which dictate the two regions
    sPoint pLeft = left->m_site;
    sPoint pRight = right->m_site;

    //Now we have two simultaneous equations for each site
    double dp = 2.0 * (pLeft->y - _y);
    double a1 = 1.0 / dp;
    double b1 = -2.0 * pLeft->x / dp;
    double c1 = _y + dp / 4 + pLeft->x * pLeft->x / dp;

    dp = 2.0 * (pRight->y - _y);

    double a2 = 1.0 / dp;
    double b2 = -2.0 * pRight->x / dp;
    double c2 = m_sweepPosition + dp / 4 + pRight->x * pRight->x / dp;

    //Substitue into the  coefficients of the quadratic equation
    double a = a1 - a2;
    double b = b1 - b2;
    double c = c1 - c2;

    //Now use the quadratic formula to solve
    double discriminant = (b * b) - (4 * a * c);

    double x1 = (-b + sqrt(discriminant)) / (2 * a);
    double x2 = (-b - sqrt(discriminant)) / (2 * a);

    double root;

    //If the left point is lower the right use the larger root,
    //otherwise use the smaller one
    (pLeft->y < pRight->y) ? root = std::max(x1, x2) : root = std::min(x1, x2);

    return root;
}

sParab Voronoi::getParabFromX(double _x)
{
    sParab par = m_root;

    double x = 0.0;

    while(!par->m_isLeaf)
    {
        x = getXOfEdge(par, m_sweepPosition);

        (x > _x) ? par.reset(par->left().get()) : par.reset(par->right().get());
    }

    return par;
}

double Voronoi::getY(sPoint _p, double _x)
{
    double dp = 2 * (_p->y - m_sweepPosition);
    double a1 = 1.0 / dp;
    double b1 = -2.0 * _p->x / dp;
    double c1 = m_sweepPosition + dp / 4 + _p->x * _p->x / dp;

    //Return y = ax^2 + bx + c
    return ((a1 * _x * _x) + (b1 * _x) + c1);
}

void Voronoi::finishEdge(sParab _par)
{
    if(_par->m_isLeaf)
    {
        return;
    }

    double mx;

    if(_par->m_edge->m_dir->x > 0.0)
    {
        double yValue = _par->m_edge->m_start->x;

        mx = std::max(yValue + 10.0, (double)m_width);
    }
    else
    {
        mx = std::min(0.0, _par->m_edge->m_start->x - 10);
    }

    sPoint end(new VoronoiPoint(mx, mx * _par->m_edge->f + _par->m_edge->g));
    _par->m_edge->m_end = end;

    finishEdge(_par->left());
    finishEdge(_par->right());
}

void Voronoi::checkCircleEvent(sParab _par)
{
    VoronoiParabola* pLeft = VoronoiParabola::getLeftParent(_par).get();
    VoronoiParabola* pRight = VoronoiParabola::getRightParent(_par).get();

    VoronoiParabola* a = VoronoiParabola::getLeftChild(sParab(pLeft)).get();
    VoronoiParabola* b = VoronoiParabola::getRightChild(sParab(pRight)).get();

    if(!a || !b || a->m_site == b->m_site)
    {
        return;
    }

    sPoint s = 0;
    s = getEdgeIntersection(pLeft->m_edge, pRight->m_edge);
    if(s == 0)
    {
        return;
    }

    double dx = a->m_site->x - s->x;
    double dy = a->m_site->y - s->y;

    double d = sqrt((dx * dx) + (dy * dy));

    if((s->y - d) >= m_sweepPosition)
    {
        return;
    }

    sEvent newEvent(new VoronoiEvent(sPoint(new VoronoiPoint(s->x, s->y - d)), false));
    m_voronoiPoints.push_back(newEvent->m_point);

    _par->m_circleEvent = newEvent;
    newEvent->m_arc = _par;

    m_queue.push(newEvent);
}

sPoint Voronoi::getEdgeIntersection(sEdge _a, sEdge _b)
{
    double x = (_b->g - _a->g) / (_a->f - _b->f);
    double y = _a->f * x + _a->g;

    if((x - _a->m_start->x) / _a->m_dir->x < 0)
    {
        return 0;
    }

    if((y - _a->m_start->x) / _a->m_dir->y < 0)
    {
        return 0;
    }

    if((x - _b->m_start->x) / _b->m_dir->x < 0)
    {
        return 0;
    }

    if((y - _b->m_start->x) / _b->m_dir->y < 0)
    {
        return 0;
    }

    sPoint p(new VoronoiPoint(x, y));
    m_voronoiPoints.push_back(p);

    return p;
}
