#include "../include/Texture.h"
#include <iostream>

Texture::Texture(unsigned int width, unsigned int height) : width(width), height(height)
{
	this->pixels = new Color[width * height];
}

Texture::Texture(const char* filePath)
{
	int comp;
	unsigned char* colors = stbi_load(filePath, &this->width, &this->height, &comp, STBI_rgb_alpha);
	this->pixels = new Color[this->width * this->height];
	unsigned int totalSize = this->width * this->height * 4;
	unsigned int basicIdx = 0;
	for (unsigned int i = 0; i < totalSize; i += 4, basicIdx++)
		this->pixels[basicIdx] = Color(colors[i], colors[i + 1], colors[i + 2], colors[i + 3]);

	stbi_image_free(colors);
}

void Texture::SetPixelColor(unsigned int x, unsigned int y, const Color& c)
{
	unsigned int idx = (y * this->width) + x;
	this->pixels[idx] = c;

	if (c != this->clearedColor)
		this->coloredPixels.push_back(idx);
}

Color* Texture::getColor()
{
	return this->pixels;
}

void Texture::setClearColor(const Color& color)
{
	this->clearedColor = color;
	std::fill(this->pixels, this->pixels + (width * height), color);
}

Color& Texture::getTextPixelColor(unsigned int x, unsigned int y)
{
	unsigned int idx = unsigned int(y * this->width + x);
	return this->pixels[idx];
}

Color& Texture::getPixelColor(int x, int y)
{
	return this->pixels[(y * this->width) + x];
}

std::vector<unsigned int>& Texture::getColoredPixel()
{
	return this->coloredPixels;
}

unsigned int Texture::getWidth() const
{
	return this->width;
}

unsigned int Texture::getHeight() const
{
	return this->height;
}

void Texture::resize(unsigned int width, unsigned int height)
{
	if (this->pixels != nullptr)
		delete[] this->pixels;

	this->pixels = new Color[width * height];
	std::fill(this->pixels, this->pixels + (width * height), this->clearedColor);

	this->coloredPixels.clear();
	this->width = width;
	this->height = height;
}

void Texture::clearColor()
{
	for (unsigned int i = 0; i < this->coloredPixels.size(); i++)
		this->pixels[this->coloredPixels[i]] = this->clearedColor;

	this->coloredPixels.clear();
}

Texture::~Texture()
{
	if (this->pixels != nullptr)
		delete[] this->pixels;
}

SDL_Surface* Texture::createSurfaceFromTexture()
{
	Uint32 rmask, gmask, bmask, amask;

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	rmask = 0xff000000 >> 0;
	gmask = 0x00ff0000 >> 0;
	bmask = 0x0000ff00 >> 0;
	amask = 0x000000ff >> 0;
#else // little endian
	rmask = 0x000000ff;
	gmask = 0x0000ff00;
	bmask = 0x00ff0000;
	amask = 0xff000000;
#endif

	SDL_Surface* surface = SDL_CreateRGBSurfaceFrom(this->pixels, this->width, this->height, 32, 4 * this->width, rmask, gmask, bmask, amask);
	return surface;
}