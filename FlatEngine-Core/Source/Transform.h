#pragma once
#include "Component.h"
#include "Vector2.h"

#include <string>
#include "json.hpp"

using json = nlohmann::json;
using namespace nlohmann::literals;


namespace FlatEngine
{
	class Transform : public Component
	{
	public:
		Transform(long myID = -1, long parentID = -1);
		~Transform();
		std::string GetData();

		static float ClampRotation(float rotation, float min = -180.0f, float max = 180.0f);

		void SetInitialPosition(Vector2 initialPos);
		void SetOrigin(Vector2 newOrigin);
		Vector2 GetOrigin();
		void UpdateOrigin(Vector2 newOrigin);
		void UpdateChildOrigins(Vector2 newOrigin);
		Vector2 GetTruePosition();
		void SetPosition(Vector2 position);
		void SetBaseScale(Vector2 scale);
		void UpdateChildBaseScale(Vector2 scale);
		Vector2 GetBaseScale();
		Vector2 GetTotalScale();
		void SetScale(Vector2 scale);
		void SetRotation(float rotation);
		Vector2 GetPosition();
		Vector2 GetScale();
		float GetRotation();
		float GetTrueRotation();
		void LookAt(Vector2 lookAt);
		void Move(Vector2 moveBy);

	private:
		Vector2 m_positionOrigin;
		Vector2 m_position;
		float m_rotationOrigin;
		Vector2 m_baseScale;
		Vector2 m_scale;		
		float m_rotation;
	};
}
