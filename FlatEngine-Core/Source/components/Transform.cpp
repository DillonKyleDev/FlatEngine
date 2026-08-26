#include "components/Body2D.h"
#include "components/Transform.h"
#include "GameObject.h"
#include "managers/SceneManager.h"
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

	Vector3 Transform::GetAbsolutePosition(Vector3 offset)
	{
		if (GetOwningObject() != nullptr)
		{
			GameObject* owner = GetOwningObject();
			Body2D* body2D = owner->Get<Body2D>();

			if (body2D != nullptr)
			{
				Vector2 body2DPos = body2D->GetPosition();
				m_position.x = body2DPos.x;
				m_position.y = body2DPos.y;
			}

			glm::vec4 rotatedPosition = glm::vec4(m_position.x + offset.x, m_position.y + offset.y, m_position.z + offset.z, 1);
			Vector3 parentAbsPosition;
			GameObject* parent = owner->GetParent();

			if (parent != nullptr)
			{
				Transform* parentTransform = parent->Get<Transform>();
				glm::mat4 parentRotationMatrix = parentTransform->GetAbsoluteRotationMatrix();
				rotatedPosition = parentRotationMatrix * rotatedPosition;
				parentAbsPosition = parentTransform->GetAbsolutePosition();				
			}

			return Vector3(rotatedPosition) + parentAbsPosition;
		}

		return m_position;
	}

	void Transform::SetPosition(Vector3 newPosition)
	{
		m_position = newPosition;

		if (GetOwningObject() != nullptr)
		{
			GameObject* owner = GetOwningObject();
			Body2D* body2D = owner->Get<Body2D>();

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
		Body2D* body2D = SceneManager::loadedScene.Get<Body2D>(GetOwnerID());

		if (body2D != nullptr)
		{
			m_rotation.z = body2D->GetRotation();
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

	Vector3 Transform::GetAbsoluteRotation()
	{
		GetRotation();

		Vector3 parentAbsRotation = Vector3();
		GameObject* owner = GetOwningObject();

		if (owner != nullptr && owner->GetParentID() != -1)
		{
			glm::mat4 parentRotationMatrix = SceneManager::loadedScene.Get<Transform>(owner->GetParentID())->GetRotationMatrix();

			parentAbsRotation = SceneManager::loadedScene.Get<Transform>(owner->GetParentID())->GetAbsoluteRotation();
		}

		return m_rotation + parentAbsRotation;		
	}

	// positive rotation = counterclockwise when viewed from the positive axis looking toward the origin (standard right-hand rule)
	glm::mat4 GetRotationMatrixInternal(Vector3 rotations)
	{		
		glm::mat4 xRotation = glm::rotate(glm::mat4(1.0f), glm::radians(rotations.x), glm::vec3(1.0f, 0.0f, 0.0f));
		glm::mat4 yRotation = glm::rotate(glm::mat4(1.0f), glm::radians(rotations.y), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 zRotation = glm::rotate(glm::mat4(1.0f), glm::radians(rotations.z), glm::vec3(0.0f, 0.0f, 1.0f));

		return (yRotation * xRotation * zRotation);
	}

	glm::mat4 Transform::GetRotationMatrix()
	{
		GetRotation();
		return GetRotationMatrixInternal(m_rotation);
	}

	glm::mat4 Transform::GetAbsoluteRotationMatrix()
	{
		glm::mat4 parentAbsRotationMatrix = glm::mat4(1);
		GameObject* owner = GetOwningObject();

		if (owner != nullptr && owner->GetParentID() != -1)
		{		
			parentAbsRotationMatrix = SceneManager::loadedScene.Get<Transform>(owner->GetParentID())->GetAbsoluteRotationMatrix();
		}

		return parentAbsRotationMatrix * GetRotationMatrix();
	}

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
