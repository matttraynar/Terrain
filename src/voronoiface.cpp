#include "voronoiface.h"

VoronoiFace::VoronoiFace(std::vector<QVector3D> _edgeVerts)
{
    m_edgeVerts = _edgeVerts;

    m_vao.create();
    m_vao.bind();

    m_verts.create();
    m_verts.bind();
    m_verts.allocate(&m_edgeVerts[0], (int)m_edgeVerts.size() * sizeof(GLfloat) * 3);

    m_verts.release();
    m_vao.release();
}

VoronoiFace::~VoronoiFace()
{

}

VoronoiFace::VoronoiFace(const VoronoiFace &_toCopy)
{
    m_edgeVerts = _toCopy.m_edgeVerts;
}

void VoronoiFace::operator =(const VoronoiFace &_toCopy)
{
    m_edgeVerts = _toCopy.m_edgeVerts;
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
