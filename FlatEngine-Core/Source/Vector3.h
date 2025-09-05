#pragma once
#include "glm.hpp"


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
		~Vector3();

		void _xyz(float newX, float newY, float newZ);
		float GetX();
		float GetY();
		float GetZ();
		void SetX(float newX);
		void SetY(float newY);
		void SetZ(float newZ);
		glm::vec3 GetGLMVec3();
		Vector3 Cross(Vector3 with);
		Vector3 operator=(Vector3 toCopy);
		Vector3 operator=(Vector2 toCopy);
		Vector3 operator*(Vector3 right);
		Vector3 operator*(float scalar);
		Vector3 operator/(Vector3 right);
		Vector3 operator/(float scalar);
		Vector3 operator+(Vector3 right);
		Vector3 operator-(Vector3 right);
		bool operator==(Vector3 right);
		bool operator!=(Vector3 right);

		float x;
		float y;
		float z;

	private:
	};
}

