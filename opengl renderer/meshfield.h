#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <vector>
#include "mesh.h"
#include "gameobject.h"
#include "stb_image.h"
#include <memory>
//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
class MeshField:public GameObject
{
public:
	unsigned int VAO;
private:
	int m_NumBlockX, m_NumBlockZ;	//	ブロック数(X、Z軸)
	float m_SizeX, m_SizeZ;			//	ブロックのサイズ
	unsigned int VBO, EBO;
	vector<unique_ptr<Texture>> m_Texture;
	vector<Vertex> m_Vertices;
	vector<unsigned int> m_Indices;
public:
	MeshField(glm::vec3 pos, glm::vec3 rot,int nNumBlockX, int nNumBlockZ, float fBlockSizeX, float fBlockSizeZ);
	MeshField(const MeshField&) = delete;
	MeshField(MeshField&&) =default;
	~MeshField();
	void DrawMeshField(Shader& shader);
	void LoadTexture(string path,string type);
	void SetupMesh();
	void Clear();
}; 