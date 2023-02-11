#include "..\include\Color.h"
#include "../include/Color.h"
#include <algorithm>

const Color Color::BLACK;
const Color Color::WHITE(255, 255, 255, 255);
const Color Color::RED(255, 0, 0, 255);
const Color Color::GREEN(0, 255, 0, 255);
const Color Color::BLUE(0, 0, 255, 255);
const Color Color::PURPLE(0, 255, 255, 255);
const Color Color::YELLOW(255, 255, 0, 255);

Color::Color(unsigned char r, unsigned char g, unsigned char b, unsigned char a) : r(r), g(g), b(b), a(a) {}

const bool Color::operator==(const Color& color) const
{
	return this->r == color.r && this->g == color.g && this->b == color.b && this->a == color.a;
}

const bool Color::operator!=(const Color& color) const
{
	return !(*this == color);
}

const Color Color::operator+(const Color& color) const
{
	float red = (this->r + color.r) > unsigned char(255) ? unsigned char(255) : unsigned char(this->r + color.r);
	float green = (this->g + color.g) > unsigned char(255) ? unsigned char(255) : unsigned char(this->g + color.g);
	float blue = (this->b + color.b) > unsigned char(255) ? unsigned char(255) : unsigned char(this->b + color.b);
	float alpha = (this->a + color.a) > unsigned char(255) ? unsigned char(255) : unsigned char(this->a + color.a);
	return Color(unsigned char(red), unsigned char(green), unsigned char(blue), unsigned char(alpha));
}

Color& Color::operator+=(const Color& color)
{
	if (*this == color)
		return *this;

	this->r = (this->r + color.r) > unsigned char(255) ? unsigned char(255) : (this->r + color.r);
	this->g = (this->g + color.g) > unsigned char(255) ? unsigned char(255) : (this->g + color.g);
	this->b = (this->b + color.b) > unsigned char(255) ? unsigned char(255) : (this->b + color.b);
	this->a = (this->a + color.a) > unsigned char(255) ? unsigned char(255) : (this->a + color.a);
	return *this;
}

const Color Color::operator*(float scale) const
{
	return Color(unsigned char(this->r * scale), unsigned char(this->g * scale), unsigned char(this->b * scale), unsigned char(this->a));
}

unsigned char& Color::getRedValue()
{
	return this->r;
}

unsigned char& Color::getGreenValue()
{
	return this->g;
}

unsigned char& Color::getBlueValue()
{
	return this->b;
}

unsigned char& Color::getAlphaValue()
{
	return this->a;
}
