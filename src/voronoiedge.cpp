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
    //Edge passed in is Edge 2
    //Param T = ((s2 - s1) x e1) / (e1 x e2);
    // http://stackoverflow.com/questions/563198/how-do-you-detect-where-two-line-segments-intersect

    QVector3D dir1 = *(m_endPTR) - *(m_startPTR);
    dir1.setY(0);

    QVector3D dir2 = *(_test->m_endPTR) - *(_test->m_startPTR);
    dir2.setY(0);

    VoronoiEdge tmpTest = *_test;

    float denom = get2DCrossProduct(dir1, dir2);
    float uNumer = get2DCrossProduct((tmpTest.m_start - *m_startPTR), dir1);

    if(denom == 0.0f && uNumer == 0.0f)
    {
        //Lines are colinear, not intereseted in this case
        return QVector3D(1000000.0f, 0.0f, 1000000.0f);
    }
    else if(denom == 0.0f && uNumer != 0.0f)
    {
        //The lines are parallel, again, we're not interested
        return QVector3D(1000000.0f, 0.0f, 1000000.0f);
    }

    float tNumer = get2DCrossProduct((tmpTest.m_start - *m_startPTR), dir2);

    float tParam = tNumer / denom;
    float uParam = uNumer / denom;

    if(denom != 0.0f && (tParam >= 0.0f && tParam <= 1.0f) && (uParam >= 0.0f && uParam <= 1.0f))
    {
        //The lines interesect so return the intersection
        return (QVector3D((tmpTest.m_start + (uParam * dir2))));
    }

    //Otherwise the lines aren't parallel but don't interesect
    return QVector3D(1000000.0f, 0.0f, 1000000.0f);
}

float VoronoiEdge::get2DCrossProduct(QVector3D a, QVector3D b)
{
    //Assuming we're ignoring height
    return((a.x() * b.z()) - (a.z() * b.x()));
}

float VoronoiEdge::getAngle(VoronoiEdge *_test)
{
    QVector3D translatedThis = m_end - m_start;
    QVector3D translatedTest = _test->m_end - _test->m_start;

    float dotProduct = QVector3D::dotProduct(translatedThis, translatedTest);

    float angle = acos(dotProduct / (translatedThis.length() * translatedTest.length()));

    return angle;
}

QVector3D VoronoiEdge::getDirection()
{
    QVector3D direction = *m_endPTR - * m_startPTR;
    direction.normalize();
    return direction;
}
