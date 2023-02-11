#pragma once

class Color
{
public:
	Color() = default;
	Color(unsigned char r, unsigned char g, unsigned char b, unsigned char a = 255);

	static const Color BLACK;
	static const Color WHITE;
	static const Color RED;
	static const Color GREEN;
	static const Color BLUE;
	static const Color PURPLE;
	static const Color YELLOW;

	const bool operator==(const Color& color) const;
	const bool operator!=(const Color& color) const;
	const Color operator+(const Color& color) const;
	Color& operator+=(const Color& color);
	const Color operator*(float scale) const;
	unsigned char& getRedValue();
	unsigned char& getGreenValue();
	unsigned char& getBlueValue();
	unsigned char& getAlphaValue();

private:
	unsigned char r = 0;
	unsigned char g = 0;
	unsigned char b = 0;
	unsigned char a = 255;
};