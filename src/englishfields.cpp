#include "englishfields.h"

#include <QDebug>

EnglishFields::EnglishFields()
{

}

EnglishFields::EnglishFields(std::vector< std::vector<float> > &_terrainHeightMap,
                                           std::vector< std::vector<QVector3D> > &_terrainNormalMap) :
    m_heightMapCopy(_terrainHeightMap),
    m_normalMapCopy(_terrainNormalMap)
{
    m_maxSteepness = 0.95f;

    checkAvailableSpace();
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


