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
				
		void SetPosition(Vector2 position);
		Vector2 GetPosition();
		Vector2 GetAbsolutePosition();
		Vector2 GetPositionOrigin();
		void SetScale(Vector2 scale);
		Vector2 GetScale();
		Vector2 GetAbsoluteScale();
		void SetRotation(float rotation);
		float GetRotation();
		float GetAbsoluteRotation();
		void LookAt(Vector2 lookAt);
		void Move(Vector2 moveBy);

	private:
		Vector2 m_position;
		Vector2 m_scale;		
		float m_rotation;
	};
}
