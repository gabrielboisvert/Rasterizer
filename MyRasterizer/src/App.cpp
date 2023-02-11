#include "..\include\App.h"
#include <cstdio>
#include <SDL.h>
#include <glm/mat4x4.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

App::App(unsigned int width, unsigned int height)
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		fprintf(stderr, "SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
		exit(1);
	}

	this->window = SDL_CreateWindow("Rasterize me baby", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
	if (this->window == nullptr)
	{
		fprintf(stderr, "Window could not be created! SDL_Error: %s\n", SDL_GetError());
		exit(1);
	}

	this->texture = new Texture(width, height);
	if (this->texture == nullptr)
	{
		fprintf(stderr, "Texture could not be created! SDL_Error: %s\n", SDL_GetError());
		exit(1);
	}

	this->rasterizer = new Rasterizer(*this->window, *this->texture);
	if (this->rasterizer == nullptr)
	{
		fprintf(stderr, "Rasterizer could not be created! SDL_Error: %s\n", SDL_GetError());
		exit(1);
	}

	this->scene = new Scene();
	if (this->scene == nullptr)
	{
		fprintf(stderr, "Scene could not be created! SDL_Error: %s\n", SDL_GetError());
		exit(1);
	}

	this->camera = new Camera();
	if (this->scene == nullptr)
	{
		fprintf(stderr, "Camera could not be created! SDL_Error: %s\n", SDL_GetError());
		exit(1);
	}

	this->init();
	
}

void App::init()
{
	this->container = new Texture("img/container.jpg");

	this->triangle = new Entity(Mesh::createTriangle());
	this->triangle->getTransform() = lm::mat4::createTransformMatrix({ 0, 0, 0 }, { 0, 0, -2 }, { 2, 2, 2 });
	this->scene->addEntity(this->triangle);


	std::shared_ptr<Mesh> s = Mesh::createSphere(20, Color::BLACK);
	std::shared_ptr<Mesh> s2 = Mesh::createSphere(20);
	this->sphere = new Entity(s2);
	this->sphere->getTransform() = lm::mat4::createTransformMatrix({ 0, 0, 0 }, { 2, 1, -3 }, { 1, 1, 1 });
	this->scene->addEntity(this->sphere);

	this->sphere2 = new Entity(s);
	this->sphere2->getTransform() = lm::mat4::createTransformMatrix({ 0, 0, 0 }, { -2, 1, -3 }, { 1, 1, 1 });
	this->sphere2->getMesh()->setTexture(this->container);
	this->scene->addEntity(this->sphere2);

	std::shared_ptr<Mesh> c = Mesh::createCube(Color::BLACK);
	std::shared_ptr<Mesh> c2 = Mesh::createCube();
	this->cube = new Entity(c);
	this->cube->getTransform() = lm::mat4::createTransformMatrix({ 0, 0, 0 }, { 2, -1, -3 }, { 1, 1, 1 });
	this->cube->getMesh()->setTexture(this->container);
	this->scene->addEntity(this->cube);

	this->cube2 = new Entity(c2);
	this->cube2->getTransform() = lm::mat4::createTransformMatrix({ 0, 0, 0 }, { -2, -1, -3 }, { 1, 1, 1 });
	this->scene->addEntity(this->cube2);

	this->cube3 = new Entity(c2, 0.25f);
	this->cube3->getTransform() = lm::mat4::createTransformMatrix({ 0, 0, 0 }, { 0, 0, -1 }, { 0.75f, 0.75f, 0.75f });
	this->scene->addEntity(this->cube3);

	this->scene->addLight(Light({ 0, 0, 0 }, 0.4f, 0.2f, 0.4f));
}

void App::run()
{
	bool quit = false;
	SDL_Event e;
	
	float angle = 0;
	lm::mat4 projection = lm::mat4::perspectiveProjection(45.f, float(this->texture->getWidth()) / float(this->texture->getHeight()), 0.1f, 1000.f);

	while (!quit)
	{
		while (SDL_PollEvent(&e) != 0)
		{
			switch (e.type)
			{
			case SDL_KEYDOWN:
				if (e.key.keysym.sym == SDLK_ESCAPE)
					quit = true;
				else if (e.key.keysym.sym == SDLK_F1)
					this->rasterizer->switchRenderMode();

				if (e.key.keysym.sym == SDLK_w)
					this->camera->setMovementDirection(1);
				else if (e.key.keysym.sym == SDLK_s)
					this->camera->setMovementDirection(-1);

				if (e.key.keysym.sym == SDLK_DOWN)
					this->camera->setVRotationDirection(-1);
				else if (e.key.keysym.sym == SDLK_UP)
					this->camera->setVRotationDirection(1);

				if (e.key.keysym.sym == SDLK_LEFT)
					this->camera->setHRotationDirection(-1);
				else if (e.key.keysym.sym == SDLK_RIGHT)
					this->camera->setHRotationDirection(1);

				break;
			case SDL_KEYUP:
				if (e.key.keysym.sym == SDLK_w)
					this->camera->setMovementDirection(0);
				else if (e.key.keysym.sym == SDLK_s)
					this->camera->setMovementDirection(0);

				if (e.key.keysym.sym == SDLK_DOWN)
					this->camera->setVRotationDirection(0);
				else if (e.key.keysym.sym == SDLK_UP)
					this->camera->setVRotationDirection(0);

				if (e.key.keysym.sym == SDLK_LEFT)
					this->camera->setHRotationDirection(0);
				else if (e.key.keysym.sym == SDLK_RIGHT)
					this->camera->setHRotationDirection(0);

				break;
			case SDL_WINDOWEVENT:
				if (e.window.event == SDL_WINDOWEVENT_RESIZED)
				{
					this->texture->resize(e.window.data1, e.window.data2);
					this->rasterizer->resize(*this->texture);
				}
				break;
			case SDL_QUIT:
				quit = true;
				break;
			}
		}

		this->cube2->getTransform() = lm::mat4::createTransformMatrix({ angle, angle, 0 }, { -3, -2, -3 }, { 1, 1, 1 });
		this->sphere2->getTransform() = lm::mat4::createTransformMatrix({ angle, angle, 0 }, { -3, 2, -3 }, { 1, 1, 1 });

		this->cube->getTransform() = lm::mat4::createTransformMatrix({ -angle, -angle, 0 }, { 3, -2, -3 }, { 1, 1, 1 });
		this->sphere->getTransform() = lm::mat4::createTransformMatrix({ -angle, -angle, 0 }, { 3, 2, -3 }, { 1, 1, 1 });
		angle += 1;
		if (angle >= 360)
			angle = 0;

		this->rasterizer->renderScene(this->scene, this->texture, projection, this->camera->update());
	}
}

App::~App()
{
	if (this->window != nullptr)
		SDL_DestroyWindow(this->window);

	if (this->rasterizer != nullptr)
		delete this->rasterizer;

	if (this->texture != nullptr)
		delete this->texture;

	if (this->scene != nullptr)
		delete this->scene;

	if (this->camera != nullptr)
		delete this->camera;

	if (this->container != nullptr)
		delete this->container;

	SDL_Quit();
}