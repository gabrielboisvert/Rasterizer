#pragma once
#include "Rasterizer.h"
#include "Camera.h"

class App
{
public:
	App(unsigned int width, unsigned int height);
	void init();
	void run();
	~App();
private:
	SDL_Window* window = nullptr;
	Rasterizer* rasterizer = nullptr;
	Texture* texture = nullptr;
	Scene* scene = nullptr;
	Camera* camera = nullptr;

	Entity* triangle;
	Entity* sphere;
	Entity* sphere2;
	Entity* cube;
	Entity* cube2;
	Entity* cube3;
	Texture* container = nullptr;
};