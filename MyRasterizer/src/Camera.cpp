#include "../include/Camera.h"

void Camera::setMovementDirection(int direction)
{
	this->moveDirection = direction;
}

void Camera::setHRotationDirection(int direction)
{
	this->rotationHDirection = direction;
}

void Camera::setVRotationDirection(int direction)
{
	this->rotationVDirection = direction;
}

lm::mat4 Camera::update()
{
	if (this->rotationHDirection == 1)
		this->rotation.Y() -= this->sensitity;
	else if (this->rotationHDirection == -1)
		this->rotation.Y() += this->sensitity;

	if (this->rotationVDirection == 1)
		this->rotation.X() += this->sensitity;
	else if (this->rotationVDirection == -1)
		this->rotation.X() -= this->sensitity;

	if (this->moveDirection == 1)
		this->position.Z() -= this->speed;
	else if (this->moveDirection == -1)
		this->position.Z() += this->speed;

	return lm::mat4::createView(this->rotation, this->position).getInverse();
}