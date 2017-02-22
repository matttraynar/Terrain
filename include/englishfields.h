#ifndef ENGLISHFIELDS_H
#define ENGLISHFIELDS_H

#include <QVector3D>

#include <vector>
#include <memory>

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
    EnglishFields(std::vector<std::vector<float> >& _terrainHeightMap, std::vector<std::vector<QVector3D> > &_terrainNormalMap);
    ~EnglishFields();

    void operator = (EnglishFields &toCopy);

    void checkAvailableSpace();
    void exploreTerrain(int x, int y, RelativePosition _rel);
    void threeFieldModel();

    inline std::vector<Line> getFieldBoundary() const { return m_fieldBoundary; }
    inline std::vector< std::vector<Line> > getFields() const { return m_fields; }
    std::vector<QVector3D> getBoundaryVerts() const;


private:

    float m_maxSteepness;
    int m_count;

    std::vector< std::vector<float> > m_heightMapCopy;
    std::vector< std::vector<QVector3D> > m_normalMapCopy;
    std::vector<Line> m_fieldBoundary;
    std::vector< std::vector<Line> > m_fields;


};

#endif // ENGLISHFIELDS_H
