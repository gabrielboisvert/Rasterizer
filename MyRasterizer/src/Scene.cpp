#include "..\include\Scene.h"

void Scene::addEntity(Entity* entity)
{
	if (entity != nullptr)
		this->entities.push_back(entity);
}

void Scene::addLight(const Light& light)
{
	this->lights.push_back(light);
}

std::vector<Entity*>& Scene::getEntities()
{
	return this->entities;
}

std::vector<Light>& Scene::getLights()
{
	return this->lights;
}

Scene::~Scene()
{
	for (unsigned int i = 0; i < entities.size(); i++)
		delete entities[i];
}
