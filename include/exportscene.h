#ifndef EXPORTSCENE_H
#define EXPORTSCENE_H

#include <QVector3D>
#include <QVector2D>
#include <vector>

#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>
#include <assimp/Exporter.hpp>

class ExportScene
{
public:
    static void sendTo(std::string _filetype, std::string _filepath, const std::vector<QVector3D> _verts, const std::vector<QVector3D> _norms, const std::vector<QVector2D> _uvs);

private:
    ExportScene();
    ~ExportScene();

    static aiScene makeNewScene();

    std::vector<aiMesh*> m_meshes;


};

#endif // EXPORTSCENE_H