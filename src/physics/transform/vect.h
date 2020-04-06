#pragma once
//#ifndef VEC3_H
//#define VEC3_H

// class vect a simple 3D vector class
class vect { // float: Data Type 

public:
	// Public representation: Not many options here.
	float x, y, z;
	// ACCESSORS		

	
	const float& operator [] (int i) const 
	{ return (&x)[i]; }
	float& operator [] (int i) 
	{ return (&x)[i]; }
	/* Constructors */

	// Default constructor
	vect();
	// Copy constructor
	vect(const vect& a);
	// Construct given three values
	vect(float nx, float ny, float nz);

	/* Boolian Operations */ 
	bool operator ==(const vect& a) const;
	bool operator !=(const vect& a) const;

	// Set the vector to zero
	void zero();

	// Unary minus returns the negative of the vector
	vect operator -() const;
	// Binary + and - add and subtract vectors
	vect operator +(const vect& a) const;
	vect operator -(const vect& a) const;
	// Multiplication and division by scalar
	vect operator *(float a) const;
	vect operator /(float a) const;

	/* Assignment operations */ 
	vect& operator = (const vect& a);
	vect& operator +=(const vect& a);
	vect& operator -=(const vect& a);
	vect& operator *=(float a);
	vect& operator /=(float a);

	// normalize the vector 
	void normalize();
	// rotate around it
	void rotate(vect& v){}
	// vector dot product 
	float operator *(const vect& a) const;
};
 
/* * * * * * * * * * * * 
 * Nonmember functions *
 * * * * * * * * * * * */

// Compute the magnitude of a vector vectorMag
extern float norm(const vect& a);
// Compute the cross product of two vectors
extern vect cross(const vect& a, const vect& b);
// Scalar on the left multiplication, for symmetry
extern vect operator *(float k, const vect& v);
// Compute the distance between two points
extern float dist(const vect& a, const vect& b);

/*
 *Global variables
 */

 // We provide a global zero vector constant
extern const vect kZeroVector;

//#endif // VEC3_H