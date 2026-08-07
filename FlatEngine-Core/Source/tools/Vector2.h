#pragma once

#include <box2d.h>
#include <imgui.h>


namespace FlatEngine
{
	class Vector3;

	class Vector2 : public ImVec2
	{
	public:
		Vector2();
		Vector2(float xyValue);
		Vector2(float x, float y);
		Vector2(const ImVec2& toCopy);
		Vector2(const Vector2& toCopy);
		Vector2(b2Vec2 b2Vec2);

		static float GetAngleBetween(Vector2 vec1, Vector2 vec2);
		static Vector2 Normalize(Vector2 vec);
		static Vector2 Rotate(Vector2 vec, float degrees);
		static b2Vec2 GetB2Vec2(Vector2 vec);

		Vector2 operator=(ImVec2& toCopy);
		Vector2 operator=(Vector2 toCopy);
		Vector2 operator=(Vector3 toCopy);
		Vector2 operator*(ImVec2& right);
		Vector2 operator*(Vector2& right);
		Vector2 operator*(float scalar);
		Vector2& operator*=(float scalar);
		Vector2 operator+(Vector2 right);
		Vector2& operator+=(Vector2 right);
		Vector2 operator-(Vector2 right);
		Vector2& operator-=(Vector2 right);
		bool operator==(Vector2 right);
		bool operator!=(Vector2 right);
		void Normalize();
		float Dot(Vector2 with);
		float AmountProjectedOnto(Vector2 projectedTo);
		Vector2 ProjectedOnto(Vector2 projectedTo);
		float CrossKResult(Vector2 with);
		float GetMagnitude();
		void Rotate(float degrees);	
		void NormalizeSelf();
		void RotateSelf(float degrees);
	};
}
