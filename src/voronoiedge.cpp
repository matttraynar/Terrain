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
