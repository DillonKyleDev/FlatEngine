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
		m_position = Vector3(0, 0, 0);
		m_scale = Vector3(1, 1, 1);		
		m_rotation = Vector3(0, 0, 0);;
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
			{ "xPosition", m_position.x },
			{ "yPosition", m_position.y },
			{ "zPosition", m_position.z },
			{ "xRotation", m_rotation.x },
			{ "yRotation", m_rotation.y },
			{ "zRotation", m_rotation.z },
			{ "xScale", m_scale.x },
			{ "yScale", m_scale.y },
			{ "zScale", m_scale.z }
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

	Vector3 Transform::GetAbsolutePosition()
	{
		Body* body = GetParent()->GetBody();

		if (body != nullptr)
		{
			Vector2 bodyPos = body->GetPosition();
			m_position.x = bodyPos.x;
			m_position.y = bodyPos.y;
		}

		Vector3 positionOrigin = Vector3();

		if (GetParent()->GetParent() != nullptr)
		{
			positionOrigin = GetParent()->GetParent()->GetTransform()->GetAbsolutePosition();
		}

		return positionOrigin + m_position;
	}

	Vector3 Transform::GetPositionOrigin()
	{
		Body* body = GetParent()->GetBody();

		Vector3 positionOrigin = Vector3();

		if (GetParent()->GetParent() != nullptr)
		{
			positionOrigin = GetParent()->GetParent()->GetTransform()->GetAbsolutePosition();
		}

		return positionOrigin;
	}

	void Transform::SetPosition(Vector3 newPosition)
	{
		m_position = newPosition;

		Body* body = GetParent()->GetBody();

		if (body != nullptr)
		{
			Vector2 newPos = Vector2(newPosition.x, newPosition.y);
			body->SetPosition(newPos);
		}
	}

	Vector3 Transform::GetPosition()
	{
		Body* body = GetParent()->GetBody();

		if (body != nullptr)
		{
			Vector2 bodyPos = body->GetPosition();
			m_position.x = bodyPos.x;
			m_position.y = bodyPos.y;
		}

		return m_position;
	}

	Vector3 Transform::GetAbsoluteScale()
	{
		Vector3 scaleOrigin = 1;

		if (GetParent()->GetParent() != nullptr)
		{
			scaleOrigin = GetParent()->GetParent()->GetTransform()->GetAbsoluteScale();
		}

		return scaleOrigin * m_scale;
	}

	void Transform::SetScale(Vector3 newScale)
	{
		m_scale = newScale;		
	}

	void Transform::SetXRotation(float newRotation)
	{
		m_rotation.x = ClampRotation(newRotation);
	}

	void Transform::SetYRotation(float newRotation)
	{
		m_rotation.y = ClampRotation(newRotation);
	}

	void Transform::SetZRotation(float newRotation)
	{
		m_rotation.z = ClampRotation(newRotation);

		if (GetParent()->GetBody() != nullptr)
		{
			GetParent()->GetBody()->SetRotation(m_rotation.z);
		}
	}

	void Transform::SetRotation(Vector3 rotation)
	{
		m_rotation = rotation;
	}

	Vector3 Transform::GetScale()
	{
		return m_scale;
	}

	float Transform::GetRotation()
	{
		Body* body = GetParent()->GetBody();

		if (body != nullptr)
		{
			m_rotation.z = body->GetRotation();
		}

		return m_rotation.z;
	}

	Vector3 Transform::GetRotations()
	{
		Body* body = GetParent()->GetBody();

		if (body != nullptr)
		{
			m_rotation.z = body->GetRotation();
		}

		return m_rotation;
	}

	glm::mat4 Transform::GetRotationMatrix()
	{
		glm::mat4 xRotation = glm::rotate(glm::mat4(1.0f), glm::radians(m_rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
		glm::mat4 yRotation = glm::rotate(glm::mat4(1.0f), glm::radians(m_rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 zRotation = glm::rotate(glm::mat4(1.0f), glm::radians(m_rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

		return (xRotation * yRotation * zRotation);
	}

	float Transform::GetAbsoluteRotation()
	{
		//Body* body = GetParent()->GetBody();

		//if (body != nullptr)
		//{			
		//	m_rotation = body->GetRotation();
		//}

		//float parentTrueRotation = 0;

		//if (GetParent()->GetParent() != nullptr)
		//{
		//	parentTrueRotation = GetParent()->GetParent()->GetTransform()->GetAbsoluteRotation();
		//}

		//return m_rotation + parentTrueRotation;
		return m_rotation.z;
	}

	glm::mat4 Transform::GetScaleMatrix()
	{
		return glm::scale(glm::mat4(1), glm::vec3(m_scale.x, m_scale.y, m_scale.z));
	}

	void Transform::LookAt(Vector3 lookAt)
	{
		//Vector3 slope = lookAt - GetAbsolutePosition();
		//float angle = atan(slope.y / slope.x) * 180.0f / (float)M_PI;
		//m_rotation = angle;
	}

	void Transform::Move(Vector3 moveBy)
	{
		SetPosition(GetPosition() + moveBy);
	}
}
