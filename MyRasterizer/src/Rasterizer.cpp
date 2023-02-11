#include "../include/Rasterizer.h"
#include "../include/Mat3/Mat3.h"
#include "SDL.h"
#include <algorithm>
#include <glm/mat4x4.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

Rasterizer::Rasterizer(SDL_Window& window, Texture& pTarget)
	: window(&window),
	suface(pTarget.createSurfaceFromTexture()),
	sufaceSize({ 0, 0, int(pTarget.getWidth()), int(pTarget.getHeight()) })
{
	this->zBuffer = new float[pTarget.getWidth() * pTarget.getHeight()];
	std::fill(this->zBuffer, this->zBuffer + (pTarget.getWidth() * pTarget.getHeight()), INFINITY);
}

void Rasterizer::clearRender(Texture& pTarget)
{
	this->clearBuffer(pTarget);
	pTarget.clearColor();
}

void Rasterizer::clearBuffer(Texture& pTarget)
{
	std::vector<unsigned int> coloredPixels = pTarget.getColoredPixel();
	for (unsigned int i = 0; i < coloredPixels.size(); i++)
		this->zBuffer[coloredPixels[i]] = INFINITY;
}

std::vector<float> Rasterizer::interpolate(lm::vec4 p1, lm::vec4 p2)
{
	std::vector<float> values;
	if (p2.X() == p1.X())
	{
		values.push_back(p1.Y());
		return values;
	}

	float a = (p2.Y() - p1.Y()) / (p2.X() - p1.X());
	float b = p1.Y();
	for (float x = p1.X(); x < p2.X(); x++, b = b + a)
		values.push_back(b);
	return values;
}

float Rasterizer::edgeFunction(const lm::vec2& a, const lm::vec2& b, const lm::vec2& c) const
{
	return (c.X() - a.X()) * (b.Y() - a.Y()) - (c.Y() - a.Y()) * (b.X() - a.X());
}

void Rasterizer::drawWireframeTriangle(Entity& entity, Texture& tex, const lm::mat4& projection, const lm::mat4& view)
{
	std::vector<VertexIndex>& indices = entity.getMesh()->getIndices();
	std::vector<Vertex>& vertices = entity.getMesh()->getVertices();
	for (int y = 0; y < indices.size(); y++)
	{
		VertexIndex& index = indices[y];

		lm::vec4 tP1 = (projection * view * entity.getTransform() * vertices[index.getA()].getPosition());
		lm::vec4 tP2 = (projection * view * entity.getTransform() * vertices[index.getB()].getPosition());
		lm::vec4 tP3 = (projection * view * entity.getTransform() * vertices[index.getC()].getPosition());

		std::vector<lm::vec4> newList = clippingTriangle(tP1, tP2, tP3);
		if (newList.size() == 0 || newList.size() == 2)
			continue;

		for (int x = 0; x < newList.size(); x++)
			this->spaceView(newList[x], tex);

		//is back face
		if (this->edgeFunction(newList[0], newList[1], newList[2]) < 0)
			continue;

		this->drawLine(newList[0], newList[1], tex, vertices[index.getA()].getColor());
		this->drawLine(newList[1], newList[2], tex, vertices[index.getB()].getColor());
		this->drawLine(newList[2], newList[0], tex, vertices[index.getC()].getColor());

		for (int i = 3; i < newList.size(); i++)
		{
			this->drawLine(newList[0], newList[i - 1], tex, vertices[index.getA()].getColor());
			this->drawLine(newList[i - 1], newList[i], tex, vertices[index.getB()].getColor());
			this->drawLine(newList[i], newList[0], tex, vertices[index.getC()].getColor());
		}
	}
}

void Rasterizer::drawLine(lm::vec4 tP1, lm::vec4 tP2, Texture& texture, Color& color)
{
	if (abs(tP2.X() - tP1.X()) > abs(tP2.Y() - tP1.Y()))
	{
		if (tP1.X() > tP2.X())
			std::swap(tP1, tP2);

		std::vector<float> ys = interpolate(tP1, tP2);
		for (float x = tP1.X(); x < tP2.X(); x++)
			if ((x >= 0 && x < texture.getWidth()) && (ys[unsigned int(x - tP1.X())] >= 0 && ys[unsigned int(x - tP1.X())] < texture.getHeight()))
				texture.SetPixelColor(unsigned int(x), unsigned int(ys[unsigned int(x - tP1.X())]), (color == Color::BLACK ? Color::WHITE : color));
	}
	else
	{
		if (tP1.Y() > tP2.Y())
			std::swap(tP1, tP2);

		std::vector<float> xs = interpolate({ tP1.Y(), tP1.X(), tP1.Z() }, { tP2.Y(), tP2.X(), tP2.Z() });
		for (float y = tP1.Y(); y < tP2.Y(); y++)
			if ((y >= 0 && y < texture.getHeight()) && (xs[unsigned int(y - tP1.Y())] >= 0 && xs[unsigned int(y - tP1.Y())] < texture.getWidth()))
				texture.SetPixelColor((unsigned int(xs[unsigned int(y - tP1.Y())])), unsigned int(y), (color == Color::BLACK ? Color::WHITE : color));
	}
}

std::tuple<float, float, float> Rasterizer::getBarycentricCoord(lm::vec4& p, Texture& tex, lm::vec2& v0, lm::vec2& v1, lm::vec2& v2, float area)
{
	this->spaceView(p, tex);

	float E0 = this->edgeFunction(v1, v2, p);
	float E1 = this->edgeFunction(v2, v0, p);
	float E2 = this->edgeFunction(v0, v1, p);

	float baryCo1 = E0 / area;
	float baryCo2 = E1 / area;
	float baryCo3 = E2 / area;

	return std::make_tuple(E0 / area, E1 / area, E2 / area);
}

Color Rasterizer::getClippedColor(lm::vec4& tP1, lm::vec4& tP2, lm::vec4& tP3, Vertex& p1, Vertex& p2, Vertex& p3, std::tuple<float, float, float>& baries)
{
	float w = (((1 / tP1.W()) * std::get<0>(baries)) + ((1 / tP2.W()) * std::get<1>(baries)) + ((1 / tP3.W()) * std::get<2>(baries)));

	float red = (((p1.getColor().getRedValue() / tP1.W()) * std::get<0>(baries)) + ((p2.getColor().getRedValue() / tP2.W()) * std::get<1>(baries)) + ((p3.getColor().getRedValue() / tP3.W()) * std::get<2>(baries))) / w;
	float green = (((p1.getColor().getGreenValue() / tP1.W()) * std::get<0>(baries)) + ((p2.getColor().getGreenValue() / tP2.W()) * std::get<1>(baries)) + ((p3.getColor().getGreenValue() / tP3.W()) * std::get<2>(baries))) / w;
	float blue = (((p1.getColor().getBlueValue() / tP1.W()) * std::get<0>(baries)) + ((p2.getColor().getBlueValue() / tP2.W()) * std::get<1>(baries)) + ((p3.getColor().getBlueValue() / tP3.W()) * std::get<2>(baries))) / w;
	float alpha = (((p1.getColor().getAlphaValue() / tP1.W()) * std::get<0>(baries)) + ((p2.getColor().getAlphaValue() / tP2.W()) * std::get<1>(baries)) + ((p3.getColor().getAlphaValue() / tP3.W()) * std::get<2>(baries))) / w;

	return Color(unsigned char(red), unsigned char(green), unsigned char(blue), unsigned char(alpha));
}

lm::vec3 Rasterizer::getClippedNormal(lm::vec4& tP1, lm::vec4& tP2, lm::vec4& tP3, Vertex& p1, Vertex& p2, Vertex& p3, std::tuple<float, float, float>& baries)
{
	float w = (((1 / tP1.W()) * std::get<0>(baries)) + ((1 / tP2.W()) * std::get<1>(baries)) + ((1 / tP3.W()) * std::get<2>(baries)));

	float normX = (((p1.getNormal().X() / tP1.W()) * std::get<0>(baries)) + ((p2.getNormal().X() / tP2.W()) * std::get<1>(baries)) + ((p3.getNormal().X() / tP2.W()) * std::get<2>(baries))) / w;
	float normY = (((p1.getNormal().Y() / tP1.W()) * std::get<0>(baries)) + ((p2.getNormal().Y() / tP2.W()) * std::get<1>(baries)) + ((p3.getNormal().Y() / tP2.W()) * std::get<2>(baries))) / w;
	float normZ = (((p1.getNormal().Z() / tP1.W()) * std::get<0>(baries)) + ((p2.getNormal().Z() / tP2.W()) * std::get<1>(baries)) + ((p3.getNormal().Z() / tP2.W()) * std::get<2>(baries))) / w;

	return lm::vec3(normX, normY, normZ);
}

lm::vec2 Rasterizer::getClippedTexCoord(lm::vec4& tP1, lm::vec4& tP2, lm::vec4& tP3, Vertex& p1, Vertex& p2, Vertex& p3, std::tuple<float, float, float>& baries)
{
	float w = (((1 / tP1.W()) * std::get<0>(baries)) + ((1 / tP2.W()) * std::get<1>(baries)) + ((1 / tP3.W()) * std::get<2>(baries)));

	float texCoordX = (((p1.getTextCoord().X() / tP1.W()) * std::get<0>(baries)) + ((p2.getTextCoord().X() / tP2.W()) * std::get<1>(baries)) + ((p3.getTextCoord().X() / tP3.W()) * std::get<2>(baries))) / w;
	float texCoordY = (((p1.getTextCoord().Y() / tP1.W()) * std::get<0>(baries)) + ((p2.getTextCoord().Y() / tP2.W()) * std::get<1>(baries)) + ((p3.getTextCoord().Y() / tP3.W()) * std::get<2>(baries))) / w;

	return lm::vec2(texCoordX, texCoordY);
}

void Rasterizer::spaceView(lm::vec4& point, Texture& screen)
{
	point.homogenize();
	point.X() = (1 + point.X()) * 0.5f * (screen.getWidth() - 1);
	point.Y() = (1 - (point.Y() + 1) * 0.5f) * (screen.getHeight() - 1);
}

void Rasterizer::fillClippedTriangle(lm::vec4 tP1, lm::vec4 tP2, lm::vec4 tP3, Vertex p1, Vertex p2, Vertex p3, Entity& entity, Scene& pScene, Texture& tex, const lm::mat4& projection, const lm::mat4& view)
{	
	lm::vec2 vec0(tP1);
	lm::vec2 vec1(tP2);
	lm::vec2 vec2(tP3);

	float area = this->edgeFunction(vec0, vec1, vec2);
	//is back face
	if (area < 0)
		return;

	int maxX = int(std::max(tP1.X(), std::max(tP2.X(), tP3.X())));
	int minX = int(std::min(tP1.X(), std::min(tP2.X(), tP3.X())));
	int maxY = int(std::max(tP1.Y(), std::max(tP2.Y(), tP3.Y())));
	int minY = int(std::min(tP1.Y(), std::min(tP2.Y(), tP3.Y())));

	for (int y = minY; y <= maxY; y++)
	{
		bool done = false;
		for (int x = minX; x <= maxX; x++)
		{
			float pX = float(x);
			float pY = float(y);
			lm::vec2 p(pX, pY);
			float E0 = this->edgeFunction(vec1, vec2, p);
			float E1 = this->edgeFunction(vec2, vec0, p);
			float E2 = this->edgeFunction(vec0, vec1, p);

			if (E0 == 0 && E1 == 0 && E2 == 0)
				continue;

			bool overlaps = true;
			if ((E0 >= 0 && E1 >= 0 && E2 >= 0))
			{
				done = true;

				float baryCo1 = E0 / area;
				float baryCo2 = E1 / area;
				float baryCo3 = E2 / area;

				// interpolate one over w
				float w = (((1 / tP1.W()) * baryCo1) + ((1 / tP2.W()) * baryCo2) + ((1 / tP3.W()) * baryCo3));

				// interpolate zBuffer
				float z = (((tP1.Z() / tP1.W()) * baryCo1) + ((tP2.Z() / tP2.W()) * baryCo2) + ((tP3.Z() / tP3.W()) * baryCo3)) / w;
				unsigned int idx = (y * tex.getWidth()) + x;
				if (this->zBuffer[idx] <= z)
					continue;
				this->zBuffer[idx] = z;

				// interpolate color
				float red = (((p1.getColor().getRedValue() / tP1.W()) * baryCo1) + ((p2.getColor().getRedValue() / tP2.W()) * baryCo2) + ((p3.getColor().getRedValue() / tP3.W()) * baryCo3)) / w;
				float green = (((p1.getColor().getGreenValue() / tP1.W()) * baryCo1) + ((p2.getColor().getGreenValue() / tP2.W()) * baryCo2) + ((p3.getColor().getGreenValue() / tP3.W()) * baryCo3)) / w;
				float blue = (((p1.getColor().getBlueValue() / tP1.W()) * baryCo1) + ((p2.getColor().getBlueValue() / tP2.W()) * baryCo2) + ((p3.getColor().getBlueValue() / tP3.W()) * baryCo3)) / w;
				float alpha = (((p1.getColor().getAlphaValue() / tP1.W()) * baryCo1) + ((p2.getColor().getAlphaValue() / tP2.W()) * baryCo2) + ((p3.getColor().getAlphaValue() / tP3.W()) * baryCo3)) / w;
				Color finalColor = Color(unsigned char(red), unsigned char(green), unsigned char(blue), unsigned char(alpha));


				if (entity.getMesh()->getTexture() != nullptr)
				{
					// interpolate texture
					float texCoordX = (((p1.getTextCoord().X() / tP1.W()) * baryCo1) + ((p2.getTextCoord().X() / tP2.W()) * baryCo2) + ((p3.getTextCoord().X() / tP3.W()) * baryCo3)) / w;
					float texCoordY = (((p1.getTextCoord().Y() / tP1.W()) * baryCo1) + ((p2.getTextCoord().Y() / tP2.W()) * baryCo2) + ((p3.getTextCoord().Y() / tP3.W()) * baryCo3)) / w;

					// Apply bilinear filtering
					float uTexel = (entity.getMesh()->getTexture()->getWidth() - 1) * texCoordX;
					float vTexel = (entity.getMesh()->getTexture()->getHeight() - 1) * texCoordY;
					
					unsigned int uInt = unsigned int(std::floorf(std::clamp(uTexel - 0.5f, 0.f, (float)(entity.getMesh()->getTexture()->getWidth() - 1))));
					unsigned int vInt = unsigned int(std::floorf(std::clamp(vTexel - 0.5f, 0.f, (float)(entity.getMesh()->getTexture()->getHeight() - 1))));


					float uFrac = uTexel - uInt - 0.5f;
					if (uFrac < 0)
						uFrac += 0.5f;

					float vFrac = vTexel - vInt - 0.5f;
					if (vFrac < 0)
						vFrac += 0.5f;

					Color C00 = entity.getMesh()->getTexture()->getTextPixelColor((uInt), (vInt));
					Color C10 = entity.getMesh()->getTexture()->getTextPixelColor((uInt + 1), (vInt));
					Color C01 = entity.getMesh()->getTexture()->getTextPixelColor((uInt), (vInt + 1));
					Color C11 = entity.getMesh()->getTexture()->getTextPixelColor((uInt + 1), (vInt + 1));

					Color CminV = C00 * (1 - uFrac) + C10 * uFrac;
					Color CmaxV = C01 * (1 - uFrac) + C11 * uFrac;

					finalColor += CminV * (1 - vFrac) + CmaxV * vFrac;
				}

				// interpolate Normal
				float normX = (((p1.getNormal().X() / tP1.W()) * baryCo1) + ((p2.getNormal().X() / tP2.W()) * baryCo2) + ((p3.getNormal().X() / tP2.W()) * baryCo3)) / w;
				float normY = (((p1.getNormal().Y() / tP1.W()) * baryCo1) + ((p2.getNormal().Y() / tP2.W()) * baryCo2) + ((p3.getNormal().Y() / tP2.W()) * baryCo3)) / w;
				float normZ = (((p1.getNormal().Z() / tP1.W()) * baryCo1) + ((p2.getNormal().Z() / tP2.W()) * baryCo2) + ((p3.getNormal().Z() / tP2.W()) * baryCo3)) / w;
				for (unsigned int i = 0; i < pScene.getLights().size(); i++)
					finalColor = pScene.getLights()[i].calculateColor(lm::vec3(normX, normY, normZ), finalColor, entity.getTransform());

				//alpha-blending
				finalColor = (finalColor * entity.getAlpha()) + (tex.getPixelColor(x, y) * (1 - entity.getAlpha()));

				tex.SetPixelColor(x, y, finalColor);
			}

			if (!((E0 >= 0) && (E1 >= 0) && (E2 >= 0)) && done)
				break;
		}
	}
}

void Rasterizer::fillTriangle(Vertex p1, Vertex p2, Vertex p3, Entity& entity, Scene& pScene, Texture& tex, const lm::mat4& projection, const lm::mat4& view)
{
	lm::vec4 tP1 = (projection * view * entity.getTransform() * p1.getPosition());
	lm::vec4 tP2 = (projection * view * entity.getTransform() * p2.getPosition());
	lm::vec4 tP3 = (projection * view * entity.getTransform() * p3.getPosition());

	std::vector<lm::vec4> newList = clippingTriangle(tP1, tP2, tP3);
	if (newList.size() == 0 || newList.size() == 2)
		return;

	this->spaceView(tP1, tex);
	this->spaceView(tP2, tex);
	this->spaceView(tP3, tex);

	lm::vec2 vec0(tP1);
	lm::vec2 vec1(tP2);
	lm::vec2 vec2(tP3);

	float area = std::abs(this->edgeFunction(vec0, vec1, vec2));
	std::tuple<float, float, float> baries1 = getBarycentricCoord(newList[0], tex, vec0, vec1, vec2, area);
	std::tuple<float, float, float> baries2 = getBarycentricCoord(newList[1], tex, vec0, vec1, vec2, area);
	std::tuple<float, float, float> baries3 = getBarycentricCoord(newList[2], tex, vec0, vec1, vec2, area);

	Vertex v1(newList[0].X(), newList[0].Y(), newList[0].Z(), getClippedNormal(tP1, tP2, tP3, p1, p2, p3, baries1), getClippedTexCoord(tP1, tP2, tP3, p1, p2, p3, baries1), getClippedColor(tP1, tP2, tP3, p1, p2, p3, baries1));
	Vertex v2(newList[1].X(), newList[1].Y(), newList[1].Z(), getClippedNormal(tP1, tP2, tP3, p1, p2, p3, baries2), getClippedTexCoord(tP1, tP2, tP3, p1, p2, p3, baries2), getClippedColor(tP1, tP2, tP3, p1, p2, p3, baries2));
	Vertex prev(newList[2].X(), newList[2].Y(), newList[2].Z(), getClippedNormal(tP1, tP2, tP3, p1, p2, p3, baries3), getClippedTexCoord(tP1, tP2, tP3, p1, p2, p3, baries3), getClippedColor(tP1, tP2, tP3, p1, p2, p3, baries3));
	fillClippedTriangle(newList[0], newList[1], newList[2], v1, v2, prev, entity, pScene, tex, projection, view);

	for (int i = 3; i < newList.size(); i++)
	{
		std::tuple<float, float, float> baries = getBarycentricCoord(newList[i], tex, vec0, vec1, vec2, area);
		Vertex current(newList[i].X(), newList[i].Y(), newList[i].Z(), getClippedNormal(tP1, tP2, tP3, p1, p2, p3, baries), getClippedTexCoord(tP1, tP2, tP3, p1, p2, p3, baries), getClippedColor(tP1, tP2, tP3, p1, p2, p3, baries));
		fillClippedTriangle(newList[0], newList[i - 1], newList[i], v1, prev, current, entity, pScene, tex, projection, view);
		prev = current;
	}
}

std::vector<lm::vec4> Rasterizer::clippingTriangle(lm::vec4& p1, lm::vec4& p2, lm::vec4& p3)
{
	std::vector<bool> pointA = this->checkBoundaries(p1);
	std::vector<bool> pointB = this->checkBoundaries(p2);
	std::vector<bool> pointC = this->checkBoundaries(p3);

	bool Ainside = true;
	for (int x = 0; x < pointA.size(); x++)
	{
		if (!pointA[x])
			Ainside = false;
	}

	bool Binside = true;
	for (int x = 0; x < pointB.size(); x++)
	{
		if (!pointB[x])
			Ainside = false;
	}

	bool Cinside = true;
	for (int x = 0; x < pointC.size(); x++)
	{
		if (!pointC[x])
			Ainside = false;
	}

	if (Ainside && Binside && Cinside)
		return std::vector<lm::vec4> {p1, p2, p3};
	else if (!Ainside && !Binside && !Cinside)
		return std::vector<lm::vec4>();
	else
	{
		std::vector<lm::vec4> outputList = {p1, p2, p3};

		for (int x = 0; x < 6; x++)
		{
			std::vector<lm::vec4> inputList = outputList;
			outputList.clear();
			for (int y = 0; y < inputList.size(); y++)
			{
				lm::vec4 currentPoint = inputList[y];
				lm::vec4 prevPoint;
				int size = int(inputList.size());
				if ((y - 1) < 0)
				{
					int index = (size + ((y - 1) % size) % size);
					prevPoint = inputList[index];
				}
				else 
					prevPoint = inputList[(y - 1) % size];

				std::vector<float> boundaryList1 = boundaryCoord(currentPoint);
				std::vector<float> boundaryList2 = boundaryCoord(prevPoint);
				lm::vec4 intersectPoint = currentPoint + (boundaryList1[x] / (boundaryList1[x] - boundaryList2[x])) * (prevPoint - currentPoint);
				if (boundaryList1[x] >= 0)
				{
					if (boundaryList2[x] <= 0)
						outputList.push_back(intersectPoint);
					outputList.push_back(currentPoint);
				}
				else if (boundaryList2[x] >= 0)
					outputList.push_back(intersectPoint);
			}

		}
		return outputList;
	}
}

std::vector<float> Rasterizer::boundaryCoord(lm::vec4& p1)
{
	std::vector<float> boundaryList;
	boundaryList.emplace_back(p1.W() + p1.Y()); //bottom
	boundaryList.emplace_back(p1.W() - p1.Y()); //top
	boundaryList.emplace_back(p1.W() - p1.X()); //right
	boundaryList.emplace_back(p1.W() + p1.X()); //left
	boundaryList.emplace_back(p1.W() - p1.Z()); //far
	boundaryList.emplace_back(p1.W() + p1.Z()); //near
	return boundaryList;
}

std::vector<bool> Rasterizer::checkBoundaries(lm::vec4& p1)
{
	std::vector<bool> boolList;

	if (p1.Y() > -abs(p1.W())) //bot
		boolList.push_back(true);
	else
		boolList.push_back(false);

	if (p1.Y() < abs(p1.W())) //top
		boolList.push_back(true);
	else
		boolList.push_back(false);

	if (p1.X() < abs(p1.W())) //Right
		boolList.push_back(true);
	else
		boolList.push_back(false);

	if (p1.X() > -abs(p1.W())) //Left
		boolList.push_back(true);
	else
		boolList.push_back(false);

	if (p1.Z() < abs(p1.W())) //Far
		boolList.push_back(true);
	else
		boolList.push_back(false);

	if (p1.Z() > -abs(p1.W())) //Near
		boolList.push_back(true);
	else
		boolList.push_back(false);

	return boolList;
}

void Rasterizer::drawEntity(Scene& pScene, Texture& pTarget, const lm::mat4& projection, const lm::mat4& view)
{
	for (unsigned int x = 0; x < pScene.getEntities().size(); x++)
	{
		Entity* entity = pScene.getEntities()[x];

		if (this->mode == RENDER_MODE::WIRED)
			this->drawWireframeTriangle(*entity, pTarget, projection, view);
		else if (this->mode == RENDER_MODE::FILLED)
		{
			std::vector<VertexIndex>& indices = entity->getMesh()->getIndices();
			std::vector<Vertex>& vertices = entity->getMesh()->getVertices();
			for (unsigned int y = 0; y < indices.size(); y++)
			{
				VertexIndex& index = indices[y];
				this->fillTriangle(
					vertices[index.getA()],
					vertices[index.getB()],
					vertices[index.getC()],
					*entity,
					pScene,
					pTarget,
					projection,
					view);
			}
		}
	}
}

void Rasterizer::renderScene(Scene* pScene, Texture* pTarget, const lm::mat4& projection, const lm::mat4& view)
{
	if (pScene == nullptr && pTarget == nullptr && this->suface != nullptr)
		return;

	this->clearRender(*pTarget);

	this->drawEntity(*pScene, *pTarget, projection, view);

	this->updateWindow();
}

void Rasterizer::updateWindow()
{
	SDL_FillRect(SDL_GetWindowSurface(window), &this->sufaceSize, 0xff000000);
	SDL_LowerBlit(this->suface, &this->sufaceSize, SDL_GetWindowSurface(window), &this->sufaceSize);
	SDL_UpdateWindowSurface(this->window);
}

void Rasterizer::resize(Texture& pTarget)
{
	if (this->suface != nullptr)
		SDL_FreeSurface(this->suface);

	if (this->zBuffer != nullptr)
		delete[] this->zBuffer;

	this->zBuffer = new float[pTarget.getWidth() * pTarget.getHeight()];
	std::fill(this->zBuffer, this->zBuffer + (pTarget.getWidth() * pTarget.getHeight()), INFINITY);

	this->sufaceSize = { 0, 0, int(pTarget.getWidth()), int(pTarget.getHeight()) };
	this->suface = pTarget.createSurfaceFromTexture();
}

void Rasterizer::setRenderMode(RENDER_MODE mode)
{
	this->mode = mode;
}

void Rasterizer::switchRenderMode()
{
	if (this->mode == RENDER_MODE::FILLED)
		this->mode = RENDER_MODE::WIRED;
	else if (this->mode == RENDER_MODE::WIRED)
		this->mode = RENDER_MODE::FILLED;
}

Rasterizer::~Rasterizer()
{
	if (this->suface != nullptr)
		SDL_FreeSurface(this->suface);

	if (this->zBuffer != nullptr)
		delete[] this->zBuffer;
}