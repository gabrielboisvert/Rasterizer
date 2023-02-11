#include "../include/Vertex.h"

Vertex::Vertex(const lm::vec3& position, const lm::vec3& normal, const lm::vec2& texCoord, Color color) : position(position), normal(normal), texCoord(texCoord), color(color) {}

Vertex::Vertex(float x, float y, float z, const lm::vec3& normal, const lm::vec2& texCoord, Color color) : position(x, y, z), normal(normal), texCoord(texCoord), color(color) {}

lm::vec3& Vertex::getPosition()
{
	return this->position;
}

lm::vec3& Vertex::getNormal()
{
	return this->normal;
}

lm::vec2& Vertex::getTextCoord()
{
	return this->texCoord;
}

Color& Vertex::getColor()
{
	return this->color;
}