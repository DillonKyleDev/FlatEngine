#include "Transform.h"
#include "FlatEngine.h"
#include "GameObject.h"
#include "Button.h"
#include "Body.h"


namespace FlatEngine
{
	Transform::Transform(long myID, long parentID)
	{
		SetType(T_Transform);
		SetID(myID);
		SetParentID(parentID);
		m_position = Vector2(0, 0);
		m_scale = Vector2(1, 1);		
		m_rotation = 0;
	}

	Transform::~Transform()
	{
	}

	std::string Transform::GetData()
	{
		json jsonData = {
			{ "type", "Transform" },
			{ "id", GetID() },
			{ "_isCollapsed", IsCollapsed() },
			{ "_isActive", IsActive() },
			{ "xPos", m_position.x },
			{ "yPos", m_position.y },
			{ "rotation", m_rotation },
			{ "xScale", m_scale.x },
			{ "yScale", m_scale.y }
		};

		std::string data = jsonData.dump();
		// Return dumped json object with required data for saving
		return data;
	}

	float Transform::ClampRotation(float rotation, float min, float max)
	{
		if (rotation < min)
		{
			rotation = max - 0.01f;
		}
		else if (rotation > max)
		{
			rotation = min + 0.01f;
		}

		return rotation;
	}

	Vector2 Transform::GetAbsolutePosition()
	{
		Body* body = GetParent()->GetBody();

		if (body != nullptr)
		{
			m_position = body->GetPosition();			
		}

		Vector2 positionOrigin = Vector2();

		if (GetParent()->GetParent() != nullptr)
		{
			positionOrigin = GetParent()->GetParent()->GetTransform()->GetAbsolutePosition();
		}

		return positionOrigin + m_position;
	}

	Vector2 Transform::GetPositionOrigin()
	{
		Body* body = GetParent()->GetBody();

		Vector2 positionOrigin = Vector2();

		if (GetParent()->GetParent() != nullptr)
		{
			positionOrigin = GetParent()->GetParent()->GetTransform()->GetAbsolutePosition();
		}

		return positionOrigin;
	}

	void Transform::SetPosition(Vector2 newPosition)
	{
		m_position = newPosition;

		Body* body = GetParent()->GetBody();

		if (body != nullptr)
		{
			body->SetPosition(newPosition);
		}
	}

	Vector2 Transform::GetAbsoluteScale()
	{
		Vector2 scaleOrigin = 1;

		if (GetParent()->GetParent() != nullptr)
		{
			scaleOrigin = GetParent()->GetParent()->GetTransform()->GetAbsoluteScale();
		}

		return scaleOrigin * m_scale;
	}

	void Transform::SetScale(Vector2 newScale)
	{
		m_scale = newScale;		
	}

	void Transform::SetRotation(float newRotation)
	{
		m_rotation = ClampRotation(newRotation);
		
		if (GetParent()->GetBody() != nullptr)
		{
			GetParent()->GetBody()->SetRotation(m_rotation);
		}
	}

	Vector2 Transform::GetPosition()
	{
		Body* body = GetParent()->GetBody();

		if (body != nullptr)
		{
			m_position = body->GetPosition();			
		}

		return m_position;
	}

	Vector2 Transform::GetScale()
	{
		return m_scale;
	}

	float Transform::GetRotation()
	{
		Body* body = GetParent()->GetBody();

		if (body != nullptr)
		{
			m_rotation = body->GetRotation();
		}

		return m_rotation;
	}

	float Transform::GetAbsoluteRotation()
	{
		Body* body = GetParent()->GetBody();

		if (body != nullptr)
		{			
			m_rotation = body->GetRotation();
		}

		float parentTrueRotation = 0;

		if (GetParent()->GetParent() != nullptr)
		{
			parentTrueRotation = GetParent()->GetParent()->GetTransform()->GetAbsoluteRotation();
		}

		return m_rotation + parentTrueRotation;
	}

	void Transform::LookAt(Vector2 lookAt)
	{
		Vector2 slope = lookAt - GetAbsolutePosition();
		float angle = atan(slope.y / slope.x) * 180.0f / (float)M_PI;
		m_rotation = angle;
	}

	void Transform::Move(Vector2 moveBy)
	{
		SetPosition(GetPosition() + moveBy);
	}
}
