#pragma once
#include "Component.h"
#include "Vector3.h"

#include "glm.hpp"
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
				
		void SetPosition(Vector3 position);
		Vector3 GetPosition();
		Vector3 GetAbsolutePosition();
		Vector3 GetPositionOrigin();
		void SetScale(Vector3 scale);
		Vector3 GetScale();
		Vector3 GetAbsoluteScale();
		void SetXRotation(float rotation);
		void SetYRotation(float rotation);
		void SetZRotation(float rotation);
		float GetRotation();
		Vector3 GetRotations();
		glm::mat4 GetRotationMatrix();
		float GetAbsoluteRotation();
		glm::mat4 GetScaleMatrix();
		void LookAt(Vector3 lookAt);
		void Move(Vector3 moveBy);

	private:
		Vector3 m_position;
		Vector3 m_scale;		
		Vector3 m_rotation;
		//float m_rotation;
	};
}
