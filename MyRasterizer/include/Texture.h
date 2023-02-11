#pragma once
#include "Color.h"
#include <SDL_surface.h>
#include <vector>

#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

class Texture
{
public:
	Texture() = delete;
	Texture(unsigned int width, unsigned int height);
	Texture(const char* filePath);
	void SetPixelColor(unsigned int x, unsigned int y, const Color& c);
	Color* getColor();
	void setClearColor(const Color& color = Color::BLACK);
	Color& getTextPixelColor(unsigned int x, unsigned int y);
	Color& getPixelColor(int x, int y);
	std::vector<unsigned int>& getColoredPixel();
	unsigned int getWidth() const;
	unsigned int getHeight() const;
	void resize(unsigned int width, unsigned int height);
	void clearColor();
	SDL_Surface* createSurfaceFromTexture();
	~Texture();

private:
	int width = 0;
	int height = 0;
	Color clearedColor;
	Color* pixels = nullptr;
	std::vector<unsigned int> coloredPixels;
};