#ifndef ENGLISHFIELDS_H
#define ENGLISHFIELDS_H

#include <QVector3D>

#include <vector>
#include <memory>

#include "borrowed/voronoi/Voronoi.h"

#include "VPoint.h"
#include "VEdge.h"

typedef std::list<VPoint *>		Vertices	;
typedef std::list<VEdge *>		Edges;

//Relative Postion layout
// TL ------ T ------ TR       yMax
//  ||         ||         ||           | |
//  L --------x-------- R          | |
//  ||         ||         ||           | |
// BL ----- B ----- BR          | |
// 0 --------------------------  xMax,0

enum RelativePosition
{
    BottomLeft,
    Bottom,
    BottomRight,

    Left,
    Right,

    TopLeft,
    Top,
    TopRight,

    Middle
};

struct Line
{
    inline Line() : p0(QVector3D(0,0,0)), p1(QVector3D(1,1,1)) {}
    inline Line(QVector3D a, QVector3D b) : p0(a), p1(b) {}

    inline float getLength() const { return (p1 - p0).length(); }

    QVector3D p0;
    QVector3D p1;
};

class EnglishFields
{
public:
    EnglishFields();
    EnglishFields(std::vector<std::vector<float> >& _terrainHeightMap, std::vector<std::vector<QVector3D> > &_terrainNormalMap, double _width);

    EnglishFields(std::vector<std::vector<float> >& _terrainHeightMap, std::vector<std::vector<QVector3D> > &_terrainNormalMap, double _width, std::vector<QVector3D> _sites);
    ~EnglishFields();

    void operator = (EnglishFields &toCopy);

    void makeDiagram();
    void makePoints();

    void checkAvailableSpace();
    void voronoi(int numPoints);
    void exploreTerrain(int x, int y, RelativePosition _rel);
    void threeFieldModel();

    inline std::vector<Line> getFieldBoundary() const { return m_fieldBoundary; }
    inline std::vector< std::vector<Line> > getFields() const { return m_fields; }
    std::vector<QVector3D> getBoundaryVerts() const;
    inline std::vector<QVector3D> getLineVerts() const { qInfo()<<m_linePoints.size(); return m_linePoints; }

    std::vector<QVector3D> m_linePoints;


private:

    float m_maxSteepness;
    int m_count;

    std::vector< std::vector<float> > m_heightMapCopy;
    std::vector< std::vector<QVector3D> > m_normalMapCopy;
    std::vector<Line> m_fieldBoundary;
    std::vector< std::vector<Line> > m_fields;

    double m_width;

    Edges m_vEdges;
    Voronoi *v;
    Vertices *ver;
    Vertices *dir;

    bool m_hasSites;
};

#endif // ENGLISHFIELDS_H
