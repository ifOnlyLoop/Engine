#pragma once

class vect;

// Quaternion for angular displacement (orientation) in 3D.
class quat{

public:
	// Public data
	// The four values of the quaternion. Normally, it will not
	// be necessary to manipulate these directly. However,
	// we leave them public, since prohibiting direct access
	// makes some operations, such as file I/O, unnecessarily
// complicated.

	float w, x, y, z;
	// Public operations
	quat();
	// Set to identity
	void identity() { w = 1.0f; x = y = z = 0.0f; }
	
	// Setup the quaternion to a specific rotation
	void setToRotateAboutX(float theta);
	void setToRotateAboutY(float theta);
	void setToRotateAboutZ(float theta);
	
	void setToRotateAboutAxis(const vect& axis, float theta);
	
	// Setup to perform object<->inertial rotations,
	// given orientation in Euler angle format
	void setToRotateObjectToInertial(float heading, float pitch, float bank);
	void setToRotateInertialToObject(float heading, float pitch, float bank);
	
	// Cross product
	quat operator *(const quat& a) const;
	void operator *(vect& a) const;
	// Multiplication with assignment, as per C++ convention
	quat& operator *=(const quat& a);
	// Normalize the quaternion.
	void normalize();
	// Extract and return the rotation angle and axis.
	float getRotationAngle() const;
	vect getRotationAxis() const;
};

// A global "identity" quaternion constant
extern const quat kquatIdentity;
// quat dot product.
extern float dotProduct(const quat& a, const quat& b);
// quat conjugation
extern quat conjugate(const quat& q);
// quat exponentiation
extern quat pow(const quat& q, float exponent);
