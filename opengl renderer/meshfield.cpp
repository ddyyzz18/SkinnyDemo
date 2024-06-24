#include "meshfield.h"

MeshField::MeshField(glm::vec3 pos, glm::vec3 rot, int nNumBlockX, int nNumBlockZ, float fBlockSizeX, float fBlockSizeZ)
{
	this->pos = pos;
	this->rot = rot;
	this->scl = glm::vec3(1.0f);

	m_NumBlockX = nNumBlockX;
	m_NumBlockZ = nNumBlockZ;
	m_SizeX = fBlockSizeX;
	m_SizeZ = fBlockSizeZ;

	m_Vertices.reserve((m_NumBlockZ+1)*(m_NumBlockX+1));	//頂点数分の領域を確保
	m_Indices.reserve(6 * m_NumBlockZ * m_NumBlockX);	//インデクス分の領域を確保

	//vertices　vectorの中身を埋める
	for (int z = 0; z < (m_NumBlockZ + 1); z++)
	{
		for (int x = 0; x < (m_NumBlockX + 1); x++)
		{
			m_Vertices.emplace_back(glm::vec3(static_cast<float>(-(m_NumBlockX * 0.5f) * m_SizeX + x * m_SizeX), 0.0f, static_cast<float>(-m_NumBlockZ * 0.5f * m_SizeZ + z * m_SizeZ)),
				glm::vec3(0.0f, 1.0f, 0.0f),
				glm::vec2(static_cast<float>(x), static_cast<float>(z)),
				glm::vec3(1.0f, 0.0f, 0.0f)
			);
		}
	}

	//indices vectorの中身を埋める
	for (int z = 0; z < (m_NumBlockZ); z++)
	{
		for (int x = 0; x < (m_NumBlockX); x++)
		{
			unsigned int index;
			index = z * (m_NumBlockX + 1) + x;
			m_Indices.push_back(index);
			index += m_NumBlockX+1;
			m_Indices.push_back(index);
			index-= m_NumBlockX;
			m_Indices.push_back(index);

			m_Indices.push_back(index);
			index += m_NumBlockX;
			m_Indices.push_back(index);
			index ++;
			m_Indices.push_back(index);
		}
	}

	SetupMesh();
}

MeshField::~MeshField()
{
	//Clear();
}

void MeshField::SetupMesh()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, m_Vertices.size() * sizeof(Vertex), &m_Vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_Indices.size() * sizeof(unsigned int), &m_Indices[0], GL_STATIC_DRAW);

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
	glBindVertexArray(0);
}

void MeshField::LoadTexture(string path,string type)
{

	unsigned int texture;
	glGenTextures(1, &texture);


	int width, height, nrChannels;
	unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
	if (data)
	{
		GLenum format;
		if (nrChannels == 1)
			format = GL_RED;
		else if (nrChannels == 3)
			format = GL_RGB;
		else if (nrChannels == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, texture); 
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);

	std::unique_ptr<Texture> t1 = std::make_unique<Texture>();
	t1->id = texture;
	t1->path = std::move(path);
	t1->type = std::move(type);

	m_Texture.push_back(std::move(t1));
}

void MeshField::Clear()
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

	for (int i = 0; i < m_Texture.size(); i++)
	{
		if (m_Texture[i]->id != 0)
		{
			glDeleteTextures(1,&m_Texture[i]->id);
		}
	}
}

void MeshField::DrawMeshField(Shader& shader)
{
	//　テクスチャをバインド
	unsigned int diffuseNr = 1;
	unsigned int specularNr = 1;
	unsigned int normalNr = 1;

	for (unsigned int i = 0; i < m_Texture.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);

		string number;
		string name = m_Texture[i]->type;
		if (name == "texture_diffuse")
			number = std::to_string(diffuseNr++);
		else if (name == "texture_specular")
			number = std::to_string(specularNr++);
		else if (name == "texture_normal")
			number = std::to_string(normalNr++);
		// テクスチャUniformを設置
		shader.setInt((name + number).c_str(), i);
		// テクスチャをバインド
		glBindTexture(GL_TEXTURE_2D, m_Texture[i]->id);
	}

	// 描画
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(m_Indices.size()), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}