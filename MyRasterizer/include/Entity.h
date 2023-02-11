#pragma once
#include "Mesh.h"
#include "Mat4/Mat4.h"

class Entity
{
public:
	Entity() = default;
	Entity(const std::shared_ptr<Mesh>& mesh, float alpha = 1);
	Mesh* getMesh();
	lm::mat4& getTransform();
	float getAlpha();

private:
	std::shared_ptr<Mesh> mesh = nullptr;
	lm::mat4 transform = lm::mat4();
	float alpha = 1;
};