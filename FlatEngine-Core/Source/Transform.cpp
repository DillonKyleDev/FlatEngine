#include "Transform.h"
#include "FlatEngine.h"
#include "GameObject.h"
#include "Button.h"
#include "BoxCollider.h"


namespace FlatEngine
{
	Transform::Transform(long myID, long parentID)
	{
		SetType(T_Transform);
		SetID(myID);
		SetParentID(parentID);
		m_origin = Vector2(0, 0);
		m_position = Vector2(0, 0);
		m_baseScale = Vector2(1, 1);
		m_scale = Vector2(1, 1);
		m_baseRotation = 0;
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
			{ "xOrigin", m_origin.x },
			{ "yOrigin", m_origin.y },
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

	void Transform::SetInitialPosition(Vector2 initialPos)
	{
		m_position = initialPos;
	}

	void Transform::SetOrigin(Vector2 newOrigin)
	{
		m_origin = Vector2(newOrigin.x * m_baseScale.x, newOrigin.y * m_baseScale.y);

		if (GetParent() != nullptr && GetParent()->HasComponent("Button"))
		{
			GetParent()->GetButton()->CalculateActiveEdges();
		}
	}

	Vector2 Transform::GetOrigin()
	{
		return m_origin;
	}

	Vector2 Transform::GetTruePosition()
	{
		return Vector2(m_origin.x + m_position.x, m_origin.y + m_position.y);
	}

	void Transform::SetPosition(Vector2 newPosition)
	{
		m_position = newPosition;

		if (GetParent() != nullptr && GetParent()->HasChildren())
		{
			for (long id : GetParent()->GetChildren())
			{
				GameObject *child = GetObjectByID(id);
				child->GetTransform()->UpdateOrigin(GetTruePosition());
			}
		}
	}


	void Transform::UpdateOrigin(Vector2 newOrigin)
	{
		m_origin = Vector2(newOrigin.x * m_baseScale.x, newOrigin.y * m_baseScale.y);
		UpdateChildOrigins(GetTruePosition());
	}

	void Transform::UpdateChildOrigins(Vector2 newOrigin)
	{
		if (GetParent()->HasChildren())
		{
			for (long ID : GetParent()->GetChildren())
			{
				GameObject *child = GetObjectByID(ID);
				Transform* childTransform = child->GetTransform();
				childTransform->SetOrigin(newOrigin);
				childTransform->UpdateChildOrigins(childTransform->GetTruePosition());
			}
		}
	}

	void Transform::UpdateChildBaseScale(Vector2 baseScale)
	{
		if (baseScale.x != 0 && baseScale.y != 0)
		{
			if (GetParent()->HasChildren())
			{
				for (long ID : GetParent()->GetChildren())
				{
					GameObject* child = GetObjectByID(ID);					
					Transform* childTransform = child->GetTransform();
					childTransform->SetBaseScale(baseScale);
					childTransform->UpdateChildBaseScale(childTransform->GetTotalScale());
				}
			}
		}
	}

	Vector2 Transform::GetBaseScale()
	{
		return m_baseScale;
	}

	void Transform::SetBaseScale(Vector2 scale)
	{
		if (scale.x != 0 && scale.y != 0)
		{
			m_baseScale = scale;
		}
	}

	Vector2 Transform::GetTotalScale()
	{
		return Vector2(m_baseScale.x * m_scale.x, m_baseScale.y * m_scale.y);
	}

	void Transform::SetScale(Vector2 newScale)
	{
		m_scale = newScale;
		//UpdateChildBaseScale(m_scale);
	}

	void Transform::SetRotation(float newRotation)
	{
		m_rotation = newRotation;

		// Just for debugging, not for release, since it would update at least once per frame otherwise
		if (GetParent()->GetBoxColliders().size())
		{
			for (BoxCollider* collider : GetParent()->GetBoxColliders())
			{
				collider->RecalculateBounds();
			}
		}
	}

	Vector2 Transform::GetPosition()
	{
		return m_position;
	}

	Vector2 Transform::GetScale()
	{
		return m_scale;
	}

	float Transform::GetRotation()
	{
		return m_rotation;
	}

	void Transform::LookAt(Vector2 lookAt)
	{
		Vector2 slope = Vector2(lookAt.x - GetTruePosition().x, lookAt.y - GetTruePosition().y);
		float angle = atan(slope.y / slope.x) * 180.0f / (float)M_PI;
		m_rotation = angle;
	}
}
