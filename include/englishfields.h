#ifndef ENGLISHFIELDS_H
#define ENGLISHFIELDS_H

//INCLUDES FOR CROPPED VORONOI FUNCTIONS
//      (See makeVoronoiDiagram() )
#include <CroppedVoronoi/hds_type.h>
#include <CroppedVoronoi/voronoi_cropping.h>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/point_generators_2.h>

typedef CGAL::Simple_cartesian< CGAL::Lazy_exact_nt< CGAL::Gmpq > > Exact_kernel;
typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef CGAL::HalfedgeDS_default< K, HDS_Item_extra > HDS;
//INCLUDES AND TYPEDEFS END

#include <iterator>

#include <QVector3D>

#include <vector>
#include <memory>
#include <math.h>
#include <time.h>

#include "voronoiface.h"

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

    inline std::vector<VoronoiFace> getRegions() const { return m_regions; }


private:
    void subdivideRegions();

    double m_width;

    std::vector<QVector3D> m_sites;

    std::vector<VoronoiFace> m_regions;
};

#endif // ENGLISHFIELDS_H
