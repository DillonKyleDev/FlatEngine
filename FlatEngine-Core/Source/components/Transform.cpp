#include "components/Body2D.h"
#include "components/Transform.h"
#include "GameObject.h"
#include "tools/Quaternion.h"
#include "tools/Vector3.h"

#include <gtc/matrix_transform.hpp>


namespace FlatEngine
{
	Transform::Transform(long ownerID)
	{
		SetType(ComponentType_Transform);
		SetOwnerID(ownerID);
		m_position = Vector3();
		m_scale = Vector3(1);		
		m_rotation = Vector3();
	}

	json Transform::GetData(bool b_IDOverride)
	{
		json componentJson = {
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
		componentJson.update(Component::GetData(b_IDOverride));

		return componentJson;
	}

	void Transform::PutData(json componentJson, std::string objectName)
	{
		if (componentJson.empty())		
			return;	
		
        Component::PutData(componentJson, objectName);

		SetPosition(Vector3(JsonHelper::CheckJsonFloat(componentJson, "xPosition", objectName), JsonHelper::CheckJsonFloat(componentJson, "yPosition", objectName), JsonHelper::CheckJsonFloat(componentJson, "zPosition", objectName)));
		SetScale(Vector3(JsonHelper::CheckJsonFloat(componentJson, "xScale", objectName), JsonHelper::CheckJsonFloat(componentJson, "yScale", objectName), JsonHelper::CheckJsonFloat(componentJson, "zScale", objectName)));
		SetRotation(Vector3(JsonHelper::CheckJsonFloat(componentJson, "xRotation", objectName), JsonHelper::CheckJsonFloat(componentJson, "yRotation", objectName), JsonHelper::CheckJsonFloat(componentJson, "zRotation", objectName)));										
	}

	Vector3 Transform::GetAbsolutePosition()
	{
		if (GetOwningObject() != nullptr)
		{
			GameObject* parent = GetOwningObject();
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

	void Transform::SetPosition(Vector3 newPosition)
	{
		m_position = newPosition;

		if (GetOwningObject() != nullptr)
		{
			GameObject* parent = GetOwningObject();
			Body2D* body2D = parent->Get<Body2D>();

			if (body2D != nullptr)
			{
				Vector2 newPos = Vector2(newPosition.x, newPosition.y);
				body2D->SetPosition(newPos);
			}
		}
	}

	Vector3 Transform::GetCleanPosition()
	{
		return m_position;
	}

	Vector3 Transform::GetCleanRotation()
	{
		return m_rotation;
	}

	Vector3 Transform::GetPosition()
	{
		Body2D* body2D = nullptr;
		if (GetOwningObject() != nullptr)
		{
			body2D = GetOwningObject()->Get<Body2D>();
		}

		if (body2D != nullptr)
		{
			Vector2 body2DPos = body2D->GetPosition();
			m_position.x = body2DPos.x;
			m_position.y = body2DPos.y;
		}

		return m_position;
	}

	Vector3 Transform::GetAbsoluteScale()
	{
		Vector3 scaleOrigin = 1;

		if (GetOwningObject() != nullptr && GetOwningObject()->GetParent() != nullptr)
		{
			scaleOrigin = GetOwningObject()->GetParent()->Get<Transform>()->GetAbsoluteScale();
		}

		return scaleOrigin * m_scale;
	}

	void Transform::SetScale(Vector3 newScale)
	{
		m_scale = newScale;		
	}

	void Transform::SetRotation(Vector3 rotation)
	{
		m_rotation = rotation;

		if (GetOwningObject() != nullptr && GetOwningObject()->Get<Body2D>() != nullptr)
		{
			GetOwningObject()->Get<Body2D>()->SetRotation(m_rotation.z);
		}
	}

	Vector3 Transform::GetScale()
	{
		return m_scale;
	}

	Vector3 Transform::GetRotation()
	{
		if (GetOwningObject() != nullptr)
		{
			Body2D* body2D = GetOwningObject()->Get<Body2D>();

			if (body2D != nullptr)
			{
				m_rotation.z = body2D->GetRotation();
			}
		}

		return m_rotation;
	}

	void Transform::AddXRotation(float rotation)
	{
		m_rotation.x += rotation;
	}
	void Transform::AddYRotation(float rotation)
	{
		m_rotation.y += rotation;
	}
	void Transform::AddZRotation(float rotation)
	{
		m_rotation.z += rotation;
	}

	// positive rotation = counterclockwise when viewed from the positive axis looking toward the origin (standard right-hand rule)
	glm::mat4 Transform::GetRotationMatrix()
	{
		if (GetOwningObject() != nullptr)
		{
			Body2D* body2D = GetOwningObject()->Get<Body2D>();

			if (body2D != nullptr)
			{
				m_rotation.z = body2D->GetRotation();
			}
		}
		
		glm::mat4 xRotation = glm::rotate(glm::mat4(1.0f), glm::radians(m_rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
		glm::mat4 yRotation = glm::rotate(glm::mat4(1.0f), glm::radians(m_rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 zRotation = glm::rotate(glm::mat4(1.0f), glm::radians(m_rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

		return (yRotation * xRotation * zRotation);
	}

	// float Transform::GetAbsoluteRotation()
	// {
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
		// return m_rotation.z;
	// }

	glm::mat4 Transform::GetScaleMatrix()
	{
		return glm::scale(glm::mat4(1), glm::vec3(m_scale.x, m_scale.y, m_scale.z));
	}

	glm::vec4 Transform::GetLookDirection()
	{		
		return GetRotationMatrix() * glm::vec4(0.0f, 0.0f, 1.0f, 0.0f);
	}

	void Transform::LookAt(Vector3 lookAt)
	{		
		Vector3 direction = lookAt - m_position;
		direction.Normalize();
		Quaternion q1 = Quaternion::EulerToQuaternion(GetRotation());
		Quaternion q2 = Quaternion::EulerToQuaternion(Vector3::DirectionToRotation(direction));
		Quaternion r = Quaternion::Slerp(q1, q2, 1);
		m_rotation = Quaternion::QuaternionToEuler(r);
		m_rotation.z = 0;
	}

	void Transform::Move(Vector3 moveBy)
	{
		SetPosition(GetPosition() + moveBy);
	}
}
