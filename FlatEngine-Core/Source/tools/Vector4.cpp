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

	Vector4::Vector4(const glm::vec4 vec4)
	{
		x = vec4.x;
		y = vec4.y;
		z = vec4.z;
		w = vec4.w;
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

	Vector4 Vector4::operator*(float& right)
	{
		Vector4 result = Vector4(x, y, z, w);

		result.x *= right;
		result.y *= right;
		result.z *= right;
		result.w *= right;

		return result;
	}

	Vector4& Vector4::operator*=(float right)
	{
		x *= right;
		y *= right;
		z *= right;
		w *= right;

		return *this;
	}

	Vector4 Vector4::operator+(Vector4& right)
	{
		Vector4 result = Vector4(x, y, z, w);

		result.x += right.x;
		result.y += right.y;
		result.z += right.z;
		result.w += right.w;

		return result;
	}

	Vector4& Vector4::operator+=(float right)
	{
		x += right;
		y += right;
		z += right;
		w += right;

		return *this;
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