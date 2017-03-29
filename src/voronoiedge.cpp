#include "voronoiedge.h"

VoronoiEdge::~VoronoiEdge()
{

}

VoronoiEdge::VoronoiEdge()
{

}

float VoronoiEdge::getLength()
{
    if(m_startPTR != NULL && m_endPTR != NULL)
    {
        return(QVector3D((*m_endPTR) - (*m_startPTR)).length());
    }

   return(QVector3D(m_end - m_start).length());
}

QVector3D VoronoiEdge::intersectEdge(VoronoiEdge *_test)
{
    float x = (m_c - _test->m_c) / (_test->m_gradient - m_gradient);
    float y = (m_gradient * x) + m_c;

    return QVector3D(x, 0.0f, y);
}
