#pragma once
#include "components/Component.h"
#include "tools/Vector3.h"
#include <SDL_stdinc.h>

#include <cstdint>
#define GLM_ENABLE_EXPERIMENTAL
#include <gtx/hash.hpp>


namespace FlatEngine 
{
	class Camera : public Component
	{
	public:
		Camera(long ownerID = -1);		
		json GetData(bool b_IDOverride = false);
		void PutData(json componentJson, std::string objectName);

		void SetPrimaryCamera(bool b_isPrimary);
		bool IsPrimary();
		glm::vec4 GetLookDirection();
		glm::vec4 GetLookDirectionNoRoll();		
		void AddOrthoVerticalViewAngle(float toAdd);		
		void AddVerticalViewAngle(float toAdd);
		void Follow();
		void AddVelocity(Vector3 velocity);
		Vector3& GetVelocity();
		void Update();

		float orthoNearClippingDistance;
		float orthoFarClippingDistance;
		float nearClippingDistance;
		float farClippingDistance;
		float perspectiveAngle;
		float horizontalViewAngle;
		float verticalViewAngle;
		bool b_orthographic;		
		uint32_t gridStep;
		float orthoHorizontalViewAngle;
		float orthoVerticalViewAngle;
		bool b_shouldFollow;
		long toFollowID;
		float followSmoothing;

	private:
		Vector3 m_lookDirection;		
		bool m_b_isPrimaryCamera;
		Vector3 m_velocity;
	};
}