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
//        srand(time(NULL));
        srand(123);

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
            int subdivideSwitch = 10 * (float)rand() / (float)RAND_MAX;

            if((int)(m_regions[i].getNumEdges()) % 2 == 0)
            {
                //We're on an even face, check our switch
                if(subdivideSwitch > 10)
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
                if(subdivideSwitch > 10)
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
    std::vector<EdgeToUpdate> edgesNeedUpdating;

    std::vector< std::vector<VoronoiEdge*> > allNewEdges;

    std::vector<VoronoiFace> newFaces;// = m_regions;

    for(int i = 0; i < m_regions.size(); ++i)
    {
        m_regions[i].checkUsable();

        if(m_regions[i].m_isUsable)
        {
            m_regions[i].updateVerts();

            std::vector<VoronoiEdge*> edges = m_regions[i].getEdges();

            QVector3D perpVector = QVector3D::crossProduct((*(m_regions[i].getEdge(0)->m_startPTR)) - (*(m_regions[i].getEdge(0)->m_endPTR)), QVector3D(0,1,0));
            perpVector.normalize();

            VoronoiEdge* furthestEdge = m_regions[i].getEdge(0);

            //        furthestEdge->m_startPTR->setY(5);

            float distance = -1000000;

            QVector3D middleVector = m_regions[i].getMiddle() - m_regions[i].getEdge(0)->getMidPoint();

            float middleDistance = QVector3D::dotProduct(middleVector, perpVector + m_regions[i].getEdge(0)->getMidPoint());
            middleDistance /= perpVector.length();

            perpVector = QVector3D::crossProduct((*(m_regions[i].getEdge(0)->m_startPTR)) - (*(m_regions[i].getEdge(0)->m_endPTR)), QVector3D(0,1,0));
            perpVector.normalize();

            if(middleDistance < 0)
            {
                perpVector *= -1;
            }

            for(auto j = edges.begin(); j != edges.end(); ++j)
            {
                QVector3D startToMid = (*(*j)->m_startPTR) - m_regions[i].getEdge(0)->getMidPoint();
                QVector3D endToMid = (*(*j)->m_endPTR) - m_regions[i].getEdge(0)->getMidPoint();

                QVector3D relativeVector = startToMid;

                if(endToMid.length() > startToMid.length())
                {
                    relativeVector = endToMid;
                }

                float tmpDistance = QVector3D::dotProduct(relativeVector, perpVector);

                tmpDistance /= perpVector.length();

                if(tmpDistance > distance)
                {
                    distance = tmpDistance;
                    furthestEdge = (*j);
                }
            }

            std::vector< QVector3D > intersections;

            std::vector< std::vector<int> > intersectedEdges;
            intersectedEdges.resize(edges.size(), std::vector<int>(0));

            QVector3D startEdge = *(m_regions[i].getEdge(0)->m_startPTR);
            startEdge.setY(0);

            QVector3D endEdge = *(m_regions[i].getEdge(0)->m_endPTR);
            endEdge.setY(0);

            QVector3D edgeVector = endEdge - startEdge;
            edgeVector.normalize();

            startEdge -= ((m_width / 2) * edgeVector);
            endEdge += ((m_width / 2) * edgeVector);

            perpVector.setY(0);

            for(int x = 1; x < 50; x+=2)
            {
                if((x * perpVector).length() > distance)
                {
                    break;
                }

                QVector3D* newStart = new QVector3D(startEdge + (x * perpVector));
                QVector3D* newEnd = new QVector3D(endEdge + (x * perpVector));

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

                VoronoiEdge* tmp = new VoronoiEdge(newStart, newEnd);

                int edgeCount = 0;

                for(auto j = edges.begin() + 1; j != edges.end(); ++j)
                {
                    QVector3D intersection = tmp->intersectEdge(*j);

                    if(intersection != QVector3D(1000000.0f, 0.0f, 1000000.0f))
                    {
                        intersections.push_back(intersection);
                        intersectedEdges[edgeCount].push_back(intersections.size() - 1);
                    }

                    ++edgeCount;
                }
            }

            if(intersections.size() > 0)
            {
                for(uint j = 0; j < intersections.size(); j+=2)
                {
                    QVector3D* start = new QVector3D(intersections[j]);
                    QVector3D* end = new QVector3D(intersections[j + 1]);

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

                    edges.push_back(newEdge);
                }

                for(uint j = 0; j < intersectedEdges.size(); ++j)
                {
                    bool edgeIsReversed = false;

                    std::vector<VoronoiEdge*> newEdges;

                    for(uint k = 0; k < intersectedEdges[j].size(); ++k)
                    {
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
                            float lengthToStart = (*start - *m_regions[i].getEdge(j)->m_startPTR).length();
                            float lengthToEnd = (*start - *m_regions[i].getEdge(j)->m_endPTR).length();

                            QVector3D* lastPoint = m_regions[i].getEdge(j)->m_startPTR;

                            if(lengthToEnd < lengthToStart)
                            {
                                edgeIsReversed = true;
                                lastPoint = m_regions[i].getEdge(j)->m_endPTR;
                            }

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

//                            newEdge->m_startPTR->setY(5);
                            newEdges.push_back(newEdge);

//                            if(edgeIsReversed)
//                            {
//                                VoronoiEdge* updateEdge1 = new VoronoiEdge(m_regions[i].getEdge(j)->m_endPTR, start);
//                                VoronoiEdge* updateEdge2 = new VoronoiEdge(start, m_regions[i].getEdge(j)->m_startPTR);

//                                ID = edgeExists(updateEdge1);

//                                if(ID != -1)
//                                {
//                                    updateEdge1 = m_allEdges[ID];
//                                }
//                                else
//                                {
//                                    m_allEdges.push_back(updateEdge1);
//                                }

//                                ID = edgeExists(updateEdge2);

//                                if(ID != -1)
//                                {
//                                    updateEdge2 = m_allEdges[ID];
//                                }
//                                else
//                                {
//                                    m_allEdges.push_back(updateEdge2);
//                                }

//                                edgesNeedUpdating.push_back(EdgeToUpdate(remainingEdge, updateEdge1, updateEdge2));
//                            }
//                            else
//                            {
//                                VoronoiEdge* updateEdge1 = new VoronoiEdge(m_regions[i].getEdge(j)->m_startPTR, start);
//                                VoronoiEdge* updateEdge2 = new VoronoiEdge(start, m_regions[i].getEdge(j)->m_endPTR);

//                                ID = edgeExists(updateEdge1);

//                                if(ID != -1)
//                                {
//                                    updateEdge1 = m_allEdges[ID];
//                                }
//                                else
//                                {
//                                    m_allEdges.push_back(updateEdge1);
//                                }

//                                ID = edgeExists(updateEdge2);

//                                if(ID != -1)
//                                {
//                                    updateEdge2 = m_allEdges[ID];
//                                }
//                                else
//                                {
//                                    m_allEdges.push_back(updateEdge2);
//                                }

//                                edgesNeedUpdating.push_back(EdgeToUpdate(remainingEdge, updateEdge1, updateEdge2));
//                            }
                        }
                        else
                        {
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

                            newEdge->m_startPTR->setY(5);
//                            newEdges.push_back(newEdge);
                        }

//                        newEdges[newEdges.size() - 1]->m_startPTR->setY(5);
//                        newEdges[newEdges.size() - 1]->m_endPTR->setY(5);
//                        else
//                        {
//                            QVector3D* previousStart = new QVector3D(intersections[intersectedEdges[j][k - 1]]);

//                            ID = vertExists(previousStart);

//                            if(ID != -1)
//                            {
//                                previousStart = m_allVerts[ID];
//                            }
//                            else
//                            {
//                                m_allVerts.push_back(previousStart);
//                            }

//                            VoronoiEdge* lastUpdatedEdge = NULL;

//                            if(edgesNeedUpdating.size() > 0)
//                            {
//                                lastUpdatedEdge = edgesNeedUpdating[edgesNeedUpdating.size() - 1].newEdge_2;
//                            }
//                            else
//                            {
//                                if(edgeIsReversed)
//                                {
//                                    lastUpdatedEdge = new VoronoiEdge(previousStart, m_regions[i].getEdge(j)->m_startPTR);
//                                }
//                                else
//                                {
//                                    lastUpdatedEdge = new VoronoiEdge(previousStart, m_regions[i].getEdge(j)->m_endPTR);
//                                }
//                            }
//                                    //NULL;



//                            ID = edgeExists(lastUpdatedEdge);

//                            if(ID != -1)
//                            {
//                                lastUpdatedEdge = m_allEdges[ID];
//                            }
//                            else
//                            {
//                                m_allEdges.push_back(lastUpdatedEdge);
//                            }

//                            float lengthToStart = (*previousStart - *lastUpdatedEdge->m_startPTR).length();
//                            float lengthToEnd = (*previousStart - *lastUpdatedEdge->m_endPTR).length();

//                            if(lengthToEnd < lengthToStart)
//                            {
//                                edgeIsReversed = true;
//                            }
//                            else
//                            {
//                                edgeIsReversed = false;
//                            }

//                            if(edgeIsReversed)
//                            {
//                                VoronoiEdge* updateEdge1 = new VoronoiEdge(lastUpdatedEdge->m_endPTR, start);
//                                VoronoiEdge* updateEdge2 = new VoronoiEdge(start, lastUpdatedEdge->m_startPTR);

//                                edgesNeedUpdating.push_back(EdgeToUpdate(lastUpdatedEdge, updateEdge1, updateEdge2));
//                            }
//                            else
//                            {
//                                VoronoiEdge* updateEdge1 = new VoronoiEdge(lastUpdatedEdge->m_startPTR, start);
//                                VoronoiEdge* updateEdge2 = new VoronoiEdge(start, lastUpdatedEdge->m_endPTR);

//                                edgesNeedUpdating.push_back(EdgeToUpdate(lastUpdatedEdge, updateEdge1, updateEdge2));
//                            }
//                        }
                    }

                    allNewEdges.push_back(newEdges);
                    newFaces.push_back(VoronoiFace(newEdges));
                }
            }

            QVector3D* str = new QVector3D(m_regions[i].getEdge(0)->getMidPoint());
            QVector3D* ending = new QVector3D((3 * perpVector) + (*str));

            edges.push_back(new VoronoiEdge(str, ending));

            m_regions[i] = VoronoiFace(edges);
        }
        else
        {
            qInfo()<<"Face "<<i<<" is not usable";
        }
    }

    for(uint i = 0; i < edgesNeedUpdating.size(); ++i)
    {
         updateFaces(edgesNeedUpdating[i].oldEdge, edgesNeedUpdating[i].newEdge_1, edgesNeedUpdating[i].newEdge_2);
    }

    m_regions = newFaces;
}
