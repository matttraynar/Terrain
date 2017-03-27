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

    subdivideRegions();
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
//        srand(451);

        uint numPoints = 6;

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
//        std::vector<QVector3D> edgeVerts;
        std::vector<VoronoiEdge*> edges;

        do{
            //Do some assertions before we start using the data
            CGAL_assertion(currentEdge != HDS::Halfedge_const_handle() );
            CGAL_assertion(currentEdge->face() != HDS::Face_handle() );
            CGAL_assertion(currentEdge->face() == face);

            //Code added by me -------------START------------------

            //Create a new point from the current edge's vertex
            QVector3D* startPoint = new QVector3D(currentEdge->vertex()->point().x(), 0.0f, currentEdge->vertex()->point().y());

            //Check whether we've already created a vertex reference
            //at this position or not
            int startID = vertExists(startPoint);

            if(startID != -1)
            {
                //We have already created the vertex,
                //change the pointer to point at the
                //pre-exisiting reference
                startPoint = m_allVerts[startID];
            }
            else
            {
                //The vertex doesn't already exist, add
                //it to our references list
                m_allVerts.push_back(startPoint);
            }

            //Move on to the next edge
            currentEdge = currentEdge->next();

            //Create a new point (edges have two verts)
            QVector3D* endPoint = new QVector3D(currentEdge->vertex()->point().x(), 0.0f, currentEdge->vertex()->point().y());

            //Do the same process (check if already existing)
            int endID = vertExists(endPoint);

            if(endID != -1)
            {
                endPoint = m_allVerts[endID];
            }
            else
            {
                m_allVerts.push_back(endPoint);
            }

            //Now we create a new voronoi edge using the start and
            //end pointers we just created
            VoronoiEdge* newEdge = new VoronoiEdge(startPoint, endPoint);

            //Again check if the edge already exists and act accordingly
            int edgeID = edgeExists(newEdge);

            if(edgeID != -1)
            {
                //Note that this time if the edge already exists we copy the
                //pointer to out edge container
                edges.push_back(m_allEdges[edgeID]);
            }
            else
            {
                //Add the new edge to our global container and the container
                //for the current voronoi face
                m_allEdges.push_back(newEdge);
                edges.push_back(newEdge);
            }

            //Code added by me --------------END-------------------

        }while(currentEdge != end);

        //Add a new voronoi region and store it
        m_regions.push_back(VoronoiFace(edges));
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

void EnglishFields::subdivideRegions()
{
    //Container for storing our edited faces in
    std::vector<VoronoiFace> newFaces;

    for(uint i = 0; i < m_regions.size(); ++i)
    {
        srand(time(NULL));

        m_regions[i].updateVerts();

        qInfo()<<"Edges: "<<m_regions[i].getNumEdges();

        if((m_regions[i].getNumEdges()) > 3)
        {
            if((int)(m_regions[i].getNumEdges()) % 2 == 0)
            {
                //Randomly generate a vertex index and weight
                int vert = m_regions[i].getNumEdges() * (float)rand() / (float)RAND_MAX;
                float weight = 7.5f * (float)rand() / (float)RAND_MAX;

                //Adjust the weight to lie within the range (-3.75f, 3.75f)
                weight -= 3.75f;

                //And then change it to a percentage
                weight /= 10.0f;

                //Now get the weighted middle of the current voronoi face
                QVector3D* regionMiddle = new QVector3D(m_regions[i].getWeightedMiddle2(vert, weight));

                int vertID = vertExists(regionMiddle);

                if(vertID != -1)
                {
                    regionMiddle = m_allVerts[vertID];
                }
                else
                {
                    m_allVerts.push_back(regionMiddle);
                }

                for(int j = 0; j < m_regions[i].getNumEdges(); j += 2)
                {
                    //Iterate through the current face's verts
                    std::vector<VoronoiEdge*> edges;

                    int v2 = clampIndex(j + 1, m_regions[i].getNumEdges());

                    //Now create a triangle
                    edges.push_back(m_regions[i].getEdge(j));
                    edges.push_back(m_regions[i].getEdge(v2));

                    QVector3D* edgeStart = m_regions[i].getEdge(j)->m_startPTR;

                    if((*edgeStart) == *(m_regions[i].getEdge(v2)->m_startPTR) ||
                       (*edgeStart) == *(m_regions[i].getEdge(v2)->m_endPTR)    )
                    {
                        edgeStart = m_regions[i].getEdge(j)->m_endPTR;
                    }

                    QVector3D* edgeEnd = m_regions[i].getEdge(v2)->m_startPTR;

                    if((*edgeEnd) == *(m_regions[i].getEdge(j)->m_startPTR) ||
                       (*edgeEnd) == *(m_regions[i].getEdge(j)->m_endPTR)    )
                    {
                        edgeEnd = m_regions[i].getEdge(v2)->m_endPTR;
                    }


                    VoronoiEdge* newEdge = new VoronoiEdge(edgeEnd, regionMiddle);

                    int edgeID = edgeExists(newEdge);

                    if(edgeID != -1)
                    {
                        edges.push_back(m_allEdges[edgeID]);
                    }
                    else
                    {
                        edges.push_back(newEdge);
                        m_allEdges.push_back(newEdge);
                    }

                    VoronoiEdge* newEdge2 = new VoronoiEdge(regionMiddle, edgeStart);

                    edgeID = edgeExists(newEdge2);

                    if(edgeID != -1)
                    {
                        edges.push_back(m_allEdges[edgeID]);
                    }
                    else
                    {
                        edges.push_back(newEdge2);
                        m_allEdges.push_back(newEdge2);
                    }

                    //Create a new voronoi face with these edges and
                    //add it to our container
//                    newFaces.push_back(VoronoiFace(edges));
                }
            }
            else
            {
                int subdivideSwitch = 100 * (float)rand() / (float)RAND_MAX;

                if(subdivideSwitch > 90)
                {
                    newFaces.push_back(m_regions[i]);
                }
                else
                {
                    int startID = (m_regions[i].getNumEdges() - 1) / 2;

                    QVector3D* endVert = m_regions[i].getEdge(startID)->m_startPTR;

                    if((*endVert) != *(m_regions[i].getEdge(startID - 1)->m_startPTR) ||
                       (*endVert) != *(m_regions[i].getEdge(startID - 1)->m_endPTR))
                    {
                        endVert = m_regions[i].getEdge(startID)->m_endPTR;
                    }

                    int endID = m_regions[i].getNumEdges() - 1;

                    QVector3D* newVert = new QVector3D((*(m_regions[i].getEdge(endID)->m_startPTR) + *(m_regions[i].getEdge(endID)->m_endPTR)) / 2.0f);

                    int vertID = vertExists(newVert);

                    if(vertID != -1)
                    {
                        newVert = m_allVerts[vertID];
                    }
                    else
                    {
                        m_allVerts.push_back(newVert);
                    }

                    std::vector<VoronoiEdge*> edges;

                    QVector3D* startVert = m_regions[i].getEdge(0);

                    if((*startVert == *(m_regions[i].getEdge(1)->m_startPTR)) ||
                      (*startVert == *(m_regions[i].getEdge(1)->m_endPTR)))
                    {
                        startVert = m_regions[i].getEdge(0)->m_endPTR;
                    }

                    //EXISTING EDGES
                    for(int i = 0; i < startID; ++i)
                    {
                        edges.push_back(m_regions[i].getEdge(i));
                    }

                    //NEW EDGE 1
                    VoronoiEdge* newEdge = new VoronoiEdge(endVert, newVert);

                    int edgeID = edgeExists(newEdge);

                    if(edgeID != -1)
                    {
                        newEdge = m_allEdges[edgeID];
                    }
                    else
                    {
                        m_allEdges.push_back(newEdge);
                    }

                    edges.push_back(newEdge);

                    //NEW EDGE 2
                    VoronoiEdge* newEdge2 = new VoronoiEdge(newVert, startVert);

                    edgeID = edgeExists(newEdge2);

                    if(edgeID != -1)
                    {
                        newEdge2 = m_allEdges[edgeID];
                    }
                    else
                    {
                        m_allEdges.push_back(newEdge2);
                    }

                    edges.push_back(newEdge2);



                    /*//Randomly generate a vertex index and weight
                    int vert = m_regions[i].getNumEdges() * (float)rand() / (float)RAND_MAX;
                    float weight = 7.5f * (float)rand() / (float)RAND_MAX;

                    //Adjust the weight to lie within the range (-3.75f, 3.75f)
                    weight -= 3.75f;

                    //And then change it to a percentage
                    weight /= 10.0f;

                    //Now get the weighted middle of the current voronoi face
                    QVector3D* regionMiddle = new QVector3D(m_regions[i].getWeightedMiddle2(vert, weight));

                    int vertID = vertExists(regionMiddle);

                    if(vertID != -1)
                    {
                        regionMiddle = m_allVerts[vertID];
                    }
                    else
                    {
                        m_allVerts.push_back(regionMiddle);
                    }

                    int edgeToSkip = m_regions[i].getNumEdges() * (float)rand()/(float)RAND_MAX;

                    //Iterate through the current face's verts
                    std::vector<QVector3D> evenSidedEdgeList;
                    int middleVertID = 0;

                    std::vector<VoronoiEdge*> evenEdges;

                    for(int j = 0; j < m_regions[i].getNumEdges(); j += 1)
                    {
                       if(j == (edgeToSkip))
                       {
                           //Iterate through the current face's verts
                           std::vector<VoronoiEdge*> edges;

                           int v2 = clampIndex(j + 1, m_regions[i].getNumEdges());

                            //Now create a triangle
                            edges.push_back(m_regions[i].getEdge(j));

                            QVector3D* edgeStart = m_regions[i].getEdge(j)->m_startPTR;
                            QVector3D* edgeEnd = m_regions[i].getEdge(v2)->m_startPTR;

                            if((*edgeStart) == *(m_regions[i].getEdge(v2)->m_startPTR) ||
                               (*edgeStart) == *(m_regions[i].getEdge(v2)->m_endPTR)    )
                            {
                                edgeStart = m_regions[i].getEdge(j)->m_endPTR;
                                edgeEnd = m_regions[i].getEdge(j)->m_startPTR;
                            }

                            VoronoiEdge* newEdge = new VoronoiEdge(edgeEnd, regionMiddle);

                            int edgeID = edgeExists(newEdge);

                            if(edgeID != -1)
                            {
                                edges.push_back(m_allEdges[edgeID]);
                            }
                            else
                            {
                                edges.push_back(newEdge);
                                m_allEdges.push_back(newEdge);
                            }

                            VoronoiEdge* newEdge2 = new VoronoiEdge(regionMiddle, edgeStart);

                            edgeID = edgeExists(newEdge2);

                            if(edgeID != -1)
                            {
                                edges.push_back(m_allEdges[edgeID]);
                            }
                            else
                            {
                                edges.push_back(newEdge2);
                                m_allEdges.push_back(newEdge2);
                            }

                            evenEdges.push_back(newEdge2);

                            middleVertID = evenSidedEdgeList.size();

                            evenEdges.push_back(newEdge);

                            //Create a new voronoi face with these edges and
                            //add it to our container
//                            newFaces.push_back(VoronoiFace(edges));
                       }
                       else
                       {
                           evenEdges.push_back(m_regions[i].getEdge(j));
                       }
                   }

                    newFaces.push_back(VoronoiFace(evenEdges));

//                   VoronoiFace evenSidedFace(evenSidedEdgeList);

//                   //Randomly generate a vertex index and weight
//                   vert = evenSidedFace.getNumVerts() * (float)rand() / (float)RAND_MAX;
//                   weight = 7.5f * (float)rand() / (float)RAND_MAX;

//                   //Adjust the weight to lie within the range (-3.75f, 3.75f)
//                   weight -= 3.75f;

//                   //And then change it to a percentage
//                   weight /= 10.0f;

//                   //Now get the weighted middle of the current voronoi face
//                   regionMiddle = evenSidedFace.getWeightedMiddle(vert, weight);

//                   for(int j = middleVertID; j < middleVertID + (evenSidedFace.getNumVerts() * 2); j += 4)
//                   {
//                       if(j == middleVertID)
//                       {
//                           //Iterate through the current face's verts
//                           std::vector<QVector3D> edgeList;

//                           //Create the index separately so we can
//                           //check to ensure it is within the range
//                           int j2 = clampIndex(j,       evenSidedFace.getNumVerts());
//                           int v2 = clampIndex(j + 2, evenSidedFace.getNumVerts());
//                           int v3 = clampIndex(j + 4, evenSidedFace.getNumVerts());
//                           int v4 = clampIndex(j + 6, evenSidedFace.getNumVerts());

//                           //Now create a triangle
//                           edgeList.push_back(evenSidedFace.getVertex(j2));
//                           edgeList.push_back(evenSidedFace.getVertex(v2));

//                           edgeList.push_back(evenSidedFace.getVertex(v2));
//                           edgeList.push_back(evenSidedFace.getVertex(v3));

//                           edgeList.push_back(evenSidedFace.getVertex(v3));
//                           edgeList.push_back(evenSidedFace.getVertex(v4));

//                           edgeList.push_back(evenSidedFace.getVertex(v4));
//                           edgeList.push_back(evenSidedFace.getVertex(middleVertID));

//                           edgeList.push_back(evenSidedFace.getVertex(middleVertID));
//                           edgeList.push_back(evenSidedFace.getVertex(j2));

//                           //Create a new voronoi face with these edges and
//                           //add it to our container
//                           newFaces.push_back(VoronoiFace(edgeList));

//                           j+= 2;
//                       }
//                       else if(j == middleVertID + (evenSidedFace.getNumVerts() * 2) - 6)
//                       {
//                           //Iterate through the current face's verts
//                           std::vector<QVector3D> edgeList;

//                           //Create the index separately so we can
//                           //check to ensure it is within the range
//                           int j2 = clampIndex(j,       evenSidedFace.getNumVerts());
//                           int v2 = clampIndex(j + 2, evenSidedFace.getNumVerts());
//                           int v3 = clampIndex(j + 4, evenSidedFace.getNumVerts());
//                           int v4 = clampIndex(j + 6, evenSidedFace.getNumVerts());

//                           //Now create a triangle
//                           edgeList.push_back(evenSidedFace.getVertex(j2));
//                           edgeList.push_back(evenSidedFace.getVertex(v2));

//                           edgeList.push_back(evenSidedFace.getVertex(v2));
//                           edgeList.push_back(evenSidedFace.getVertex(v3));

//                           edgeList.push_back(evenSidedFace.getVertex(v3));
//                           edgeList.push_back(evenSidedFace.getVertex(v4));

//                           edgeList.push_back(evenSidedFace.getVertex(v4));
//                           edgeList.push_back(evenSidedFace.getVertex(middleVertID));

//                           edgeList.push_back(evenSidedFace.getVertex(middleVertID));
//                           edgeList.push_back(evenSidedFace.getVertex(j2));

//                           //Create a new voronoi face with these edges and
//                           //add it to our container
//                           newFaces.push_back(VoronoiFace(edgeList));

//                           j+= 2;
//                       }
//                       else
//                       {
//                           //Iterate through the current face's verts
//                           std::vector<QVector3D> edgeList;

//                           //Create the index separately so we can
//                           //check to ensure it is within the range
//                           int j2 = clampIndex(j,       evenSidedFace.getNumVerts());
//                           int v2 = clampIndex(j + 2, evenSidedFace.getNumVerts());
//                           int v3 = clampIndex(j + 4, evenSidedFace.getNumVerts());

//                           //Now create a triangle
//                           edgeList.push_back(evenSidedFace.getVertex(j2));
//                           edgeList.push_back(evenSidedFace.getVertex(v2));

//                           edgeList.push_back(evenSidedFace.getVertex(v2));
//                           edgeList.push_back(evenSidedFace.getVertex(v3));

//                           edgeList.push_back(evenSidedFace.getVertex(v3));
//                           edgeList.push_back(evenSidedFace.getVertex(middleVertID));

//                           edgeList.push_back(evenSidedFace.getVertex(middleVertID));
//                           edgeList.push_back(evenSidedFace.getVertex(j2));

//                           //Create a new voronoi face with these edges and
//                           //add it to our container
//                           newFaces.push_back(VoronoiFace(edgeList));
//                       }
//                   }
                }*/
            }
        }
/*
//        if(m_regions[i].getNumVerts() > 3)
//        {
//            if((int)m_regions[i].getNumVerts() % 2 == 0)
//            {
//                //Randomly generate a vertex index and weight
//                int vert = m_regions[i].getNumVerts() * (float)rand() / (float)RAND_MAX;
//                float weight = 7.5f * (float)rand() / (float)RAND_MAX;

//                //Adjust the weight to lie within the range (-3.75f, 3.75f)
//                weight -= 3.75f;

//                //And then change it to a percentage
//                weight /= 10.0f;

//                //Now get the weighted middle of the current voronoi face
//                QVector3D regionMiddle = m_regions[i].getWeightedMiddle(vert, weight);

//                for(int j = 0; j < m_regions[i].getNumVerts() * 2; j += 4)
//                {
//                        //Iterate through the current face's verts
//                        std::vector<QVector3D> edgeList;

//                        //Create the index separately so we can
//                        //check to ensure it is within the range
//                        int v2 = clampIndex(j + 2, m_regions[i].getNumVerts());
//                        int v3 = clampIndex(j + 4, m_regions[i].getNumVerts());

//                        //Now create a triangle
//                        edgeList.push_back(m_regions[i].getVertex(j));
//                        edgeList.push_back(m_regions[i].getVertex(v2));

//                        edgeList.push_back(m_regions[i].getVertex(v2));
//                        edgeList.push_back(m_regions[i].getVertex(v3));

//                        edgeList.push_back(m_regions[i].getVertex(v3));
//                        edgeList.push_back(regionMiddle);

//                        edgeList.push_back(regionMiddle);
//                        edgeList.push_back(m_regions[i].getVertex(j));

//                        //Create a new voronoi face with these edges and
//                        //add it to our container
//                        newFaces.push_back(VoronoiFace(edgeList));
//                }
//            }
//            else
//            {
//                int subdivideSwitch = 100 * (float)rand() / (float)RAND_MAX;

//                if(subdivideSwitch > 49)
//                {
//                    newFaces.push_back(m_regions[i]);
//                }
//                else
//                {
//                    //Randomly generate a vertex index and weight
//                    int vert = m_regions[i].getNumVerts() * (float)rand() / (float)RAND_MAX;
//                    float weight = 7.5f * (float)rand() / (float)RAND_MAX;

//                    //Adjust the weight to lie within the range (-3.75f, 3.75f)
//                    weight -= 3.75f;

//                    //And then change it to a percentage
//                    weight /= 10.0f;

//                    int vertToSkip = m_regions[i].getNumVerts() * (float)rand()/(float)RAND_MAX;

//                    //Now get the weighted middle of the current voronoi face
//                    QVector3D regionMiddle = m_regions[i].getWeightedMiddle(vert, weight);

//                    //Iterate through the current face's verts
//                    std::vector<QVector3D> evenSidedEdgeList;
//                    int middleVertID = 0;

//                   for(int j = 0; j < m_regions[i].getNumVerts() * 2; j += 2)
//                   {

//                       if(j == (vertToSkip * 2) || j == (vertToSkip * 2) - 1 ||
//                               (vertToSkip == 0 && (j == vertToSkip || j == (m_regions[i].getNumVerts() * 2) - 1)))
//                       {
//                            //Iterate through the current face's verts
//                            std::vector<QVector3D> edgeList;

//                            //Create the index separately so we can
//                            //check to ensure it is within the range
//                            int v2 = j + 2;

//                            if(v2 == m_regions[i].getNumVerts() * 2)
//                            {
//                               v2 = 0;
//                            }

//                            //Now create a triangle
//                            edgeList.push_back(m_regions[i].getVertex(j));
//                            edgeList.push_back(m_regions[i].getVertex(v2));

//                            edgeList.push_back(m_regions[i].getVertex(v2));
//                            edgeList.push_back(regionMiddle);

//                            edgeList.push_back(regionMiddle);
//                            edgeList.push_back(m_regions[i].getVertex(j));

//                            evenSidedEdgeList.push_back(m_regions[i].getVertex(j));
//                            evenSidedEdgeList.push_back(regionMiddle);

//                            middleVertID = evenSidedEdgeList.size();

//                            evenSidedEdgeList.push_back(regionMiddle);
//                            evenSidedEdgeList.push_back(m_regions[i].getVertex(v2));

//                            //Create a new voronoi face with these edges and
//                            //add it to our container
//                            newFaces.push_back(VoronoiFace(edgeList));
//                       }
//                       else
//                       {
//                           int v2 = j + 2;

//                           if(v2 == m_regions[i].getNumVerts() * 2)
//                           {
//                              v2 = 0;
//                           }

//                           evenSidedEdgeList.push_back(m_regions[i].getVertex(j));
//                           evenSidedEdgeList.push_back(m_regions[i].getVertex(v2));
//                       }
//                   }

//                   VoronoiFace evenSidedFace(evenSidedEdgeList);

//                   //Randomly generate a vertex index and weight
//                   vert = evenSidedFace.getNumVerts() * (float)rand() / (float)RAND_MAX;
//                   weight = 7.5f * (float)rand() / (float)RAND_MAX;

//                   //Adjust the weight to lie within the range (-3.75f, 3.75f)
//                   weight -= 3.75f;

//                   //And then change it to a percentage
//                   weight /= 10.0f;

//                   //Now get the weighted middle of the current voronoi face
//                   regionMiddle = evenSidedFace.getWeightedMiddle(vert, weight);

//                   for(int j = middleVertID; j < middleVertID + (evenSidedFace.getNumVerts() * 2); j += 4)
//                   {
//                       if(j == middleVertID)
//                       {
//                           //Iterate through the current face's verts
//                           std::vector<QVector3D> edgeList;

//                           //Create the index separately so we can
//                           //check to ensure it is within the range
//                           int j2 = clampIndex(j,       evenSidedFace.getNumVerts());
//                           int v2 = clampIndex(j + 2, evenSidedFace.getNumVerts());
//                           int v3 = clampIndex(j + 4, evenSidedFace.getNumVerts());
//                           int v4 = clampIndex(j + 6, evenSidedFace.getNumVerts());

//                           //Now create a triangle
//                           edgeList.push_back(evenSidedFace.getVertex(j2));
//                           edgeList.push_back(evenSidedFace.getVertex(v2));

//                           edgeList.push_back(evenSidedFace.getVertex(v2));
//                           edgeList.push_back(evenSidedFace.getVertex(v3));

//                           edgeList.push_back(evenSidedFace.getVertex(v3));
//                           edgeList.push_back(evenSidedFace.getVertex(v4));

//                           edgeList.push_back(evenSidedFace.getVertex(v4));
//                           edgeList.push_back(evenSidedFace.getVertex(middleVertID));

//                           edgeList.push_back(evenSidedFace.getVertex(middleVertID));
//                           edgeList.push_back(evenSidedFace.getVertex(j2));

//                           //Create a new voronoi face with these edges and
//                           //add it to our container
//                           newFaces.push_back(VoronoiFace(edgeList));

//                           j+= 2;
//                       }
//                       else if(j == middleVertID + (evenSidedFace.getNumVerts() * 2) - 6)
//                       {
//                           //Iterate through the current face's verts
//                           std::vector<QVector3D> edgeList;

//                           //Create the index separately so we can
//                           //check to ensure it is within the range
//                           int j2 = clampIndex(j,       evenSidedFace.getNumVerts());
//                           int v2 = clampIndex(j + 2, evenSidedFace.getNumVerts());
//                           int v3 = clampIndex(j + 4, evenSidedFace.getNumVerts());
//                           int v4 = clampIndex(j + 6, evenSidedFace.getNumVerts());

//                           //Now create a triangle
//                           edgeList.push_back(evenSidedFace.getVertex(j2));
//                           edgeList.push_back(evenSidedFace.getVertex(v2));

//                           edgeList.push_back(evenSidedFace.getVertex(v2));
//                           edgeList.push_back(evenSidedFace.getVertex(v3));

//                           edgeList.push_back(evenSidedFace.getVertex(v3));
//                           edgeList.push_back(evenSidedFace.getVertex(v4));

//                           edgeList.push_back(evenSidedFace.getVertex(v4));
//                           edgeList.push_back(evenSidedFace.getVertex(middleVertID));

//                           edgeList.push_back(evenSidedFace.getVertex(middleVertID));
//                           edgeList.push_back(evenSidedFace.getVertex(j2));

//                           //Create a new voronoi face with these edges and
//                           //add it to our container
//                           newFaces.push_back(VoronoiFace(edgeList));

//                           j+= 2;
//                       }
//                       else
//                       {
//                           //Iterate through the current face's verts
//                           std::vector<QVector3D> edgeList;

//                           //Create the index separately so we can
//                           //check to ensure it is within the range
//                           int j2 = clampIndex(j,       evenSidedFace.getNumVerts());
//                           int v2 = clampIndex(j + 2, evenSidedFace.getNumVerts());
//                           int v3 = clampIndex(j + 4, evenSidedFace.getNumVerts());

//                           //Now create a triangle
//                           edgeList.push_back(evenSidedFace.getVertex(j2));
//                           edgeList.push_back(evenSidedFace.getVertex(v2));

//                           edgeList.push_back(evenSidedFace.getVertex(v2));
//                           edgeList.push_back(evenSidedFace.getVertex(v3));

//                           edgeList.push_back(evenSidedFace.getVertex(v3));
//                           edgeList.push_back(evenSidedFace.getVertex(middleVertID));

//                           edgeList.push_back(evenSidedFace.getVertex(middleVertID));
//                           edgeList.push_back(evenSidedFace.getVertex(j2));

//                           //Create a new voronoi face with these edges and
//                           //add it to our container
//                           newFaces.push_back(VoronoiFace(edgeList));
//                       }
//                   }
//                }
//            }*/
        }
        else
        {
//            newFaces.push_back(m_regions[i]);
        }

//        newFaces[newFaces.size() - 1].updateVerts();
    }

    //Finally update the region container
    m_regions = newFaces;
}

int EnglishFields::clampIndex(int index, int numVertices)
{
    if(index >= (numVertices * 2))
    {
        index -= (numVertices * 2);
    }

    return index;
}

int EnglishFields::edgeExists(VoronoiEdge *_edge)
{
    //Create an index count
    int count = 0;

    for(auto i = m_allEdges.begin(); i != m_allEdges.end(); ++i)
    {
        //Compare the edge passed in with the current edge in
        //the global container. The double dereference looks
        //bad but is actuall just dereferencing the iterator and
        //then dereferencing the pointer it contains
        if((*_edge) == (**i))
        {
            //The edge already exists in the global container,
            //return the count(which will be the index the edge
            //is stored at)
            return count;
        }

        //Otherwise increment the count
        ++count;
    }

    //Need to return something that is obviously not
    //a valid index value. -1 will do
    return -1;
}

int EnglishFields::vertExists(QVector3D *_vert)
{
    //Exactly the same process as with edgeExists()
    int count = 0;

    for(auto i = m_allVerts.begin(); i != m_allVerts.end(); ++i)
    {
        if((*_vert) == (**i))
        {
            return count;
        }

        ++count;
    }

    return -1;
}
