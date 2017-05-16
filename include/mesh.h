#ifndef MESH_H
#define MESH_H

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/Exporter.hpp>

#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QVector3D>

class Mesh
{
public:
    Mesh(const char* filepath);
    ~Mesh();

    void prepareMesh(QOpenGLShaderProgram& _pgm);
    void draw();

private:
    //Array and buffer object for the mesh
    QOpenGLVertexArrayObject m_vao;
    QOpenGLBuffer m_vbo;
    QOpenGLBuffer m_nbo;
    QOpenGLBuffer m_ibo;


    //Containers for the mesh data
    std::vector<QVector3D> m_verts;
    std::vector<QVector3D> m_norms;
    std::vector<uint> m_meshIndex;
};

#endif // MESH_H
