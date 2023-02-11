#pragma once
#include <vector>
#include "Vec3/Vec3.h"
#include "Color.h"
#include "Vertex.h"
#include "Texture.h"
#define CUBE_LENGTH 1.0f
#define SPHERE_RADIUS 1.0f
#define TRIANGLE_LENGTH 1.0f

class VertexIndex
{
public:
	VertexIndex(int a, int b, int c);
	int getA();
	int getB();
	int getC();

private:
	int A;
	int B;
	int C;
};

class Mesh
{
public:
	Mesh();
	Mesh(const Mesh& mesh);
	Mesh(Mesh* mesh);

	static std::shared_ptr<Mesh> createCube(Color color = Color::WHITE);
	static std::shared_ptr<Mesh> createSphere(unsigned int sectors, Color color = Color::WHITE);
	static std::shared_ptr<Mesh> createTriangle();

	std::vector<VertexIndex>& getIndices();
	std::vector<Vertex>& getVertices();
	void setTexture(Texture* texture);
	Texture* getTexture();

private:
	std::vector<Vertex> vertices;
	std::vector<VertexIndex> indices;
	Texture* texture = nullptr;
};