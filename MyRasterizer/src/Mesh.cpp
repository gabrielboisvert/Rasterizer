#include "../include/Mesh.h"

Mesh::Mesh() {}

Mesh::Mesh(const Mesh& mesh)
{
	this->vertices = mesh.vertices;
	this->indices = mesh.indices;
	this->texture = mesh.texture;
}

Mesh::Mesh(Mesh* mesh)
{
	this->vertices = mesh->vertices;
	this->indices = mesh->indices;
	this->texture = mesh->texture;
}

std::shared_ptr<Mesh> Mesh::createCube(Color color)
{
	float dim = CUBE_LENGTH / 2;
	Mesh* newMesh = new Mesh;
	
	//front
	newMesh->vertices.emplace_back(-dim, -dim, dim, lm::vec3(0, 0, 1), lm::vec2(0, 0), color);
	newMesh->vertices.emplace_back(dim, -dim, dim, lm::vec3(0, 0, 1), lm::vec2(1, 0), color);
	newMesh->vertices.emplace_back(-dim, dim, dim, lm::vec3(0, 0, 1), lm::vec2(0, 1), color);
	newMesh->vertices.emplace_back(dim, dim, dim, lm::vec3(0, 0, 1), lm::vec2(1, 1), color);
	newMesh->indices.emplace_back(0, 1, 2);
	newMesh->indices.emplace_back(1, 3, 2);

	//left
	newMesh->vertices.emplace_back(-dim, -dim, dim, lm::vec3(-1, 0, 0), lm::vec2(1, 0), color);
	newMesh->vertices.emplace_back(-dim, dim, -dim, lm::vec3(-1, 0, 0), lm::vec2(0, 1), color);
	newMesh->vertices.emplace_back(-dim, -dim, -dim, lm::vec3(-1, 0, 0), lm::vec2(0, 0), color);
	newMesh->vertices.emplace_back(-dim, dim, dim, lm::vec3(-1, 0, 0), lm::vec2(1, 1), color);
	newMesh->indices.emplace_back(4, 5, 6);
	newMesh->indices.emplace_back(4, 7, 5);

	//back
	newMesh->vertices.emplace_back(-dim, -dim, -dim, lm::vec3(0, 0, -1), lm::vec2(0, 0), color);
	newMesh->vertices.emplace_back(dim, dim, -dim, lm::vec3(0, 0, -1), lm::vec2(1, 1), color);
	newMesh->vertices.emplace_back(-dim, dim, -dim, lm::vec3(0, 0, -1), lm::vec2(0, 1), color);
	newMesh->vertices.emplace_back(dim, -dim, -dim, lm::vec3(0, 0, -1), lm::vec2(1, 0), color);
	newMesh->indices.emplace_back(8, 9, 11);
	newMesh->indices.emplace_back(8, 10, 9);

	//right
	newMesh->vertices.emplace_back(dim, -dim, dim, lm::vec3(1, 0, 0), lm::vec2(1, 0), color);
	newMesh->vertices.emplace_back(dim, dim, -dim, lm::vec3(1, 0, 0), lm::vec2(0, 1), color);
	newMesh->vertices.emplace_back(dim, -dim, -dim, lm::vec3(1, 0, 0), lm::vec2(0, 0), color);
	newMesh->vertices.emplace_back(dim, dim, dim, lm::vec3(1, 0, 0), lm::vec2(1, 1), color);
	newMesh->indices.emplace_back(15, 12, 14);
	newMesh->indices.emplace_back(13, 15, 14);

	//bottom
	newMesh->vertices.emplace_back(-dim, -dim, dim, lm::vec3(0, -1, 0), lm::vec2(0, 1), color);
	newMesh->vertices.emplace_back(dim, -dim, dim, lm::vec3(0, -1, 0), lm::vec2(1, 1), color);
	newMesh->vertices.emplace_back(-dim, -dim, -dim, lm::vec3(0, -1, 0), lm::vec2(0, 0), color);
	newMesh->vertices.emplace_back(dim, -dim, -dim, lm::vec3(0, -1, 0), lm::vec2(1, 0), color);
	newMesh->indices.emplace_back(18, 17, 16);
	newMesh->indices.emplace_back(17, 18, 19);

	//top
	newMesh->vertices.emplace_back(-dim, dim, dim, lm::vec3(0, 1, 0), lm::vec2(0, 1), color);
	newMesh->vertices.emplace_back(-dim, dim, -dim, lm::vec3(0, 1, 0), lm::vec2(0, 0), color);
	newMesh->vertices.emplace_back(dim, dim, dim, lm::vec3(0, 1, 0), lm::vec2(1, 1), color);
	newMesh->vertices.emplace_back(dim, dim, -dim, lm::vec3(0, 1, 0), lm::vec2(1, 0), color);
	newMesh->indices.emplace_back(21, 20, 22);
	newMesh->indices.emplace_back(22, 23, 21);

	return std::make_shared<Mesh>(newMesh);
}

std::shared_ptr<Mesh> Mesh::createSphere(unsigned int sectors, Color color)
{
	Mesh* newMesh = new Mesh;

	float lengthInv = 1.0f / SPHERE_RADIUS;                                   
	float sectorStep = float(2 * M_PI / sectors);
	float stackStep = float(M_PI / sectors);

	for (unsigned int i = 0; i <= sectors; ++i)
	{
		float stackAngle = float(M_PI / 2 - i * stackStep);
		float xy = SPHERE_RADIUS * cosf(stackAngle);
		float z = SPHERE_RADIUS * sinf(stackAngle);

		int k1 = i * (sectors + 1);
		int k2 = k1 + sectors + 1;

		for (unsigned int j = 0; j <= sectors; ++j, ++k1, ++k2)
		{
			float sectorAngle = j * sectorStep;           // starting from 0 to 2pi

			float x = xy * cosf(sectorAngle);             
			float y = xy * sinf(sectorAngle);             
			lm::vec3 position(x, y, z);

			float nx = x * lengthInv;
			float ny = y * lengthInv;
			float nz = z * lengthInv;
			lm::vec3 normal(nx, ny, nz);

			float s = (float)j / sectors;
			float t = (float)i / sectors;
			lm::vec2 texCoord(s, t);

			newMesh->vertices.emplace_back(position, normal, texCoord, color);

			if(i < sectors)
				if (j < sectors)
				{
					if (i != 0)
						newMesh->indices.emplace_back(k1, k2, k1 + 1);

					if (i != (sectors - 1))
						newMesh->indices.emplace_back(k1 + 1, k2, k2 + 1);
				}
		}
	}
	return std::make_shared<Mesh>(newMesh);
}

std::shared_ptr<Mesh> Mesh::createTriangle()
{
	Mesh* newMesh = new Mesh;
	newMesh->vertices.reserve(8);
	float dim = TRIANGLE_LENGTH / 2;
	newMesh->vertices.emplace_back(-dim, -dim, 0.f, lm::vec3(0, 0, 1), lm::vec2(0, 0), Color::GREEN);
	newMesh->vertices.emplace_back(dim, -dim, 0.f, lm::vec3(0, 0, 1), lm::vec2(1, 0), Color::RED);
	newMesh->vertices.emplace_back(0.f, dim, 0.f, lm::vec3(0, 0, 1), lm::vec2(0.5f, 0.5f), Color::BLUE);

	newMesh->indices.emplace_back(0, 1, 2);
	return std::make_shared<Mesh>(newMesh);
}

std::vector<VertexIndex>& Mesh::getIndices()
{
	return this->indices;
}

std::vector<Vertex>& Mesh::getVertices()
{
	return this->vertices;
}

void Mesh::setTexture(Texture* texture)
{
	this->texture = texture;
}

Texture* Mesh::getTexture()
{
	return this->texture;
}

VertexIndex::VertexIndex(int a, int b, int c)
{
	this->A = a;
	this->B = b;
	this->C = c;
}

int VertexIndex::getA()
{
	return this->A;
}

int VertexIndex::getB()
{
	return this->B;
}

int VertexIndex::getC()
{
	return this->C;
}
