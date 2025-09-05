#pragma once
#include "Vector4.h"
#include "Vector3.h"
#include "Component.h"


namespace FlatEngine 
{
	class Camera : public Component
	{
	public:
		Camera(long myID = -1, long parentID = -1);		
		~Camera();
		std::string GetData();

		void SetPrimaryCamera(bool b_isPrimary);
		bool IsPrimary();
		void SetDimensions(float width, float height);
		void SetZoom(float zoom);
		float GetZoom();
		void SetLookDirection(Vector3 lookDir);
		Vector3 GetLookDirection();
		float GetNearClippingDistance();
		void SetNearClipping(float nearDistance);
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
		Vector3 GetVelocity();
		void Update();

	private:
		float m_width;
		float m_height;
		float m_zoom;
		Vector3 m_lookDirection;
		float m_nearClippingDistance;
		float m_farClippingDistance;
		float m_perspectiveAngle;
		Vector4 m_frustrumColor;
		bool m_b_isPrimaryCamera;
		bool m_b_shouldFollow;
		long m_toFollowID;
		float m_followSmoothing;
		float m_horizontalViewAngle;
		float m_verticalViewAngle;
		Vector3 m_velocity;
	};
}