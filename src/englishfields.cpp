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

    makeVoronoiDiagram();

}

EnglishFields::EnglishFields(double _width,
                                          std::vector<QVector3D> _sites)
{
    //Set up and create the voronoi, this time with the given
    //input sites
    m_width = _width;
    m_sites = _sites;

    makeVoronoiDiagram();
}

EnglishFields::~EnglishFields()
{

}

void EnglishFields::operator =(EnglishFields &toCopy)
{
    //Copy the data
    m_width =        toCopy.m_width;
    m_sites =        toCopy.m_sites;
    m_regions =     toCopy.m_regions;
}

void EnglishFields::makeVoronoiDiagram()
{
    //Using slightly modified code from Sebatien Loriot
    //             github.com/sloriot/cgal-voronoi-cropping
    //Comment added by me for clarity

    //A container to keep the initial sites in
    std::vector< K::Point_2 > points;

    if(m_sites.size() != 0)
    {
        //We know the number of points so set the container size
        points.reserve(m_sites.size());

        //If we have then add them to the vector
        for(uint i = 0; i < m_sites.size(); ++i)
        {
            points.push_back(K::Point_2(m_sites[i].x(), m_sites[i].y()));
        }
    }
    else
    {
        //If not randomly create m_width (e.g. 50) points in the range (-m_width / 2.0, m_width / 2.0)
        srand(time(NULL));

        uint numPoints = 5;

        points.reserve(numPoints);

        for(uint i = 0; i < numPoints; ++i)
        {
            points.push_back(K::Point_2((m_width * (double)rand()/(double)RAND_MAX) - (m_width / 2.0), (m_width * (double)rand()/(double)RAND_MAX) - (m_width / 2.0)));
        }
    }

    //A new half-edge data structure to store the Voronoi
    //edges in
    HDS hds;

    //Create a boundary rectangle we'll intersect the diagram with
    K::Iso_rectangle_2 bbox(-(m_width / 2.0), -(m_width / 2.0), (m_width / 2.0), (m_width / 2.0));

    //Create the cropped voronoi (uses Lorien's code)
    create_hds_for_cropped_voronoi_diagram<K, Exact_kernel>(points.begin(), points.end(), bbox, hds);

    //Now we need to process the faces
    for(auto face = hds.faces_begin(); face != hds.faces_end(); ++face)
    {
        //Get two edge handles, one we'll use to iterate and one which stores
        //the first edge of the face (this way we'll be able to check when the
        //cycle around the face has completed
        HDS::Halfedge_const_handle currentEdge = face->halfedge();
        HDS::Halfedge_const_handle end = currentEdge;

        //A container for storing edge vertices in
        std::vector<QVector3D> edgeVerts;

        do{
            //Do some assertions before we start using the data
            CGAL_assertion(currentEdge != HDS::Halfedge_const_handle() );
            CGAL_assertion(currentEdge->face() != HDS::Face_handle() );
            CGAL_assertion(currentEdge->face() == face);

            //Add the edge to our container
            QVector3D newPoint(currentEdge->vertex()->point().x(), 0.0f, currentEdge->vertex()->point().y());
            edgeVerts.push_back(newPoint);

            //Move on to the next edge
            currentEdge = currentEdge->next();

            //And add a new point (edges have two verts)
            newPoint = QVector3D(currentEdge->vertex()->point().x(), 0.0f, currentEdge->vertex()->point().y());
            edgeVerts.push_back(newPoint);

        }while(currentEdge != end);

        //Finally duplicate the last edge vertex
        edgeVerts.push_back(QVector3D(currentEdge->vertex()->point().x(), 0.0f, currentEdge->vertex()->point().y()));

        //Add a new voronoi region and store it
        m_regions.push_back(VoronoiFace(edgeVerts));
    }
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
