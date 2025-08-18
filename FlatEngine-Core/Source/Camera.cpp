#include "Camera.h"
#include "FlatEngine.h"
#include "Transform.h"
#include "GameObject.h"


namespace FlatEngine 
{
	Camera::Camera(long myID, long parentID)
	{
		SetType(T_Camera);
		SetID(myID);
		SetParentID(parentID);
		m_b_isPrimaryCamera = false;
		m_width = 50;
		m_height = 30;
		m_zoom = 10;
		m_frustrumColor = Vector4(255,255,255,255);
		m_b_shouldFollow = false;
		m_toFollowID = -1;
		m_followSmoothing = 0.1f;
	}

	Camera::~Camera()
	{
	}

	std::string Camera::GetData()
	{
		json jsonData = {
			{ "type", "Camera" },
			{ "id", GetID() },
			{ "_isCollapsed", IsCollapsed() },
			{ "_isActive", IsActive() },
			{ "width", m_width },
			{ "height", m_height },
			{ "_isPrimaryCamera", m_b_isPrimaryCamera },
			{ "zoom", m_zoom },
			{ "frustrumRed", m_frustrumColor.x },
			{ "frustrumGreen", m_frustrumColor.y },
			{ "frustrumBlue", m_frustrumColor.z },
			{ "frustrumAlpha", m_frustrumColor.w },
			{ "_follow", m_b_shouldFollow },
			{ "followSmoothing", m_followSmoothing },
			{ "following", m_toFollowID },
		};

		std::string data = jsonData.dump();
		// Return dumped json object with required data for saving
		return data;
	}

	void Camera::SetPrimaryCamera(bool b_isPrimary)
	{
		m_b_isPrimaryCamera = b_isPrimary;
	}

	bool Camera::IsPrimary()
	{
		return m_b_isPrimaryCamera;
	}

	void Camera::SetFrustrumColor(Vector4 color)
	{
		m_frustrumColor = color;
	}

	Vector4 Camera::GetFrustrumColor()
	{
		return m_frustrumColor;
	}

	void Camera::Follow()
	{
		GameObject *followTarget = GetObjectByID(m_toFollowID);
		if (m_b_shouldFollow && followTarget != nullptr && followTarget->HasComponent("Transform"))
		{
			Transform* cameraTransform = GetParent()->GetTransform();
			Vector2 followPos = followTarget->GetTransform()->GetAbsolutePosition();
			Vector2 currentPos = cameraTransform->GetPosition(); // Shouldn't have a parent if following so don't need GetTruePosition()

			cameraTransform->SetPosition(Lerp(currentPos, followPos, m_followSmoothing));
		}
	}

	void Camera::SetShouldFollow(bool b_shouldFollow)
	{
		m_b_shouldFollow = b_shouldFollow;
	}

	bool Camera::GetShouldFollow()
	{
		return m_b_shouldFollow;
	}

	void Camera::SetToFollowID(long toFollow)
	{
		m_toFollowID = toFollow;
	}

	long Camera::GetToFollowID()
	{
		return m_toFollowID;
	}

	void Camera::SetFollowSmoothing(float smoothing)
	{
		m_followSmoothing = smoothing;
	}

	float Camera::GetFollowSmoothing()
	{
		return m_followSmoothing;
	}

	void Camera::SetZoom(float newZoom)
	{
		if (newZoom >= 1 && newZoom <= 100)
		{
			m_zoom = newZoom;
		}
	}

	float Camera::GetZoom()
	{
		return m_zoom;
	}

	void Camera::SetDimensions(float newWidth, float newHeight)
	{
		m_width = newWidth;
		m_height = newHeight;
	}

	float Camera::GetWidth()
	{
		return m_width;
	}

	float Camera::GetHeight()
	{
		return m_height;
	}
}