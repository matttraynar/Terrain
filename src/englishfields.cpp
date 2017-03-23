#include "englishfields.h"

#include <QDebug>

EnglishFields::EnglishFields()
{
    //Blank constructor, does nothing

}

EnglishFields::EnglishFields(double _width)
{
    //Set up and create the voronoi diagram.
    m_width = _width;

    makeVoronoiDiagram2();

}

EnglishFields::EnglishFields(double _width,
                                          std::vector<QVector3D> _sites)
{
    //Set up and create the voronoi, this time with the given
    //input sites
    m_width = _width;
    m_sites = _sites;

    makeVoronoiDiagram2();
}

EnglishFields::~EnglishFields()
{

}

void EnglishFields::operator =(EnglishFields &toCopy)
{
    //Copy the data
    m_width =        toCopy.m_width;
    m_linePoints = toCopy.m_linePoints;
    m_sites =        toCopy.m_sites;
    m_regions =     toCopy.m_regions;
}

void EnglishFields::makeVoronoiDiagram()
{
//    //Create a container for 2D points
//    std::vector<Point_2> points;

//    //Check if we've been given any initial sites
//    if(m_sites.size() != 0)
//    {
//        //If we have then add them to the vector
//        for(uint i = 0; i < m_sites.size(); ++i)
//        {
//            points.push_back(Point_2(m_sites[i].x(), m_sites[i].y()));
//        }
//    }
//    else
//    {
//        //If not randomly create m_width (e.g. 50) points in the range (-m_width / 2.0, m_width / 2.0)
//        srand(time(NULL));

//        for(uint i = 0; i < m_width; ++i)
//        {
//            points.push_back(Point_2((m_width * (double)rand()/(double)RAND_MAX) - (m_width / 2.0), (m_width * (double)rand()/(double)RAND_MAX) - (m_width / 2.0)));
//        }
//    }

//    //Create a new triangulation
//    Delaunay_triangulation_2 dt2;

//    //Insert points into the triangulation
//    dt2.insert(points.begin(),points.end());

//    //Construct a rectangle to use as a boundary
//    Iso_rectangle_2 bbox(-m_width / 2.0, -m_width / 2.0, m_width / 2.0, m_width / 2.0);

//    //Create a new cropped voronoi
//    Cropped_voronoi_from_delaunay vor(bbox);

//    //extract the cropped Voronoi diagram
//    dt2.draw_dual(vor);

//    //Create a container for storing edges in the form std::pair(start, end)
//    std::vector< std::pair< QVector3D, QVector3D > > edgeList;

//    //Iterate throught the segments of the cropped voronoi diagram
//    for (auto i = vor.m_cropped_vd.begin(); i != vor.m_cropped_vd.end(); ++i)
//    {
//        //Create variables for the source and target (end and start)
//        QVector3D start((*i).source().x(), 0.0, (*i).source().y());
//        QVector3D end((*i).target().x(), 0.0, (*i).target().y());

//        //Subdivide between source and target so the edge will
//        //fit the landscape better later. This step also adds the edges
//        //to the given container
//        subdivideEdge(start, end, edgeList);
//    }

//    //Add in the boundary edges
//    QVector3D v1( - (m_width / 2.0),    0, - (m_width / 2.0));
//    QVector3D v2( - (m_width / 2.0),    0,   (m_width / 2.0));
//    QVector3D v3(   (m_width / 2.0),    0,   (m_width / 2.0));
//    QVector3D v4(   (m_width / 2.0),    0, - (m_width / 2.0));

//    //Subdivide them
//    subdivideEdge(v1, v2, edgeList);
//    subdivideEdge(v2, v3, edgeList);
//    subdivideEdge(v3, v4, edgeList);
//    subdivideEdge(v4, v1, edgeList);

//    //Finally convert the edges into points for drawing
//    for(int i = 0; i < edgeList.size(); ++i)
//    {
//        m_linePoints.push_back(edgeList[i].first);
//        m_linePoints.push_back(edgeList[i].second);
//    }
}

void EnglishFields::makeVoronoiDiagram2()
{
    int n = 6;

    HDS hds;

    std::vector< K::Point_2 > points;

    points.reserve(n);

    srand(123);

    for(int i = 0; i < n; ++i)
    {
        points.push_back(K::Point_2((m_width * (double)rand()/(double)RAND_MAX) - (m_width / 2.0), (m_width * (double)rand()/(double)RAND_MAX) - (m_width / 2.0)));
    }

    K::Iso_rectangle_2 bbox(-(m_width / 2.0), -(m_width / 2.0), (m_width / 2.0), (m_width / 2.0));

    create_hds_for_cropped_voronoi_diagram<K, Exact_kernel>(points.begin(), points.end(), bbox, hds);

    for(auto face = hds.faces_begin(); face != hds.faces_end(); ++face)
    {
        HDS::Halfedge_const_handle currentEdge = face->halfedge();
        HDS::Halfedge_const_handle end = currentEdge;

        do{
            CGAL_assertion(currentEdge != HDS::Halfedge_const_handle() );
            CGAL_assertion(currentEdge->face() != HDS::Face_handle() );
            CGAL_assertion(currentEdge->face() == face);

            m_linePoints.push_back(QVector3D(currentEdge->vertex()->point().x(), 0.0f, currentEdge->vertex()->point().y()));

            currentEdge = currentEdge->next();
        }while(currentEdge != end);

        m_linePoints.push_back(QVector3D(currentEdge->vertex()->point().x(), 0.0f, currentEdge->vertex()->point().y()));

        m_regions.push_back(VoronoiFace(m_linePoints));

        m_linePoints.clear();
    }

    qInfo()<<m_regions.size();
}

void EnglishFields::subdivideEdge(QVector3D _start, QVector3D _end, std::vector< std::pair< QVector3D, QVector3D > > & edgeList)
{
    //Get the length of the edge we're subdividing
    float length = (_end - _start).length();

    //Set the resolution
    int resolution = 2;

    //Initialise two variables which we'll use in a moment
    QVector3D newStart = _start;

    //Set the end point to a proportion of the edge length
    QVector3D newEnd = ((_end - _start) / (int)length * resolution) + _start;

    //Iterate throught the edge
    for(int j = 0; j < (int)(length * resolution) - resolution; ++j)
    {
        //First check that if we add a new edge now we won't go past
        //the given end point
        if((newEnd - _start).length() > (_end - _start).length())
        {
            break;
        }

        //Add the new edge
        edgeList.push_back(std::make_pair(newStart, newEnd));

        //Update the start and end points for the next edge
        newStart = newEnd;
        newEnd += ((_end - _start) / (int)(length * resolution));
    }

    //Finally add on the last piece of the edge
    edgeList.push_back(std::make_pair(newStart, _end));
}
