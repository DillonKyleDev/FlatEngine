#include "tools/Numbers.h"
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

	Vector3::Vector3(glm::vec3 vec3)
	{
		x = vec3.x;
		y = vec3.y;
		z = vec3.z;
	}

	Vector3::Vector3(const glm::vec4 vec4)
	{
		x = vec4.x;
		y = vec4.y;
		z = vec4.z;
	}

	Vector3::Vector3(Vector2 vec2, float newZ)
	{
		x = vec2.x;
		y = vec2.y;
		z = newZ;
	}

	Vector3 Vector3::Normalize(Vector3 vec)
	{
		vec.Normalize();
		return vec;
	}

	Vector3 Vector3::DirectionToRotation(Vector3 n)
	{
		n.Normalize();
		float theta = Numbers::ATan(n.x, n.z);
		float phi   = Numbers::ASin(Numbers::Clamp(-n.y, -1.0f, 1.0f));
		Vector3 eulerRotation = Vector3(phi, theta, 0);
		return eulerRotation;
	}

	glm::vec3 Vector3::GetGLMVec3()
	{
		return glm::vec3(x, y, z);
	}
	glm::vec4 Vector3::GetGLMVec4()
	{
		return glm::vec4(x, y, z, 0);
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

	void Vector3::Normalize()
	{
		if (x == 0 && y == 0 && z == 0)
		{
			return;
		}

		float magnitude = GetMagnitude();

		if (magnitude != 0)
		{
			x *= (1 / magnitude);
			y *= (1 / magnitude);
			z *= (1 / magnitude);
		}
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
		Vector3 result = Vector3(x, y, z);
		result.x *= scalar;
		result.y *= scalar;
		result.z *= scalar;
		
		return result;
	}

	Vector3& Vector3::operator*=(float scalar)
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

	Vector3& Vector3::operator+=(Vector3 right)
	{
		x += right.x;
		y += right.y;
		z += right.z;

		return *this;
	}

	Vector3 Vector3::operator-(Vector3 right)
	{
		Vector3 result = Vector3(x, y, z);

		result.x -= right.x;
		result.y -= right.y;
		result.z -= right.z;

		return result;
	}

	Vector3& Vector3::operator-=(Vector3 right)
	{
		x -= right.x;
		y -= right.y;
		z -= right.z;

		return *this;
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