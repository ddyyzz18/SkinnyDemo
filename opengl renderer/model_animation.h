#pragma once
#ifndef MODEL_H
#define MODEL_H

#include <glad/glad.h> 

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "stb_image.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "mesh.h"
#include "shader.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include "assimp_glm_helpers.h"
#include "animdata.h"

struct NodeData
{
	glm::mat4 transformation;			//��m�[�h�ւ̕ϊ��s��
	std::string name;
	int childrenCount;
	std::vector<NodeData> children;		//�q���m�[�h�̃x�N�g��
	bool unmasked;						//���̉e�����}�X�N���Ă邩�itrue���g�p���@false���}�X�N���j
};

using namespace std;

class Model
{
public:
	vector<Mesh> m_Meshes;									//�@���b�V���x�N�g��
	string m_Directory;										//�@���f���t�@�C���p�X
	bool m_GammaCorrection;									//�@�K���}�␳

public:
	Model(string const& path, bool gamma = false); 
	~Model();
	void Draw(Shader& shader);								// �`��
	void Uninit();
	inline auto& GetBoneInfoMap() { return m_BoneInfoMap; }		
	inline int& GetBoneCount() { return m_BoneCounter; }
	void loadModel(string const& path);
	void SaveModelBin();
	// filepath����e�N�X�`�����擾
	static inline Texture loadMaterialTexturesManully(const char* path, string type, const string& m_Directory)
	{
		Texture texture;
		texture.id = TextureFromFile(path, m_Directory);
		texture.type = type;
		texture.path = path;

		return texture;
	}
	inline NodeData& GetRootNode() { return m_RootNode; }
private:
	std::map<string, BoneInfo> m_BoneInfoMap;				//�@�{���h���[���{���h�C���t�H�@�̃}�b�v
	int m_BoneCounter = 0;									//�@�{���h��
	NodeData m_RootNode;									//  ���[�g�m�[�h
#ifdef TEXTURE_LOADED_CHEC									
	vector<Texture> textures_loaded;						//  �擾�ς݂̃e�N�X�`��
#endif
private:
    void SetVertexBoneDataToDefault(Vertex& vertex);
	void processMesh(aiMesh* mesh, const aiScene* scene, int mesh_index);
	void ExtractBoneWeightForVertices(std::vector<Vertex>& vertices, aiMesh* mesh, const aiScene* scene, int mesh_index);
	static unsigned int TextureFromFile(const char* path, const string& m_Directory, bool gamma = false);
	void loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName, const aiScene* scene, vector<Texture>& textures);
	unsigned int TextureFrom_FBX_EmbeddedTexture(const aiTexture* aiTex, bool gamma = false);
	unsigned int GenerateTex(unsigned char* data, int width, int height, int nrComponents, bool gamma = false);
	void ReadHierarchyData(NodeData& dest, const aiNode* src);
	//void SaveTextureBin(unsigned char* data);
};





#endif
