#include "englishfields.h"

//#include "voronoi.h"

#include <time.h>

#include <QDebug>

EnglishFields::EnglishFields()
{

}

EnglishFields::EnglishFields(std::vector< std::vector<float> > &_terrainHeightMap,
                                          std::vector< std::vector<QVector3D> > &_terrainNormalMap,
                                          double _width) :
    m_heightMapCopy(_terrainHeightMap),
    m_normalMapCopy(_terrainNormalMap)
{
    m_maxSteepness = 0.95f;

    m_width = _width;

    m_hasSites = false;

    makeDiagram();

    makePoints();
}

EnglishFields::EnglishFields(std::vector<std::vector<float> >& _terrainHeightMap,
                                          std::vector<std::vector<QVector3D> > &_terrainNormalMap,
                                          double _width,
                                          std::vector<QVector3D> _sites) :
    m_heightMapCopy(_terrainHeightMap),
    m_normalMapCopy(_terrainNormalMap)
{
    m_maxSteepness = 0.95f;

    m_width = _width;

    m_hasSites = true;

    ver = new Vertices();
    dir = new Vertices();

    for(int i = 0; i < _sites.size(); ++i)
    {
        if(_sites[i].x() == _sites[i].z())
        {
//            _sites[i].setZ(_sites[i].z() - 0.5);
        }

        ver->push_back(new VPoint((double)(_sites[i].x() + m_width/2.0), (double)(_sites[i].z() + m_width/2.0)));
        dir->push_back(new VPoint( (double)rand()/(double)RAND_MAX - 0.5, (double)rand()/(double)RAND_MAX - 0.5));
    }

    makeDiagram();

    makePoints();

}

EnglishFields::~EnglishFields()
{

}

void EnglishFields::operator =(EnglishFields &toCopy)
{
    m_maxSteepness = toCopy.m_maxSteepness;
    m_count = toCopy.m_count;

    m_heightMapCopy = toCopy.m_heightMapCopy;
    m_normalMapCopy = toCopy.m_normalMapCopy;

    m_fieldBoundary = toCopy.m_fieldBoundary;
    m_fields = toCopy.m_fields;

    m_linePoints = toCopy.m_linePoints;
}

void EnglishFields::makeDiagram()
{
    if(!m_hasSites)
    {
        ver = new Vertices();
        dir = new Vertices();

        srand (time(NULL));


        for(int i=0; i < 250; i++)
        {
            ver->push_back(new VPoint( m_width * (double)rand()/(double)RAND_MAX , m_width * (double)rand()/(double)RAND_MAX ));

            dir->push_back(new VPoint( (double)rand()/(double)RAND_MAX - 0.5, (double)rand()/(double)RAND_MAX - 0.5));
        }
    }

    qInfo()<<"Size: "<<ver->size();

    v = new Voronoi();

    m_vEdges = (*(v->GetEdges(ver, m_width, m_width)));
}

void EnglishFields::makePoints()
{
    std::vector< std::pair< QVector3D, QVector3D > > edgeList;

    for(auto i = m_vEdges.begin(); i != m_vEdges.end(); ++i)
    {
        QVector3D start((*i)->start->x - (m_width / 2.0), 0.0, (*i)->start->y - (m_width / 2.0));
        QVector3D end((*i)->end->x - (m_width / 2.0), 0.0, (*i)->end->y - (m_width / 2.0));

        subdivideEdge(start, end, edgeList);

//        edgeList.push_back(std::make_pair(start, end));
    }

    QVector3D v1( - (m_width / 2.0),    0, - (m_width / 2.0));
    QVector3D v2( - (m_width / 2.0),    0,   (m_width / 2.0));
    QVector3D v3(   (m_width / 2.0),    0,   (m_width / 2.0));
    QVector3D v4(   (m_width / 2.0),    0, - (m_width / 2.0));

    subdivideEdge(v1, v2, edgeList);
    subdivideEdge(v2, v3, edgeList);
    subdivideEdge(v3, v4, edgeList);
    subdivideEdge(v4, v1, edgeList);

//    edgeList.push_back(std::make_pair(v1, v2));
//    edgeList.push_back(std::make_pair(v2, v3));
//    edgeList.push_back(std::make_pair(v3, v4));
//    edgeList.push_back(std::make_pair(v4, v1));


    for(int i = 0; i < edgeList.size(); ++i)
    {
        m_linePoints.push_back(edgeList[i].first);
        m_linePoints.push_back(edgeList[i].second);
    }
}

void EnglishFields::subdivideEdge(QVector3D _start, QVector3D _end, std::vector< std::pair< QVector3D, QVector3D > > & edgeList)
{
    float length = (_end - _start).length();

    int resolution = 1;

    QVector3D newStart = _start;
    QVector3D newEnd = ((_end - _start) / (int)length * resolution) + _start;

    for(int j = 0; j < (int)(length * resolution) - resolution; ++j)
    {
        if((newEnd - _start).length() > (_end - _start).length())
        {
            break;
        }

        edgeList.push_back(std::make_pair(newStart, newEnd));

        newStart = newEnd;
        newEnd += ((_end - _start) / (int)(length * resolution));

    }

    edgeList.push_back(std::make_pair(newStart, _end));
}

void EnglishFields::checkAvailableSpace()
{

    int iStart = 0;
    int jStart = 0;

    int count = 0;

    voronoi(5);

//    //First we need to find a starting point for the boundary.
//    for(int i = 0; i < m_normalMapCopy.size(); ++i)
//    {
//        for(int j = 0; j < m_normalMapCopy[i].size(); ++j)
//        {
//            if(m_normalMapCopy[i][j].y() > m_maxSteepness)
//            {
//                iStart = i;
//                jStart = j;
//            }
//            count++;
//        }
//    }

//    if(count == 0)
//    {
//        qInfo()<<"Terrain not suitable for fields";
//        return;
//    }
//    else
//    {
//        qInfo()<<"Creating fields";
//        exploreTerrain(iStart, jStart, Middle);
//    }

}

void EnglishFields::voronoi(int numPoints)
{
    std::vector<QVector3D> initialPoints;

    for(uint i = 0; i < numPoints; ++i)
    {
        initialPoints.push_back(QVector3D(rand() % m_normalMapCopy.size() + 10, 0, rand() % m_normalMapCopy[0].size() + 10));
    }

    int width = m_normalMapCopy.size();
    int depth = m_normalMapCopy[0].size();

    for(int i = 0; i < width; ++i)
    {
        for(int j = 0; j < depth; ++j)
        {
            int index = -1;
            float dist = INT_MAX;

            for(uint point = 0; point < initialPoints.size(); ++point)
            {
                const QVector3D& p = initialPoints[point];

                float xd = i - p.x();
                float yd = j - p.z();

//                d = (xd * xd) + (yd * yd);

//                if(d < dist)
//                {
//                    dist = d;
//                    index = point;
//                }
            }
        }
    }
}

void EnglishFields::exploreTerrain(int x, int y, RelativePosition _rel)
{
    if(m_count > 500)
    {
            return;
    }

    if(x > 0 && x < m_normalMapCopy.size() &&
            y > 0)
    {
        if(y < m_normalMapCopy[x].size())
        {
            QVector3D testNorm(0,0,0);
            Line tmp;
            tmp.p0 = QVector3D(x, m_heightMapCopy[x][y], y);

            if(_rel != BottomLeft)
            {
                testNorm = m_normalMapCopy[x - 1][y - 1];

                if(testNorm.y() > m_maxSteepness)
                {
                    tmp.p1 = QVector3D(x - 1, m_heightMapCopy[x - 1][y - 1], y -1);

                    m_fieldBoundary.push_back(tmp);

                    exploreTerrain(x - 1, y - 1, BottomLeft);
                }
            }

            if(_rel != Bottom)
            {
                testNorm = m_normalMapCopy[x][y - 1];

                if(testNorm.y() > m_maxSteepness)
                {
                    tmp.p1 = QVector3D(x, m_heightMapCopy[x][y - 1], y - 1);

                    m_fieldBoundary.push_back(tmp);

                    exploreTerrain(x, y - 1, Bottom);
                }
            }

            if(_rel != BottomRight)
            {
                testNorm = m_normalMapCopy[x + 1][y - 1];

                if(testNorm.y() > m_maxSteepness)
                {
                    tmp.p1 = QVector3D(x + 1, m_heightMapCopy[x + 1][y], y);

                    m_fieldBoundary.push_back(tmp);

                    exploreTerrain(x + 1, y - 1, BottomRight);
                }
            }

            if(_rel != Left)
            {
                testNorm = m_normalMapCopy[x - 1][y];

                if(testNorm.y() > m_maxSteepness)
                {
                    tmp.p1 = QVector3D(x - 1, m_heightMapCopy[x - 1][y], y);

                    m_fieldBoundary.push_back(tmp);

                    exploreTerrain(x - 1, y, Left);
                }
            }

            if(_rel != Right)
            {
                testNorm = m_normalMapCopy[x + 1][y];

                if(testNorm.y() > m_maxSteepness)
                {
                    tmp.p1 = QVector3D(x + 1, m_heightMapCopy[x + 1][y], y);

                    m_fieldBoundary.push_back(tmp);

                    exploreTerrain(x + 1, y, Right);
                }
            }

            if(_rel != TopLeft)
            {
                testNorm = m_normalMapCopy[x - 1][y + 1];

                if(testNorm.y() > m_maxSteepness)
                {
                    tmp.p1 = QVector3D(x - 1, m_heightMapCopy[x - 1][y + 1], y + 1);

                    m_fieldBoundary.push_back(tmp);

                    exploreTerrain(x - 1, y + 1, TopLeft);
                }
            }

            if(_rel != Top)
            {
                testNorm = m_normalMapCopy[x][y + 1];

                if(testNorm.y() > m_maxSteepness)
                {
                    tmp.p1 = QVector3D(x, m_heightMapCopy[x][y + 1], y + 1);

                    m_fieldBoundary.push_back(tmp);

                    exploreTerrain(x, y + 1, Top);
                }
            }

            if(_rel != TopRight)
            {
                testNorm = m_normalMapCopy[x + 1][y + 1];

                if(testNorm.y() > m_maxSteepness)
                {
                    tmp.p1 = QVector3D(x + 1, m_heightMapCopy[x + 1][y + 1], y + 1);

                    m_fieldBoundary.push_back(tmp);

                    exploreTerrain(x + 1, y + 1, TopRight);
                }
            }
        }
    }

    m_count++;

    return;
}

void EnglishFields::threeFieldModel()
{

}

std::vector<QVector3D> EnglishFields::getBoundaryVerts() const
{
    std::vector<QVector3D> verts;

    //Iterate through the lines making up the boundary and add the
    //start and end points to the vertex container
    for(uint i = 0; i < m_fieldBoundary.size(); ++i)
    {
        verts.push_back(m_fieldBoundary[i].p0);
        verts.push_back(m_fieldBoundary[i].p1);
    }

    //Return the list of vertices
    return verts;
}


