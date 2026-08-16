#pragma once

#include <ext/vector_float3.hpp>
#include <ext/vector_float4.hpp>


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
		Vector3(const glm::vec4 vec4);
		Vector3(Vector2 vec2, float z);
		
		static Vector3 Normalize(Vector3 vec);
		static Vector3 DirectionToRotation(Vector3 n);

		glm::vec3 GetGLMVec3();
		glm::vec4 GetGLMVec4();
		Vector3 Cross(Vector3 with);
		float GetMagnitude();
		void Normalize();
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