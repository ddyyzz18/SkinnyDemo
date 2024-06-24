#include "vertex.h"
#include <iostream>

Vertex::Vertex()
{

}

Vertex::Vertex(glm::vec3&& pos, glm::vec3&& Nor, glm::vec2&& Tex, glm::vec3&& Tan)
{
	Position = std::move(pos);
	Normal = std::move(Nor);
	TexCoords = std::move(Tex);
	Tangent = std::move(Tan);

}