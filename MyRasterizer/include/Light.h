#pragma once
#include "Vec2/Vec2.h"
#include "Vec3/Vec3.h"
#include "Mat4/Mat4.h"
#include "Color.h"
#include "Vertex.h"


class Light
{
private:
	lm::vec3 position;
	lm::vec3 direction = lm::vec3(0, 0, 1);
	lm::vec3 ambiantColor = lm::vec3(1.f, 1.f, 1.f);
	lm::vec3 diffuseColor = lm::vec3(1.f, 1.f, 1.f);
	lm::vec3 specularColor = lm::vec3(1.f, 1.f, 1.f);
	float ambiantComponent;
	float diffuseComponent;
	float specularComponent;
	float shininess = 256;

public:
	Light(lm::vec3 position, float ambiantComponent, float diffuseComponent, float specularComponent);

	lm::vec3& getPosition();

	float getAmbiant();

	float getDiffuse();

	float getSpecular();

	Color calculateColor(const lm::vec3& normal, Color& color, lm::mat4& transform);

	lm::vec3 getLightReflection(lm::vec3 v0, lm::vec3 v1);
};