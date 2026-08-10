#pragma once

#include <ext/vector_float3.hpp>


namespace FlatEngine
{
	class Vector2;

	class Vector3
	{
	public:
		Vector3();
		Vector3(float xyzwValue);
		Vector3(float x, float y, float z);
		Vector3(const Vector3& toCopy);
		Vector3(Vector3&& toMove) noexcept = default;
		Vector3(const glm::vec3 vec3);
		
		static Vector3 Normalize(Vector3 vec);
		
		glm::vec3 GetGLMVec3();
		Vector3 Cross(Vector3 with);
		float GetMagnitude();
		Vector3 operator=(Vector3 toCopy);
		Vector3 operator=(Vector2 toCopy);
		Vector3 operator*(Vector3 right);
		Vector3 operator*(float scalar);
		Vector3& operator*=(float scalar);
		Vector3 operator/(Vector3 right);
		Vector3 operator/(float scalar);
		Vector3 operator+(Vector3 right);
		Vector3& operator+=(Vector3 right);
		Vector3 operator-(Vector3 right);
		Vector3& operator-=(Vector3 right);
		bool operator==(Vector3 right);
		bool operator!=(Vector3 right);

		float x;
		float y;
		float z;
	};
}