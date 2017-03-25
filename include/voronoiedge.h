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

        m_startPTR = NULL;
        m_endPTR = NULL;
    }

    VoronoiEdge(QVector3D* _start, QVector3D* _end)
    {
        m_startPTR = _start;
        m_endPTR = _end;
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

    QVector3D m_start;
    QVector3D m_end;

    QVector3D* m_startPTR;
    QVector3D* m_endPTR;

private:
    VoronoiEdge();

};

#endif // VORONOIEDGE_H
