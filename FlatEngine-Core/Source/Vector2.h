#pragma once
#include <imgui.h>
#include <box2d.h>


namespace FlatEngine
{
	class Vector2 : public ImVec2
	{
	public:
		Vector2();
		Vector2(float xyValue);
		Vector2(float x, float y);
		Vector2(const ImVec2& toCopy);
		Vector2(const Vector2& toCopy);
		Vector2(b2Vec2 b2Vec2);
		~Vector2();

		static float GetAngleBetween(Vector2 vec1, Vector2 vec2);
		static Vector2 Normalize(Vector2 vec);
		static Vector2 Rotate(Vector2 vec, float degrees);
		static b2Vec2 GetB2Vev2(Vector2 vec);

		float GetX();
		float GetY();
		void SetX(float newX);
		void SetY(float newY);
		void _xy(float newX, float newY);
		Vector2 operator=(ImVec2& toCopy);
		Vector2 operator*(ImVec2& right);
		Vector2 operator*(Vector2& right);
		Vector2 operator*(float scalar);
		Vector2 operator+(const Vector2& right);
		Vector2 operator-(const Vector2& right);
		Vector2 NormalizeCardinal();
		void Normalize();
		bool operator==(const Vector2& right);
		bool operator!=(const Vector2& right);
		float Dot(Vector2 with);
		float AmountProjectedOnto(Vector2 projectedTo);
		Vector2 ProjectedOnto(Vector2 projectedTo);
		float CrossKResult(Vector2 with);
		float GetMagnitude();
		void Rotate(float degrees);	
		void NormalizeSelf();
	};
}
