#include "components/Body2D.h"
#include "components/Transform.h"
#include "GameObject.h"

#include <gtc/matrix_transform.hpp>


namespace FlatEngine
{
	Transform::Transform(long myID, long parentObjectID)
	{
		SetType(ComponentType_Transform);
		SetID(myID);
		SetParentObjectID(parentObjectID);		
		m_position = Vector3();
		m_scale = Vector3(1);		
		m_rotation = Vector3();
	}

	json Transform::GetData(bool b_IDOverride)
	{
		json jsonData = {
			{ "type", (int)GetType() },
			{ "id", b_IDOverride ? -1 : GetID() },
			{ "b_isCollapsed", IsCollapsed() },
			{ "b_isActive", IsActive() },
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

		return jsonData;
	}

	void Transform::PutData(json componentJson, std::string objectName)
	{
        Component::PutData(componentJson, objectName);

		SetPosition(Vector3(JsonHelper::CheckJsonFloat(componentJson, "xPosition", objectName), JsonHelper::CheckJsonFloat(componentJson, "yPosition", objectName), JsonHelper::CheckJsonFloat(componentJson, "zPosition", objectName)));
		SetScale(Vector3(JsonHelper::CheckJsonFloat(componentJson, "xScale", objectName), JsonHelper::CheckJsonFloat(componentJson, "yScale", objectName), JsonHelper::CheckJsonFloat(componentJson, "zScale", objectName)));
		SetRotation(Vector3(JsonHelper::CheckJsonFloat(componentJson, "xRotation", objectName), JsonHelper::CheckJsonFloat(componentJson, "yRotation", objectName), JsonHelper::CheckJsonFloat(componentJson, "zRotation", objectName)));										
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
		if (GetParentObject() != nullptr)
		{
			GameObject* parent = GetParentObject();
			Body2D* body2D = parent->Get<Body2D>();

			if (body2D != nullptr)
			{
				Vector2 body2DPos = body2D->GetPosition();
				m_position.x = body2DPos.x;
				m_position.y = body2DPos.y;
			}

			Vector3 positionOrigin = Vector3();

			if (parent->GetParent() != nullptr)
			{
				positionOrigin = parent->GetParent()->Get<Transform>()->GetAbsolutePosition();
			}

			return positionOrigin + m_position;
		}

		return Vector3();
	}

	Vector3 Transform::GetPositionOrigin()
	{
		if (GetParentObject() != nullptr)
		{
			GameObject* parent = GetParentObject();			
			Vector3 positionOrigin = Vector3();

			if (parent->GetParent() != nullptr)
			{
				positionOrigin = parent->GetParent()->Get<Transform>()->GetAbsolutePosition();
			}

			return positionOrigin;
		}

		return Vector3();
	}

	void Transform::SetPosition(Vector3 newPosition)
	{
		m_position = newPosition;

		if (GetParentObject() != nullptr)
		{
			GameObject* parent = GetParentObject();
			Body2D* body2D = parent->Get<Body2D>();

			if (body2D != nullptr)
			{
				Vector2 newPos = Vector2(newPosition.x, newPosition.y);
				body2D->SetPosition(newPos);
			}
		}
	}

	Vector3 Transform::GetPosition()
	{
		Body2D* body2D = nullptr;
		if (GetParentObject() != nullptr)
		{
			body2D = GetParentObject()->Get<Body2D>();
		}

		if (body2D != nullptr)
		{
			Vector2 body2DPos = body2D->GetPosition();
			m_position.x = body2DPos.x;
			m_position.y = body2DPos.y;
		}

		return m_position;
	}

	Vector3 Transform::GetCleanPosition()
	{
		return m_position;
	}

	Vector3 Transform::GetAbsoluteScale()
	{
		Vector3 scaleOrigin = 1;

		if (GetParentObject() != nullptr && GetParentObject()->GetParent() != nullptr)
		{
			scaleOrigin = GetParentObject()->GetParent()->Get<Transform>()->GetAbsoluteScale();
		}

		return scaleOrigin * m_scale;
	}

	void Transform::AddRotation(float x, float y, float z)
	{
		m_rotation.x += x;
		m_rotation.y += y;
		m_rotation.z += z;
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

		if (GetParentObject() != nullptr && GetParentObject()->Get<Body2D>() != nullptr)
		{
			GetParentObject()->Get<Body2D>()->SetRotation(m_rotation.z);
		}
	}

	void Transform::SetRotation(Vector3 rotation)
	{
		m_rotation = rotation;

		if (GetParentObject() != nullptr && GetParentObject()->Get<Body2D>() != nullptr)
		{
			GetParentObject()->Get<Body2D>()->SetRotation(m_rotation.z);
		}
	}

	Vector3 Transform::GetScale()
	{
		return m_scale;
	}

	float Transform::GetRotation()
	{
		Body2D* body2D = GetParentObject()->Get<Body2D>();

		if (body2D != nullptr)
		{
			m_rotation.z = body2D->GetRotation();
		}

		return m_rotation.z;
	}

	Vector3 Transform::GetRotations()
	{
		if (GetParentObject() != nullptr)
		{
			Body2D* body2D = GetParentObject()->Get<Body2D>();

			if (body2D != nullptr)
			{
				m_rotation.z = body2D->GetRotation();
			}
		}

		return m_rotation;
	}

	Vector3 Transform::GetCleanRotations()
	{
		return m_rotation;
	}

	glm::mat4 Transform::GetRotationMatrix()
	{
		if (GetParentObject() != nullptr)
		{
			Body2D* body2D = GetParentObject()->Get<Body2D>();

			if (body2D != nullptr)
			{
				m_rotation.z = body2D->GetRotation();
			}
		}
		
		glm::mat4 xRotation = glm::rotate(glm::mat4(1.0f), glm::radians(m_rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
		glm::mat4 yRotation = glm::rotate(glm::mat4(1.0f), glm::radians(-m_rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 zRotation = glm::rotate(glm::mat4(1.0f), glm::radians(m_rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

		return (xRotation * yRotation * zRotation);
	}

	float Transform::GetAbsoluteRotation()
	{
		//Body* body = GetParent()->Get<Body>();

		//if (body != nullptr)
		//{			
		//	m_rotation = body->GetRotation();
		//}

		//float parentTrueRotation = 0;

		//if (GetParent()->GetParent() != nullptr)
		//{
		//	parentTrueRotation = GetParent()->GetParent()->Get<Transform>()->GetAbsoluteRotation();
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
		Vector2 xyDirection = Vector2::Normalize(Vector2(lookAt.x, lookAt.y) - Vector2(m_position.x, m_position.y));
		Vector2 xzDirection = Vector2::Normalize(Vector2(lookAt.x, lookAt.z) - Vector2(m_position.x, m_position.z));

		// TODO
		//m_rotation.x = 0;
		//m_rotation.y = ClampRotation(glm::degrees(glm::atan(xzDirection.y / xzDirection.x))); // arctan(z / x) components of the xz direction
		//if (xzDirection.x < 0)
		//{
		//	m_rotation.y += 180;
		//}
		//m_rotation.z = ClampRotation(glm::degrees(glm::atan(xyDirection.y / xyDirection.x)));
		//if (xyDirection.x < 0)
		//{
		//	m_rotation.z += 180;
		//}

	}

	void Transform::Move(Vector3 moveBy)
	{
		SetPosition(GetPosition() + moveBy);
	}
}
