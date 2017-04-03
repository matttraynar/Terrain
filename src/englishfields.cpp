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

    makeFieldFeatures();
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
//    while(!m_allEdges.empty())
//    {
//        delete (m_allEdges[0]);
//        m_allEdges.erase(m_allEdges.begin());
//    }
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
        qInfo()<<"Seed: "<<time(NULL);
//        srand(1491206802);

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
        if((m_regions[i].getNumEdges()) > 3)
        {            
            int subdivideSwitch = 100 * (float)rand() / (float)RAND_MAX;

            if((int)(m_regions[i].getNumEdges()) % 2 == 0)
            {
                //We're on an even face, check our switch
                if(subdivideSwitch > 70)
                {
                    //This branch calculates a weighted middle for the center of the face
                    //and then uses this point to create new edges (which in turn are used
                    //to create new faces). Generally not used as actually looks a little too
                    //messy (can happen more often by decreasing the value needed to
                    //activate the branch

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
                        newFaces.push_back(VoronoiFace(edges));
                    }
                }
                else
                {
                    //Otherwise we just duplicate the even-sided face
                    newFaces.push_back(m_regions[i]);
                }
            }
            else
            {
                //This branch means we're on an odd-sided face
                if(subdivideSwitch > 70)
                {
                    //Sometimes just duplicate the current face
                    newFaces.push_back(m_regions[i]);
                }
                else
                {
                    //Otherwise first get the index of the 'middle' edge.
                    //For a face with edges {0,1,2,3,4} this ID is 2
                    int startID = (m_regions[i].getNumEdges() - 1) / 2;

                    //FIRST NEW FACE -------------------------
                    //Create a container for storing edge references
                    std::vector<VoronoiEdge*> edges;

                    //Add the neccesary edges which already exist to the
                    //container. Taking our example this will be edges {0,1}
                    for(int j = 0; j < startID; ++j)
                    {
                        edges.push_back(m_regions[i].getEdge(j));
                    }

                    //Now we get the start vertex of our 'middle' edge (the end vertex
                    //of our current list of edges)
                    QVector3D* endVert = m_regions[i].getEdge(startID)->m_startPTR;

                    //Because we're using references there is no way to be sure that
                    //all edges on this face are aligned {start->end} -- {start->end}. We
                    //can check this because the vertex should be equal to either the
                    //start or end vertex of the previous edge
                    if((*endVert) != *(m_regions[i].getEdge(startID - 1)->m_startPTR) &&
                       (*endVert) != *(m_regions[i].getEdge(startID - 1)->m_endPTR))
                    {
                        //If it is not equal then we know that (in this frame of reference)
                        //the m_startPTR actually points to the *end* of the edge and
                        //m_endPTR actually points to the *start*. So update the end vert
                        endVert = m_regions[i].getEdge(startID)->m_endPTR;
                    }

                    //Next we get the ID of the last edge. With our example this
                    //ID will be set to 4
                    int endID = m_regions[i].getNumEdges() - 1;

                    //This is a similar process to getting endVert, only this time we're
                    //getting the vertex the current edge list starts at
                    QVector3D* startVert = m_regions[i].getEdge(0)->m_startPTR;

                    if((*startVert == *(m_regions[i].getEdge(1)->m_startPTR)) ||
                      (*startVert == *(m_regions[i].getEdge(1)->m_endPTR)))
                    {
                        startVert = m_regions[i].getEdge(0)->m_endPTR;
                    }

                    //Now we create a new vertex which lies halfway along the last edge (edge number 4)
                    QVector3D* newVert = new QVector3D((*(m_regions[i].getEdge(endID)->m_startPTR) + *(m_regions[i].getEdge(endID)->m_endPTR)) / 2.0f);

                    //Because we're using references we don't want to duplicate vertices
                    //so first we check if it already exists
                    int vertID = vertExists(newVert);

                    //If it does we update the pointer, otherwise we add it to our container
                    if(vertID != -1)
                    {
                        newVert = m_allVerts[vertID];
                    }
                    else
                    {
                        m_allVerts.push_back(newVert);
                    }

                    //Now we need to create a new edge. This will run from the end of our current
                    //edge list to the midpoint we just calculated
                    VoronoiEdge* newEdge = new VoronoiEdge(endVert, newVert);

                    //Again, we only want one edge instance, so check whether it exists
                    //already and act accordingly
                    int edgeID = edgeExists(newEdge);

                    if(edgeID != -1)
                    {
                        newEdge = m_allEdges[edgeID];
                    }
                    else
                    {
                        m_allEdges.push_back(newEdge);
                    }

                    //Finally add the edge to our container
                    edges.push_back(newEdge);

                    //Next we must add the joining edge from the midpoint to the start
                    //vertex of our edge list
                    VoronoiEdge* newEdge2 = new VoronoiEdge(newVert, startVert);

                    //Find out if our edge already exists or not (simply overwriting a variable)
                    edgeID = edgeExists(newEdge2);

                    if(edgeID != -1)
                    {
                        newEdge2 = m_allEdges[edgeID];
                    }
                    else
                    {
                        m_allEdges.push_back(newEdge2);
                    }

                    //And add it to our edge list
                    edges.push_back(newEdge2);

                    //The edge list is now complete so we create a new face using it
                    //and add it to our face container
                    newFaces.push_back(VoronoiFace(edges));

                    //Now we need to repeat the process but this time using the edges
                    //on the other 'half'. With our example this is edges {2,3} (remember
                    //edge 4 is being bisected)

                    //First clear our container so we can reuse it
                    edges.clear();

                    //Add the existing edges again (note the for loop range has changed)
                    for(int j = startID; j < endID; ++j)
                    {
                        edges.push_back(m_regions[i].getEdge(j));
                    }

                    //Calculate the end vertex of this new edge list
                    QVector3D* endVert2 = m_regions[i].getEdge(endID)->m_startPTR;

                    //Check the value again to make sure it is correct
                    if((*endVert2 != *(m_regions[i].getEdge(endID - 1)->m_startPTR)) &&
                       (*endVert2 != *(m_regions[i].getEdge(endID - 1)->m_endPTR)))
                    {
                        endVert2 = m_regions[i].getEdge(endID)->m_endPTR;
                    }

                    //Now create a new edge from the end of our edge list to the
                    //point midway along edge 4
                    VoronoiEdge* newEdge3 = new VoronoiEdge(endVert2, newVert);

                    //Check again for duplicates
                    edgeID = edgeExists(newEdge3);

                    if(edgeID != -1)
                    {
                        newEdge3 = m_allEdges[edgeID];
                    }
                    else
                    {
                        m_allEdges.push_back(newEdge3);
                    }

                    //Add the new edge
                    edges.push_back(newEdge3);

                    //Because we created the edge between the mid point and the start
                    //of edge 2 we can duplicate it here. Note this is an example of where
                    //we do not know the direction of the edge. Our faces are defined in
                    //a clockwise order where the first face had edges:
                    //          {0,1, newEdge, newEdge2}
                    //and this new face will have edges:
                    //          {2,3,newEdge3,newEdge}
                    //Note that the first face traverses 'newEdge' from start->end and the
                    //second face traverses 'newEdge' from end->start
                    edges.push_back(newEdge);

                    //Time to create a new face with these edges and add it to our container
                    newFaces.push_back(VoronoiFace(edges));

                    m_updaterEdges.push_back(EdgeToUpdate(m_regions[i].getEdge(endID), newEdge3, newEdge2));
                }
            }
        }
        else
        {
            newFaces.push_back(m_regions[i]);
        }
    }

    //Finally update the region container
    m_regions = newFaces;

    if(m_updaterEdges.size() > 0)
    {
        for(auto i = m_updaterEdges.begin(); i != m_updaterEdges.end(); ++i)
        {
            updateFaces(i->oldEdge, i->newEdge_1, i->newEdge_2);
        }
    }
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

int EnglishFields::edgeExists(VoronoiEdge *_edge, std::vector<VoronoiEdge *> _edges)
{    //Create an index count
    int count = 0;

    for(auto i = _edges.begin(); i != _edges.end(); ++i)
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

void EnglishFields::updateFaces(VoronoiEdge *_prev, VoronoiEdge *_e1, VoronoiEdge *_e2)
{
    int edgeID = -1;

    for(uint i = 0; i < m_regions.size(); ++i)
    {
        edgeID = edgeExists(_prev, m_regions[i].getEdges());

        if(edgeID != -1)
        {
            m_regions[i].updateEdge(edgeID, _e1, _e2);
            edgeID = -1;
        }
    }

    if(edgeID != -1)
    {
        edgeID = edgeExists(_prev);

        m_allEdges.erase(m_allEdges.begin() + edgeID);
    }

}

void EnglishFields::makeFieldFeatures()
{
    //A container for storing our newly edited faces in
    std::vector<VoronoiFace> newFaces;

    for(int i = 0; i < m_regions.size(); ++i)
    {
        int ridgeSwitch = 100 * (float)rand() / (float)RAND_MAX;

        if(ridgeSwitch > 70)
        {
            //Run a quick function which deems whether the face has enough
            //area to make a conviincing field
            m_regions[i].checkUsable();

            //If the face is usable do some stuff
            if(m_regions[i].m_isUsable)
            {
                //Not sure if needed but always good to ensure the verts are updated
                m_regions[i].updateVerts();

                //Get the current container of edges for this face
                std::vector<VoronoiEdge*> edges = m_regions[i].getEdges();

                //Calculate the perpendicular vector to the first edge on the face
                QVector3D perpVector = QVector3D::crossProduct((*(m_regions[i].getEdge(0)->m_startPTR)) - (*(m_regions[i].getEdge(0)->m_endPTR)), QVector3D(0,1,0));
                perpVector.normalize();

                //Create a variable to store the edge which is furthest away.
                //Set it to Edge 0 (this will change)
                VoronoiEdge* furthestEdge = m_regions[i].getEdge(0);

                //Set a very small distance which is likely to be overwritten later
                float distance = -1000000;

                //Calculate a vector between the midpoint of Edge 0 and the center of the face
                QVector3D middleVector = m_regions[i].getMiddle() - m_regions[i].getEdge(0)->getMidPoint();

                //Now we calculate the projected length of this vector on the perpendicular vector
                float middleDistance = QVector3D::dotProduct(middleVector, perpVector);// + m_regions[i].getEdge(0)->getMidPoint());
                middleDistance /= perpVector.length();

                //If this distance is negative then the perpendicular vector is facing out from the face,
                //reverse it by doing *-1
                if(middleDistance < 0)
                {
                    perpVector *= -1;
                }

                //Now we iterate over the face's edges
                for(auto j = edges.begin(); j != edges.end(); ++j)
                {
                    //Calculate two vectors, one from the current edge's start vertex to Edge 0's midpoint and then another
                    //from the edge's end vertex to the midpoint. This is essentially the process of joining each vertex
                    //in the face to Edge 's midpoint
                    QVector3D startToMid = (*(*j)->m_startPTR) - m_regions[i].getEdge(0)->getMidPoint();
                    QVector3D endToMid = (*(*j)->m_endPTR) - m_regions[i].getEdge(0)->getMidPoint();

                    //Now we get the longest of the two vectors. This will be the connection between the midpoint
                    //and the furthest vertex from that point
                    QVector3D relativeVector = startToMid;

                    if(endToMid.length() > startToMid.length())
                    {
                        relativeVector = endToMid;
                    }

                    //Calculate the projected length of our joining vector on the perpendicular vector
                    float tmpDistance = QVector3D::dotProduct(relativeVector, perpVector);
                    tmpDistance /= perpVector.length();

                    //If we've calculated a larger distance than currently stored update the value
                    //and store a reference to the edge with the furthest vertex on it
                    if(tmpDistance > distance)
                    {
                        distance = tmpDistance;
                        furthestEdge = (*j);
                    }
                }

                //Now we create two new containers: one for storing intersection points
                //and another for storing the indices of the intersected edges and their
                //corresponding intersections
                std::vector< QVector3D > intersections;

                //This one is resized to have the same size as the number of edges,
                //this will be important later
                std::vector< std::vector<int> > intersectedEdges;
                intersectedEdges.resize(edges.size(), std::vector<int>(0));

                //First get the two vertices of Edge 0
                QVector3D startEdge = *(m_regions[i].getEdge(0)->m_startPTR);
                QVector3D endEdge = *(m_regions[i].getEdge(0)->m_endPTR);

                //Calculate the direction vector of this edge
                QVector3D edgeVector = endEdge - startEdge;
                edgeVector.normalize();

                //We add this direction vector on to create a large line segment.
                //This is done so that when we check for intersections later this
                //segment is almost guaranteed to intersect (if it intersects at all)
                startEdge -= ((m_width / 2) * edgeVector);
                endEdge += ((m_width / 2) * edgeVector);

                //Just make sure there's no height values in the perpendicular vector
                perpVector.setY(0);

                std::vector<VoronoiEdge*> intersectionSegments;

                //Now we are going to create our ridge and furrows
                for(int x = 1; x < 50; x+=2)
                {
                    //If the current x position is further away from Edge 0 than the
                    //furthest away vertex we can stop creating ridge and furrows
                    if((x * perpVector).length() > distance)
                    {
                        break;
                    }

                    //We create two new vertices which will dictate a new edge. The edge
                    //will be parallell to Edge 0 and moves further away as the loop continues
                    QVector3D* newStart = new QVector3D(startEdge + (x * perpVector));
                    QVector3D* newEnd = new QVector3D(endEdge + (x * perpVector));

                    //Do the standard to check to see if we already have references to these
                    //two points
                    int vertID = vertExists(newStart);

                    if(vertID != -1)
                    {
                        newStart = m_allVerts[vertID];
                    }
                    else
                    {
                        m_allVerts.push_back(newStart);
                    }

                    vertID = vertExists(newEnd);

                    if(vertID != -1)
                    {
                        newEnd = m_allVerts[vertID];
                    }
                    else
                    {
                        m_allVerts.push_back(newEnd);
                    }

                    //Now we create a new line segment/edge with these verts
                    VoronoiEdge* tmp = new VoronoiEdge(newStart, newEnd);

                    intersectionSegments.push_back(tmp);

                    //Start a counter, this will be used to store edge indices
                    int edgeCount = 1;

                    //Iterate through the face's edges. Note that we start at index
                    //1 because the new line segment is parallel to Edge 0 and
                    //we don't need to check for intersections there
                    for(auto j = edges.begin() + 1; j != edges.end(); ++j)
                    {
                        //Calculate any line segment intersection between the new
                        //segment and the current edge
                        QVector3D intersection = tmp->intersectEdge(*j);

                        //The above function returns an obviously wrong 3D point
                        //if there was no intersection so we can check if there
                        //actually was one or not
                        if(intersection != QVector3D(1000000.0f, 0.0f, 1000000.0f))
                        {
                            //Add the intersection to our container
                            intersections.push_back(intersection);

                            //And store the index of the new intersection in the
                            //corresponding container for the current edge
                            intersectedEdges[edgeCount].push_back(intersections.size() - 1);
                        }

                        //Increment the edge count regardless of intersection or not
                        ++edgeCount;
                    }
                }

                //Ridge and furrows are completed, now check if we need to
                //deal with any intersections
                qInfo()<<"Face "<<i<<" has "<<intersections.size()<<" intersections";
                if(intersections.size() > 0)
                {
                    //We're going to create a new edge list, as we don't need
                    //the one containing the face edges any more we can reuse it
                    edges.clear();

                    //Make sure we've added Edge 0 first though
                    edges.push_back(m_regions[i].getEdge(0));

                    //                QVector3D* lastIntersectionStart = m_regions[i].getEdge(0)->m_startPTR;
                    //                float lastSeparation = 0.0f;

                    //Iterate through our intersections
                    for(uint j = 0; j < intersections.size() - 1; j+=2)
                    {
                        //Because we are using line segment intersection (instead of normal
                        //line intersection) we know there will only be two intersections
                        //per ridge/furrow. This means we can create a new edge by just using
                        //two intersections which are next to each other in the container
                        QVector3D* start = new QVector3D(intersections[j]);
                        QVector3D* end = new QVector3D(intersections[j + 1]);

                        //                    if(j == 0)
                        //                    {
                        //                        lastSeparation = (*start - *(m_regions[i].getEdge(0)->m_startPTR)).length();

                        //                        lastIntersectionStart = m_regions[i].getEdge(0)->m_startPTR;

                        //                        if((*start - *(m_regions[i].getEdge(0)->m_endPTR)).length() < lastSeparation)
                        //                        {
                        //                            lastSeparation = (*start - *(m_regions[i].getEdge(0)->m_endPTR)).length();

                        //                            lastIntersectionStart = m_regions[i].getEdge(0)->m_endPTR;
                        //                        }

                        //                        qInfo()<<"Last: "<<lastSeparation;
                        //                    }
                        //                    else if(j != 0)
                        //                    {
                        //                        float thisSeparation = (*start - *lastIntersectionStart).length();

                        //                        if(thisSeparation > 3.0f* lastSeparation)
                        //                        {
                        //                            qInfo()<<"BREAKING";
                        //                            qInfo()<<"Last: "<<lastSeparation<<" This: "<<thisSeparation;
                        //                            edges.clear();
                        //                            edges = m_regions[i].getEdges();
                        //                            intersectedEdges.clear();

                        //                            break;
                        //                        }
                        //                        else
                        //                        {
                        //                            lastSeparation = thisSeparation;
                        //                        }
                        //                    }

                        //Standard check for pre-existing references
                        int ID = vertExists(start);

                        if(ID != -1)
                        {
                            start = m_allVerts[ID];
                        }
                        else
                        {
                            m_allVerts.push_back(start);
                        }

                        ID = vertExists(end);

                        if(ID != -1)
                        {
                            end = m_allVerts[ID];
                        }
                        else
                        {
                            m_allVerts.push_back(end);
                        }

                        //Create a new edge with our points and check for references
                        VoronoiEdge* newEdge = new VoronoiEdge(start, end);

                        ID = edgeExists(newEdge);

                        if(ID != -1)
                        {
                            newEdge = m_allEdges[ID];
                        }
                        else
                        {
                            m_allEdges.push_back(newEdge);
                        }

                        //Add the new edge to our container
                        edges.push_back(newEdge);
                    }

                    //Now we need to update the pre-existing edges that we've added
                    //vertices to (at the intersection point)
                    for(uint j = 0; j < intersectedEdges.size(); ++j)
                    {
                        //A boolean value used to mark if the edge actually goes
                        //from endPTR---->startPTR instead of startPTR---->endPTR
                        bool edgeIsReversed = false;

                        if(intersectedEdges[j].size() == 0)
                        {
                            //If the current container has no size then this means the current
                            //edge had no intersections on it, we need to just add the
                            //unedited edge to our container
                            edges.push_back(m_regions[i].getEdge(j));
                        }
                        else if(intersectedEdges[j].size() == 1)
                        {
                            //This means the current edge only had one intersection,
                            //so we need to construct the two new edges
                            QVector3D* start = new QVector3D(intersections[intersectedEdges[j][0]]);

                            int ID = vertExists(start);

                            if(ID != -1)
                            {
                                start = m_allVerts[ID];
                            }
                            else
                            {
                                m_allVerts.push_back(start);
                            }

                            //Consturct and edge from the current edge's startPTR to the intersection
                            VoronoiEdge* newEdge = new VoronoiEdge(m_regions[i].getEdge(j)->m_startPTR, start);
                            ID = edgeExists(newEdge);

                            if(ID != -1)
                            {
                                newEdge = m_allEdges[ID];
                            }
                            else
                            {
                                m_allEdges.push_back(newEdge);
                            }

                            //Add it to the container
                            edges.push_back(newEdge);

                            //And now a new edge from the intersection point to the current edge's endPTR
                            VoronoiEdge* newEdge2 = new VoronoiEdge(start, m_regions[i].getEdge(j)->m_endPTR);
                            ID = edgeExists(newEdge2);

                            if(ID != -1)
                            {
                                newEdge2 = m_allEdges[ID];
                            }
                            else
                            {
                                m_allEdges.push_back(newEdge2);
                            }

                            //FInally add the second new edge to the container
                            edges.push_back(newEdge2);

                        }
                        else
                        {
                            //This means the current edge container has more than
                            //one intersection in it, so we iterate through
                            for(uint k = 0; k < intersectedEdges[j].size(); ++k)
                            {
                                //Get the intersection point using the index we stored in this container
                                QVector3D* start = new QVector3D(intersections[intersectedEdges[j][k]]);

                                int ID = vertExists(start);

                                if(ID != -1)
                                {
                                    start = m_allVerts[ID];
                                }
                                else
                                {
                                    m_allVerts.push_back(start);
                                }

                                if(k == 0)
                                {
                                    //This is our first new edge segment. We first need to figure out the direction
                                    //of the edge (start--->end or end--->start). This is done by getting the length
                                    //from each vertex to our current intersection point.
                                    float lengthToStart = (*start - *m_regions[i].getEdge(j)->m_startPTR).length();
                                    float lengthToEnd = (*start - *m_regions[i].getEdge(j)->m_endPTR).length();

                                    //This is the default for the start vertex
                                    QVector3D* startPoint = m_regions[i].getEdge(j)->m_startPTR;

                                    if(lengthToEnd < lengthToStart)
                                    {
                                        //If the distance from the intersection is actually shorter towards
                                        //the end vertex then the direction is end--->start. Mark this using
                                        //the boolean and update the start vertex for our edge
                                        edgeIsReversed = true;
                                        startPoint = m_regions[i].getEdge(j)->m_endPTR;
                                    }

                                    //Create a new edge from the edge's start to our intersection
                                    VoronoiEdge* newEdge = new VoronoiEdge(startPoint, start);
                                    ID = edgeExists(newEdge);

                                    if(ID != -1)
                                    {
                                        newEdge = m_allEdges[ID];
                                    }
                                    else
                                    {
                                        m_allEdges.push_back(newEdge);
                                    }

                                    //After checking for references add it to our container
                                    edges.push_back(newEdge);
                                }
                                else if(k != intersectedEdges[j].size() - 1)
                                {
                                    //This case takes care of all new edges up until the final one.
                                    //First we get the position of the last intersection in our container
                                    QVector3D* lastPoint = new QVector3D(intersections[intersectedEdges[j][k - 1]]);

                                    ID = vertExists(lastPoint);

                                    if(ID != -1)
                                    {
                                        lastPoint = m_allVerts[ID];
                                    }
                                    else
                                    {
                                        m_allVerts.push_back(lastPoint);
                                    }

                                    //Then we create an edge between the last intersection and
                                    //the current one
                                    VoronoiEdge* newEdge = new VoronoiEdge(lastPoint, start);

                                    ID = edgeExists(newEdge);

                                    if(ID != -1)
                                    {
                                        newEdge = m_allEdges[ID];
                                    }
                                    else
                                    {
                                        m_allEdges.push_back(newEdge);
                                    }

                                    //And finally add it to the container
                                    edges.push_back(newEdge);
                                }
                                else
                                {
                                    //This case takes care of the last new edge we need to add.
                                    //Similar to the above case we get the position of the last intersection
                                    QVector3D* lastPoint = new QVector3D(intersections[intersectedEdges[j][k - 1]]);

                                    ID = vertExists(lastPoint);

                                    if(ID != -1)
                                    {
                                        lastPoint = m_allVerts[ID];
                                    }
                                    else
                                    {
                                        m_allVerts.push_back(lastPoint);
                                    }

                                    //This time though we set the end point and the end point of the current edge
                                    VoronoiEdge* newEdge = new VoronoiEdge(lastPoint, m_regions[i].getEdge(j)->m_endPTR);

                                    //If the edge direction is reversed (end--->start) the 'end point' is actually stored
                                    //in the startPTR, so update new change to reflect this
                                    if(edgeIsReversed)
                                    {
                                        newEdge = new VoronoiEdge(lastPoint, m_regions[i].getEdge(j)->m_startPTR);
                                    }

                                    ID = edgeExists(newEdge);

                                    if(ID != -1)
                                    {
                                        newEdge = m_allEdges[ID];
                                    }
                                    else
                                    {
                                        m_allEdges.push_back(newEdge);
                                    }

                                    //Add the edge to the container
                                    edges.push_back(newEdge);
                                }
                            }
                        }

                    }
                }
                else
                {
                    qInfo()<<"Got a distance of "<<distance;
                    newFaces.push_back(VoronoiFace(intersectionSegments));
                }
                //Code which adds a small line showing the perpendicular
                //vector we've been using
                //            QVector3D* str = new QVector3D(m_regions[i].getEdge(0)->getMidPoint());
                //            QVector3D* ending = new QVector3D((3 * perpVector) + (*str));

                //            edges.push_back(new VoronoiEdge(str, ending));

                //And now we add a new face with our update edges.
                newFaces.push_back(VoronoiFace(edges));
            }
            else
            {
                //The field isn't usable, print and simply add the unedited face
                //to our container
                qInfo()<<"Face "<<i<<" is not usable";
                newFaces.push_back(m_regions[i]);
            }
        }
        else
        {
            //The field isn't usable, print and simply add the unedited face
            //to our container
            qInfo()<<"Switched off face "<<i;
            newFaces.push_back(m_regions[i]);
        }
    }

    //Now update our regions container to have the faces
    //with the updated edges in it
    m_regions = newFaces;

    for(uint i = 0; i < m_regions.size(); ++i)
    {
        m_regions[i].updateVerts();
    }
}
