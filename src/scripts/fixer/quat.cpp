#include "quat.h"
#include <assert.h>
#include <math.h>
#include "quat.h"
#include "./src/transforms/MathUtil.h"
#include "./src/transforms/vect.h"

// The global identity quaternion
/*const quat kquatIdentity = 
{
	1.0f, 0.0f, 0.0f, 0.0f
};*/

quat::quat(): x(0.0f),y(0.0f),z(0.0f),w(1.0f){}

// Setup the quaternion to rotate about the specified axis

void quat::setToRotateAboutX(float theta) {
	// Compute the half angle
	float thetaOver2 = theta * .5f;
	// Set the values
	w = cos(thetaOver2);
	x = sin(thetaOver2);
	y = 0.0f;
	z = 0.0f;
}

void quat::setToRotateAboutY(float theta) {
	// Compute the half angle
	float thetaOver2 = theta * .5f;
	// Set the values
	w = cos(thetaOver2);
	x = 0.0f;
	y = sin(thetaOver2);
	z = 0.0f;
}
void quat::setToRotateAboutZ(float theta) {
	// Compute the half angle
	float thetaOver2 = theta * .5f;
	// Set the values
	w = cos(thetaOver2);
	x = 0.0f;
	y = 0.0f;
	z = sin(thetaOver2);
}
void quat::setToRotateAboutAxis(const vect& axis, float theta) {
	// The axis of rotation must be normalized
	assert(fabs(norm(axis) - 1.0f) < .01f);
	// Compute the half angle and its sin
	float thetaOver2 = theta * .5f;
	float sinThetaOver2 = sin(thetaOver2);
	// Set the values
	w = cos(thetaOver2);
	x = axis.x * sinThetaOver2;
	y = axis.y * sinThetaOver2;
	z = axis.z * sinThetaOver2;
}


//---------------------------------------------------------------------------
// EulerAngles::setToRotateObjectToInertial
//
// Setup the quaternion to perform an object->inertial rotation, given the
// orientation in Euler angle format
//
// See 10.6.5 for more information.
void quat::setToRotateObjectToInertial(float heading, float pitch, float bank) {
	// Compute sine and cosine of the half angles
	float sp, sb, sh;
	float cp, cb, ch;
	sinCos(sp, cp, pitch * 0.5f);
	sinCos(sb, cb, bank * 0.5f);
	sinCos(sh, ch, heading * 0.5f);
	// Compute values
	w = ch * cp * cb + sh * sp * sb;
	x = ch * sp * cb + sh * cp * sb;
	y = -ch * sp * sb + sh * cp * cb;
	z = -sh * sp * cb + ch * cp * sb;
}

//---------------------------------------------------------------------------
// EulerAngles::setToRotateInertialToObject
//
// Setup the quaternion to perform an object->inertial rotation, given the
// orientation in Euler angle format
//
// See 10.6.5 for more information.
void quat::setToRotateInertialToObject(float heading, float pitch, float bank) {
	// Compute sine and cosine of the half angles
	float sp, sb, sh;
	float cp, cb, ch;
	sinCos(sp, cp, pitch * 0.5f);
	sinCos(sb, cb, bank * 0.5f);
	sinCos(sh, ch, heading * 0.5f);
	// Compute values
	w = ch * cp * cb + sh * sp * sb;
	x = -ch * sp * cb - sh * cp * sb;
	y = ch * sp * sb - sh * cb * cp;
	z = sh * sp * cb - ch * cp * sb;
}
//---------------------------------------------------------------------------
// quat::operator *
//
// quat cross product, which concatenates multiple angular
// displacements. The order of multiplication, from left to right,
// corresponds to the order that the angular displacements are
// applied. This is backward from the *standard* definition of
// quaternion multiplication. See section 10.4.8 for the rationale
// behind this deviation from the standard.
quat quat::operator *(const quat& a) const {
	quat result;
	result.w = w * a.w - x * a.x - y * a.y - z * a.z;
	result.x = w * a.x + x * a.w + z * a.y - y * a.z;
	result.y = w * a.y + y * a.w + x * a.z - z * a.x;
	result.z = w * a.z + z * a.w + y * a.x - x * a.y;
	return result;
}
void quat::operator *(vect& a) const {
	//quat result;
	a.w = w * a.w - x * a.x - y * a.y - z * a.z;
	a.x = w * a.x + x * a.w + z * a.y - y * a.z;
	a.y = w * a.y + y * a.w + x * a.z - z * a.x;
	a.z = w * a.z + z * a.w + y * a.x - x * a.y;
	//return result;
}

// quat::operator *=
quat& quat::operator *=(const quat& a) {
	// Multuiply and assign
	*this = *this * a;
	// Return reference to l-value
	return *this;
}


// quat::normalize

// "Normalize" a quaternion. Note that normally, quaternions are always
//  normalized (within limits of numerical precision). "error creep"

void quat::normalize() {
	// Compute magnitude of the quaternion
	float mag = (float)sqrt(w * w + x * x + y * y + z * z);
	// Check for bogus length to protect against divide by zero
	if (mag > 0.0f) {
		// Normalize it
		float oneOverMag = 1.0f / mag;
		w *= oneOverMag;
		x *= oneOverMag;
		y *= oneOverMag;
		z *= oneOverMag;
	}
	else {
		// Houston, we have a problem
		assert(false);
		// In a release build, just slam it to something
		identity();
	}
}


// quat::getRotationAngle

float quat::getRotationAngle() const {
	// Compute the half angle. Remember that w = cos(theta / 2)
	float thetaOver2 = safeAcos(w);
	// Return the rotation angle
	return thetaOver2 * 2.0f;
}


// quat::getRotationAxis

vect quat::getRotationAxis() const {
	// Compute sin^2(theta/2). Remember that w = cos(theta/2),
	// and sin^2(x) + cos^2(x) = 1
	float sinThetaOver2Sq = 1.0f - w * w;
	// Protect against numerical imprecision
	if (sinThetaOver2Sq <= 0.0f) {
		// Identity quaternion, or numerical imprecision. Just
		// return any valid vector, since it doesn't matter
		return vect(1.0f, 0.0f, 0.0f);
	}
	// Compute 1 / sin(theta/2)
	float oneOverSinThetaOver2 = 1.0f / sqrt(sinThetaOver2Sq);
	// Return axis of rotation
	return vect(
		x * oneOverSinThetaOver2,
		y * oneOverSinThetaOver2,
		z * oneOverSinThetaOver2
		);
}



// Nonmember functions




// dotProduct

float dotProduct(const quat& a, const quat& b) {
	return a.w * b.w + a.x * b.x + a.y * b.y + a.z * b.z;
}


// conjugate

quat conjugate(const quat& q) {
	quat result;
	// Same rotation amount
	result.w = q.w;
	// Opposite axis of rotation
	result.x = -q.x;
	result.y = -q.y;
	result.z = -q.z;
	// Return it
	return result;
}


// quat exponentiation.

quat pow(const quat& q, float exponent) {
	// Check for the case of an identity quaternion.
	// This will protect against divide by zero
	if (fabs(q.w) > .9999f) {
		return q;
	}
	// Extract the half angle alpha (alpha = theta/2)
	float alpha = acos(q.w);
	// Compute new alpha value
	float newAlpha = alpha * exponent;
	// Compute new w value

	quat result;
	result.w = cos(newAlpha);
	// Compute new xyz values
	float mult = sin(newAlpha) / sin(alpha);
	result.x = q.x * mult;
	result.y = q.y * mult;
	result.z = q.z * mult;
	// Return it
	return result;
}