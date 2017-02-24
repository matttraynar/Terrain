#ifndef VORONOITYPES_H
#define VORONOITYPES_H

#include <memory>

//Predeclare the event class
class VoronoiEvent;

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
};

//-------------------------------------- VORONOI EDGE -----------------------------//

class VoronoiEdge
{
public:
    VoronoiEdge(VoronoiPoint* _start, VoronoiPoint* _leftRegion, VoronoiPoint* _rightRegion);

    //Start and end of the edge
    std::shared_ptr<VoronoiPoint> m_start;
    std::shared_ptr<VoronoiPoint> m_end;

    //Direction vector
    std::shared_ptr<VoronoiPoint> m_dir;

    //Points which dictate the regions to the left
    //and right of the edge
    std::shared_ptr<VoronoiPoint> m_left;
    std::shared_ptr<VoronoiPoint> m_right;

    //Variables of the line equation of the edge:
    //      y = fx + g;
    double f;
    double g;

    std::shared_ptr<VoronoiEdge> m_neighbour;
};

//---------------------------------- VORONOI PARABOLA -------------------------//

class VoronoiParabola
{
public:
    VoronoiParabola();
    VoronoiParabola(VoronoiPoint* _site);

    inline void setLeft(VoronoiParabola* _left) {m_left.reset(_left); _left->m_parent.reset(this);}
    inline void setRight(VoronoiParabola* _right) {m_left.reset(_right); _right->m_parent.reset(this);}

    inline std::shared_ptr<VoronoiParabola> left() const {return m_left;}
    inline std::shared_ptr<VoronoiParabola> right() const {return m_right;}

    //Returns closest left/right leaf
    static std::shared_ptr<VoronoiParabola> getLeft(std::shared_ptr<VoronoiParabola> _p);
    static std::shared_ptr<VoronoiParabola> getRight(std::shared_ptr<VoronoiParabola> _p);

    //Returns closest left/right parent
    static std::shared_ptr<VoronoiParabola> getLeftParent(std::shared_ptr<VoronoiParabola> _p);
    static std::shared_ptr<VoronoiParabola> getRightParent(std::shared_ptr<VoronoiParabola> _p);

    //Returns closest leaf on the left/right of the current parabola
    static std::shared_ptr<VoronoiParabola> getLeftChild(std::shared_ptr<VoronoiParabola> _p);
    static std::shared_ptr<VoronoiParabola> getRightChild(std::shared_ptr<VoronoiParabola> _p);

    bool m_isLeaf;
    std::shared_ptr<VoronoiPoint> m_site;
    std::shared_ptr<VoronoiEdge> m_edge;
    std::shared_ptr<VoronoiEvent> m_circleEvent;
    std::shared_ptr<VoronoiParabola> m_parent;

private:
    std::shared_ptr<VoronoiParabola> m_left;
    std::shared_ptr<VoronoiParabola> m_right;
};

//------------------------------------- VORONOI EVENT ----------------------------//

class VoronoiEvent
{
public:
    inline VoronoiEvent(VoronoiPoint* _p, bool _isSiteEvent)
    {
        m_point.reset(_p);
        m_y = m_point->y;
        m_isSiteEvent = _isSiteEvent;
        m_arc = 0;
    }

    inline ~VoronoiEvent(){ };

    struct CompareEvent : public std::binary_function<VoronoiEvent*, VoronoiEvent*, bool>
    {
        inline bool operator() (const std::shared_ptr<VoronoiEvent> _left, const std::shared_ptr<VoronoiEvent> _right) const
        {
            return (_left->m_y < _right->m_y);
        }
    };

    std::shared_ptr<VoronoiPoint> m_point;
    double m_y;
    bool m_isSiteEvent;
    std::shared_ptr<VoronoiParabola> m_arc;
};

#endif // VORONOITYPES_H
