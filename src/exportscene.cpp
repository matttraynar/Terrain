#include "exportscene.h"

#include <iostream>

ExportScene::ExportScene()
{

}

ExportScene::~ExportScene()
{

}

void ExportScene::sendTo(std::string _filetype, std::string _filepath, const std::vector<QVector3D> _verts, const std::vector<QVector3D> _norms, const std::vector<QVector2D> _uvs,
                                      float _terrainSize, bool _triangulate, bool _isTerrain)
{
    if(!_verts.empty())
    {
        aiScene scene = makeNewScene();

        scene.mRootNode = new aiNode();

        scene.mMaterials = new aiMaterial*[ 1 ];
        scene.mMaterials[ 0 ] = nullptr;
        scene.mNumMaterials = 1;

        scene.mMaterials[ 0 ] = new aiMaterial();

        scene.mMeshes = new aiMesh*[ 1 ];
        scene.mMeshes[ 0 ] = nullptr;
        scene.mNumMeshes = 1;

        scene.mMeshes[ 0 ] = new aiMesh();
        scene.mMeshes[ 0 ]->mMaterialIndex = 0;

        scene.mRootNode->mMeshes = new unsigned int[ 1 ];
        scene.mRootNode->mMeshes[ 0 ] = 0;
        scene.mRootNode->mNumMeshes = 1;

        auto pMesh = scene.mMeshes[ 0 ];

        const auto& vVertices = _verts;

        pMesh->mVertices = new aiVector3D[ vVertices.size() ];
        pMesh->mNormals = new aiVector3D[ vVertices.size() ];
        pMesh->mNumVertices = (uint)vVertices.size();

        pMesh->mTextureCoords[ 0 ] = new aiVector3D[ (uint)vVertices.size() ];
        pMesh->mNumUVComponents[ 0 ] = vVertices.size();

        int j = 0;

        bool hasNorms = (_norms.size() > 0 ) ? true : false;
        bool hasUVs = (_uvs.size() > 0 ) ? true : false;

        for ( auto itr = vVertices.begin(); itr != vVertices.end(); ++itr )
        {
            pMesh->mVertices[ itr - vVertices.begin() ] = aiVector3D( vVertices[j].x(), vVertices[j].y(), vVertices[j].z() );

            if(hasNorms)
            {
                pMesh->mNormals[ itr - vVertices.begin() ] = aiVector3D( _norms[j].x(), _norms[j].y(), _norms[j].z() );
            }

            if(hasUVs)
            {
                if(_isTerrain)
                {
                    pMesh->mTextureCoords[0][ itr - vVertices.begin() ] = aiVector3D((vVertices[j].x() + (_terrainSize / 2.0f)) / _terrainSize , 1.0f - (vVertices[j].z() + (_terrainSize / 2.0f)) / _terrainSize, 0 );
                }
                else
                {
                    pMesh->mTextureCoords[0][ itr - vVertices.begin() ] = aiVector3D(_uvs[j].x(), _uvs[j].y(), 0.0f);
                }
            }

            j++;
        }

        pMesh->mFaces = new aiFace[ vVertices.size() / 4 ];
        pMesh->mNumFaces = (unsigned int)(vVertices.size() / 4);

        int k = 0;
        for(int i = 0; i < (vVertices.size() / 4); i++)
        {
            aiFace &face = pMesh->mFaces[i];
            face.mIndices = new unsigned int[4];
            face.mNumIndices = 4;

            face.mIndices[0] = k;
            face.mIndices[1] = k+1;
            face.mIndices[2] = k+2;
            face.mIndices[3] = k+3;
            k = k + 4;
        }

        Assimp::Exporter exporter;
        exporter.Export(&scene, _filetype, _filepath);

        if(_triangulate)
        {
            Assimp::Importer fixer;
            aiScene editScene = *fixer.ReadFile(_filepath, aiProcess_Triangulate);

            exporter.Export(&editScene, _filetype, _filepath);
        }
    }
}

void ExportScene::getFrom(std::string _filepath, std::vector<QVector3D> &_verts, std::vector<QVector3D> &_norms, float& _min, float& _max)
{
    //Create a new assimp importer and scene with the file path
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(_filepath,
                                                 aiProcess_GenSmoothNormals |
                                                 aiProcess_SortByPType);

        //Check for loading errors
        if(!scene)
        {
//            qWarning() << "Error: File didn't load: ASSIMP: " << importer.GetErrorString();
            exit(1);
        }

        //Check for meshes
        if(scene->HasMeshes())
        {
            //There are meshes so we can iterate through them
            for(uint i = 0; i < scene->mNumMeshes; ++i)
            {
//                //Get the number of faces (just to make the next for loop more readable
//                uint numFaces = scene->mMeshes[i]->mNumFaces;

//                for(uint j = 0; j < numFaces; ++j)
//                {
//                    //Get each face in the mesh and add the index to the index container
//                    auto face = scene->mMeshes[i]->mFaces[j];

//                    m_meshIndex.push_back(face.mIndices[0]);
//                    m_meshIndex.push_back(face.mIndices[1]);
//                    m_meshIndex.push_back(face.mIndices[2]);
//                }

                //Update the num verts. This is again to make the next for loop readable
                uint numVerts = scene->mMeshes[i]->mNumVertices;

                for(uint j = 0; j < numVerts; ++j)
                {
                    //Iterate throught the vertex and normal data in the current mesh
                    auto vert = scene->mMeshes[i]->mVertices[j];
                    auto norm = scene->mMeshes[i]->mNormals[j];

                    //Add this data to the correct container
                    _verts.push_back(QVector3D(vert.x,vert.y,vert.z));
                    _norms.push_back(QVector3D(norm.x,norm.y,norm.z));

                    if(vert.y < _min)
                    {
                        _min = vert.y;
                    }

                    if(vert.y > _max)
                    {
                        _max = vert.y;
                    }
                }
            }
        }
}

aiScene ExportScene::makeNewScene()
{
    aiScene nScene;
    nScene.mFlags = 0;
    nScene.mRootNode = NULL;

    nScene.mNumMeshes = 0;
    nScene.mMeshes = NULL;

    nScene.mNumMaterials = 0;
    nScene.mMaterials = NULL;

    nScene.mNumAnimations = 0;
    nScene.mAnimations = NULL;

    nScene.mNumTextures = 0;
    nScene.mTextures = NULL;

    nScene.mNumLights = 0;
    nScene.mLights = NULL;

    nScene.mNumCameras = 0;
    nScene.mCameras = NULL;

    nScene.mPrivate = NULL;

    return nScene;
}

