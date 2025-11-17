#include "vector.h"
#include <assert.h>
#include <math.h>

Vector::Vector( float x, float y, float z) : X(x), Y(y), Z(z) {
    
}

Vector::Vector() {
	
}

float Vector::dot(const Vector& v) const {
    return this->X*v.X + this->Y*v.Y + this->Z*v.Z;
}

Vector Vector::cross(const Vector& v) const {
    float x = this->Y * v.Z - this->Z * v.Y;
    float y = this->Z * v.X - this->X * v.Z;
    float z = this->X * v.Y - this->Y * v.X;
    return Vector(x, y, z);
}

Vector Vector::operator+(const Vector& v) const {
    return Vector(this->X+v.X, this->Y+v.Y, this->Z+v.Z ); 
}

Vector Vector::operator-(const Vector& v) const {
    return Vector(this->X-v.X, this->Y-v.Y, this->Z-v.Z );
}

Vector Vector::operator*(float c) const {
	return Vector(c*this->X, c*this->Y, c*this->Z); 
}

Vector Vector::operator-() const {
	return Vector(this->X * (-1), this->Y * (-1), this->Z * (-1)); 
}

Vector& Vector::operator+=(const Vector& v) {
	this->X += v.X;
	this->Y += v.Y;
	this->Z += v.Z;
	return *this;
}

Vector& Vector::normalize() {
	float absoluteLength = length();
	if(absoluteLength == 0.0f) {return *this;}
	this->X /= absoluteLength;				// v0 ist 1/absoluteLength aber man kann es auch so abkuerzen
	this->Y /= absoluteLength;				// Man kommt zum selben Ergebnis
	this->Z /= absoluteLength;
	return *this;
}

float Vector::length() const {
	return sqrt(lengthSquared());
}

float Vector::lengthSquared() const {
	return this->X * this->X + this->Y * this->Y + this->Z * this->Z; 
}

 // Formel: https://www.onlinemathe.de/forum/Reflexion-eines-Vektors-an-einer-Normalen
Vector Vector::reflection( const Vector& normal) const {
	float dotNormal = 2 * dot(normal);
	return *this - (normal * dotNormal);
}

bool Vector::triangleIntersection( const Vector& d, const Vector& a, const Vector& b, const Vector& c, float& s) const {
	float epsilon = 1e-6f;
	// Vektor n --> Normalenvektor der Ebene
	Vector n = ((b - a).cross(c - a)).normalize(); //.normalize weil das genau das ist was im Bruch steht.

	float nDotd = n.dot(d);

	// Ist d-> parallel zur Ebene?
	// n ist ortthogonal zu d, wenn der Skalarprodukt 0 ist und wäre somit parallel zur Ebene.
	if (nDotd <= 0 + epsilon && nDotd >= 0 - epsilon) {
		return false;
	}

	// Distanz vom Ursprung zur Ebene (siehe Ebenengleichung)
	float distace = n.dot(a);

	// Schnittpunktfaktor s (siehe Strahlgleichung)
	s = (distace - n.dot(*this)) / n.dot(d);

	// zeigt d-> in die richtige Richtung?
	if (s <= 0 + epsilon) {
		return false;
	}

	Vector p = *this + d * s;

	float flaecheABC = (b - a).cross(c - a).length() / 2;
	float flaecheABP = (b - a).cross(p - a).length() / 2;
	float flaecheACP = (c - a).cross(p - a).length() / 2;
	float flaecheBCP = (c - b).cross(p - b).length() / 2;

	//Approximation ist wichtig, da float ungenau ist.
    return flaecheABC + epsilon >= flaecheABP + flaecheACP + flaecheBCP;
}
