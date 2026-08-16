#pragma once

#include <ext/vector_float4.hpp>
#include <imgui.h>


namespace FlatEngine
{
	class Vector4 : public ImVec4
	{
	public:
		Vector4();
		Vector4(float xyzwValue);
		Vector4(float x, float y, float z, float w);
		// Vector4(Vector4&& toMove) noexcept = default;
		Vector4(const ImVec4& toCopy);
		Vector4(const glm::vec4 vec4);

		Vector4 operator=(ImVec4& toCopy);
		Vector4 operator*(ImVec4& right);
		Vector4 operator*(Vector4& right);
		Vector4 operator*(float& right);
		Vector4& operator*=(float right);
		Vector4 operator+(Vector4& right);
		Vector4& operator+=(float right);
		bool operator==(const Vector4& right);
		bool operator!=(const Vector4& right);
	};
}

