#ifndef VORONOIEDGE_H
#define VORONOIEDGE_H

#include <QVector3D>

#include <QDebug>

class VoronoiEdge
{
public:
    VoronoiEdge(QVector3D _start, QVector3D _end)
    {
        m_start = _start;
        m_end = _end;
        m_midPoint = QVector3D(1000000,1000000,1000000);

        if(m_end.x() - m_start.x() == 0)
        {
            m_gradient = 0;
        }
        else
        {
            m_gradient = (m_end.z() - m_start.z()) / (m_end.x() - m_start.x());
        }

        m_invGradient = -1.0f/m_gradient;
        m_c = m_start.z() - (m_gradient * m_start.x());

        m_startPTR = NULL;
        m_endPTR = NULL;
    }

    VoronoiEdge(QVector3D* _start, QVector3D* _end)
    {
        m_startPTR = _start;
        m_endPTR = _end;
        m_midPoint = QVector3D(1000000,1000000,1000000);

        m_start = *m_startPTR;
        m_end = *m_endPTR;

        if(m_end.x() - m_start.x() == 0)
        {
            m_gradient = 0;
        }
        else
        {
            m_gradient = (m_end.z() - m_start.z()) / (m_end.x() - m_start.x());
        }

        m_invGradient = -1.0f/m_gradient;
        m_c = m_start.z() - (m_gradient * m_start.x());
    }

    ~VoronoiEdge();

    void hasPTRS()
    {
        if(m_startPTR != NULL && m_endPTR != NULL)
        {
            qInfo()<<"Pointers not NULL";
        }
        else
        {
            qInfo()<<"Pointers are NULL";
        }
    }

    QVector3D intersectEdge(VoronoiEdge* _test);

    inline QVector3D getStart() const
    {
        if(m_startPTR != NULL)
        {
            return *m_startPTR;
        }

        return m_start;
    }

    inline QVector3D getEnd() const
    {
        if(m_startPTR != NULL)
        {
            return *m_endPTR;
        }

        return m_end;
    }

    inline float getGradient() const { return m_gradient; }

    inline QVector3D getMidPoint()
    {
        if(m_midPoint == QVector3D(1000000,1000000,1000000))
        {
            m_midPoint = m_end + m_start;
            m_midPoint /= 2.0f;
        }

        return m_midPoint;
    }

    bool operator == (const VoronoiEdge &RHS)
    {
        if(m_startPTR != NULL && m_endPTR != NULL)
        {
            return(m_startPTR == RHS.m_startPTR && m_endPTR == RHS.m_endPTR) ||
                    (m_startPTR == RHS.m_endPTR && m_endPTR == RHS.m_startPTR);
        }

        return((m_start == RHS.m_start && m_end == RHS.m_end) ||
                 (m_start == RHS.m_end && m_end == RHS.m_start));
    }

    float getLength();

    void updateGradient()
    {
        if(m_end.x() - m_start.x() == 0)
        {
            m_gradient = 0;
        }
        else
        {
            m_gradient = (m_end.z() - m_start.z()) / (m_end.x() - m_start.x());
        }
    }

    float getAngle(VoronoiEdge*_test);

    QVector3D getDirection();

    QVector3D m_start;
    QVector3D m_end;

    QVector3D* m_startPTR;
    QVector3D* m_endPTR;

private:
    VoronoiEdge();

    float get2DCrossProduct(QVector3D a, QVector3D b);

    QVector3D m_midPoint;

    float m_gradient;
    float m_invGradient;
    float m_c;

};

#endif // VORONOIEDGE_H
