#pragma once
#include "components/Component.h"
#include "tools/Vector3.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <gtx/hash.hpp>


namespace FlatEngine
{
	class GameObject;

	class Transform : public Component
	{
	public:
		Transform(long ownerID = -1);
		json GetData(bool b_IDOverride = false);
		void PutData(json componentJson, std::string objectName);

		static float ClampRotation(float rotation, float min = -180.0f, float max = 180.0f);				

		void SetPosition(Vector3 position);
		void SetScale(Vector3 scale);
		void SetRotation(Vector3 rotation);
		void AddXRotation(float rotation);
		void AddYRotation(float rotation);
		void AddZRotation(float rotation);
		Vector3 GetPosition();
		Vector3 GetPositionOrigin();
		Vector3 GetCleanPosition();
		Vector3 GetCleanRotation();
		Vector3 GetAbsolutePosition();
		Vector3 GetScale();
		Vector3 GetAbsoluteScale();
		Vector3 GetRotation();		
		glm::mat4 GetRotationMatrix();		
		glm::mat4 GetScaleMatrix();
		glm::vec4 GetLookDirection();
		void LookAt(Vector3 lookAt);
		void Move(Vector3 moveBy);

	private:
		Vector3 m_position;
		Vector3 m_scale;		
		Vector3 m_rotation;
	};
}
