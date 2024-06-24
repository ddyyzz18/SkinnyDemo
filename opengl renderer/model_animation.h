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
	glm::mat4 transformation;			//母ノードへの変換行列
	std::string name;
	int childrenCount;
	std::vector<NodeData> children;		//子供ノードのベクトル
	bool unmasked;						//骨の影響をマスクしてるか（true＝使用中　false＝マスク中）
};

using namespace std;

class Model
{
public:
	vector<Mesh> m_Meshes;									//　メッシュベクトル
	string m_Directory;										//　モデルファイルパス
	bool m_GammaCorrection;									//　ガンマ補正

public:
	Model(string const& path, bool gamma = false); 
	~Model();
	void Draw(Shader& shader);								// 描画
	void Uninit();
	inline auto& GetBoneInfoMap() { return m_BoneInfoMap; }		
	inline int& GetBoneCount() { return m_BoneCounter; }
	void loadModel(string const& path);
	void SaveModelBin();
	// filepathからテクスチャを取得
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
	std::map<string, BoneInfo> m_BoneInfoMap;				//　ボンド名ー＞ボンドインフォ　のマップ
	int m_BoneCounter = 0;									//　ボンド数
	NodeData m_RootNode;									//  ルートノード
#ifdef TEXTURE_LOADED_CHEC									
	vector<Texture> textures_loaded;						//  取得済みのテクスチャ
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
