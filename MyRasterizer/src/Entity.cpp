#include "../include/Entity.h"

Entity::Entity(const std::shared_ptr<Mesh>& mesh, float alpha) : mesh(mesh), alpha(alpha) {}

Mesh* Entity::getMesh()
{
    return this->mesh.get();
}

lm::mat4& Entity::getTransform()
{
    return this->transform;
}

float Entity::getAlpha()
{
    return this->alpha;
}
