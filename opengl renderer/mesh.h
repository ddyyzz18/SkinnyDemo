#pragma once
#ifndef MESH_H
#define MESH_H

#include "shader.h"
#include "vertex.h"
#include <vector>
using namespace std;


class Mesh {
public:
    // mesh Data
    vector<Vertex>              m_Vertices;
    vector<unsigned int>        m_Indices;
    vector<Texture>             m_Textures;
    unsigned int VAO;

private:
    // render data 
    unsigned int VBO, EBO;

public:
    // constructor
    Mesh() = default;
    //Mesh(vector<Vertex>& vertices, vector<unsigned int>& indices, vector<Texture>& textures);
    Mesh(vector<Vertex>&& vertices, vector<unsigned int>&& indices, vector<Texture>&& textures);
    ~Mesh() {};

    // •`‰æ
    void Draw(Shader& shader);
    // I—¹ˆ—
    void Uninit();
    // ‰Šú‰»
    void SetupMesh();

    void SaveMeshBin(std::ofstream&);


};
#endif
