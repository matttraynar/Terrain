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

    m_isUsable = true;

    m_reversedEdge = false;
}

VoronoiFace::VoronoiFace(std::vector<VoronoiEdge *> _edgeList)
{
    m_edges = _edgeList;
    m_isUsable = true;
    m_midPointIsCalculated = false;

    m_reversedEdge = false;

    updateVerts();
}

VoronoiFace::VoronoiFace(std::vector<uint> _indices)
{
    m_indices = _indices;
    m_isUsable = true;
    m_midPointIsCalculated = false;

    m_reversedEdge = false;
}

void VoronoiFace::loadVerts(std::vector<VoronoiEdge *> &_edges)
{
    m_edges.clear();

    for(uint i = 0; i < m_indices.size(); ++i)
    {
        m_edges.push_back(_edges[m_indices[i]]);
    }

    updateEdgeCount();

    removeDuplicates();
}

void VoronoiFace::removeDuplicates()
{
    for(uint i = 0; i < m_indices.size(); ++i)
    {
        uint index = m_indices[i];

        auto iter = std::find(m_indices.begin(),  m_indices.end(), index);
        int length = distance(m_indices.begin(), iter);

        auto duplicateIter = std::find(iter + 1, m_indices.end(), index);

        while(duplicateIter != m_indices.end())
        {
            length = distance(m_indices.begin(), duplicateIter);
            m_indices.erase(m_indices.begin() + length);

            duplicateIter = std::find(iter + 1, m_indices.end(), index);
        }

    }
}

bool VoronoiFace::usesEdge(uint ID)
{
    if(std::find(m_indices.begin(), m_indices.end(), ID) != m_indices.end())
    {
        return true;
    }

    return false;
}

void VoronoiFace::replaceEdge(uint ID, std::vector<uint> newIDs)
{
    auto existingID = std::find(m_indices.begin(), m_indices.end(), ID);

    if(existingID != m_indices.end())
    {
        for(uint i = 0; i < newIDs.size(); ++i)
        {
            m_indices.push_back(newIDs[i]);
        }
    }
}

void VoronoiFace::removeEdge(uint ID)
{
    int removeIndex = -1;

    for(uint i = 0; i < m_indices.size(); ++i)
    {
        if(m_indices[i] == ID && removeIndex == -1)
        {
            removeIndex = i;
            continue;
        }

        if(m_indices[i] > ID)
        {
            m_indices[i]--;
        }
    }

    if(removeIndex != -1)
    {
        m_indices.erase(m_indices.begin() + removeIndex);
    }
}

void VoronoiFace::storeOriginalEdges(std::vector<VoronoiEdge *> &_edges)
{
    m_originalEdges.clear();

    for(uint i = 0; i < m_indices.size(); ++i)
    {
        m_originalEdges.push_back(new VoronoiEdge(*(_edges[m_indices[i]])));
    }
}

std::vector<uint> VoronoiFace::getEdgeIDsInRange(uint start, uint end)
{
    uint newID = m_indices[start];
    std::vector<uint> rangeIDs;

//    while(newID != m_indices[end])
//    {
//        newID = getNextEdge(newID);

//        if(newID != -1)
//        {
//            rangeIDs.push_back(newID);
//        }
//        else
//        {
//            break;
//        }
//    }

    while(start != end)
    {
        start++;

        if(start == m_indices.size())
        {
            start -= m_indices.size();
        }

        rangeIDs.push_back(m_indices[start]);
    }

    return rangeIDs;
}

uint VoronoiFace::getNextEdge(uint index)
{
    int localEdgeID = -1;

    auto indexPosition = std::find(m_indices.begin(), m_indices.end(), index);

    if(indexPosition != m_indices.end())
    {
        localEdgeID = distance(m_indices.begin(), indexPosition);
    }

    uint i = -1;

    if(localEdgeID != -1)
    {
        VoronoiEdge* currentEdge = m_edges[localEdgeID];

        for(i = 0; i < m_edges.size(); ++i)
        {
            if(i == localEdgeID)
            {
                continue;
            }

            if(m_reversedEdge)
            {
                if(m_edges[i]->m_startPTR == currentEdge->m_startPTR)
                {
                    m_reversedEdge = false;
                    break;
                }
            }
            else
            {
                if(m_edges[i]->m_startPTR == currentEdge->m_endPTR)
                {
                    m_reversedEdge = false;
                    break;
                }
            }
        }

        if(i == m_edges.size())
        {
            for(i = 0; i < m_edges.size(); ++i)
            {
                if(i == localEdgeID)
                {
                    continue;
                }

                if(m_reversedEdge)
                {
                    if(m_edges[i]->m_endPTR == currentEdge->m_startPTR)
                    {
                        m_reversedEdge = true;
                        break;
                    }
                }
                else
                {
                    if(m_edges[i]->m_endPTR == currentEdge->m_endPTR)
                    {
                        m_reversedEdge = true;
                        break;
                    }
                }
            }
        }
    }

    if(i == m_edges.size())
    {
        return -1;
    }

    return m_indices[i];
}

uint VoronoiFace::getNextUnnconnectedEdge(uint index, bool up)
{
    int localEdgeID = -1;

    auto indexPosition = std::find(m_indices.begin(), m_indices.end(), index);

    if(indexPosition != m_indices.end())
    {
        if (up)
        {
            localEdgeID = distance(m_indices.begin(), indexPosition + 1);
        }
        else
        {
            localEdgeID = distance(m_indices.begin(), indexPosition - 1);
        }
    }

    if(localEdgeID == m_indices.size())
    {
        localEdgeID -=2;
    }
    return m_indices[localEdgeID];
}

void VoronoiFace::organiseEdgeIDs()
{
//    qInfo()<<"------------- NEW FACE -------------";
    std::vector<uint> newIDOrder;

//    for(uint i = 0; i < m_indices.size(); ++i)
//    {
//        qInfo()<<"Index "<<i<<": "<<m_indices[i];
//    }

    uint startID = m_indices[0];
//    qInfo()<<"Start ID: "<<startID;

    uint nextID = getNextEdge(startID);
//    qInfo()<<"ID: "<<nextID;

    newIDOrder.push_back(startID);
    newIDOrder.push_back(nextID);

    while(nextID != startID)
    {
        if(newIDOrder.size() > 100)
        {
            break;
        }
        nextID = getNextEdge(nextID);
//        qInfo()<<"ID: "<<nextID;
        newIDOrder.push_back(nextID);
    }
     newIDOrder.erase(newIDOrder.begin() + newIDOrder.size() - 1);

//    for(uint i = 0; i < newIDOrder.size(); ++i)
//    {
//        qInfo()<<"Index "<<i<<": "<<newIDOrder[i];
//    }

     m_indices = newIDOrder;

}

//-----------------------
VoronoiFace::~VoronoiFace()
{

}

VoronoiFace::VoronoiFace(const VoronoiFace &_toCopy)
{
    m_edgeVerts = _toCopy.m_edgeVerts;

    m_edges = _toCopy.m_edges;

    m_isUsable = _toCopy.m_isUsable;

    m_indices = _toCopy.m_indices;

    m_originalEdges = _toCopy.m_originalEdges;

    m_reversedEdge = false;

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

    m_isUsable = _toCopy.m_isUsable;

    m_indices = _toCopy.m_indices;

    m_originalEdges = _toCopy.m_originalEdges;

    m_reversedEdge = false;

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

    m_midPointIsCalculated = false;
}

void VoronoiFace::updateEdge(int index, VoronoiEdge *_e1, VoronoiEdge *_e2)
{
    if(_e1->m_startPTR != m_edges[index]->m_startPTR &&
       _e1->m_endPTR != m_edges[index]->m_startPTR)
    {
        m_edges[index] = _e2;
        m_edges.insert(m_edges.begin() + index + 1, _e1);
    }
    else
    {
        m_edges[index] = _e1;
        m_edges.insert(m_edges.begin() + index + 1, _e2);
    }
}

void VoronoiFace::passVBOToShader(QOpenGLShaderProgram &_pgm)
{
    updateVerts();

    if(m_edgeVerts.size() > 0)
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
}

void VoronoiFace::draw()
{
//    if(m_segmentIndices.empty())
//    {
//        m_vao.bind();
//        glDrawArrays(GL_LINES, 0, (int)m_edgeVerts.size());

//        m_vao.release();

//        for(uint i = 0; i < m_edges.size(); ++i)
//        {
//            m_edges[i]->drawWall();
//        }
//    }
//    else
//    {
        for(uint i = 0; i < m_segmentIndices.size(); ++i)
        {
            for(uint j = 0; j < m_segmentIndices[i].size(); ++j)
            {
//                qInfo()<<"Drawing: "<<m_segmentIndices[i][j];
                if(j == m_skips[i])
                {
                    continue;
                }

                m_edges[m_segmentIndices[i][j]]->drawWall();
            }
        }
//    }
}

void VoronoiFace::checkUsable()
{
    if(m_isUsable == true)
    {
        if(m_edges.size() == 3)
        {
            qInfo()<<"Not enough edges";
            m_isUsable = false;
        }
        else
        {
            int count = 0;

            for(auto i = m_edges.begin(); i != m_edges.end(); ++i)
            {
                if((*i)->getLength() <1.0f)
                {
                    count++;
                }
            }

            if(count > 2)
            {
                qInfo()<<"Too many short edges";
                m_isUsable = false;
            }
        }
    }
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
        m_midPointIsCalculated = true;

        for(auto i = m_edges.begin(); i != m_edges.end(); ++i)
        {
            m_midPoint += (*(*i)->m_startPTR);
            m_midPoint += (*(*i)->m_endPTR);
        }

        m_midPoint /= m_edges.size() * 2;
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

QVector3D VoronoiFace::getWeightedMiddle2(int vert, float weight)
{

    QVector3D wMiddle;

    for(uint i = 0; i < m_edges.size(); ++i)
    {
        wMiddle += *(m_edges[i]->m_startPTR);
    }

    wMiddle /= m_edges.size();

    QVector3D wVector = *(m_edges[vert]->m_startPTR) - wMiddle;

    if(wVector.length() > 5)
    {
        weight /= 2.0f;
    }

    wMiddle += (wVector * weight);

    return wMiddle;
}
