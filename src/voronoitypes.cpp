#include "voronoitypes.h"

//-------------------------------------- VORONOI EDGE -----------------------------//
VoronoiEdge::VoronoiEdge(VoronoiPoint *_start, VoronoiPoint *_leftRegion, VoronoiPoint *_rightRegion)
{
    m_start.reset(_start);
    m_left.reset(_leftRegion);
    m_right.reset(_rightRegion);
}

//---------------------------------- VORONOI PARABOLA -------------------------//
VoronoiParabola::VoronoiParabola()
{
    m_site = 0;
    m_isLeaf = false;
    m_circleEvent = 0;
    m_edge = 0;
    m_parent = 0;
}

VoronoiParabola::VoronoiParabola(VoronoiPoint *_site)
{
    m_site.reset(_site);
    m_isLeaf = true;
    m_circleEvent = 0;
    m_edge = 0;
    m_parent = 0;
}

std::shared_ptr<VoronoiParabola> VoronoiParabola::getLeft(std::shared_ptr<VoronoiParabola> _p)
{
    return getLeftChild(getLeftParent(_p));
}

std::shared_ptr<VoronoiParabola> VoronoiParabola::getRight(std::shared_ptr<VoronoiParabola> _p)
{
    return getRightChild(getRightParent(_p));
}

std::shared_ptr<VoronoiParabola> VoronoiParabola::getLeftParent(std::shared_ptr<VoronoiParabola> _p)
{
    //Get the parent of the current parabola
    std::shared_ptr<VoronoiParabola> parent = _p->m_parent;

    //And store the current parabola
    std::shared_ptr<VoronoiParabola> prevP = _p;

    //Keep going left through our tree
    while(parent->left() == prevP)
    {
        //If the current parent branch has no parent we've
        //reached an edge
        if(!parent->m_parent) { return 0; }

        //Otherwise move "up" a level
        // - Previous parent becomes current parent
        // - New parent becomes parent of current parent
        prevP.reset(parent.get());
        parent.reset(parent->m_parent.get());
    }

    return parent;
}

std::shared_ptr<VoronoiParabola> VoronoiParabola::getRightParent(std::shared_ptr<VoronoiParabola> _p)
{
    //Same as with getLeftParent() only going right in
    //the while loop
    std::shared_ptr<VoronoiParabola> parent = _p->m_parent;
    std::shared_ptr<VoronoiParabola> prevP = _p;

    while(parent->right() == prevP)
    {
        if(!parent->m_parent) { return 0; }

        prevP.reset(parent.get());
        parent.reset(parent->m_parent.get());
    }

    return parent;
}

std::shared_ptr<VoronoiParabola> VoronoiParabola::getLeftChild(std::shared_ptr<VoronoiParabola> _p)
{
    //Get the parabola that is closest to the current one on the left.
    //This is done by looking at the parabola to the left and traversing
    //its children to the right until the closest parabola is found
    //E.G. L ------------------------ _p ----------------------- R
    //        x
    // Next Step
    //        L ------------|----------- _p ----------------------- R
    //                      x

    //        L -------------------|---- _p ----------------------- R
    //                             x
    //Etc.

    //If the given parabola is empty, return (recursion stop)
    if(!_p) { return 0; }

    //Get the parabola to the left of this on the beach line
    std::shared_ptr<VoronoiParabola> parent = _p->left();

    //See if the parabola is a leaf in the tree or not
    while(!parent->m_isLeaf)
    {
        //Keep going right through the tree (until we find a parent)
        parent.reset(parent->right().get());
    }

    return parent;
}

std::shared_ptr<VoronoiParabola> VoronoiParabola::getRightChild(std::shared_ptr<VoronoiParabola> _p)
{
    //Similar to the getLeftChild() function, only traversing
    //to the left this time
    if(!_p) { return 0; }

    std::shared_ptr<VoronoiParabola> parent = _p->right();

    while(!parent->m_isLeaf)
    {
        parent.reset(parent->left().get());
    }

    return parent;
}

//------------------------------------- VORONOI EVENT ----------------------------//
