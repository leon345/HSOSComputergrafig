#include "color.h"
#include <assert.h>

Color::Color() : R(0), G(0), B(0) {}

Color::Color( float r, float g, float b) : R(r), G(g), B(b) {}

Color Color::operator*(const Color& c) const {
	float r;
	float g;
	float b;

	r = this->R * c.R;
	g = this->G * c.G;
	b = this->B * c.B;
	
    return Color(r, g, b);
}

Color Color::operator*(const float Factor) const {
	float r;
	float g;
	float b;

	r = this->R * Factor;
	g = this->G * Factor;
	b = this->B * Factor;

	return Color(r, g, b);
}

Color Color::operator+(const Color& c) const {
	float r;
	float g;
	float b;

	r = this->R + c.R;
	g = this->G + c.G;
	b = this->B + c.B;

	return Color(r, g, b);
}

Color& Color::operator+=(const Color& c) {
	this->R += c.R;
	this->G += c.G;
	this->B += c.B;
	
	return *this;
}
