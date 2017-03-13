#ifndef ENGLISHFIELDS_H
#define ENGLISHFIELDS_H

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Delaunay_triangulation_2.h>
#include <iterator>

#include <QVector3D>

#include <vector>
#include <memory>
#include <math.h>

#include <QDebug>

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef K::Point_2 Point_2;
typedef K::Iso_rectangle_2 Iso_rectangle_2;
typedef K::Segment_2 Segment_2;
typedef K::Ray_2 Ray_2;
typedef K::Line_2 Line_2;
typedef CGAL::Delaunay_triangulation_2<K>  Delaunay_triangulation_2;

//A class to recover Voronoi diagram from stream.
//Rays, lines and segments are cropped to a rectangle
//so that only segments are stored
struct Cropped_voronoi_from_delaunay{
  std::list<Segment_2> m_cropped_vd;
  Iso_rectangle_2 m_bbox;

  Cropped_voronoi_from_delaunay(const Iso_rectangle_2& bbox):m_bbox(bbox){}

  template <class RSL>
  void crop_and_extract_segment(const RSL& rsl){
    CGAL::Object obj = CGAL::intersection(rsl,m_bbox);
    const Segment_2* s=CGAL::object_cast<Segment_2>(&obj);
    if (s) m_cropped_vd.push_back(*s);
  }

  void operator<<(const Ray_2& ray)    { crop_and_extract_segment(ray); }
  void operator<<(const Line_2& line)  { crop_and_extract_segment(line); }
  void operator<<(const Segment_2& seg){ crop_and_extract_segment(seg); }
};


//#include "VPoint.h"
//#include "VEdge.h"

//typedef std::list<VPoint *>		Vertices	;
//typedef std::list<VEdge *>		Edges;

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
    void makeVoronoiDiagram();

    void subdivideEdge(QVector3D _start, QVector3D _end, std::vector< std::pair< QVector3D, QVector3D > > & edgeLis);

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

//    Edges m_vEdges;
//    Voronoi *v;
//    Vertices *ver;
//    Vertices *dir;

    bool m_hasSites;
};

#endif // ENGLISHFIELDS_H
