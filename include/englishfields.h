#ifndef ENGLISHFIELDS_H
#define ENGLISHFIELDS_H

#include <CroppedVoronoi/hds_type.h>
#include <CroppedVoronoi/voronoi_cropping.h>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/point_generators_2.h>

typedef CGAL::Simple_cartesian< CGAL::Lazy_exact_nt< CGAL::Gmpq > > Exact_kernel;
typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef CGAL::HalfedgeDS_default< K, HDS_Item_extra > HDS;

////CGAL includes
//#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
//#include <CGAL/Delaunay_triangulation_2.h>
//#include <CGAL/Voronoi_diagram_2.h>
//#include <CGAL/Delaunay_triangulation_adaptation_traits_2.h>
//#include <CGAL/Delaunay_triangulation_adaptation_policies_2.h>

#include <iterator>

#include <QVector3D>

#include <vector>
#include <memory>
#include <math.h>
#include <time.h>

#include "voronoiface.h"

//////OTHER INCLUDES AND TYPEDEFS
////// includes for defining the Voronoi diagram adaptor
////#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
////#include <CGAL/Delaunay_triangulation_2.h>
////#include <CGAL/Voronoi_diagram_2.h>
////#include <CGAL/Delaunay_triangulation_adaptation_traits_2.h>
////#include <CGAL/Delaunay_triangulation_adaptation_policies_2.h>

//// typedefs for defining the adaptor
//typedef CGAL::Exact_predicates_inexact_constructions_kernel                                 K;
//typedef CGAL::Delaunay_triangulation_2<K>                                                            DT;
//typedef CGAL::Delaunay_triangulation_adaptation_traits_2<DT>                                 AT;
//typedef CGAL::Delaunay_triangulation_caching_degeneracy_removal_policy_2<DT>    AP;
//typedef CGAL::Voronoi_diagram_2<DT,AT,AP>                                                         VD;

//// typedef for the result type of the point location
//typedef AT::Site_2                    Site_2;
//typedef AT::Point_2                   Point_2;
//typedef VD::Locate_result             Locate_result;
//typedef VD::Vertex_handle             Vertex_handle;
//typedef VD::Face_handle               Face_handle;
//typedef VD::Halfedge_handle           Halfedge_handle;
//typedef VD::Ccb_halfedge_circulator   Ccb_halfedge_circulator;


//////FOLLOWING CODE TAKEN FROM CGAL'S TRIANGULATION_2 DEMO
////typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
////typedef K::Point_2 Point_2;
////typedef K::Iso_rectangle_2 Iso_rectangle_2;
////typedef K::Segment_2 Segment_2;
////typedef K::Ray_2 Ray_2;
////typedef K::Line_2 Line_2;
////typedef CGAL::Delaunay_triangulation_2<K>  Delaunay_triangulation_2;

////A class to recover Voronoi diagram from stream.
////Rays, lines and segments are cropped to a rectangle
////so that only segments are stored
//struct Cropped_voronoi_from_delaunay
//{
//      std::list<Segment_2> m_cropped_vd;
//      Iso_rectangle_2 m_bbox;

//      Cropped_voronoi_from_delaunay(const Iso_rectangle_2& bbox):m_bbox(bbox){}

//      template <class RSL>
//      void crop_and_extract_segment(const RSL& rsl)
//      {
//            CGAL::Object obj = CGAL::intersection(rsl,m_bbox);

//            const Segment_2* s=CGAL::object_cast<Segment_2>(&obj);

//            if (s) m_cropped_vd.push_back(*s);
//      }

//      void operator<<(const Ray_2& ray)         { crop_and_extract_segment(ray); }
//      void operator<<(const Line_2& line)        { crop_and_extract_segment(line); }
//      void operator<<(const Segment_2& seg) { crop_and_extract_segment(seg); }
//};

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
    inline std::vector<VoronoiFace> getRegions() const { return m_regions; }


private:
    double m_width;

    std::vector<QVector3D> m_sites;
    std::vector<QVector3D> m_linePoints;

    std::vector<VoronoiFace> m_regions;
};

#endif // ENGLISHFIELDS_H
