#include "model_animation.h"

Model::Model(string const& path, bool gamma) : m_GammaCorrection(gamma)
{
#ifdef TEXTURE_LOADED_CHEC
	textures_loaded.clear();
#endif
	loadModel(path);

}

Model::~Model()
{
	
}

void Model::Uninit()
{
	for (int i = 0; i < m_Meshes.size(); i++)
	{
		m_Meshes[i].Uninit();
	}
}
void Model::Draw(Shader& shader)
{
	for (unsigned int i = 0; i < m_Meshes.size(); i++)
		m_Meshes[i].Draw(shader);
}

void Model::loadModel(string const& path)
{
	// ASSIMPを使用して、モデルを読み込む
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

	// エラー処理
#ifdef _DEBUG
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
	{
		cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
		return;
	}
#endif
	// m_Directoryを取得
	m_Directory = path.substr(0, path.find_last_of('/'));
#ifdef _DEBUG
	cout << "Parsing " << scene->mNumMeshes << " meshes\n" << endl;
#endif
	m_Meshes.reserve(scene->mNumMeshes);
	for (unsigned int i = 0; i < scene->mNumMeshes; i++)
	{

		aiMesh* mesh = scene->mMeshes[i];
		processMesh(mesh, scene, i);
	}
	ReadHierarchyData(m_RootNode, scene->mRootNode);
}
//　vertex構造体中のBone関連変数を初期化
void Model::SetVertexBoneDataToDefault(Vertex& vertex)
{
	for (int i = 0; i < MAX_BONE_INFLUENCE; i++)
	{
		vertex.BoneData.BoneIDs[i] = -1;
		vertex.BoneData.Weights[i] = 0.0f;
	}
}

//　Meshの中身を埋める
void Model::processMesh(aiMesh* mesh, const aiScene* scene, int mesh_index)
{

	vector<Vertex> vertices;
	vector<unsigned int> indices;
	vector<Texture> textures;
	vertices.reserve(mesh->mNumVertices);
	indices.reserve(mesh->mNumFaces);

	//　vertices　ベクトルの中身を埋める
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;
		SetVertexBoneDataToDefault(vertex);
		vertex.Position = AssimpGLMHelpers::GetGLMVec(mesh->mVertices[i]);
		vertex.Normal = AssimpGLMHelpers::GetGLMVec(mesh->mNormals[i]);
		vertex.Tangent = AssimpGLMHelpers::GetGLMVec(mesh->mTangents[i]);

		if (mesh->mTextureCoords[0])
		{

			vertex.TexCoords.x = mesh->mTextureCoords[0][i].x;
			vertex.TexCoords.y = mesh->mTextureCoords[0][i].y;
		}
		else
			vertex.TexCoords = glm::vec2(0.0f, 0.0f);

		vertices.push_back(std::move(vertex));

	}

	//　indices　ベクトルの中身を埋める
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			indices.push_back(std::move(face.mIndices[j]));
	}

	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
	//　テクスチャを取得
	vector<Texture> temp;
	loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse", scene, temp);
	textures.insert(textures.end(), temp.begin(), temp.end());
	temp.clear();
	loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular", scene, temp);
	textures.insert(textures.end(), temp.begin(), temp.end());
	temp.clear();
	loadMaterialTextures(material, aiTextureType_NORMALS, "texture_normal", scene, temp);
	textures.insert(textures.end(), temp.begin(), temp.end());
	temp.clear();
	loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height", scene, temp);
	textures.insert(textures.end(), temp.begin(), temp.end());

	// 頂点のボンド関連変数を取得
	if (mesh->HasBones())
		ExtractBoneWeightForVertices(vertices, mesh, scene, mesh_index);


	m_Meshes.emplace_back(std::move(vertices), std::move(indices), std::move(textures));

}

//　vertex内のBoneData構造体の中身を埋める
void Model::ExtractBoneWeightForVertices(std::vector<Vertex>& vertices, aiMesh* mesh, const aiScene* scene, int mesh_index)
{
	auto& boneInfoMap = m_BoneInfoMap;
	int& boneCount = m_BoneCounter;

	for (unsigned int boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex)
	{
		int boneID = -1;
		std::string boneName = mesh->mBones[boneIndex]->mName.C_Str();
		// 未格納のボンドを取得
		if (boneInfoMap.find(boneName) == boneInfoMap.end())
		{
			glm::mat4 tempoffset = AssimpGLMHelpers::ConvertMatrixToGLMFormat(mesh->mBones[boneIndex]->mOffsetMatrix);

			BoneInfo newBoneInfo;
			newBoneInfo.id = boneCount;
			newBoneInfo.offset = tempoffset;
			boneInfoMap[boneName] = newBoneInfo;
			boneID = boneCount;
			boneCount++;

		}
		else
		{
			boneID = boneInfoMap[boneName].id;
		}

		assert(boneID != -1);
		auto weights = mesh->mBones[boneIndex]->mWeights;		//　このボンドが各頂点を影響する重さ（weight）
		int numWeights = mesh->mBones[boneIndex]->mNumWeights;	//　このボンド影響する頂点の数

		//　vertexのボンド関連変数を取得
		for (int weightIndex = 0; weightIndex < numWeights; ++weightIndex)
		{
			int vertexId = weights[weightIndex].mVertexId;		//　ボンド影響する頂点のＩＤ
			float weight = weights[weightIndex].mWeight;		//　ボンドがこの頂点を影響するweight
			assert(weights[weightIndex].mVertexId <= vertices.size());
			vertices[vertexId].BoneData.AddBoneData(boneID, weight);

		}
	}

}

//　テクスチャを読み込む
unsigned int Model::TextureFromFile(const char* path, const string& m_Directory, bool gamma)
{
	string filename = string(path);
	filename = m_Directory + '/' + filename;

	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

void Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName, const aiScene* scene, vector<Texture>& textures)
{

	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);
#ifdef TEXTURE_LOADED_CHEC
		// テクスチャ取得済みかをチェック
		bool skip = false;
		for (unsigned int j = 0; j < textures_loaded.size(); j++)
		{
			if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
			{
				textures.push_back(textures_loaded[j]);
				skip = true;
				break;
			}
		}
		if (!skip)
		{   // 未取得のテクスチャを取得
			Texture texture;

			auto tex = scene->GetEmbeddedTexture(str.C_Str());
			if (tex != nullptr)
			{
				//FBXの場合
				texture.id = TextureFrom_FBX_EmbeddedTexture(tex);
			}
			else
			{

				texture.id = TextureFromFile(str.C_Str(), m_Directory);
			}

			texture.type = typeName;
			texture.path = str.C_Str();
			textures.push_back(texture);
			textures_loaded.push_back(texture);
		}
#endif
		Texture texture;

		auto tex = scene->GetEmbeddedTexture(str.C_Str());
		if (tex != nullptr)
		{
			//FBXの場合
			texture.id = TextureFrom_FBX_EmbeddedTexture(tex);
		}
		else
		{

			texture.id = TextureFromFile(str.C_Str(), m_Directory);
		}

		texture.type = typeName;
		texture.path = str.C_Str();
		textures.push_back(std::move(texture));
	}
}


unsigned int Model::TextureFrom_FBX_EmbeddedTexture(const aiTexture* aiTex, bool gamma)
{
	int width, height, channels;

	unsigned char* data = nullptr;

	//FBX modelならメモリーからテクスチャを取得
	if (aiTex->mHeight == 0)
	{
		data = stbi_load_from_memory(reinterpret_cast<unsigned char*>(aiTex->pcData), aiTex->mWidth, &width, &height, &channels, 0);
	}
	else
	{
		data = stbi_load_from_memory(reinterpret_cast<unsigned char*>(aiTex->pcData), aiTex->mWidth * aiTex->mHeight, &width, &height, &channels, 0);
	}

	return GenerateTex(data, width, height, channels);
}

unsigned int Model::GenerateTex(unsigned char* data, int width, int height, int nrComponents, bool gamma)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);

		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

//NodeDataを再帰的に読み込む
void Model::ReadHierarchyData(NodeData& dest, const aiNode* src)
{
	assert(src);

	dest.name = src->mName.data;
	dest.transformation = AssimpGLMHelpers::ConvertMatrixToGLMFormat(src->mTransformation);
	dest.childrenCount = src->mNumChildren;
	dest.unmasked = true;
	for (unsigned int i = 0; i < src->mNumChildren; i++)
	{
		NodeData newData;
		ReadHierarchyData(newData, src->mChildren[i]);
		dest.children.push_back(std::move(newData));
	}
}


void Model::SaveModelBin()
{
	static int id = 0;
	size_t lastSlashPos = m_Directory.find_last_of('/');
	std::string filename;
	if (lastSlashPos != std::string::npos)
	{
		filename = m_Directory.substr(lastSlashPos + 1);
		filename = filename.substr(0, filename.find_last_of('.'));
		filename.append(".bin");
	}
	else
	{
		filename = "Unknown";
		filename.append(std::to_string(id));
		filename.append(".bin");
		id++;
	}

	std::ofstream outFile(filename, std::ios::out | std::ios::binary);
	if (!outFile.is_open()) {
		std::cerr << "Failed to open the file for writing." << std::endl;
		return;
	}


	outFile.close();
}

