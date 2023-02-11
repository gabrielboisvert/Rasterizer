#pragma once
#include "Vec2/Vec2.h"
#include "Vec3/Vec3.h"
#include "Color.h"

class Vertex
{
private:
	lm::vec3 position;
	lm::vec3 normal;
	lm::vec2 texCoord;
	Color color;
	
public:
	Vertex(const lm::vec3& position, const lm::vec3& normal, const lm::vec2& textCoord, Color color = Color::WHITE);
	Vertex(float x, float y, float z, const lm::vec3& normal, const lm::vec2& textCoord, Color color = Color::WHITE);

	lm::vec3& getPosition();
	lm::vec3& getNormal();
	lm::vec2& getTextCoord();
	Color& getColor();
};