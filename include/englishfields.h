#ifndef ENGLISHFIELDS_H
#define ENGLISHFIELDS_H

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Delaunay_triangulation_2.h>
#include <iterator>

#include <QVector3D>

#include <vector>
#include <memory>
#include <math.h>
#include <time.h>

//FOLLOWING CODE TAKEN FROM CGAL'S TRIANGULATION_2 DEMO
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
struct Cropped_voronoi_from_delaunay
{
      std::list<Segment_2> m_cropped_vd;
      Iso_rectangle_2 m_bbox;

      Cropped_voronoi_from_delaunay(const Iso_rectangle_2& bbox):m_bbox(bbox){}

      template <class RSL>
      void crop_and_extract_segment(const RSL& rsl)
      {
            CGAL::Object obj = CGAL::intersection(rsl,m_bbox);

            const Segment_2* s=CGAL::object_cast<Segment_2>(&obj);

            if (s) m_cropped_vd.push_back(*s);
      }

      void operator<<(const Ray_2& ray)         { crop_and_extract_segment(ray); }
      void operator<<(const Line_2& line)        { crop_and_extract_segment(line); }
      void operator<<(const Segment_2& seg) { crop_and_extract_segment(seg); }
};

//------------------------------------ END ------------------------------------------

class EnglishFields
{
public:
    EnglishFields();
    EnglishFields(double _width);
    EnglishFields(double _width, std::vector<QVector3D> _sites);

    ~EnglishFields();

    void operator = (EnglishFields &toCopy);

    void makeVoronoiDiagram();

    void subdivideEdge(QVector3D _start, QVector3D _end, std::vector< std::pair< QVector3D, QVector3D > > & edgeLis);
    inline std::vector<QVector3D> getLineVerts() const { return m_linePoints; }

private:
    double m_width;

    std::vector<QVector3D> m_sites;
    std::vector<QVector3D> m_linePoints;
};

#endif // ENGLISHFIELDS_H
