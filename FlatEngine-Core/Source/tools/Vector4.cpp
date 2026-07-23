#include "tools/Vector4.h"


namespace FlatEngine
{
	Vector4::Vector4()
	{
		x = 0;
		y = 0;
		z = 0;
		w = 0;
	}

	Vector4::Vector4(float xyzwValue)
	{
		x = xyzwValue;
		y = xyzwValue;
		z = xyzwValue;
		w = xyzwValue;
	}

	Vector4::Vector4(float xValue, float yValue, float zValue, float wValue)
	{
		x = xValue;
		y = yValue;
		z = zValue;
		w = wValue;
	}

	Vector4::Vector4(const ImVec4& toCopy)
	{
		x = toCopy.x;
		y = toCopy.y;
		z = toCopy.z;
		w = toCopy.w;
	}

	Vector4::~Vector4()
	{
	}

	void Vector4::_xyzw(float newX, float newY, float newZ, float newW)
	{
		x = newX;
		y = newY;
		z = newZ;
		w = newW;
	}

	float Vector4::GetX()
	{
		return x;
	}

	float Vector4::GetY()
	{
		return y;
	}

	float Vector4::GetZ()
	{
		return z;
	}

	float Vector4::GetW()
	{
		return w;
	}

	void Vector4::SetX(float newX)
	{
		x = newX;
	}

	void Vector4::SetY(float newY)
	{
		y = newY;
	}

	void Vector4::SetZ(float newZ)
	{
		z = newZ;
	}

	void Vector4::SetW(float newW)
	{
		w = newW;
	}

	Vector4 Vector4::operator=(ImVec4& toCopy)
	{
		x *= toCopy.x;
		y *= toCopy.y;
		z *= toCopy.z;
		w *= toCopy.w;

		return *this;
	}

	Vector4 Vector4::operator*(ImVec4& right)
	{
		Vector4 result = Vector4(x, y, z, w);

		result.x *= right.x;
		result.y *= right.y;
		result.z *= right.z;
		result.w *= right.w;

		return result;
	}

	Vector4 Vector4::operator*(Vector4& right)
	{
		Vector4 result = Vector4(x, y, z, w);

		result.x *= right.x;
		result.y *= right.y;
		result.z *= right.z;
		result.w *= right.w;

		return result;
	}

	bool Vector4::operator==(const Vector4& right)
	{
		return (x == right.x && y == right.y && z == right.z && w == right.w);
	}

	bool Vector4::operator!=(const Vector4& right)
	{
		return !(x == right.x && y == right.y && z == right.z && w == right.w);
	}
}