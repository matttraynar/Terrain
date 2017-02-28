#ifndef VORONOITYPES_H
#define VORONOITYPES_H

#include <memory>

#include <QDebug>


//Predeclare the classes so we can typdef some pointers
class VoronoiPoint;
class VoronoiEdge;
class VoronoiParabola;
class VoronoiEvent;

//typedef std::shared_ptr<VoronoiPoint> sPoint;
//typedef std::shared_ptr<VoronoiEdge> sEdge;
//typedef std::shared_ptr<VoronoiParabola> sParab;
//typedef std::shared_ptr<VoronoiEvent> sEvent;

typedef VoronoiPoint* sPoint;
typedef VoronoiEdge* sEdge;
typedef VoronoiParabola* sParab;
typedef VoronoiEvent* sEvent;


//-------------------------------------- VORONOI POINT -----------------------------//

class VoronoiPoint
{
public:
    double x;
    double y;

    inline VoronoiPoint(double _x, double _y)
    {
        x = _x;
        y = _y;
    }

    void printPoint()
    {
        qInfo()<<"Point: ("<<x<<","<<y<<")";
    }
};

//-------------------------------------- VORONOI EDGE -----------------------------//

class VoronoiEdge
{
public:
//    VoronoiEdge(VoronoiPoint* _start, VoronoiPoint* _leftRegion, VoronoiPoint* _rightRegion);
    VoronoiEdge(sPoint _start, sPoint _leftRegion, sPoint _rightRegion);

    void printEdge()
    {
        qInfo()<<"Start: "<<m_start<<" End: "<<m_end;
    }

    //Start and end of the edge
    sPoint m_start;
    sPoint m_end;

    //Direction vector
    sPoint m_dir;

    //Points which dictate the regions to the left
    //and right of the edge
    sPoint m_left;
    sPoint m_right;

    //Variables of the line equation of the edge:
    //      y = fx + g;
    double f;
    double g;

    sEdge m_neighbour;
};

//---------------------------------- VORONOI PARABOLA -------------------------//

class VoronoiParabola
{
public:
    VoronoiParabola();
//    VoronoiParabola(VoronoiPoint* _site);
    VoronoiParabola(sPoint _site);

//    inline void setLeft(VoronoiParabola* _left) {m_left.reset(_left); _left->m_parent.reset(this);}
//    inline void setRight(VoronoiParabola* _right) {m_left.reset(_right); _right->m_parent.reset(this);}

    inline void setLeft(sParab _left) {m_left = _left; _left->m_parent = this;}
    inline void setRight(sParab _right) {m_right = _right; _right->m_parent = this;}

    inline sParab left() const {return m_left;}
    inline sParab right() const {return m_right;}

    //Returns closest left/right leaf
    static sParab getLeft(sParab _p);
    static sParab getRight(sParab _p);

    //Returns closest left/right parent
    static sParab getLeftParent(sParab _p);
    static sParab getRightParent(sParab _p);

    //Returns closest leaf on the left/right of the current parabola
    static sParab getLeftChild(sParab _p);
    static sParab getRightChild(sParab _p);

    bool m_isLeaf;
    sPoint m_site;
    sEdge m_edge;
    sEvent m_circleEvent;
    sParab m_parent;

private:
    sParab m_left;
    sParab m_right;
};

//------------------------------------- VORONOI EVENT ----------------------------//

class VoronoiEvent
{
public:
//    inline VoronoiEvent(VoronoiPoint* _p, bool _isSiteEvent)
    inline VoronoiEvent(sPoint _p, bool _isSiteEvent)
    {
        m_point = _p;
        m_y = m_point->y;
        m_isSiteEvent = _isSiteEvent;
        m_arc = 0;
    }

    inline ~VoronoiEvent(){ }

//    struct CompareEvent : public std::binary_function<VoronoiEvent*, VoronoiEvent*, bool>
    struct CompareEvent : public std::binary_function<sEvent, sEvent, bool>
    {
        inline bool operator() (const sEvent _left, const sEvent _right) const
        {
            return (_left->m_y < _right->m_y);
        }
    };

    inline void print()
    {
        qInfo()<<m_point->y;
    }

    sPoint m_point;
    double m_y;
    bool m_isSiteEvent;
    sParab m_arc;
};

#endif // VORONOITYPES_H
