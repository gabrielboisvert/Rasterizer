#pragma once
#include "Mat4/Mat4.h"
#include "Vec3/Vec3.h"

class Camera
{
public:
	void setMovementDirection(int direction);
	void setHRotationDirection(int direction);

	void setVRotationDirection(int direction);

	lm::mat4 update();

private:
	float sensitity = 1;
	float speed = 0.01f;
	int rotationHDirection = 0;
	int rotationVDirection = 0;
	int moveDirection = 0;

	lm::vec3 position = lm::vec3(0, 0, 1.5f);
	lm::vec3 rotation;
};