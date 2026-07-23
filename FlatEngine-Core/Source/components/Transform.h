#pragma once
#include "components/Component.h"
#include "tools/JsonHelper.h"
#include "tools/Vector3.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <gtx/hash.hpp>
#include <string>


namespace FlatEngine
{
	class GameObject;

	class Transform : public Component
	{
	public:
		Transform(long myID = -1, long parentID = -1);
		std::string GetData();
		void PutData(json componentJson);

		static float ClampRotation(float rotation, float min = -180.0f, float max = 180.0f);				

		void SetPosition(Vector3 position);
		Vector3& GetPosition();
		Vector3 GetAbsolutePosition();
		Vector3 GetPositionOrigin();
		void SetScale(Vector3 scale);
		Vector3& GetScale();
		Vector3 GetAbsoluteScale();
		void AddRotation(float x, float y, float z);
		void SetXRotation(float rotation);
		void SetYRotation(float rotation);
		void SetZRotation(float rotation);
		void SetRotation(Vector3 rotation);
		float GetRotation();
		Vector3& GetRotations();
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
