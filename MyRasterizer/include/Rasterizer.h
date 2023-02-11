#pragma once
#include "Scene.h"
#include "Texture.h"
#include <SDL_video.h>
#include "Vec2/Vec2.h"
#include <tuple>

class Rasterizer
{
public:

	enum class RENDER_MODE
	{
		WIRED,
		FILLED
	};

	Rasterizer(SDL_Window& window, Texture& pTarget);
	
	std::vector<float> interpolate(lm::vec4 p1, lm::vec4 p2);

	float edgeFunction(const lm::vec2& a, const lm::vec2& b, const lm::vec2& c) const;
	
	void drawWireframeTriangle(Entity& entity, Texture& tex, const lm::mat4& projection, const lm::mat4& view);

	void drawLine(lm::vec4 p1, lm::vec4 p2, Texture& texture, Color& color);

	std::tuple<float, float, float> getBarycentricCoord(lm::vec4& p, Texture& tex, lm::vec2& v0, lm::vec2& v1, lm::vec2& v2, float area);

	Color getClippedColor(lm::vec4& tP1, lm::vec4& tP2, lm::vec4& tP3, Vertex& p1, Vertex& p2, Vertex& p3, std::tuple<float, float, float>& baries);

	lm::vec3 getClippedNormal(lm::vec4& tP1, lm::vec4& tP2, lm::vec4& tP3, Vertex& p1, Vertex& p2, Vertex& p3, std::tuple<float, float, float>& baries);

	lm::vec2 getClippedTexCoord(lm::vec4& tP1, lm::vec4& tP2, lm::vec4& tP3, Vertex& p1, Vertex& p2, Vertex& p3, std::tuple<float, float, float>& baries);

	void spaceView(lm::vec4& point, Texture& screen);

	void fillTriangle(Vertex p1, Vertex p2, Vertex p3, Entity& entity, Scene& pScene, Texture& tex, const lm::mat4& projection, const lm::mat4& view);

	void fillClippedTriangle(lm::vec4 tP1, lm::vec4 tP2, lm::vec4 tP3, Vertex p1, Vertex p2, Vertex p3, Entity& entity, Scene& pScene, Texture& tex, const lm::mat4& projection, const lm::mat4& view);

	std::vector<lm::vec4> clippingTriangle(lm::vec4& p1, lm::vec4& p2, lm::vec4& p3);

	std::vector<float> boundaryCoord(lm::vec4& p1);

	std::vector<bool> checkBoundaries(lm::vec4& p1);

	void renderScene(Scene* pScene, Texture* pTarget, const lm::mat4& projection, const lm::mat4& view);

	void updateWindow();

	void resize(Texture& pTarget);

	void setRenderMode(RENDER_MODE mode);

	void switchRenderMode();

	void drawEntity(Scene& pScene, Texture& pTarget, const lm::mat4& projection, const lm::mat4& view);

	void clearRender(Texture& pTarget);

	void clearBuffer(Texture& pTarget);

	~Rasterizer();

private:
	SDL_Window* window = nullptr;
	SDL_Surface* suface = nullptr;
	SDL_Rect sufaceSize = { 0, 0, 0, 0 };
	float* zBuffer = nullptr;
	RENDER_MODE mode = RENDER_MODE::FILLED;
};