#pragma once
#include <vector>
#include "Entity.h"
#include "Light.h"

class Scene
{
public:
	void addEntity(Entity* entity);
	void addLight(const Light& light);
	std::vector<Entity*>& getEntities();
	std::vector<Light>& getLights();

	~Scene();

private:
	std::vector<Entity*> entities;
	std::vector<Light> lights;
};