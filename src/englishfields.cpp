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
    m_maxDisplacementIterations = 3;

//    makeVoronoiDiagram(time(NULL));
    makeVoronoiDiagram(1492340143);

    subdivide();
    editEdges();

//    makeFieldFeatures();
}

EnglishFields::EnglishFields(double _width,
                                          std::vector<QVector3D> _sites)
{
    //Set up and create the voronoi, this time with the given
    //input sites
    m_width = _width;
    m_sites = _sites;

    makeVoronoiDiagram(time(NULL));
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

//Field generation
void EnglishFields::makeVoronoiDiagram(int _seed)
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
        srand(_seed);
        qInfo()<<"Seed: "<<_seed;

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
        std::vector<uint> edgeIDs;

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
                edgeIDs.push_back(edgeID);
            }
            else
            {
                //Add the new edge to our global container and the container
                //for the current voronoi face
                m_allEdges.push_back(newEdge);
                edges.push_back(newEdge);
                edgeIDs.push_back(m_allEdges.size() - 1);
            }

            //Code added by me --------------END-------------------

        }while(currentEdge != end);

        //Add a new voronoi region and store it
//        m_regions.push_back(VoronoiFace(edges));
        m_regions.push_back(VoronoiFace(edgeIDs));
    }

    for(uint i = 0; i < m_regions.size(); ++i)
    {
        m_regions[i].loadVerts(m_allEdges);
        m_regions[i].storeOriginalEdges(m_allEdges);
    }
}

void EnglishFields::subdivide()
{
    //Container for storing our edited faces in
    std::vector<uint> subdividedFaces;

    int startFaceCount = m_regions.size();

    for(uint i = 0; i < startFaceCount; ++i)
    {
        if((m_regions[i].getEdgeCount()) > 3)
        {
            float subdivideSwitch = 100.0f * (float)rand() / (float)RAND_MAX;

            if((int)(m_regions[i].getEdgeCount()) % 2 == 0)
            {
                qInfo()<<"IGNORING "<<i;
                //We're on an even face, check our switch
                if(subdivideSwitch > 0.0f)
                {
                    subdividedFaces.push_back(i);
                    //This branch calculates a weighted middle for the center of the face
                    //and then uses this point to create new edges (which in turn are used
                    //to create new faces). Generally not used as actually looks a little too
                    //messy (can happen more often by decreasing the value needed to
                    //activate the branch

                    //Randomly generate a vertex index and weight
                    int vert = m_regions[i].getEdgeCount() * (float)rand() / (float)RAND_MAX;
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

                    //Even sided face {0,1,2,3}
                    int edgeCount = m_regions[i].getEdgeCount();
                    for(int j = 0; j < edgeCount; j += 2)
                    {
                        //Iterate through the current face's verts
                        std::vector<uint> edgeIDs;
                        std::vector<uint> newEdgeIDs;

                        int v2 = clampIndex(j + 1, edgeCount);

                        //Get the first two edges (loop 1: {0, 1})
                        //Now create a triangle
                        edgeIDs.push_back(m_regions[i].getEdgeID(j));
                        edgeIDs.push_back(m_regions[i].getEdgeID(v2));

                        newEdgeIDs.push_back(m_regions[i].getEdgeID(j));
                        newEdgeIDs.push_back(m_regions[i].getEdgeID(v2));

                        //Get the start of the loop (beginning of edge 0)
                        VoronoiEdge* startReference = m_allEdges[m_regions[i].getEdgeID(j)];
                        VoronoiEdge* nextReference = m_allEdges[m_regions[i].getEdgeID(v2)];

                        QVector3D* edgeStart = startReference->m_startPTR;

                        if((*edgeStart) == *(nextReference->m_startPTR) ||
                           (*edgeStart) == *(nextReference->m_endPTR)    )
                        {
                            edgeStart = startReference->m_endPTR;
                        }

                        //Get the end of the loop (end of edge 1)
                        QVector3D* edgeEnd = nextReference->m_startPTR;

                        if((*edgeEnd) == *(startReference->m_startPTR) ||
                           (*edgeEnd) == *(startReference->m_endPTR)    )
                        {
                            edgeEnd = nextReference->m_endPTR;
                        }

                        //Create an edge from the end of the loop to the middle
                        VoronoiEdge* newEdge = new VoronoiEdge(edgeEnd, regionMiddle);

                        int edgeID = edgeExists(newEdge);

                        if(edgeID != -1)
                        {
                            edgeIDs.push_back(edgeID);
                            newEdgeIDs.push_back(edgeID);
                        }
                        else
                        {
                            m_allEdges.push_back(newEdge);
                            edgeIDs.push_back(m_allEdges.size() - 1);
                            newEdgeIDs.push_back(m_allEdges.size() - 1);
                        }

                        //And another from the middle to the start of the loop
                        VoronoiEdge* newEdge2 = new VoronoiEdge(regionMiddle, edgeStart);

                        edgeID = edgeExists(newEdge2);

                        if(edgeID != -1)
                        {
                            edgeIDs.push_back(edgeID);
                            newEdgeIDs.push_back(edgeID);
                        }
                        else
                        {
                            m_allEdges.push_back(newEdge2);
                            edgeIDs.push_back(m_allEdges.size() - 1);
                            newEdgeIDs.push_back(m_allEdges.size() - 1);
                        }

                        //Create a new voronoi face with these edges and
                        //add it to our container
                        m_regions.push_back(VoronoiFace(edgeIDs));
                    }
                }
            }
            else
            {
                //This branch means we're on an odd-sided face
                if(subdivideSwitch < 70.0f)
                {
                    subdividedFaces.push_back(i);
                    std::vector<uint> newEdgeIDs;

                    qInfo()<<"Subdividing "<<i;
                    //Otherwise first get the index of the 'middle' edge.
                    //For a face with edges {0,1,2,3,4} this ID is 2
                    int startID = (m_regions[i].getEdgeCount() - 1) / 2;

                    if(!checkContains(m_regions[i].getEdgeID(startID), m_editedEdgeIDs))
                    {
                        //FIRST NEW FACE -------------------------
                        std::vector<uint> newIDs;

                        //Add the neccesary edges which already exist to the
                        //container. Taking our example this will be edges {0,1}
                        for(int j = 0; j < startID; ++j)
                        {
                            newIDs.push_back(m_regions[i].getEdgeID(j));
                        }

                        //Now we get the start vertex of our 'middle' edge (the end vertex
                        //of our current list of edges)
                        VoronoiEdge* previousEdgeReference = m_allEdges[m_regions[i].getEdgeID(startID) - 1];
                        VoronoiEdge* middleEdgeReference = m_allEdges[m_regions[i].getEdgeID(startID)];

                        QVector3D* endVert = middleEdgeReference->m_startPTR;

                        //Because we're using references there is no way to be sure that
                        //all edges on this face are aligned {start->end} -- {start->end}. We
                        //can check this because the vertex should be equal to either the
                        //start or end vertex of the previous edge
                        if((*endVert) != *(previousEdgeReference->m_startPTR) &&
                           (*endVert) != *(previousEdgeReference->m_endPTR))
                        {
                            //If it is not equal then we know that (in this frame of reference)
                            //the m_startPTR actually points to the *end* of the edge and
                            //m_endPTR actually points to the *start*. So update the end vert
                            endVert = middleEdgeReference->m_endPTR;
                        }

                        //Next we get the ID of the last edge. With our example this
                        //ID will be set to 4
                        int endID = m_regions[i].getEdgeCount() - 1;

                        //This is a similar process to getting endVert, only this time we're
                        //getting the vertex the current edge list starts at
                        VoronoiEdge* startEdgeReference = m_allEdges[m_regions[i].getEdgeID(0)];
                        VoronoiEdge* secondEdgeReference = m_allEdges[m_regions[i].getEdgeID(1)];
                        VoronoiEdge* endEdgeReference = m_allEdges[m_regions[i].getEdgeID(endID)];

                        QVector3D* startVert = startEdgeReference->m_startPTR;

                        if((*startVert == *(secondEdgeReference->m_startPTR)) ||
                          (*startVert == *(secondEdgeReference->m_endPTR)))
                        {
                            startVert = startEdgeReference->m_endPTR;
                        }

                        //Now we create a new vertex which lies halfway along the last edge (edge number 4)
                        QVector3D* newVert = new QVector3D((*(endEdgeReference->m_startPTR) + *(endEdgeReference->m_endPTR)) / 2.0f);

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
                            newIDs.push_back(edgeID);
                        }
                        else
                        {
                            m_allEdges.push_back(newEdge);
                            newIDs.push_back(m_allEdges.size() - 1);
                        }

                        //Next we must add the joining edge from the midpoint to the start
                        //vertex of our edge list
                        VoronoiEdge* newEdge2 = new VoronoiEdge(newVert, startVert);

                        //Find out if our edge already exists or not (simply overwriting a variable)
                        edgeID = edgeExists(newEdge2);

                        if(edgeID != -1)
                        {
                            newIDs.push_back(edgeID);
                            newEdgeIDs.push_back(edgeID);
                        }
                        else
                        {
                            m_allEdges.push_back(newEdge2);
                            newIDs.push_back(m_allEdges.size() - 1);
                            newEdgeIDs.push_back(m_allEdges.size() - 1);
                        }

                        //The edge list is now complete so we create a new face using it
                        //and add it to our face container
                        m_regions.push_back(VoronoiFace(newIDs));

                        //Now we need to repeat the process but this time using the edges
                        //on the other 'half'. With our example this is edges {2,3} (remember
                        //edge 4 is being bisected)

                        //First clear our container so we can reuse it
                        newIDs.clear();

                        //Add the existing edges again (note the for loop range has changed)
                        for(int j = startID; j < endID; ++j)
                        {
                            newIDs.push_back(m_regions[i].getEdgeID(j));
                        }

                        //Calculate the end vertex of this new edge list
                        QVector3D* endVert2 = endEdgeReference->m_startPTR;
                        VoronoiEdge* penultimateEdgeReference = m_allEdges[m_regions[i].getEdgeID(endID - 1)];

                        //Check the value again to make sure it is correct
                        if((*endVert2 != *(penultimateEdgeReference->m_startPTR)) &&
                           (*endVert2 != *(penultimateEdgeReference->m_endPTR)))
                        {
                            endVert2 = endEdgeReference->m_endPTR;
                        }

                        //Now create a new edge from the end of our edge list to the
                        //point midway along edge 4
                        VoronoiEdge* newEdge3 = new VoronoiEdge(endVert2, newVert);

                        //Check again for duplicates
                        edgeID = edgeExists(newEdge3);

                        if(edgeID != -1)
                        {
                            newIDs.push_back(edgeID);
                            newEdgeIDs.push_back(edgeID);
                        }
                        else
                        {
                            m_allEdges.push_back(newEdge3);
                            newIDs.push_back(m_allEdges.size() - 1);
                            newEdgeIDs.push_back(m_allEdges.size() - 1);
                        }

                        //Because we created the edge between the mid point and the start
                        //of edge 2 we can duplicate it here. Note this is an example of where
                        //we do not know the direction of the edge. Our faces are defined in
                        //a clockwise order where the first face had edges:
                        //          {0,1, newEdge, newEdge2}
                        //and this new face will have edges:
                        //          {2,3,newEdge3,newEdge}
                        //Note that the first face traverses 'newEdge' from start->end and the
                        //second face traverses 'newEdge' from end->start

                        edgeID = edgeExists(newEdge);
                        newIDs.push_back(edgeID);

                        //Time to create a new face with these edges and add it to our container
                        m_regions.push_back(VoronoiFace(newIDs));

                        updateEdge(m_regions[i].getEdgeID(endID), newEdgeIDs);
                        m_editedEdgeIDs.push_back(m_regions[i].getEdgeID(endID));
                    }
                }
            }
        }
    }

    for(uint i = 0; i < m_editedEdgeIDs.size(); ++i)
    {
        removeEdge(m_editedEdgeIDs[i]);

        for(uint j = 0; j < m_editedEdgeIDs.size(); ++j)
        {
            if(m_editedEdgeIDs[j] > m_editedEdgeIDs[i])
            {
                m_editedEdgeIDs[j]--;
            }
        }
    }

    m_editedEdgeIDs.clear();

    if(subdividedFaces.size() > 0)
    {
        for(uint i = 0; i < subdividedFaces.size(); ++i)
        {
            m_regions.erase(m_regions.begin() + subdividedFaces[i]);

            for(uint j = 0; j < subdividedFaces.size(); ++j)
            {
                if(subdividedFaces[j] > subdividedFaces[i])
                {
                    subdividedFaces[j]--;
                }
            }

        }
    }

    for(uint i = 0; i < m_regions.size(); ++i)
    {
        m_regions[i].loadVerts(m_allEdges);
        m_regions[i].storeOriginalEdges(m_allEdges);
    }
}

void EnglishFields::editEdges()
{
    int startFaceCount = m_regions.size();

    for(int i = 0; i < startFaceCount; ++i)
    {
        qInfo()<<"Current Face: "<<i;

        displaceEdge(m_regions[i]);
    }

    for(uint i = 0; i < m_editedEdgeIDs.size(); ++i)
    {
        removeEdge(m_editedEdgeIDs[i]);

        for(uint j = 0; j < m_editedEdgeIDs.size(); ++j)
        {
            if(m_editedEdgeIDs[j] > m_editedEdgeIDs[i])
            {
                m_editedEdgeIDs[j]--;
            }
        }
    }

    m_editedEdgeIDs.clear();

    qInfo()<<"Finished updating";

    for(uint i = 0; i < m_regions.size(); ++i)
    {
        m_regions[i].loadVerts(m_allEdges);
    }

    qInfo()<<"Verts loaded";

    for(int i = 0; i < startFaceCount; ++i)
    {
        if(i != 5)
        {
            continue;
        }
        threeField(m_regions[i]);
    }

    //Container for storing our edited faces in
    std::vector<uint> subdividedFaces;

    for(uint i = 0; i < m_regions.size(); ++i)
    {
        m_regions[i].loadVerts(m_allEdges);
    }


}

void EnglishFields::displaceEdge(VoronoiFace &_face)
{
    int startEdgeCount = _face.getEdgeCount();

    std::vector<uint> updatedEdgeIDs;

    for(uint i = 0; i < startEdgeCount; ++i)
    {
        if(!checkContains(_face.getEdgeID(i), m_editedEdgeIDs))
        {
            VoronoiEdge* currentEdge = m_allEdges[_face.getEdgeID(i)];

            if(!isBoundaryEdge(currentEdge))
            {
                midPointEdge(m_allEdges[_face.getEdgeID(i)], 1, updatedEdgeIDs);
            }
            else
            {
                midPointEdge(m_allEdges[_face.getEdgeID(i)], m_maxDisplacementIterations + 1, updatedEdgeIDs);
            }

            updateEdge(_face.getEdgeID(i), updatedEdgeIDs);
            updatedEdgeIDs.clear();

            m_editedEdgeIDs.push_back(_face.getEdgeID(i));
        }
    }
}

void EnglishFields::midPointEdge(VoronoiEdge* edge, int iteration, std::vector<uint> &_newIDs)
{
    if(iteration > m_maxDisplacementIterations)
    {
        m_allEdges.push_back(edge);
        _newIDs.push_back(m_allEdges.size() - 1);
        return;
    }

    if(edge->getLength() <1.0f)
    {
        m_allEdges.push_back(edge);
        _newIDs.push_back(m_allEdges.size() - 1);
        return;
    }

    PerlinNoise noise;

    QVector3D* midPoint = new QVector3D(edge->getMidPoint());

    float yValue = float(_newIDs.size())/float(2.0f * m_maxDisplacementIterations);

    while(yValue > 0.99f)
    {
        yValue /= 2.0f;
    }

    float noiseValue = (noise.noise(float(iteration) / float(m_maxDisplacementIterations), yValue, 0.0f) - 0.5f);

//    qInfo()<<"Iteration: "<<iteration<<" YValue: "<<yValue<<" Noise: "<<noiseValue;

    midPoint->setX((((4.0f / float(iteration)) * (double)rand()/(double)RAND_MAX) - (4.0f / float(iteration))/2.0f) + midPoint->x());
    midPoint->setX(((2.0f * midPoint->x()) + noiseValue) / 2.0f);

    midPoint->setZ((((4.0f  / float(iteration))* (double)rand()/(double)RAND_MAX) - (4.0f / float(iteration))/2.0f) + midPoint->z());
    midPoint->setZ(((2.0f *midPoint->z()) + noiseValue) / 2.0f);

    midPointEdge(new VoronoiEdge(edge->m_startPTR, midPoint), iteration + 1, _newIDs);
    midPointEdge(new VoronoiEdge(midPoint, edge->m_endPTR), iteration + 1, _newIDs);
}

void EnglishFields::threeField(VoronoiFace &_face)
{
    QVector3D* middleVert = new QVector3D(_face.getMiddle());

    //Check for references
    int ID = vertExists(middleVert);

    if(ID != -1)
    {
        middleVert = m_allVerts[ID];
    }
    else
    {
        m_allVerts.push_back(middleVert);
    }

    QVector3D* edgeMidPoint = new QVector3D(_face.getOriginalEdge(0)->getMidPoint());

    VoronoiEdge* middleEdge = new VoronoiEdge(edgeMidPoint, middleVert);

    //Set up some variables we'll use in the loop below
    VoronoiEdge* halfEdge = NULL;
    VoronoiEdge* edgeToUse = middleEdge;
    int edgeIndex = 0;
    bool usesStart = false;

    //Iterate through the face edges
    for(uint i = 0; i < _face.getOriginalEdgeCount() * 2; ++i)
    {
        //We're testing each edge twice, easy way to do this
        //is to check for odd/even edges
        if(i % 2 == 0)
        {
            //Get a new edge from the current edge midpoint to the middle of the face
            middleEdge = new VoronoiEdge(new QVector3D(_face.getOriginalEdge(i/2)->getMidPoint()), middleVert);

            //And the edge from the midpoint to the start vertex of the current edge
            halfEdge = new VoronoiEdge(new QVector3D(_face.getOriginalEdge(i/2)->getMidPoint()), new QVector3D(*(_face.getOriginalEdge(i/2)->m_startPTR)));

            usesStart = true;
        }
        else
        {
            //This version uses the half edge from the midpoint to the end vertex of the current edge
            halfEdge = new VoronoiEdge(new QVector3D(_face.getOriginalEdge(i/2)->getMidPoint()), new QVector3D(*(_face.getOriginalEdge(i/2)->m_endPTR)));

            usesStart = false;
        }

        //Get the angle between the edge to the middle of the face and
        //the half edge we're using
        float angle = halfEdge->getAngle(middleEdge);

        //Convert to degrees
        angle *= 180.0f/3.141516;

        //Check if the angle is within a certain range and the current vector between the middle
        //vertex and the current midpoint is larger than the value we currently have
        if((angle > 45.0f && angle < 135.0f) && (middleEdge->getLength() > edgeToUse->getLength()))
        {
            //Update the longest edge we've stored
            edgeToUse = new VoronoiEdge(new QVector3D(_face.getOriginalEdge(i/2)->getMidPoint()), middleVert);

            //And the index of the edge this 'new' edge starts from
            edgeIndex = i/2;
        }
    }

    //Now we're going to check for an intersection with our 'middle edge'
    for(uint i = 0; i < _face.getOriginalEdgeCount(); ++i)
    {
        //Ignore the edge that our 'middleEdge' starts from
        if(i == edgeIndex)
        {
            continue;
        }

        //Check for any intersections with our middle edge
        QVector3D intersection = edgeToUse->intersectEdge(_face.getOriginalEdge(i));

        if(intersection != QVector3D(1000000.0f, 0.0f, 1000000.0f))
        {
            //If there was an intersection then this edge and face aren't viable
            qInfo()<<"Edge "<<edgeIndex<<" intersects with "<<i;
            return;
        }
    }

    qInfo()<<"Found a good edge at: "<<edgeIndex;

    edgeToUse->m_startPTR = new QVector3D(*(edgeToUse->m_startPTR) - (10 * edgeToUse->getDirection()));

    int middleIntersection = -1;
    //Now we'll check for intersections
    for(uint i = 0; i < _face.getEdgeCount(); ++i)
    {
        //Get intersection points
        QVector3D intersection = edgeToUse->intersectEdge(m_allEdges[_face.getEdgeID(i)]);

        if(intersection != QVector3D(1000000.0f, 0.0f, 1000000.0f))
        {
            //This means there was an intersection. Because we're using
            //line segments for edges this will be the only intersection (if at all)
            edgeToUse->m_startPTR = new QVector3D(intersection);
            middleIntersection = i;

            //Which means we can break out from our edge search
            break;
        }
    }

    //Now we're going to create two new end points which stem from the middle vertex and create
    //edges parallel to the chosen edge
    QVector3D* newStartPoint = new QVector3D(*(_face.getOriginalEdge(edgeIndex)->m_startPTR) + (_face.getMiddle() - _face.getOriginalEdge(edgeIndex)->getMidPoint()));
    QVector3D* newEndPoint = new QVector3D(*(_face.getOriginalEdge(edgeIndex)->m_endPTR) + (_face.getMiddle() - _face.getOriginalEdge(edgeIndex)->getMidPoint()));

    //Create our first new edge
    VoronoiEdge* fieldEdge = new VoronoiEdge(middleVert, newStartPoint);

    //Move the end point of the edge so that it is likely to intersect with the edges of the face
    fieldEdge->m_endPTR = new QVector3D(*(fieldEdge->m_endPTR) + (fieldEdge->getDirection() * (m_width / 2.0f)));

    int intersect_1 = -1;

    //Now we'll check for intersections
    for(uint i = 0; i < _face.getEdgeCount(); ++i)
    {
        //Get intersection points
        QVector3D intersection = fieldEdge->intersectEdge(m_allEdges[_face.getEdgeID(i)]);

        if(intersection != QVector3D(1000000.0f, 0.0f, 1000000.0f))
        {
            //This means there was an intersection. Because we're using
            //line segments for edges this will be the only intersection (if at all)
            fieldEdge->m_endPTR = new QVector3D(intersection);
            intersect_1 = i;

            //Which means we can break out from our edge search
            break;
        }
    }

    //Now we repeat the process, only using the second point we created
    VoronoiEdge* fieldEdge2 = new VoronoiEdge(middleVert, newEndPoint);

    fieldEdge2->m_endPTR = new QVector3D(*(fieldEdge2->m_endPTR) + (fieldEdge2->getDirection() * (m_width / 2.0f)));

    int intersect_2 = -1;

    for(uint i = 0; i < _face.getEdgeCount(); ++i)
    {
        QVector3D intersection = fieldEdge2->intersectEdge(m_allEdges[_face.getEdgeID(i)]);

        if(intersection != QVector3D(1000000.0f, 0.0f, 1000000.0f))
        {
            fieldEdge2->m_endPTR = new QVector3D(intersection);

            intersect_2 = i;

            break;
        }
    }

    std::vector<uint> newIDs;

    VoronoiEdge* startSplit = new VoronoiEdge(m_allEdges[_face.getEdgeID(middleIntersection)]->m_startPTR, edgeToUse->m_startPTR);
    VoronoiEdge* endSplit = new VoronoiEdge(edgeToUse->m_startPTR, m_allEdges[_face.getEdgeID(middleIntersection)]->m_endPTR);

    int splitID = edgeExists(startSplit);

    if(splitID != -1)
    {
        startSplit = m_allEdges[splitID];
    }
    else
    {
        m_allEdges.push_back(startSplit);
        splitID = m_allEdges.size() - 1;
    }

    int splitID2 = edgeExists(endSplit);

    if(splitID2 != -1)
    {
        endSplit = m_allEdges[splitID2];
    }
    else
    {
        m_allEdges.push_back(endSplit);
        splitID2 = m_allEdges.size() - 1;
    }

    bool isReversed = false;

    if(startSplit->m_startPTR != m_allEdges[_face.getEdgeID(middleIntersection) - 1]->m_startPTR ||
       startSplit->m_startPTR != m_allEdges[_face.getEdgeID(middleIntersection) - 1]->m_endPTR)
    {
        isReversed = true;
    }

    qInfo()<<"Intersects: "<<intersect_1<<", "<<middleIntersection<<", "<<intersect_2;

    //Now check the middle edge for references
    int middleEdgeID = edgeExists(edgeToUse);

    if(middleEdgeID != -1)
    {
        edgeToUse = m_allEdges[middleEdgeID];
        newIDs.push_back(middleEdgeID);
    }
    else
    {
        m_allEdges.push_back(edgeToUse);
        newIDs.push_back(m_allEdges.size() - 1);
    }

    //Create a random switch value
    float threeFieldSwitch = 100.0f * (float)rand()/(float)RAND_MAX;

    threeFieldSwitch = 80.0f;

    if(threeFieldSwitch < 40.0f)
    {
        //This switch is the most popular and adds
        //both new edges to the container
        ID = vertExists(fieldEdge->m_endPTR);

        if(ID != -1)
        {
            fieldEdge->m_endPTR = m_allVerts[ID];
        }
        else
        {
            m_allVerts.push_back(fieldEdge->m_endPTR);
        }

        m_allEdges.push_back(fieldEdge);
        newIDs.push_back(m_allEdges.size() - 1);

        ID = vertExists(fieldEdge2->m_endPTR);

        if(ID != -1)
        {
            fieldEdge2->m_endPTR = m_allVerts[ID];
        }
        else
        {
            m_allVerts.push_back(fieldEdge2->m_endPTR);
        }

        m_allEdges.push_back(fieldEdge2);
        newIDs.push_back(m_allEdges.size() - 1);
    }
    else if(threeFieldSwitch < 70.0f)
    {
        //THIS SWITCH IS FINISHED

        //This only adds one new edge
        ID = vertExists(fieldEdge->m_endPTR);

        if(ID != -1)
        {
            fieldEdge->m_endPTR = m_allVerts[ID];
        }
        else
        {
            m_allVerts.push_back(fieldEdge->m_endPTR);
        }

        m_allEdges.push_back(fieldEdge);
        newIDs.push_back(m_allEdges.size() - 1);

        std::vector<uint> newFace1 = _face.getEdgeIDsInRange(intersect_1, middleIntersection);
        newFace1.erase(newFace1.begin() + newFace1.size() - 1);
        std::vector<uint> newFace2 = _face.getEdgeIDsInRange(middleIntersection, intersect_1 - 1);

        newFace1.push_back(m_allEdges.size() - 2);
        newFace1.push_back(m_allEdges.size() - 1);

        newFace2.push_back(m_allEdges.size() - 2);
        newFace2.push_back(m_allEdges.size() - 1);

        QVector3D* midpoint = new QVector3D(m_allEdges[_face.getEdgeID(intersect_1)]->getMidPoint());

        int midpointID = vertExists(midpoint);

        if(midpointID != -1)
        {
            midpoint = m_allVerts[midpointID];
        }
        else
        {
            m_allVerts.push_back(midpoint);
        }

        VoronoiEdge* splitEdge1 = new VoronoiEdge(m_allEdges[_face.getEdgeID(intersect_1)]->m_startPTR, fieldEdge->m_endPTR);
        VoronoiEdge* splitEdge2 = new VoronoiEdge(fieldEdge->m_endPTR, m_allEdges[_face.getEdgeID(intersect_1)]->m_endPTR);

        int midEdgeID = edgeExists(splitEdge1);

        if(midEdgeID != -1)
        {
            splitEdge1 = m_allEdges[midEdgeID];
        }
        else
        {
            m_allEdges.push_back(splitEdge1);
            midEdgeID = m_allEdges.size() - 1;
        }

        int midEdgeID2 = edgeExists(splitEdge2);

        if(midEdgeID2 != -1)
        {
            splitEdge2 = m_allEdges[midEdgeID2];
        }
        else
        {
            m_allEdges.push_back(splitEdge2);
            midEdgeID2 = m_allEdges.size() - 1;
        }

        if(splitEdge1->m_startPTR == m_allEdges[_face.getEdgeID(intersect_1) - 1]->m_startPTR ||
           splitEdge1->m_startPTR == m_allEdges[_face.getEdgeID(intersect_1) - 1]->m_endPTR)
        {
            newFace1.push_back(midEdgeID2);
            newFace2.push_back(midEdgeID);
        }
        else
        {
            newFace1.push_back(midEdgeID);
            newFace2.push_back(midEdgeID2);
        }

        if(isReversed)
        {
            newFace1.push_back(splitID);
            newFace2.push_back(splitID2);
        }
        else
        {
            newFace1.push_back(splitID2);
            newFace2.push_back(splitID);
        }

        m_regions.push_back(VoronoiFace(newFace1));

        m_regions.push_back(VoronoiFace(newFace2));
    }
    else
    {
//        //This edge also only adds one edge
//        ID = vertExists(fieldEdge2->m_endPTR);

//        if(ID != -1)
//        {
//            fieldEdge2->m_endPTR = m_allVerts[ID];
//        }
//        else
//        {
//            m_allVerts.push_back(fieldEdge2->m_endPTR);
//        }

//        m_allEdges.push_back(fieldEdge2);
//        newIDs.push_back(m_allEdges.size() - 1);

        //NEW
        //This only adds one new edge
        ID = vertExists(fieldEdge2->m_endPTR);

        if(ID != -1)
        {
            fieldEdge2->m_endPTR = m_allVerts[ID];
        }
        else
        {
            m_allVerts.push_back(fieldEdge2->m_endPTR);
        }

        m_allEdges.push_back(fieldEdge2);
        newIDs.push_back(m_allEdges.size() - 1);

        qInfo()<<"Getting range IDs";
        std::vector<uint> newFace1 = _face.getEdgeIDsInRange(intersect_2, middleIntersection);
        newFace1.erase(newFace1.begin() + newFace1.size() - 1);
        uint endPoint = (intersect_2 > 0) ? intersect_2 : _face.getEdgeCount() - 1;
        qInfo()<<"Middle: "<<middleIntersection<<" Intersection: "<<endPoint;
        qInfo()<<"Value: "<<_face.getEdgeID(middleIntersection)<<", "<<_face.getEdgeID(intersect_2);


        std::vector<uint> newFace2 = _face.getEdgeIDsInRange(middleIntersection, intersect_2);

        newFace2.erase(newFace2.begin() + newFace2.size() - 1);

        newFace1.push_back(m_allEdges.size() - 2);
        newFace1.push_back(m_allEdges.size() - 1);

        newFace2.push_back(m_allEdges.size() - 2);
        newFace2.push_back(m_allEdges.size() - 1);

        QVector3D* midpoint = new QVector3D(m_allEdges[_face.getEdgeID(intersect_2)]->getMidPoint());

        int midpointID = vertExists(midpoint);

        if(midpointID != -1)
        {
            midpoint = m_allVerts[midpointID];
        }
        else
        {
            m_allVerts.push_back(midpoint);
        }

        VoronoiEdge* splitEdge1 = new VoronoiEdge(m_allEdges[_face.getEdgeID(intersect_2)]->m_startPTR, fieldEdge2->m_endPTR);
        VoronoiEdge* splitEdge2 = new VoronoiEdge(fieldEdge2->m_endPTR, m_allEdges[_face.getEdgeID(intersect_2)]->m_endPTR);

        int midEdgeID = edgeExists(splitEdge1);

        if(midEdgeID != -1)
        {
            splitEdge1 = m_allEdges[midEdgeID];
        }
        else
        {
            m_allEdges.push_back(splitEdge1);
            midEdgeID = m_allEdges.size() - 1;
        }

        int midEdgeID2 = edgeExists(splitEdge2);

        if(midEdgeID2 != -1)
        {
            splitEdge2 = m_allEdges[midEdgeID2];
        }
        else
        {
            m_allEdges.push_back(splitEdge2);
            midEdgeID2 = m_allEdges.size() - 1;
        }

        if(splitEdge1->m_startPTR == m_allEdges[_face.getEdgeID(intersect_2) - 1]->m_startPTR ||
           splitEdge1->m_startPTR == m_allEdges[_face.getEdgeID(intersect_2) - 1]->m_endPTR)
        {
            newFace1.push_back(midEdgeID);
            newFace2.push_back(midEdgeID2);
        }
        else
        {
            newFace1.push_back(midEdgeID2);
            newFace2.push_back(midEdgeID);
        }

        if(isReversed)
        {
            newFace1.push_back(splitID);
            newFace2.push_back(splitID2);
        }
        else
        {
            newFace1.push_back(splitID2);
            newFace2.push_back(splitID);
        }

        m_regions.push_back(VoronoiFace(newFace1));

        m_regions.push_back(VoronoiFace(newFace2));
    }

//    m_regions.push_back(VoronoiFace(newIDs));
}

void EnglishFields::updateEdge(uint _oldID, std::vector<uint> _newIDs)
{
    for(uint i = 0; i < m_regions.size(); ++i)
    {
        if(m_regions[i].usesEdge(_oldID))
        {
            m_regions[i].replaceEdge(_oldID, _newIDs);
        }
    }
}

void EnglishFields::removeEdge(uint ID)
{
    for(uint i = 0; i < m_regions.size(); ++i)
    {
        m_regions[i].removeEdge(ID);
    }

    m_allEdges.erase(m_allEdges.begin() + ID);
}

bool EnglishFields::checkContains(uint ID, std::vector<uint> IDs)
{
    for(uint i = 0; i < IDs.size(); ++i)
    {
        if(IDs[i] == ID)
        {
            return true;
        }
    }

    return false;
}

bool EnglishFields::isBoundaryEdge(VoronoiEdge *_edge)
{
    float halfWidth = m_width / 2.0f;

    if((_edge->m_startPTR->x() == halfWidth && _edge->m_endPTR->x() == halfWidth) ||
       (_edge->m_startPTR->z() == halfWidth && _edge->m_endPTR->z() == halfWidth) ||
       (_edge->m_startPTR->x() == -halfWidth && _edge->m_endPTR->x() == -halfWidth) ||
       (_edge->m_startPTR->z() == -halfWidth && _edge->m_endPTR->z() == -halfWidth))
    {
        return true;
    }

    return false;
}







//----------------------------------------------- OLD? -----------------------------------------------//
//void EnglishFields::subdivideRegions()
//{
//    //Container for storing our edited faces in
//    std::vector<VoronoiFace> newFaces;

//    for(uint i = 0; i < m_regions.size(); ++i)
//    {
//        if((m_regions[i].getNumEdges()) > 3)
//        {
//            int subdivideSwitch = 100.0 * (float)rand() / (float)RAND_MAX;

//            if((int)(m_regions[i].getNumEdges()) % 2 == 0)
//            {
//                //We're on an even face, check our switch
//                if(subdivideSwitch > 90)
//                {
//                    //This branch calculates a weighted middle for the center of the face
//                    //and then uses this point to create new edges (which in turn are used
//                    //to create new faces). Generally not used as actually looks a little too
//                    //messy (can happen more often by decreasing the value needed to
//                    //activate the branch

//                    //Randomly generate a vertex index and weight
//                    int vert = m_regions[i].getNumEdges() * (float)rand() / (float)RAND_MAX;
//                    float weight = 7.5f * (float)rand() / (float)RAND_MAX;

//                    //Adjust the weight to lie within the range (-3.75f, 3.75f)
//                    weight -= 3.75f;

//                    //And then change it to a percentage
//                    weight /= 10.0f;

//                    //Now get the weighted middle of the current voronoi face
//                    QVector3D* regionMiddle = new QVector3D(m_regions[i].getWeightedMiddle2(vert, weight));

//                    int vertID = vertExists(regionMiddle);

//                    if(vertID != -1)
//                    {
//                        regionMiddle = m_allVerts[vertID];
//                    }
//                    else
//                    {
//                        m_allVerts.push_back(regionMiddle);
//                    }

//                    for(int j = 0; j < m_regions[i].getNumEdges(); j += 2)
//                    {
//                        //Iterate through the current face's verts
//                        std::vector<VoronoiEdge*> edges;

//                        int v2 = clampIndex(j + 1, m_regions[i].getNumEdges());

//                        //Now create a triangle
//                        edges.push_back(m_regions[i].getEdge(j));
//                        edges.push_back(m_regions[i].getEdge(v2));

//                        QVector3D* edgeStart = m_regions[i].getEdge(j)->m_startPTR;

//                        if((*edgeStart) == *(m_regions[i].getEdge(v2)->m_startPTR) ||
//                           (*edgeStart) == *(m_regions[i].getEdge(v2)->m_endPTR)    )
//                        {
//                            edgeStart = m_regions[i].getEdge(j)->m_endPTR;
//                        }

//                        QVector3D* edgeEnd = m_regions[i].getEdge(v2)->m_startPTR;

//                        if((*edgeEnd) == *(m_regions[i].getEdge(j)->m_startPTR) ||
//                           (*edgeEnd) == *(m_regions[i].getEdge(j)->m_endPTR)    )
//                        {
//                            edgeEnd = m_regions[i].getEdge(v2)->m_endPTR;
//                        }


//                        VoronoiEdge* newEdge = new VoronoiEdge(edgeEnd, regionMiddle);

//                        int edgeID = edgeExists(newEdge);

//                        if(edgeID != -1)
//                        {
//                            edges.push_back(m_allEdges[edgeID]);
//                        }
//                        else
//                        {
//                            edges.push_back(newEdge);
//                            m_allEdges.push_back(newEdge);
//                        }

//                        VoronoiEdge* newEdge2 = new VoronoiEdge(regionMiddle, edgeStart);

//                        edgeID = edgeExists(newEdge2);

//                        if(edgeID != -1)
//                        {
//                            edges.push_back(m_allEdges[edgeID]);
//                        }
//                        else
//                        {
//                            edges.push_back(newEdge2);
//                            m_allEdges.push_back(newEdge2);
//                        }

//                        //Create a new voronoi face with these edges and
//                        //add it to our container
//                        newFaces.push_back(VoronoiFace(edges));
//                    }
//                }
//                else
//                {
//                    //Otherwise we just duplicate the even-sided face
//                    newFaces.push_back(m_regions[i]);
//                }
//            }
//            else
//            {
//                //This branch means we're on an odd-sided face
//                if(subdivideSwitch > 70)
//                {
//                    //Sometimes just duplicate the current face
//                    newFaces.push_back(m_regions[i]);
//                }
//                else
//                {
//                    //Otherwise first get the index of the 'middle' edge.
//                    //For a face with edges {0,1,2,3,4} this ID is 2
//                    int startID = (m_regions[i].getNumEdges() - 1) / 2;

//                    //FIRST NEW FACE -------------------------
//                    //Create a container for storing edge references
//                    std::vector<VoronoiEdge*> edges;

//                    //Add the neccesary edges which already exist to the
//                    //container. Taking our example this will be edges {0,1}
//                    for(int j = 0; j < startID; ++j)
//                    {
//                        edges.push_back(m_regions[i].getEdge(j));
//                    }

//                    //Now we get the start vertex of our 'middle' edge (the end vertex
//                    //of our current list of edges)
//                    QVector3D* endVert = m_regions[i].getEdge(startID)->m_startPTR;

//                    //Because we're using references there is no way to be sure that
//                    //all edges on this face are aligned {start->end} -- {start->end}. We
//                    //can check this because the vertex should be equal to either the
//                    //start or end vertex of the previous edge
//                    if((*endVert) != *(m_regions[i].getEdge(startID - 1)->m_startPTR) &&
//                       (*endVert) != *(m_regions[i].getEdge(startID - 1)->m_endPTR))
//                    {
//                        //If it is not equal then we know that (in this frame of reference)
//                        //the m_startPTR actually points to the *end* of the edge and
//                        //m_endPTR actually points to the *start*. So update the end vert
//                        endVert = m_regions[i].getEdge(startID)->m_endPTR;
//                    }

//                    //Next we get the ID of the last edge. With our example this
//                    //ID will be set to 4
//                    int endID = m_regions[i].getNumEdges() - 1;

//                    //This is a similar process to getting endVert, only this time we're
//                    //getting the vertex the current edge list starts at
//                    QVector3D* startVert = m_regions[i].getEdge(0)->m_startPTR;

//                    if((*startVert == *(m_regions[i].getEdge(1)->m_startPTR)) ||
//                      (*startVert == *(m_regions[i].getEdge(1)->m_endPTR)))
//                    {
//                        startVert = m_regions[i].getEdge(0)->m_endPTR;
//                    }

//                    //Now we create a new vertex which lies halfway along the last edge (edge number 4)
//                    QVector3D* newVert = new QVector3D((*(m_regions[i].getEdge(endID)->m_startPTR) + *(m_regions[i].getEdge(endID)->m_endPTR)) / 2.0f);

//                    //Because we're using references we don't want to duplicate vertices
//                    //so first we check if it already exists
//                    int vertID = vertExists(newVert);

//                    //If it does we update the pointer, otherwise we add it to our container
//                    if(vertID != -1)
//                    {
//                        newVert = m_allVerts[vertID];
//                    }
//                    else
//                    {
//                        m_allVerts.push_back(newVert);
//                    }

//                    //Now we need to create a new edge. This will run from the end of our current
//                    //edge list to the midpoint we just calculated
//                    VoronoiEdge* newEdge = new VoronoiEdge(endVert, newVert);

//                    //Again, we only want one edge instance, so check whether it exists
//                    //already and act accordingly
//                    int edgeID = edgeExists(newEdge);

//                    if(edgeID != -1)
//                    {
//                        newEdge = m_allEdges[edgeID];
//                    }
//                    else
//                    {
//                        m_allEdges.push_back(newEdge);
//                    }

//                    //Finally add the edge to our container
//                    edges.push_back(newEdge);

//                    //Next we must add the joining edge from the midpoint to the start
//                    //vertex of our edge list
//                    VoronoiEdge* newEdge2 = new VoronoiEdge(newVert, startVert);

//                    //Find out if our edge already exists or not (simply overwriting a variable)
//                    edgeID = edgeExists(newEdge2);

//                    if(edgeID != -1)
//                    {
//                        newEdge2 = m_allEdges[edgeID];
//                    }
//                    else
//                    {
//                        m_allEdges.push_back(newEdge2);
//                    }

//                    //And add it to our edge list
//                    edges.push_back(newEdge2);

//                    //The edge list is now complete so we create a new face using it
//                    //and add it to our face container
//                    newFaces.push_back(VoronoiFace(edges));

//                    //Now we need to repeat the process but this time using the edges
//                    //on the other 'half'. With our example this is edges {2,3} (remember
//                    //edge 4 is being bisected)

//                    //First clear our container so we can reuse it
//                    edges.clear();

//                    //Add the existing edges again (note the for loop range has changed)
//                    for(int j = startID; j < endID; ++j)
//                    {
//                        edges.push_back(m_regions[i].getEdge(j));
//                    }

//                    //Calculate the end vertex of this new edge list
//                    QVector3D* endVert2 = m_regions[i].getEdge(endID)->m_startPTR;

//                    //Check the value again to make sure it is correct
//                    if((*endVert2 != *(m_regions[i].getEdge(endID - 1)->m_startPTR)) &&
//                       (*endVert2 != *(m_regions[i].getEdge(endID - 1)->m_endPTR)))
//                    {
//                        endVert2 = m_regions[i].getEdge(endID)->m_endPTR;
//                    }

//                    //Now create a new edge from the end of our edge list to the
//                    //point midway along edge 4
//                    VoronoiEdge* newEdge3 = new VoronoiEdge(endVert2, newVert);

//                    //Check again for duplicates
//                    edgeID = edgeExists(newEdge3);

//                    if(edgeID != -1)
//                    {
//                        newEdge3 = m_allEdges[edgeID];
//                    }
//                    else
//                    {
//                        m_allEdges.push_back(newEdge3);
//                    }

//                    //Add the new edge
//                    edges.push_back(newEdge3);

//                    //Because we created the edge between the mid point and the start
//                    //of edge 2 we can duplicate it here. Note this is an example of where
//                    //we do not know the direction of the edge. Our faces are defined in
//                    //a clockwise order where the first face had edges:
//                    //          {0,1, newEdge, newEdge2}
//                    //and this new face will have edges:
//                    //          {2,3,newEdge3,newEdge}
//                    //Note that the first face traverses 'newEdge' from start->end and the
//                    //second face traverses 'newEdge' from end->start
//                    edges.push_back(newEdge);

//                    //Time to create a new face with these edges and add it to our container
//                    newFaces.push_back(VoronoiFace(edges));

//                    m_updaterEdges.push_back(EdgeToUpdate(m_regions[i].getEdge(endID), newEdge3, newEdge2));
//                }
//            }
//        }
//        else
//        {
//            newFaces.push_back(m_regions[i]);
//        }
//    }

//    //Finally update the region container
//    m_regions = newFaces;

//    if(m_updaterEdges.size() > 0)
//    {
//        for(auto i = m_updaterEdges.begin(); i != m_updaterEdges.end(); ++i)
//        {
//            updateFaces(i->oldEdge, i->newEdge_1, i->newEdge_2);
//        }
//    }
//}

//void EnglishFields::makeFieldFeatures()
//{
//    //A container for storing our newly edited faces in
//    std::vector<VoronoiFace> newFaces;

//    for(int i = 0; i < m_regions.size(); ++i)
//    {

////        continue;

//        //Run a quick function which deems whether the face has enough
//        //area to make a conviincing field
//        m_regions[i].checkUsable();

//        makeOrganic(m_regions[i]);

////        if(i == 2)
////        {
////            for(uint j = 0; j < m_regions[i].getEdges().size(); ++j)
////            {
////                m_regions[i].getEdge(j)->m_startPTR->setY(2);
////            }
////        }


//        m_regions[i].m_isUsable = false;

//        if(m_regions[i].m_isUsable)
//        {
//            qInfo()<<"Face: "<<i;
//            int ridgeSwitch = 100 * (float)rand() / (float)RAND_MAX;

////            ridgeSwitch = 20;

//            if(ridgeSwitch > 50)
//            {
//                threeField(m_regions[i], newFaces);
//            }
//            else
//            {
//                newFaces.push_back(m_regions[i]);
//            }

////            //If the face is usable do some stuff
////            if(ridgeSwitch > 80)
////            {
////                qInfo()<<"Making ridge and furrows";
//////                ridgeAndFurrow(m_regions[i], newFaces);
////            }
////            else if(ridgeSwitch > 40)
////            {
////                qInfo()<<"Make three fields";
////                threeField(m_regions[i], newFaces);
////            }
////            else
////            {
////                qInfo()<<"Leaving face as is";
//////                newFaces.push_back(m_regions[i]);
////            }
//        }
//        else
//        {
//            //The field isn't usable, print and simply add the unedited face
//            //to our container
//            qInfo()<<"Switched off face "<<i;
//            makeOrganic(m_regions[i]);
//            newFaces.push_back(m_regions[i]);
//        }
//    }

//    //Now update our regions container to have the faces
//    //with the updated edges in it
//    m_regions = newFaces;

//    for(uint i = 0; i < m_regions.size(); ++i)
//    {
//        m_regions[i].updateVerts();
//    }
//}

//void EnglishFields::ridgeAndFurrow(VoronoiFace face, std::vector<VoronoiFace> &_facesToUpdate)
//{
//    //Not sure if needed but always good to ensure the verts are updated
//    face.updateVerts();

//    //Get the current container of edges for this face
//    std::vector<VoronoiEdge*> edges = face.getEdges();

//    //Calculate the perpendicular vector to the first edge on the face
//    QVector3D perpVector = QVector3D::crossProduct((*(face.getEdge(0)->m_startPTR)) - (*(face.getEdge(0)->m_endPTR)), QVector3D(0,1,0));
//    perpVector.normalize();

//    //Create a variable to store the edge which is furthest away.
//    //Set it to Edge 0 (this will change)
//    VoronoiEdge* furthestEdge = face.getEdge(0);

//    //Set a very small distance which is likely to be overwritten later
//    float distance = -1000000;

//    //Calculate a vector between the midpoint of Edge 0 and the center of the face
//    QVector3D middleVector = face.getMiddle() - face.getEdge(0)->getMidPoint();

//    //Now we calculate the projected length of this vector on the perpendicular vector
//    float middleDistance = QVector3D::dotProduct(middleVector, perpVector);// + m_regions[i].getEdge(0)->getMidPoint());
//    middleDistance /= perpVector.length();

//    //If this distance is negative then the perpendicular vector is facing out from the face,
//    //reverse it by doing *-1
//    if(middleDistance < 0)
//    {
//        perpVector *= -1;
//    }

//    //Now we iterate over the face's edges
//    for(auto j = edges.begin(); j != edges.end(); ++j)
//    {
//        //Calculate two vectors, one from the current edge's start vertex to Edge 0's midpoint and then another
//        //from the edge's end vertex to the midpoint. This is essentially the process of joining each vertex
//        //in the face to Edge 's midpoint
//        QVector3D startToMid = (*(*j)->m_startPTR) - face.getEdge(0)->getMidPoint();
//        QVector3D endToMid = (*(*j)->m_endPTR) - face.getEdge(0)->getMidPoint();

//        //Now we get the longest of the two vectors. This will be the connection between the midpoint
//        //and the furthest vertex from that point
//        QVector3D relativeVector = startToMid;

//        if(endToMid.length() > startToMid.length())
//        {
//            relativeVector = endToMid;
//        }

//        //Calculate the projected length of our joining vector on the perpendicular vector
//        float tmpDistance = QVector3D::dotProduct(relativeVector, perpVector);
//        tmpDistance /= perpVector.length();

//        //If we've calculated a larger distance than currently stored update the value
//        //and store a reference to the edge with the furthest vertex on it
//        if(tmpDistance > distance)
//        {
//            distance = tmpDistance;
//            furthestEdge = (*j);
//        }
//    }

//    //Now we create two new containers: one for storing intersection points
//    //and another for storing the indices of the intersected edges and their
//    //corresponding intersections
//    std::vector< QVector3D > intersections;

//    //This one is resized to have the same size as the number of edges,
//    //this will be important later
//    std::vector< std::vector<int> > intersectedEdges;
//    intersectedEdges.resize(edges.size(), std::vector<int>(0));

//    //First get the two vertices of Edge 0
//    QVector3D startEdge = *(face.getEdge(0)->m_startPTR);
//    QVector3D endEdge = *(face.getEdge(0)->m_endPTR);

//    //Calculate the direction vector of this edge
//    QVector3D edgeVector = endEdge - startEdge;
//    edgeVector.normalize();

//    //We add this direction vector on to create a large line segment.
//    //This is done so that when we check for intersections later this
//    //segment is almost guaranteed to intersect (if it intersects at all)
//    startEdge -= ((m_width / 2) * edgeVector);
//    endEdge += ((m_width / 2) * edgeVector);

//    //Just make sure there's no height values in the perpendicular vector
//    perpVector.setY(0);

//    std::vector<VoronoiEdge*> intersectionSegments;

//    //Now we are going to create our ridge and furrows
//    for(int x = 1; x < 50; x+=1)
//    {
//        //If the current x position is further away from Edge 0 than the
//        //furthest away vertex we can stop creating ridge and furrows
//        if((x * perpVector).length() > distance)
//        {
//            break;
//        }

//        //We create two new vertices which will dictate a new edge. The edge
//        //will be parallell to Edge 0 and moves further away as the loop continues
//        QVector3D* newStart = new QVector3D(startEdge + (x * perpVector));
//        QVector3D* newEnd = new QVector3D(endEdge + (x * perpVector));

//        //Do the standard to check to see if we already have references to these
//        //two points
//        int vertID = vertExists(newStart);

//        if(vertID != -1)
//        {
//            newStart = m_allVerts[vertID];
//        }
//        else
//        {
//            m_allVerts.push_back(newStart);
//        }

//        vertID = vertExists(newEnd);

//        if(vertID != -1)
//        {
//            newEnd = m_allVerts[vertID];
//        }
//        else
//        {
//            m_allVerts.push_back(newEnd);
//        }

//        //Now we create a new line segment/edge with these verts
//        VoronoiEdge* tmp = new VoronoiEdge(newStart, newEnd);

//        intersectionSegments.push_back(tmp);

//        //Start a counter, this will be used to store edge indices
//        int edgeCount = 1;

//        //Iterate through the face's edges. Note that we start at index
//        //1 because the new line segment is parallel to Edge 0 and
//        //we don't need to check for intersections there
//        for(auto j = edges.begin() + 1; j != edges.end(); ++j)
//        {
//            //Calculate any line segment intersection between the new
//            //segment and the current edge
//            QVector3D intersection = tmp->intersectEdge(*j);

//            //The above function returns an obviously wrong 3D point
//            //if there was no intersection so we can check if there
//            //actually was one or not
//            if(intersection != QVector3D(1000000.0f, 0.0f, 1000000.0f))
//            {
//                //Add the intersection to our container
//                intersections.push_back(intersection);

//                //And store the index of the new intersection in the
//                //corresponding container for the current edge
//                intersectedEdges[edgeCount].push_back(intersections.size() - 1);
//            }

//            //Increment the edge count regardless of intersection or not
//            ++edgeCount;
//        }
//    }

//    //Ridge and furrows are completed, now check if we need to
//    //deal with any intersections
//    qInfo()<<intersections.size()<<" intersections";
//    if(intersections.size() > 0)
//    {
//        //We're going to create a new edge list, as we don't need
//        //the one containing the face edges any more we can reuse it
//        edges.clear();

//        //Make sure we've added Edge 0 first though
//        edges.push_back(face.getEdge(0));

//        //                QVector3D* lastIntersectionStart = m_regions[i].getEdge(0)->m_startPTR;
//        //                float lastSeparation = 0.0f;

//        //Iterate through our intersections
//        for(uint j = 0; j < intersections.size() - 1; j+=2)
//        {
//            //Because we are using line segment intersection (instead of normal
//            //line intersection) we know there will only be two intersections
//            //per ridge/furrow. This means we can create a new edge by just using
//            //two intersections which are next to each other in the container
//            QVector3D* start = new QVector3D(intersections[j]);
//            QVector3D* end = new QVector3D(intersections[j + 1]);

//            //                    if(j == 0)
//            //                    {
//            //                        lastSeparation = (*start - *(m_regions[i].getEdge(0)->m_startPTR)).length();

//            //                        lastIntersectionStart = m_regions[i].getEdge(0)->m_startPTR;

//            //                        if((*start - *(m_regions[i].getEdge(0)->m_endPTR)).length() < lastSeparation)
//            //                        {
//            //                            lastSeparation = (*start - *(m_regions[i].getEdge(0)->m_endPTR)).length();

//            //                            lastIntersectionStart = m_regions[i].getEdge(0)->m_endPTR;
//            //                        }

//            //                        qInfo()<<"Last: "<<lastSeparation;
//            //                    }
//            //                    else if(j != 0)
//            //                    {
//            //                        float thisSeparation = (*start - *lastIntersectionStart).length();

//            //                        if(thisSeparation > 3.0f* lastSeparation)
//            //                        {
//            //                            qInfo()<<"BREAKING";
//            //                            qInfo()<<"Last: "<<lastSeparation<<" This: "<<thisSeparation;
//            //                            edges.clear();
//            //                            edges = m_regions[i].getEdges();
//            //                            intersectedEdges.clear();

//            //                            break;
//            //                        }
//            //                        else
//            //                        {
//            //                            lastSeparation = thisSeparation;
//            //                        }
//            //                    }

//            //Standard check for pre-existing references
//            int ID = vertExists(start);

//            if(ID != -1)
//            {
//                start = m_allVerts[ID];
//            }
//            else
//            {
//                m_allVerts.push_back(start);
//            }

//            ID = vertExists(end);

//            if(ID != -1)
//            {
//                end = m_allVerts[ID];
//            }
//            else
//            {
//                m_allVerts.push_back(end);
//            }

//            //Create a new edge with our points and check for references
//            VoronoiEdge* newEdge = new VoronoiEdge(start, end);

//            ID = edgeExists(newEdge);

//            if(ID != -1)
//            {
//                newEdge = m_allEdges[ID];
//            }
//            else
//            {
//                m_allEdges.push_back(newEdge);
//            }

//            //Add the new edge to our container
//            edges.push_back(newEdge);
//        }

//        //Now we need to update the pre-existing edges that we've added
//        //vertices to (at the intersection point)
//        for(uint j = 0; j < intersectedEdges.size(); ++j)
//        {
//            //A boolean value used to mark if the edge actually goes
//            //from endPTR---->startPTR instead of startPTR---->endPTR
//            bool edgeIsReversed = false;

//            if(intersectedEdges[j].size() == 0)
//            {
//                //If the current container has no size then this means the current
//                //edge had no intersections on it, we need to just add the
//                //unedited edge to our container
//                edges.push_back(face.getEdge(j));
//            }
//            else if(intersectedEdges[j].size() == 1)
//            {
//                //This means the current edge only had one intersection,
//                //so we need to construct the two new edges
//                QVector3D* start = new QVector3D(intersections[intersectedEdges[j][0]]);

//                int ID = vertExists(start);

//                if(ID != -1)
//                {
//                    start = m_allVerts[ID];
//                }
//                else
//                {
//                    m_allVerts.push_back(start);
//                }

//                //Consturct and edge from the current edge's startPTR to the intersection
//                VoronoiEdge* newEdge = new VoronoiEdge(face.getEdge(j)->m_startPTR, start);
//                ID = edgeExists(newEdge);

//                if(ID != -1)
//                {
//                    newEdge = m_allEdges[ID];
//                }
//                else
//                {
//                    m_allEdges.push_back(newEdge);
//                }

//                //Add it to the container
//                edges.push_back(newEdge);

//                //And now a new edge from the intersection point to the current edge's endPTR
//                VoronoiEdge* newEdge2 = new VoronoiEdge(start, face.getEdge(j)->m_endPTR);
//                ID = edgeExists(newEdge2);

//                if(ID != -1)
//                {
//                    newEdge2 = m_allEdges[ID];
//                }
//                else
//                {
//                    m_allEdges.push_back(newEdge2);
//                }

//                //FInally add the second new edge to the container
//                edges.push_back(newEdge2);

//            }
//            else
//            {
//                //This means the current edge container has more than
//                //one intersection in it, so we iterate through
//                for(uint k = 0; k < intersectedEdges[j].size(); ++k)
//                {
//                    //Get the intersection point using the index we stored in this container
//                    QVector3D* start = new QVector3D(intersections[intersectedEdges[j][k]]);

//                    int ID = vertExists(start);

//                    if(ID != -1)
//                    {
//                        start = m_allVerts[ID];
//                    }
//                    else
//                    {
//                        m_allVerts.push_back(start);
//                    }

//                    if(k == 0)
//                    {
//                        //This is our first new edge segment. We first need to figure out the direction
//                        //of the edge (start--->end or end--->start). This is done by getting the length
//                        //from each vertex to our current intersection point.
//                        float lengthToStart = (*start - *face.getEdge(j)->m_startPTR).length();
//                        float lengthToEnd = (*start - *face.getEdge(j)->m_endPTR).length();

//                        //This is the default for the start vertex
//                        QVector3D* startPoint = face.getEdge(j)->m_startPTR;

//                        if(lengthToEnd < lengthToStart)
//                        {
//                            //If the distance from the intersection is actually shorter towards
//                            //the end vertex then the direction is end--->start. Mark this using
//                            //the boolean and update the start vertex for our edge
//                            edgeIsReversed = true;
//                            startPoint = face.getEdge(j)->m_endPTR;
//                        }

//                        //Create a new edge from the edge's start to our intersection
//                        VoronoiEdge* newEdge = new VoronoiEdge(startPoint, start);
//                        ID = edgeExists(newEdge);

//                        if(ID != -1)
//                        {
//                            newEdge = m_allEdges[ID];
//                        }
//                        else
//                        {
//                            m_allEdges.push_back(newEdge);
//                        }

//                        //After checking for references add it to our container
//                        edges.push_back(newEdge);
//                    }
//                    else if(k != intersectedEdges[j].size() - 1)
//                    {
//                        //This case takes care of all new edges up until the final one.
//                        //First we get the position of the last intersection in our container
//                        QVector3D* lastPoint = new QVector3D(intersections[intersectedEdges[j][k - 1]]);

//                        ID = vertExists(lastPoint);

//                        if(ID != -1)
//                        {
//                            lastPoint = m_allVerts[ID];
//                        }
//                        else
//                        {
//                            m_allVerts.push_back(lastPoint);
//                        }

//                        //Then we create an edge between the last intersection and
//                        //the current one
//                        VoronoiEdge* newEdge = new VoronoiEdge(lastPoint, start);

//                        ID = edgeExists(newEdge);

//                        if(ID != -1)
//                        {
//                            newEdge = m_allEdges[ID];
//                        }
//                        else
//                        {
//                            m_allEdges.push_back(newEdge);
//                        }

//                        //And finally add it to the container
//                        edges.push_back(newEdge);
//                    }
//                    else
//                    {
//                        //This case takes care of the last new edge we need to add.
//                        //Similar to the above case we get the position of the last intersection
//                        QVector3D* lastPoint = new QVector3D(intersections[intersectedEdges[j][k - 1]]);

//                        ID = vertExists(lastPoint);

//                        if(ID != -1)
//                        {
//                            lastPoint = m_allVerts[ID];
//                        }
//                        else
//                        {
//                            m_allVerts.push_back(lastPoint);
//                        }

//                        //This time though we set the end point and the end point of the current edge
//                        VoronoiEdge* newEdge = new VoronoiEdge(lastPoint, face.getEdge(j)->m_endPTR);

//                        //If the edge direction is reversed (end--->start) the 'end point' is actually stored
//                        //in the startPTR, so update new change to reflect this
//                        if(edgeIsReversed)
//                        {
//                            newEdge = new VoronoiEdge(lastPoint, face.getEdge(j)->m_startPTR);
//                        }

//                        ID = edgeExists(newEdge);

//                        if(ID != -1)
//                        {
//                            newEdge = m_allEdges[ID];
//                        }
//                        else
//                        {
//                            m_allEdges.push_back(newEdge);
//                        }

//                        //Add the edge to the container
//                        edges.push_back(newEdge);
//                    }

////                    edges[edges.size() - 1]->m_startPTR->setY(5);
//                }
//            }
//        }
//    }
//    else
//    {
//        qInfo()<<"Got a distance of "<<distance;
////        _facesToUpdate.push_back(VoronoiFace(intersectionSegments));
//    }
//    //Code which adds a small line showing the perpendicular
//    //vector we've been using
//    //            QVector3D* str = new QVector3D(m_regions[i].getEdge(0)->getMidPoint());
//    //            QVector3D* ending = new QVector3D((3 * perpVector) + (*str));

//    //            edges.push_back(new VoronoiEdge(str, ending));

//    //And now we add a new face with our updated edges.
//    _facesToUpdate.push_back(VoronoiFace(edges));
//}

//void EnglishFields::threeField(VoronoiFace face, std::vector<VoronoiFace> &_facesToUpdate)
//{
//    //Not sure if needed but always good to ensure the verts are updated
//    face.updateVerts();

//    //Get the current container of edges for this face
//    std::vector<VoronoiEdge*> edges = face.getEdges();

//    //The middle vertex of the face
//    QVector3D* middleVert = new QVector3D(face.getMiddle());

//    //Check for references
//    int ID = vertExists(middleVert);

//    if(ID != -1)
//    {
//        middleVert = m_allVerts[ID];
//    }
//    else
//    {
//        m_allVerts.push_back(middleVert);
//    }

//    //Now make an edge from the middle of Edge 0 to the middle of the face
//    VoronoiEdge* middleEdge = new VoronoiEdge(new QVector3D(face.getEdge(0)->getMidPoint()), middleVert);

//    //Set up some variables we'll use in the loop below
//    VoronoiEdge* halfEdge = NULL;
//    VoronoiEdge* edgeToUse = middleEdge;
//    int edgeIndex = 0;
//    bool usesStart = false;

//    //Iterate through the face edges
//    for(uint i = 0; i < edges.size() * 2; ++i)
//    {
//        //We're testing each edge twice, easy way to do this
//        //is to check for odd/even edges
//        if(i % 2 == 0)
//        {
//            //Get a new edge from the current edge midpoint to the middle of the face
//            middleEdge = new VoronoiEdge(new QVector3D(face.getEdge(i/2)->getMidPoint()), middleVert);

//            //And the edge from the midpoint to the start vertex of the current edge
//            halfEdge = new VoronoiEdge(new QVector3D(face.getEdge(i/2)->getMidPoint()), new QVector3D(*(face.getEdge(i/2)->m_startPTR)));

//            usesStart = true;
//        }
//        else
//        {
//            //This version uses the half edge from the midpoint to the end vertex of the current edge
//            halfEdge = new VoronoiEdge(new QVector3D(face.getEdge(i/2)->getMidPoint()), new QVector3D(*(face.getEdge(i/2)->m_endPTR)));

//            usesStart = false;
//        }

//        //Get the angle between the edge to the middle of the face and
//        //the half edge we're using
//        float angle = halfEdge->getAngle(middleEdge);

//        //Convert to degrees
//        angle *= 180.0f/3.141516;

//        //Check if the angle is within a certain range and the current vector between the middle
//        //vertex and the current midpoint is larger than the value we currently have
//        if((angle > 45.0f && angle < 135.0f) && (middleEdge->getLength() > edgeToUse->getLength()))
//        {
//            //Update the longest edge we've stored
//            edgeToUse = new VoronoiEdge(new QVector3D(face.getEdge(i/2)->getMidPoint()), middleVert);

//            //And the index of the edge this 'new' edge starts from
//            edgeIndex = i/2;
//        }
//    }

//    //Now we're going to check for an intersection with our 'middle edge'
//    for(uint i = 0; i < edges.size(); ++i)
//    {
//        //Ignore the edge that our 'middleEdge' starts from
//        if(i == edgeIndex)
//        {
//            continue;
//        }

//        //Check for any intersections with our middle edge
//        QVector3D intersection = edgeToUse->intersectEdge(edges[i]);

//        if(intersection != QVector3D(1000000.0f, 0.0f, 1000000.0f))
//        {
//            //If there was an intersection then this edge and face aren't viable
//            qInfo()<<"Edge "<<edgeIndex<<" intersects with "<<i;
//            return;
//        }
//    }

//    qInfo()<<"Found a good edge at: "<<edgeIndex;

//    //Now we're going to create two new end points which stem from the middle vertex and create
//    //edges parallel to the chosen edge
//    QVector3D* newStartPoint = new QVector3D(*(face.getEdge(edgeIndex)->m_startPTR) + (face.getMiddle() - face.getEdge(edgeIndex)->getMidPoint()));
//    QVector3D* newEndPoint = new QVector3D(*(face.getEdge(edgeIndex)->m_endPTR) + (face.getMiddle() - face.getEdge(edgeIndex)->getMidPoint()));

//    //Create our first new edge
//    VoronoiEdge* fieldEdge = new VoronoiEdge(middleVert, newStartPoint);

//    //Move the end point of the edge so that it is likely to intersect with the edges of the face
//    fieldEdge->m_endPTR = new QVector3D(*(fieldEdge->m_endPTR) + (fieldEdge->getDirection() * (m_width / 2.0f)));

//    int intersect_1 = -1;

//    //Now we'll check for intersections
//    for(uint i = 0; i < edges.size(); ++i)
//    {
//        //Get intersection points
//        QVector3D intersection = fieldEdge->intersectEdge(edges[i]);

//        if(intersection != QVector3D(1000000.0f, 0.0f, 1000000.0f))
//        {
//            //This means there was an intersection. Because we're using
//            //line segments for edges this will be the only intersection (if at all)
//            fieldEdge->m_endPTR = new QVector3D(intersection);
//            intersect_1 = i;

//            //Which means we can break out from our edge search
//            break;
//        }
//    }

//    //Now we repeat the process, only using the second point we created
//    VoronoiEdge* fieldEdge2 = new VoronoiEdge(middleVert, newEndPoint);

//    fieldEdge2->m_endPTR = new QVector3D(*(fieldEdge2->m_endPTR) + (fieldEdge2->getDirection() * (m_width / 2.0f)));

//    int intersect_2 = -1;

//    for(uint i = 0; i < edges.size(); ++i)
//    {
//        QVector3D intersection = fieldEdge2->intersectEdge(edges[i]);

//        if(intersection != QVector3D(1000000.0f, 0.0f, 1000000.0f))
//        {
//            fieldEdge2->m_endPTR = new QVector3D(intersection);

//            intersect_2 = i;

//            break;
//        }
//    }

//    //Now check the middle edge for references
//    ID = edgeExists(edgeToUse);

//    if(ID != -1)
//    {
//        edgeToUse = m_allEdges[ID];
//    }
//    else
//    {
//        m_allEdges.push_back(edgeToUse);
//    }

//    //And add it to our container
//    edges.push_back(edgeToUse);

//    //Create a random switch value
//    float threeFieldSwitch = 10.0f * (float)rand()/(float)RAND_MAX;

//    std::vector<VoronoiEdge*> newField_1;

//    qInfo()<<"Intersects: "<<intersect_1<<" & "<<intersect_2;

//    if(threeFieldSwitch < 8.0f)
//    {
//        //This switch is the most popular and adds
//        //both new edges to the container
//        ID = vertExists(fieldEdge->m_endPTR);

//        if(ID != -1)
//        {
//            fieldEdge->m_endPTR = m_allVerts[ID];
//        }
//        else
//        {
//            m_allVerts.push_back(fieldEdge->m_endPTR);
//        }

//        edges.push_back(fieldEdge);

//        ID = vertExists(fieldEdge2->m_endPTR);

//        if(ID != -1)
//        {
//            fieldEdge2->m_endPTR = m_allVerts[ID];
//        }
//        else
//        {
//            m_allVerts.push_back(fieldEdge2->m_endPTR);
//        }

//        edges.push_back(fieldEdge2);
//    }
//    else if(threeFieldSwitch < 9.0f)
//    {
//        //This only adds one new edge
//        ID = vertExists(fieldEdge->m_endPTR);

//        if(ID != -1)
//        {
//            fieldEdge->m_endPTR = m_allVerts[ID];
//        }
//        else
//        {
//            m_allVerts.push_back(fieldEdge->m_endPTR);
//        }

//        edges.push_back(fieldEdge);
//    }
//    else
//    {
//        //This edge also only adds one edge
//        ID = vertExists(fieldEdge2->m_endPTR);

//        if(ID != -1)
//        {
//            fieldEdge2->m_endPTR = m_allVerts[ID];
//        }
//        else
//        {
//            m_allVerts.push_back(fieldEdge2->m_endPTR);
//        }

//        edges.push_back(fieldEdge2);
//    }

////    _facesToUpdate.push_back(VoronoiFace(newField_1));

//    //Update the face container that was passed in
//    _facesToUpdate.push_back(VoronoiFace(edges));
//}

//void EnglishFields::makeOrganic(VoronoiFace &_face)
//{
//    std::vector<VoronoiEdge*> faceEdges;

//    static std::vector<VoronoiEdge*> editedEdges;
//    std::vector<VoronoiEdge*> newEdges;
//    static std::vector<std::vector<VoronoiEdge*>> updatedEdges;

//    qInfo()<<"Count: "<<_face.getEdges().size();
//    for(int i = 0; i < _face.getEdges().size(); ++i)
//    {
//        //Create a switch will turn the midpoint displacement on
//        float switcher = 10.0f* (float)rand() / (float)RAND_MAX;

//        //Check if we've already edited this edge
//        int ID = edgeExists(_face.getEdge(i), editedEdges);

//        if(ID == -1)
//        {
//            //Now check the switch
//            if(switcher > 2.5f)
//            {
//                //Displace the edge
//                midPointDisplace(_face.getEdge(i), 1, newEdges);
//            }
//            else
//            {
//                //Otherwise just use our unedited edge
//                newEdges.push_back(_face.getEdge(i));
//            }

//            //Update the static vectors
//            editedEdges.push_back(_face.getEdge(i));
//            updatedEdges.push_back(newEdges);
//        }
//        else
//        {
//            //Otherwise jus gee udated edges and duplicate them
//            newEdges = updatedEdges[ID];
//        }

//        //Now add the edited edges to our container for the face
//        for(int j = 0; j < newEdges.size(); ++j)
//        {
//            faceEdges.push_back(newEdges[j]);
//        }
//    }

//    qInfo()<<"Final Count: "<<faceEdges.size();

//    _face = VoronoiFace(faceEdges);
//}

//void EnglishFields::midPointDisplace(VoronoiEdge *edge, int iteration, std::vector<VoronoiEdge*> &_edges)
//{
//    if(iteration > 6)
//    {
//        _edges.push_back(edge);
//        return;
//    }

//    if((edge->m_startPTR->x() >= m_width / 2.0f || edge->m_startPTR->z() >= m_width / 2.0f || edge->m_startPTR->x() <= -1.0f * m_width / 2.0f || edge->m_startPTR->z() <= -1.0f * m_width / 2.0f) &&
//       (edge->m_endPTR->x() >= m_width / 2.0f || edge->m_endPTR->z() >= m_width / 2.0f || edge->m_endPTR->x() <= -1.0f * m_width / 2.0f || edge->m_endPTR->z() <= -1.0f * m_width / 2.0f))
//    {
//        _edges.push_back(edge);
//        return;
//    }

//    if(edge->getLength() < 2.5f)
//    {
//        _edges.push_back(edge);
//        return;
//    }

//    PerlinNoise noise;

//    QVector3D* midPoint = new QVector3D(edge->getMidPoint());

//    midPoint->setX((((4.0f / float(iteration)) * (double)rand()/(double)RAND_MAX) - (4.0f / float(iteration))/2.0f) + midPoint->x());
//    midPoint->setX(((2.0f * midPoint->x()) + noise.noise(float(iteration), 0.0f, 0.0f)) / 2.0f);

//    midPoint->setZ((((4.0f  / float(iteration))* (double)rand()/(double)RAND_MAX) - (4.0f / float(iteration))/2.0f) + midPoint->z());
//    midPoint->setZ(((2.0f *midPoint->z()) + noise.noise(float(iteration), 0.0f, 0.0f)) / 2.0f);

//    midPointDisplace(new VoronoiEdge(edge->m_startPTR, midPoint), iteration + 1, _edges);
//    midPointDisplace(new VoronoiEdge(midPoint, edge->m_endPTR), iteration + 1, _edges);
//}

////Field processing
//void EnglishFields::subdivideEdge(QVector3D _start, QVector3D _end, std::vector< std::pair< QVector3D, QVector3D > > & edgeList)
//{
//    //Get the length of the edge we're subdividing
//    float length = (_end - _start).length();

//    //Set the resolution
//    int resolution = 2;

//    //Initialise two variables which we'll use in a moment
//    QVector3D newStart = _start;

//    //Set the end point to a proportion of the edge length
//    QVector3D newEnd = ((_end - _start) / (int)length * resolution) + _start;

//    //Iterate throught the edge
//    for(int j = 0; j < (int)(length * resolution) - resolution; ++j)
//    {
//        //First check that if we add a new edge now we won't go past
//        //the given end point
//        if((newEnd - _start).length() > (_end - _start).length())
//        {
//            break;
//        }

//        //Add the new edge
//        edgeList.push_back(std::make_pair(newStart, newEnd));

//        //Update the start and end points for the next edge
//        newStart = newEnd;
//        newEnd += ((_end - _start) / (int)(length * resolution));
//    }

//    //Finally add on the last piece of the edge
//    edgeList.push_back(std::make_pair(newStart, _end));
//}

//void EnglishFields::subdivideEdge(VoronoiEdge *edge, std::vector<VoronoiEdge *> &_edges)
//{
//    PerlinNoise noise;

//    static float yValue = 0;

//    float length = edge->getLength();

//    int resolution = 1;

//    QVector3D* newStart = edge->m_startPTR;
//    QVector3D* newEnd = new QVector3D(((edge->m_end - edge->m_start) / (int)length * resolution) + edge->m_start);

//    for(float i = 0; i < (int)(length * resolution) - resolution; i += 0.5f)
//    {
//        qInfo()<<"New Segment: "<<(*newEnd - *newStart).length();

//        if((*newEnd - edge->m_start).length() > (length))
//        {
//            break;
//        }
//        _edges.push_back(new VoronoiEdge(newStart, newEnd));

//        newStart = newEnd;

//        int ridgeSwitch = 100 ;//* (float)rand() / (float)RAND_MAX;

//        bool modifyX = false;
//        if(newStart->x() != m_width / 2.0f && newStart->x() != -1 * m_width / 2.0f)
//        {
//            modifyX = true;
//        }

//        bool modifyZ = false;
//        if(newStart->z() != m_width / 2.0f && newStart->z() != -1 * m_width / 2.0f)
//        {
//            modifyZ = true;
//        }

////        modifyX = false;
////        modifyZ = false;

////        modifyX = true;
////        modifyZ = true;

//        if(ridgeSwitch > 50 && (modifyX || modifyZ))
//        {
//            float seedX = float(i) / float(((float)length * resolution) - resolution);// (newStart->x() + (m_width / 2.0f)) / m_width;
//            float seedY = (newStart->z() + (m_width / 2.0f)) / m_width;

//            if(seedX > 0.99f)
//            {
//                seedX = 0.99f;
//            }

//            if(seedY > 0.99f)
//            {
//                seedY = 0.99f;
//            }

//            qInfo()<<"SeedX: "<<seedX;
//            qInfo()<<"SeedY: "<<seedY;

//            qInfo()<<"Noise: "<<noise.noise(seedX, 0.0f, 0.0f);

//            if(modifyX)
//            {
//                qInfo()<<"Previous  X: "<<newStart->x();
//                float newX = newStart->x() + ((noise.noise(seedX, 0.0f, 0.0f) - 0.5f) * float(4.0f));
//                qInfo()<<"Changing X: "<<newX;

//                if(newX < (m_width / 2.0f) && newX > (-1 * m_width/2.0f))
//                {
//                    newStart->setX(newX);
//                }
//                else
//                {
//                    qInfo()<<"Not changed";
//                }
//            }

//            if(modifyZ)
//            {
//                qInfo()<<"Previous  Z: "<<newStart->z();
//                float newZ = newStart->z() + ((noise.noise(seedX, 0.0f, 0.0f) - 0.5f) * float(4.0f));
//                qInfo()<<"Changing Z: "<<newZ;

//                if(newZ < (m_width / 2.0f) && newZ > (-1 * m_width/2.0f))
//                {
//                    newStart->setZ(newZ);
//                }
//                else
//                {
//                    qInfo()<<"Not changed";
//                }

//            }
//        }


//        newEnd = new QVector3D(((edge->m_end - edge->m_start) / (int)length * resolution) + *newEnd);
//    }

//    _edges.push_back(new VoronoiEdge(newStart, edge->m_endPTR));

//    yValue += 1.0f;
//}

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
