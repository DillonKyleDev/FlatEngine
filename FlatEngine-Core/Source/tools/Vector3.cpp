#include "tools/Vector2.h"
#include "tools/Vector3.h"


namespace FlatEngine
{
	Vector3::Vector3()
	{
		x = 0;
		y = 0;
		z = 0;
	}

	Vector3::Vector3(float xyzValue)
	{
		x = xyzValue;
		y = xyzValue;
		z = xyzValue;
	}

	Vector3::Vector3(float xValue, float yValue, float zValue)
	{
		x = xValue;
		y = yValue;
		z = zValue;
	}

	Vector3::Vector3(const Vector3& toCopy)
	{
		x = toCopy.x;
		y = toCopy.y;
		z = toCopy.z;
	}

	Vector3::~Vector3()
	{
	}

	Vector3 Vector3::Normalize(Vector3 vec)
	{
		Vector3 temp = Vector3(0);

		if (vec.x == 0 && vec.y == 0 && vec.z == 0)
		{
			return Vector3(0);
		}

		float magnitude = vec.GetMagnitude();

		if (magnitude != 0)
		{
			temp = vec * (1 / magnitude);
		}

		return temp;
	}

	void Vector3::_xyz(float newX, float newY, float newZ)
	{
		x = newX;
		y = newY;
		z = newZ;
	}

	float Vector3::GetX()
	{
		return x;
	}

	float Vector3::GetY()
	{
		return y;
	}

	float Vector3::GetZ()
	{
		return z;
	}

	void Vector3::SetX(float newX)
	{
		x = newX;
	}

	void Vector3::SetY(float newY)
	{
		y = newY;
	}

	void Vector3::SetZ(float newZ)
	{
		z = newZ;
	}

	glm::vec3 Vector3::GetGLMVec3()
	{
		return glm::vec3(x, y, z);
	}

	Vector3 Vector3::Cross(Vector3 with)
	{
		// i   j   k
		// X1  Y1  Z1
		// X2  Y2  Z2
		float newX = (y * with.z) - (with.y * z);
		float newY = -((x * with.z) - (with.x * z));
		float newZ = (x * with.y) - (with.x * y);

		return Vector3(newX, newY, newZ);
	}

	float Vector3::GetMagnitude()
	{
		return std::sqrt((x * x) + (y * y) + (z * z));
	}

	Vector3 Vector3::operator=(Vector3 toCopy)
	{
		x = toCopy.x;
		y = toCopy.y;
		z = toCopy.z;

		return *this;
	}

	Vector3 Vector3::operator=(Vector2 toCopy)
	{
		return Vector3(toCopy.x, toCopy.y, 0);
	}

	Vector3 Vector3::operator*(Vector3 right)
	{
		x *= right.x;
		y *= right.y;
		z *= right.z;

		return *this;
	}

	Vector3 Vector3::operator*(float scalar)
	{
		x *= scalar;
		y *= scalar;
		z *= scalar;
		
		return *this;
	}

	Vector3 Vector3::operator/(Vector3 right)
	{
		Vector3 result = Vector3(x, y, z);

		result.x /= right.x;
		result.y /= right.y;
		result.z /= right.z;

		return result;
	}

	Vector3 Vector3::operator/(float scalar)
	{
		Vector3 result = Vector3(x, y, z);

		result.x /= scalar;
		result.y /= scalar;
		result.z /= scalar;

		return result;
	}

	Vector3 Vector3::operator+(Vector3 right)
	{
		Vector3 result = Vector3(x, y, z);

		result.x += right.x;
		result.y += right.y;
		result.z += right.z;

		return result;
	}

	Vector3 Vector3::operator-(Vector3 right)
	{
		Vector3 result = Vector3(x, y, z);

		result.x -= right.x;
		result.y -= right.y;
		result.z -= right.z;

		return result;
	}

	bool Vector3::operator==(Vector3 right)
	{
		return (x == right.x && y == right.y && z == right.z);
	}

	bool Vector3::operator!=(Vector3 right)
	{
		return !(x == right.x && y == right.y && z == right.z);
	}
}