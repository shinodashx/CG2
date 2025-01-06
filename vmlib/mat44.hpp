#ifndef MAT44_HPP_E7187A26_469E_48AD_A3D2_63150F05A4CA
#define MAT44_HPP_E7187A26_469E_48AD_A3D2_63150F05A4CA

#include <cmath>
#include <cassert>
#include <cstdlib>

#include "vec3.hpp"
#include "vec4.hpp"

/** Mat44f: 4x4 matrix with floats
 *
 * See vec2f.hpp for discussion. Similar to the implementation, the Mat44f is
 * intentionally kept simple and somewhat bare bones.
 *
 * The matrix is stored in row-major order (careful when passing it to OpenGL).
 *
 * The overloaded operator () allows access to individual elements. Example:
 *    Mat44f m = ...;
 *    float m12 = m(1,2);
 *    m(0,3) = 3.f;
 *
 * The matrix is arranged as:
 *
 *   ⎛ 0,0  0,1  0,2  0,3 ⎞
 *   ⎜ 1,0  1,1  1,2  1,3 ⎟
 *   ⎜ 2,0  2,1  2,2  2,3 ⎟
 *   ⎝ 3,0  3,1  3,2  3,3 ⎠
 */
struct Mat44f
{
	float v[16];

	constexpr
	float& operator() (std::size_t aI, std::size_t aJ) noexcept
	{
		assert( aI < 4 && aJ < 4 );
		return v[aI*4 + aJ];
	}
	constexpr
	float const& operator() (std::size_t aI, std::size_t aJ) const noexcept
	{
		assert( aI < 4 && aJ < 4 );
		return v[aI*4 + aJ];
	}
};

// Identity matrix
constexpr Mat44f kIdentity44f = { {
	1.f, 0.f, 0.f, 0.f,
	0.f, 1.f, 0.f, 0.f,
	0.f, 0.f, 1.f, 0.f,
	0.f, 0.f, 0.f, 1.f
} };

// Common operators for Mat44f.
// Note that you will need to implement these yourself.

constexpr Mat44f operator*(Mat44f const &aLeft, Mat44f const &aRight) noexcept
{
	Mat44f result = kIdentity44f;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			result(i, j) = 0;
			for (int k = 0; k < 4; k++)
			{
				result(i, j) += aLeft(i, k) * aRight(k, j);
			}
		}
	}
	return result;
}

constexpr Vec4f operator*(Mat44f const &aLeft, Vec4f const &aRight) noexcept
{
	Vec4f result = {0, 0, 0, 0};
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			result[i] += aLeft(i, j) * aRight[j];
		}
	}
	return result;
}

// Functions:

Mat44f invert( Mat44f const& aM ) noexcept;

inline
Mat44f transpose( Mat44f const& aM ) noexcept
{
	Mat44f ret;
	for( std::size_t i = 0; i < 4; ++i )
	{
		for( std::size_t j = 0; j < 4; ++j )
			ret(j,i) = aM(i,j);
	}
	return ret;
}

inline Mat44f make_rotation_x(float aAngle) noexcept
{
	float cosAngle = cos(aAngle);
	float sinAngle = sin(aAngle);
	Mat44f result = kIdentity44f;
	result(1, 1) = cosAngle;
	result(1, 2) = -sinAngle;
	result(2, 1) = sinAngle;
	result(2, 2) = cosAngle;
	return result;
}

inline Mat44f make_rotation_y(float aAngle) noexcept
{
	float cosAngle = cos(aAngle);
	float sinAngle = sin(aAngle);
	Mat44f result = kIdentity44f;
	result(0, 0) = cosAngle;
	result(0, 2) = sinAngle;
	result(2, 0) = -sinAngle;
	result(2, 2) = cosAngle;
	return result;
}

inline Mat44f make_rotation_z(float aAngle) noexcept
{
	float cosAngle = cos(aAngle);
	float sinAngle = sin(aAngle);
	Mat44f result = kIdentity44f;
	result(0, 0) = cosAngle;
	result(0, 1) = -sinAngle;
	result(1, 0) = sinAngle;
	result(1, 1) = cosAngle;
	return result;
}

inline Mat44f make_translation(Vec3f aTranslation) noexcept
{
	Vec3f translation = aTranslation;
	Mat44f result = kIdentity44f;
	result(0, 3) = translation[0];
	result(1, 3) = translation[1];
	result(2, 3) = translation[2];
	return result;
}

inline Mat44f make_perspective_projection(float aFovInRadians, float aAspect, float aNear, float aFar) noexcept
{
	float s = 1.f / tan(aFovInRadians / 2.f);
	Mat44f result = kIdentity44f;
	result(0, 0) = s / aAspect;
	result(1, 1) = s;
	result(2, 2) = -1.f * (aFar + aNear) / (aFar - aNear);
	result(2, 3) = -2.f * (2.f * aFar * aNear) / (aFar - aNear);
	result(3, 2) = -1;
	return result;
}

inline Mat44f make_scaling(float aX, float aY, float aZ) noexcept
{
	Mat44f result = kIdentity44f;
	result(0, 0) = aX;
	result(1, 1) = aY;
	result(2, 2) = aZ;
	return result;
}

#endif // MAT44_HPP_E7187A26_469E_48AD_A3D2_63150F05A4CA
