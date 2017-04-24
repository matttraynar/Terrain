#ifndef VORONOIEDGE_H
#define VORONOIEDGE_H

#include <QVector3D>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>

#include <QDebug>

class VoronoiEdge
{
public:
    VoronoiEdge(QVector3D _start, QVector3D _end)
    {
        m_start = _start;
        m_end = _end;
        m_midPoint = QVector3D(1000000,1000000,1000000);

        m_startNormal = QVector3D(10000, 10000, 10000);
        m_startNormal2 = QVector3D(10000, 10000, 10000);

        m_endNormal = QVector3D(10000, 10000, 10000);
        m_endNormal2 = QVector3D(10000, 10000, 10000);

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

        m_startNormal = QVector3D(10000, 10000, 10000);
        m_startNormal2 = QVector3D(10000, 10000, 10000);

        m_endNormal = QVector3D(10000, 10000, 10000);
        m_endNormal2 = QVector3D(10000, 10000, 10000);

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

    VoronoiEdge(const VoronoiEdge &toCopy);

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
        if(m_startPTR != NULL && m_endPTR != NULL)
        {
            m_midPoint = *m_startPTR + *m_endPTR;
        }
        else
        {
            m_midPoint = m_start + m_end;
        }

        m_midPoint /= 2.0f;

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

    inline void print()
    {
        if(m_startPTR != NULL && m_endPTR != NULL)
        {
            qInfo()<<"Start: "<<*m_startPTR<<" End: "<<*m_endPTR;
        }
        else
        {
            qInfo()<<"Start: "<<m_start<<" End: "<<m_end;
        }
    };

    void makeWall();
    void makeVBO(QOpenGLShaderProgram &_pgm);
    void drawWall();

    int usesVert(QVector3D* _vert);

    inline void setStartNormal(QVector3D _normal) { m_startNormal = _normal; }
    inline void setStartNormal2(QVector3D _normal) { m_startNormal2 = _normal; }
    inline void setEndNormal(QVector3D _normal) { m_endNormal = _normal; }
    inline void setEndNormal2(QVector3D _normal) { m_endNormal2 = _normal; }

    inline QVector3D getStartNormal1() const { return m_startNormal; }
    inline QVector3D getStartNormal2() const { return m_startNormal2; }

    inline QVector3D getEndNormal1() const { return m_endNormal; }
    inline QVector3D getEndNormal2() const { return m_endNormal2; }

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

    QVector3D m_startNormal;
    QVector3D m_startNormal2;

    QVector3D m_endNormal;
    QVector3D m_endNormal2;

    QOpenGLVertexArrayObject m_vao;
    QOpenGLBuffer m_vbo;
    QOpenGLBuffer m_nbo;
    QOpenGLBuffer m_ibo;

    std::vector<QVector3D> m_verts;
    std::vector<QVector3D> m_norms;
    std::vector<uint> m_indices;

};

#endif // VORONOIEDGE_H
