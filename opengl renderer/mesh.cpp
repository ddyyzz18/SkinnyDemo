#include "mesh.h"


Mesh:: Mesh(vector<Vertex>&& vertices, vector<unsigned int>&& indices, vector<Texture>&& textures):
	m_Vertices(std::move(vertices)), m_Indices(std::move(indices)), m_Textures(std::move(textures))
{
#ifdef _DEBUG
	std::cout << "Mesh Moved!";
#endif
	SetupMesh();
}

void Mesh::Draw(Shader& shader)
{
	//　テクスチャをバインド
	unsigned int diffuseNr = 1;
	unsigned int specularNr = 1;
	unsigned int normalNr = 1;
	//unsigned int heightNr = 1;

	for (unsigned int i = 0; i < m_Textures.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i); 

		string number;
		string name = m_Textures[i].type;
		if (name == "texture_diffuse")
			number = std::to_string(diffuseNr++);
		else if (name == "texture_specular")
			number = std::to_string(specularNr++);
		else if (name == "texture_normal")
			number = std::to_string(normalNr++);
		//else if (name == "texture_height")
		//	number = std::to_string(heightNr++);

		// テクスチャUniformを設置
		shader.setInt((name + number).c_str(), i);
		// テクスチャをバインド
		glBindTexture(GL_TEXTURE_2D, m_Textures[i].id);
	}

	// 描画
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(m_Indices.size()), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

//　終了処理
void Mesh::Uninit()
{

	if (VBO != 0) {
		glDeleteBuffers(1, &VBO);
		VBO = 0;
	}

	if (EBO != 0) {
		glDeleteBuffers(1, &EBO);
		EBO = 0;
	}
	if (VAO != 0) {
		glDeleteVertexArrays(1, &VAO);
		VAO = 0;
	}
	for (int i = 0; i < m_Textures.size(); i++)
	{
		if (m_Textures[i].id != 0)
		{
			glDeleteTextures(1, &m_Textures[i].id);
		}
	}
}

//VBO、VAO、EBO初期化
void Mesh::SetupMesh()
{
	// 生成
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, m_Vertices.size() * sizeof(Vertex), &m_Vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_Indices.size() * sizeof(unsigned int), &m_Indices[0], GL_STATIC_DRAW);

	// vertex attributeを設置
	// 
	// vertex Positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	// vertex normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
	// vertex texture coords
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
	// vertex tangent
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
	// vertex bitangent
	/*glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));*/
	// ids
	glEnableVertexAttribArray(5);
	glVertexAttribIPointer(5, 4, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, BoneData.BoneIDs));
	// weights
	glEnableVertexAttribArray(6);
	glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, BoneData.Weights));
	glBindVertexArray(0);
}

void Mesh::SaveMeshBin(std::ofstream& os)
{
	size_t numVertices = m_Vertices.size();
	os.write(reinterpret_cast<char*>(&numVertices), sizeof(int));
	os.write(reinterpret_cast<char*>(m_Vertices.data()), m_Vertices.size() * sizeof(Vertex));
	size_t numIndices = m_Indices.size();
	os.write(reinterpret_cast<char*>(&numIndices), sizeof(int));
	os.write(reinterpret_cast<char*>(m_Indices.data()), m_Indices.size() * sizeof(unsigned int));
	size_t numTextures = m_Textures.size();
	os.write(reinterpret_cast<char*>(&numTextures), sizeof(int));
	os.write(reinterpret_cast<char*>(m_Textures.data()), m_Textures.size() * sizeof(Texture));

}