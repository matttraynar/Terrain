#include "exportscene.h"

#include <iostream>

#include <QDebug>

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
