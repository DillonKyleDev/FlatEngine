#pragma once
#include "components/Component.h"
#include "tools/Vector4.h"
#include "tools/Vector3.h"

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
		bool ForceZUp();
		void SetForceZUp(bool b_forceZUp);
		void SetDimensions(float width, float height);
		void SetZoom(float zoom);
		float GetZoom();
		glm::vec4 GetLookDirection();
		glm::vec4 GetLookDirectionNoRoll();
		float GetNearClippingDistance();
		void SetNearClippingDistance(float nearDistance);
		float GetFarClippingDistance();
		void SetFarClippingDistance(float farDistance);
		float GetPerspectiveAngle();
		void SetPerspectiveAngle(float angle);
		void SetHorizontalViewAngle(float angle);
		void SetVerticalViewAngle(float angle);
		float GetHorizontalViewAngle();
		float GetVerticalViewAngle();
		void AddToHorizontalViewAngle(float toAdd);
		void AddToVerticalViewAngle(float toAdd);
		float GetWidth();
		float GetHeight();
		void SetFrustrumColor(Vector4 color);
		Vector4 GetFrustrumColor();
		void Follow();
		void SetShouldFollow(bool b_shouldFollow);
		bool GetShouldFollow();
		void SetToFollowID(long toFollow);
		long GetToFollowID();
		void SetFollowSmoothing(float smoothing);
		float GetFollowSmoothing();
		void AddVelocity(Vector3 velocity);
		Vector3& GetVelocity();
		void Update();

		float m_nearClippingDistance;
		float m_farClippingDistance;
		float m_perspectiveAngle;
		float m_horizontalViewAngle;
		float m_verticalViewAngle;
		bool b_orthographic;
		float m_orthoSize;
		float m_orthoHorizontalViewAngle;
		float m_orthoVerticalViewAngle;

	private:
		bool m_b_forceZup;
		float m_width;
		float m_height;
		float m_zoom;
		Vector3 m_lookDirection;
		Vector4 m_frustrumColor;
		bool m_b_isPrimaryCamera;
		bool m_b_shouldFollow;
		long m_toFollowID;
		float m_followSmoothing;
		Vector3 m_velocity;

	};
}