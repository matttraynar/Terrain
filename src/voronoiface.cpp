#include "voronoiface.h"

VoronoiFace::VoronoiFace(std::vector<QVector3D> _edgeVerts)
{
    m_edgeVerts = _edgeVerts;
    m_midPointIsCalculated = false;

    m_vao.create();
    m_vao.bind();

    m_verts.create();
    m_verts.bind();
    m_verts.allocate(&m_edgeVerts[0], (int)m_edgeVerts.size() * sizeof(GLfloat) * 3);

    m_verts.release();
    m_vao.release();
}

VoronoiFace::VoronoiFace(std::vector<VoronoiEdge *> _edgeList)
{
    m_edges = _edgeList;

    updateVerts();
}

VoronoiFace::~VoronoiFace()
{

}

VoronoiFace::VoronoiFace(const VoronoiFace &_toCopy)
{
    m_edgeVerts = _toCopy.m_edgeVerts;

    m_edges = _toCopy.m_edges;

    updateVerts();

    m_midPointIsCalculated = _toCopy.m_midPointIsCalculated;

    if(!m_midPointIsCalculated)
    {
        m_midPoint = _toCopy.m_midPoint;
    }
}

void VoronoiFace::operator =(const VoronoiFace &_toCopy)
{
    m_edgeVerts = _toCopy.m_edgeVerts;

    m_edges = _toCopy.m_edges;
    updateVerts();

    m_midPointIsCalculated = _toCopy.m_midPointIsCalculated;

    if(!m_midPointIsCalculated)
    {
        m_midPoint = _toCopy.m_midPoint;
    }
}

void VoronoiFace::updateVerts()
{
    m_edgeVerts.clear();

    for(uint i = 0; i < m_edges.size(); ++i)
    {
        m_edgeVerts.push_back(m_edges[i]->getStart());
        m_edgeVerts.push_back(m_edges[i]->getEnd());
    }
}

void VoronoiFace::passVBOToShader(QOpenGLShaderProgram &_pgm)
{
    _pgm.bind();
    m_vao.create();
    m_vao.bind();

    m_verts.create();
    m_verts.bind();
    m_verts.allocate(&m_edgeVerts[0], (int)m_edgeVerts.size() * sizeof(GLfloat) * 3);

    _pgm.enableAttributeArray("vertexPos");
    _pgm.setAttributeArray("vertexPos", GL_FLOAT, 0, 3);

    m_verts.release();
    m_vao.release();

    _pgm.release();
}

void VoronoiFace::draw()
{
    m_vao.bind();

    glDrawArrays(GL_LINES, 0, (int)m_edgeVerts.size());

    m_vao.release();
}

void VoronoiFace::print()
{
    for(uint i = 0; i < m_edgeVerts.size(); ++i)
    {
        qInfo()<<"Point "<<i<<": ("<<m_edgeVerts[i].x()<<", 0, "<<m_edgeVerts[i].z()<<") \n";
    }
}

void VoronoiFace::adjustHeight(int index, float newHeight)
{
    m_edgeVerts[index].setY(newHeight);
}

QVector3D VoronoiFace::getMiddle()
{
    if(!m_midPointIsCalculated)
    {
        for(uint i = 0; i < m_edgeVerts.size(); ++i)
        {
            m_midPoint += m_edgeVerts[i];
        }

        m_midPoint /= m_edgeVerts.size();

        m_midPointIsCalculated = true;
    }

    return m_midPoint;
}

QVector3D VoronoiFace::getWeightedMiddle(int vert, float weight)
{
    int actualIndex = vert * 2;

    if(actualIndex == m_edgeVerts.size() * 2)
    {
        actualIndex = 0;
    }

    QVector3D wMiddle = getMiddle();

    QVector3D wVector = m_edgeVerts[actualIndex] - wMiddle;

    wMiddle += wVector * weight;

    return wMiddle;
}
