#include "voronoiedge.h"

VoronoiEdge::~VoronoiEdge()
{

}

VoronoiEdge::VoronoiEdge()
{

}

VoronoiEdge::VoronoiEdge(const VoronoiEdge &toCopy)
{
    m_start = toCopy.m_start;
    m_startPTR = toCopy.m_startPTR;
    m_end = toCopy.m_end;
    m_endPTR = toCopy.m_endPTR;

    m_startNormal = toCopy.m_startNormal;
    m_startNormal2 = toCopy.m_startNormal2;

    m_endNormal = toCopy.m_endNormal;
    m_endNormal2 = toCopy.m_endNormal2;
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

    float denom = get2DCrossProduct(dir1, dir2);
    float uNumer = get2DCrossProduct((*(_test->m_startPTR) - *m_startPTR), dir1);

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

    float tNumer = get2DCrossProduct((*(_test->m_startPTR) - *m_startPTR), dir2);

    float tParam = tNumer / denom;
    float uParam = uNumer / denom;

    if(denom != 0.0f && (tParam >= 0.0f && tParam <= 1.0f) && (uParam >= 0.0f && uParam <= 1.0f))
    {
        //The lines interesect so return the intersection
        return (QVector3D((*(_test->m_startPTR) + (uParam * dir2))));
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

void VoronoiEdge::makeWall()
{
    if(m_startNormal == QVector3D(10000, 10000, 10000) && m_endNormal != QVector3D(10000, 10000, 10000) && m_startNormal2 == QVector3D(10000, 10000, 10000) && m_endNormal2 == QVector3D(10000, 10000, 10000))
    {
        QVector3D perpVector = QVector3D::crossProduct(getDirection(), QVector3D(0.0f, 1.0f, 0.0f));
        perpVector.normalize();

        perpVector /= 8.0f;

        m_endNormal /= 8.0f;

        m_verts.push_back(*m_startPTR + perpVector);
        m_verts.push_back(*m_startPTR - perpVector);
        m_verts.push_back(*m_endPTR - m_endNormal);
        m_verts.push_back(*m_endPTR + m_endNormal);

        m_verts.push_back(*m_startPTR + QVector3D(0,0.25f,0) + perpVector);
        m_verts.push_back(*m_startPTR + QVector3D(0,0.25f,0) - perpVector);
        m_verts.push_back(*m_endPTR + QVector3D(0,0.25f, 0) - m_endNormal);
        m_verts.push_back(*m_endPTR + QVector3D(0,0.25f,0) + m_endNormal);

        //Bottom
        m_norms.push_back((perpVector.normalized()) / 2.0f);
        m_norms[m_norms.size() - 1].normalize();

        m_norms.push_back((-perpVector.normalized()) / 2.0f);
        m_norms[m_norms.size() - 1].normalize();

        m_norms.push_back((-m_endNormal.normalized()) / 2.0f);
        m_norms[m_norms.size() - 1].normalize();

        m_norms.push_back((m_endNormal.normalized()) / 2.0f);
        m_norms[m_norms.size() - 1].normalize();

        //Top
        m_norms.push_back((QVector3D(0, 1, 0) + perpVector.normalized()) / 2.0f);
        m_norms[m_norms.size() - 1].normalize();

        m_norms.push_back((QVector3D(0, 1, 0) - perpVector.normalized()) / 2.0f);
        m_norms[m_norms.size() - 1].normalize();

        m_norms.push_back((QVector3D(0, 1, 0) - m_endNormal.normalized()) / 2.0f);
        m_norms[m_norms.size() - 1].normalize();

        m_norms.push_back((QVector3D(0, 1, 0) + m_endNormal.normalized()) / 2.0f);
        m_norms[m_norms.size() - 1].normalize();
    }
    else if(m_startNormal != QVector3D(10000, 10000, 10000) && m_endNormal == QVector3D(10000, 10000, 10000) && m_startNormal2 == QVector3D(10000, 10000, 10000) && m_endNormal2 == QVector3D(10000, 10000, 10000))
    {
        QVector3D perpVector = QVector3D::crossProduct(getDirection(), QVector3D(0.0f, 1.0f, 0.0f));
        perpVector.normalize();

        perpVector /= 8.0f;

//        qInfo()<<"Edge vert";
        m_startNormal /= 8.0f;

        m_verts.push_back(*m_startPTR + m_startNormal);
        m_verts.push_back(*m_startPTR - m_startNormal);
        m_verts.push_back(*m_endPTR - perpVector);
        m_verts.push_back(*m_endPTR + perpVector);

        m_verts.push_back(*m_startPTR + QVector3D(0,0.25f,0) + m_startNormal);
        m_verts.push_back(*m_startPTR + QVector3D(0,0.25f,0) - m_startNormal);
        m_verts.push_back(*m_endPTR + QVector3D(0,0.25f, 0) - perpVector);
        m_verts.push_back(*m_endPTR + QVector3D(0,0.25f,0) + perpVector);

        //Bottom
        m_norms.push_back((m_startNormal.normalized()) / 2.0f);
        m_norms[m_norms.size() - 1].normalize();

        m_norms.push_back((-m_startNormal.normalized()) / 2.0f);
        m_norms[m_norms.size() - 1].normalize();

        m_norms.push_back((-perpVector.normalized()) / 2.0f);
        m_norms[m_norms.size() - 1].normalize();

        m_norms.push_back((perpVector.normalized()) / 2.0f);
        m_norms[m_norms.size() - 1].normalize();

        //Top
        m_norms.push_back((QVector3D(0, 1, 0) + m_startNormal.normalized()) / 2.0f);
        m_norms[m_norms.size() - 1].normalize();

        m_norms.push_back((QVector3D(0, 1, 0) - m_startNormal.normalized()) / 2.0f);
        m_norms[m_norms.size() - 1].normalize();

        m_norms.push_back((QVector3D(0, 1, 0) - perpVector.normalized()) / 2.0f);
        m_norms[m_norms.size() - 1].normalize();

        m_norms.push_back((QVector3D(0, 1, 0) + perpVector.normalized()) / 2.0f);
        m_norms[m_norms.size() - 1].normalize();
    }
    else if(m_startNormal != QVector3D(10000, 10000, 10000) && m_endNormal != QVector3D(10000, 10000, 10000) && m_startNormal2 == QVector3D(10000, 10000, 10000) && m_endNormal2 == QVector3D(10000, 10000, 10000))
    {
        qInfo()<<"Normal vert";
        m_startNormal /= 8.0f;
        m_endNormal /= 8.0f;

        m_verts.push_back(*m_startPTR + m_startNormal);
        m_verts.push_back(*m_startPTR - m_startNormal);
        m_verts.push_back(*m_endPTR - m_endNormal);
        m_verts.push_back(*m_endPTR + m_endNormal);

        m_verts.push_back(*m_startPTR + QVector3D(0,0.25f,0) + m_startNormal);
        m_verts.push_back(*m_startPTR + QVector3D(0,0.25f,0) - m_startNormal);
        m_verts.push_back(*m_endPTR + QVector3D(0,0.25f, 0) - m_endNormal);
        m_verts.push_back(*m_endPTR + QVector3D(0,0.25f,0) + m_endNormal);

        //Bottom
        m_norms.push_back((m_startNormal.normalized()) / 2.0f);
        m_norms[m_norms.size() - 1].normalize();

        m_norms.push_back((-m_startNormal.normalized()) / 2.0f);
        m_norms[m_norms.size() - 1].normalize();

        m_norms.push_back((-m_endNormal.normalized()) / 2.0f);
        m_norms[m_norms.size() - 1].normalize();

        m_norms.push_back((m_endNormal.normalized()) / 2.0f);
        m_norms[m_norms.size() - 1].normalize();

        //Top
        m_norms.push_back((QVector3D(0, 1, 0) + m_startNormal.normalized()) / 2.0f);
        m_norms[m_norms.size() - 1].normalize();

        m_norms.push_back((QVector3D(0, 1, 0) - m_startNormal.normalized()) / 2.0f);
        m_norms[m_norms.size() - 1].normalize();

        m_norms.push_back((QVector3D(0, 1, 0) - m_endNormal.normalized()) / 2.0f);
        m_norms[m_norms.size() - 1].normalize();

        m_norms.push_back((QVector3D(0, 1, 0) + m_endNormal.normalized()) / 2.0f);
        m_norms[m_norms.size() - 1].normalize();
    }
    else if(m_startNormal2 != QVector3D(10000, 10000, 10000) && m_endNormal2 == QVector3D(10000, 10000, 10000))
    {
//        QVector3D perpVector = QVector3D::crossProduct(getDirection(), QVector3D(0.0f, 1.0f, 0.0f));
//        perpVector.normalize();

//        perpVector /= 8.0f;

////        qInfo()<<"Edge vert";
//        m_startNormal /= 8.0f;

//        m_verts.push_back(*m_startPTR + m_startNormal);
//        m_verts.push_back(*m_startPTR + m_startNormal2);
//        m_verts.push_back(*m_endPTR - perpVector);
//        m_verts.push_back(*m_endPTR + perpVector);

//        m_verts.push_back(*m_startPTR + QVector3D(0,0.25f,0) + m_startNormal);
//        m_verts.push_back(*m_startPTR + QVector3D(0,0.25f,0) + m_startNormal2);
//        m_verts.push_back(*m_endPTR + QVector3D(0,0.25f, 0) - perpVector);
//        m_verts.push_back(*m_endPTR + QVector3D(0,0.25f,0) + perpVector);

//        //Bottom
//        m_norms.push_back((m_startNormal.normalized()) / 2.0f);
//        m_norms[m_norms.size() - 1].normalize();

//        m_norms.push_back((m_startNormal2.normalized()) / 2.0f);
//        m_norms[m_norms.size() - 1].normalize();

//        m_norms.push_back((-perpVector.normalized()) / 2.0f);
//        m_norms[m_norms.size() - 1].normalize();

//        m_norms.push_back((perpVector.normalized()) / 2.0f);
//        m_norms[m_norms.size() - 1].normalize();

//        //Top
//        m_norms.push_back((QVector3D(0, 1, 0) + m_startNormal.normalized()) / 2.0f);
//        m_norms[m_norms.size() - 1].normalize();

//        m_norms.push_back((QVector3D(0, 1, 0) + m_startNormal2.normalized()) / 2.0f);
//        m_norms[m_norms.size() - 1].normalize();

//        m_norms.push_back((QVector3D(0, 1, 0) - perpVector.normalized()) / 2.0f);
//        m_norms[m_norms.size() - 1].normalize();

//        m_norms.push_back((QVector3D(0, 1, 0) + perpVector.normalized()) / 2.0f);
//        m_norms[m_norms.size() - 1].normalize();
    }
    else if(m_startNormal2 == QVector3D(10000, 10000, 10000) && m_endNormal2 != QVector3D(10000, 10000, 10000))
    {
//        QVector3D perpVector = QVector3D::crossProduct(getDirection(), QVector3D(0.0f, 1.0f, 0.0f));
//        perpVector.normalize();

//        perpVector /= 8.0f;

//        m_endNormal /= 8.0f;

//        m_verts.push_back(*m_startPTR + perpVector);
//        m_verts.push_back(*m_startPTR - perpVector);
//        m_verts.push_back(*m_endPTR + m_endNormal);
//        m_verts.push_back(*m_endPTR + m_endNormal2);

//        m_verts.push_back(*m_startPTR + QVector3D(0,0.25f,0) + perpVector);
//        m_verts.push_back(*m_startPTR + QVector3D(0,0.25f,0) - perpVector);
//        m_verts.push_back(*m_endPTR + QVector3D(0,0.25f, 0) + m_endNormal);
//        m_verts.push_back(*m_endPTR + QVector3D(0,0.25f,0) + m_endNormal2);

//        //Bottom
//        m_norms.push_back((perpVector.normalized()) / 2.0f);
//        m_norms[m_norms.size() - 1].normalize();

//        m_norms.push_back((-perpVector.normalized()) / 2.0f);
//        m_norms[m_norms.size() - 1].normalize();

//        m_norms.push_back((m_endNormal.normalized()) / 2.0f);
//        m_norms[m_norms.size() - 1].normalize();

//        m_norms.push_back((m_endNormal2.normalized()) / 2.0f);
//        m_norms[m_norms.size() - 1].normalize();

//        //Top
//        m_norms.push_back((QVector3D(0, 1, 0) + perpVector.normalized()) / 2.0f);
//        m_norms[m_norms.size() - 1].normalize();

//        m_norms.push_back((QVector3D(0, 1, 0) - perpVector.normalized()) / 2.0f);
//        m_norms[m_norms.size() - 1].normalize();

//        m_norms.push_back((QVector3D(0, 1, 0) + m_endNormal.normalized()) / 2.0f);
//        m_norms[m_norms.size() - 1].normalize();

//        m_norms.push_back((QVector3D(0, 1, 0) + m_endNormal2.normalized()) / 2.0f);
//        m_norms[m_norms.size() - 1].normalize();
    }
    /*    else
    {
        qInfo()<<"Corner vert";
        m_startNormal /= 8.0f;
        m_startNormal2 /= 8.0f;
        m_endNormal /= 8.0f;
        m_endNormal2 /= 8.0f;

        m_verts.push_back(*m_startPTR + m_startNormal);
        m_verts.push_back(*m_startPTR + m_startNormal2);
        m_verts.push_back(*m_endPTR + m_endNormal);
        m_verts.push_back(*m_endPTR + m_endNormal2);

        m_verts.push_back(*m_startPTR + QVector3D(0,0.25f,0) + m_startNormal);
        m_verts.push_back(*m_startPTR + QVector3D(0,0.25f,0) + m_startNormal2);
        m_verts.push_back(*m_endPTR + QVector3D(0,0.25f, 0) + m_endNormal);
        m_verts.push_back(*m_endPTR + QVector3D(0,0.25f,0) + m_endNormal2);

        //Bottom
        m_norms.push_back((m_startNormal.normalized()) / 2.0f);
        m_norms[m_norms.size() - 1].normalize();

        m_norms.push_back((m_startNormal2.normalized()) / 2.0f);
        m_norms[m_norms.size() - 1].normalize();

        m_norms.push_back((m_endNormal.normalized()) / 2.0f);
        m_norms[m_norms.size() - 1].normalize();

        m_norms.push_back((m_endNormal2.normalized()) / 2.0f);
        m_norms[m_norms.size() - 1].normalize();

        //Top
        m_norms.push_back((QVector3D(0, 1, 0) + m_startNormal.normalized()) / 2.0f);
        m_norms[m_norms.size() - 1].normalize();

        m_norms.push_back((QVector3D(0, 1, 0) + m_startNormal2.normalized()) / 2.0f);
        m_norms[m_norms.size() - 1].normalize();

        m_norms.push_back((QVector3D(0, 1, 0) + m_endNormal.normalized()) / 2.0f);
        m_norms[m_norms.size() - 1].normalize();

        m_norms.push_back((QVector3D(0, 1, 0) + m_endNormal2.normalized()) / 2.0f);
        m_norms[m_norms.size() - 1].normalize();

    }
*/


    m_indices.push_back(0);
    m_indices.push_back(1);
    m_indices.push_back(2);
    m_indices.push_back(3);

    m_indices.push_back(0);
    m_indices.push_back(1);
    m_indices.push_back(5);
    m_indices.push_back(4);

    m_indices.push_back(3);
    m_indices.push_back(0);
    m_indices.push_back(4);
    m_indices.push_back(7);

    m_indices.push_back(1);
    m_indices.push_back(2);
    m_indices.push_back(6);
    m_indices.push_back(5);

    m_indices.push_back(5);
    m_indices.push_back(6);
    m_indices.push_back(7);
    m_indices.push_back(4);

    m_indices.push_back(2);
    m_indices.push_back(3);
    m_indices.push_back(7);
    m_indices.push_back(6);
}

void VoronoiEdge::makeVBO(QOpenGLShaderProgram &_pgm)
{
    if(m_verts.size() > 0)
    {
        _pgm.bind();
        m_vao.create();
        m_vao.bind();

        m_vbo.create();
        m_vbo.bind();
        m_vbo.allocate(&m_verts[0], (int)m_verts.size() * sizeof(GLfloat) * 3);

        _pgm.enableAttributeArray("vertexPos");
        _pgm.setAttributeArray("vertexPos", GL_FLOAT, 0, 3);

        m_vbo.release();

        m_nbo.create();
        m_nbo.bind();
        m_nbo.allocate(&m_norms[0], (int)m_norms.size() * sizeof(GLfloat) * 3);

        _pgm.enableAttributeArray("vertexNorm");
        _pgm.setAttributeArray("vertexNorm", GL_FLOAT, 0, 3);

        m_nbo.release();

        m_ibo.create();
        m_ibo.bind();
        m_ibo.allocate(&m_indices[0], (int)m_indices.size() * sizeof(uint));

        m_vao.release();

        _pgm.release();
    }
}

void VoronoiEdge::drawWall()
{
    if(m_vao.isCreated())
    {
        m_vao.bind();
        //    glDrawArrays(GL_QUADS, 0, (int)m_verts.size());
        glDrawElements(GL_QUADS, (int)m_indices.size(), GL_UNSIGNED_INT, &m_indices[0]);
        m_vao.release();
    }
}

int VoronoiEdge::usesVert(QVector3D *_vert)
{
    if(m_startPTR == _vert)
    {
        return 0;
    }
    else if(m_endPTR == _vert)
    {
        return 1;
    }

    return -1;
}
