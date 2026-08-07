#include "components/Camera.h"
#include "components/Transform.h"
#include "managers/SceneManager.h"


namespace FlatEngine 
{
	Camera::Camera(long ownerID)
	{
		SetType(ComponentType_Camera);
		SetOwnerID(ownerID);
		orthoNearClippingDistance = -500;
		orthoFarClippingDistance = 500;
		nearClippingDistance = 0.1f;
		farClippingDistance = 100.0f;
		perspectiveAngle = 90.0f;
		b_shouldFollow = false;
		toFollowID = -1;
		followSmoothing = 0.1f;
		horizontalViewAngle = 0.0f;
		verticalViewAngle = 0.0f;
		b_orthographic = true;		
		gridStep = 20;
		orthoHorizontalViewAngle = 180.0;
		orthoVerticalViewAngle = 0.0;		
		m_b_isPrimaryCamera = false;		
		m_lookDirection = Vector3(0.0f, 0.0f, 1.0f);
	}

	json Camera::GetData(bool b_IDOverride)
	{
		json jsonData = {
			{ "type", (int)GetType()},
			{ "b_isCollapsed", IsCollapsed() },
			{ "b_isActive", IsActive() },
			{ "gridStep", gridStep },
			{ "b_orthographic", b_orthographic },
			{ "b_isPrimaryCamera", m_b_isPrimaryCamera },
			{ "b_follow", b_shouldFollow },
			{ "followSmoothing", followSmoothing },
			{ "following", toFollowID },
			{ "perspectiveAngle", perspectiveAngle },
			{ "nearClippingDistance", nearClippingDistance },
			{ "farClippingDistance", farClippingDistance },
			{ "horizontalViewAngle", horizontalViewAngle },
			{ "verticalViewAngle", verticalViewAngle }			
		};

		return jsonData;
	}

	void Camera::PutData(json componentJson, std::string objectName)
	{
        Component::PutData(componentJson, objectName);
		
		bool b_isPrimaryCamera = JsonHelper::CheckJsonBool(componentJson, "b_isPrimaryCamera", objectName);		
		SetPrimaryCamera(b_isPrimaryCamera);
		SceneManager::loadedScene.SetPrimaryCamera(this);                                    
		b_orthographic = JsonHelper::CheckJsonBool(componentJson, "b_orthographic", objectName);
		gridStep = JsonHelper::CheckJsonFloat(componentJson, "gridStep", objectName);
		perspectiveAngle = JsonHelper::CheckJsonFloat(componentJson, "perspectiveAngle", objectName);
		nearClippingDistance = JsonHelper::CheckJsonFloat(componentJson, "nearClippingDistance", objectName);
		farClippingDistance = JsonHelper::CheckJsonFloat(componentJson, "farClippingDistance", objectName);
		horizontalViewAngle = JsonHelper::CheckJsonFloat(componentJson, "horizontalViewAngle", objectName);
		verticalViewAngle = JsonHelper::CheckJsonFloat(componentJson, "verticalViewAngle", objectName);
		b_shouldFollow = JsonHelper::CheckJsonBool(componentJson, "b_follow", objectName);
		followSmoothing = JsonHelper::CheckJsonFloat(componentJson, "followSmoothing", objectName);
		toFollowID = JsonHelper::CheckJsonLong(componentJson, "following", objectName);
    }

	void Camera::SetPrimaryCamera(bool b_isPrimary)
	{
		m_b_isPrimaryCamera = b_isPrimary;
		if (b_isPrimary)
		{
			SceneManager::loadedScene.SetPrimaryCamera(this);
		}
	}

	bool Camera::IsPrimary()
	{
		return m_b_isPrimaryCamera;
	}

	void Camera::Follow()
	{
		//GameObject *followTarget = GetObjectByID(m_toFollowID);
		//if (m_b_shouldFollow && followTarget != nullptr && followTarget->HasComponent("Transform"))
		//{
		//	Transform* cameraTransform = GetParent()->Get<Transform>();
		//	Vector2 followPos = followTarget->Get<Transform>()->GetAbsolutePosition();
		//	Vector2 currentPos = cameraTransform->GetPosition(); // Shouldn't have a parent if following so don't need GetTruePosition()

		//	cameraTransform->SetPosition(Lerp(currentPos, followPos, m_followSmoothing));
		//}
	}

	glm::vec4 Camera::GetLookDirection()
	{
		Vector3 rotation = SceneManager::loadedScene.GetObjectByID(GetOwnerID())->Get<Transform>()->GetRotations();
		glm::mat4 rollCameraMatrix         = glm::rotate(glm::mat4(1.0f), glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
		glm::mat4 horCameraRotationMatrix  = glm::rotate(glm::mat4(1.0f), glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
		glm::mat4 vertCameraRotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 rotationMatrix = horCameraRotationMatrix * vertCameraRotationMatrix;

		if (rotation.x != 0) // prevent unnecessary matrix multiplication if possible
		{
			rotationMatrix *= rollCameraMatrix;
		}

		return rotationMatrix * glm::vec4(0.0f, 0.0f, 1.0f, 0.0f); // default looking in the z direction;
	}

	// Meant for the Scene View Camera because it does not exist in the Scene objects pool and would crash with above function
	glm::vec4 Camera::GetLookDirectionNoRoll()
	{
		glm::mat4 horCameraRotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(b_orthographic ? orthoHorizontalViewAngle : horizontalViewAngle), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 vertCameraRotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(b_orthographic ? orthoVerticalViewAngle : verticalViewAngle), glm::vec3(1.0f, 0.0f, 0.0f));
		glm::mat4 rotationMatrix = horCameraRotationMatrix * vertCameraRotationMatrix;

		return rotationMatrix * glm::vec4(0.0f, 0.0f, 1.0f, 0.0f);
	}

	void Camera::AddToVerticalViewAngle(float toAdd)
	{
		if (verticalViewAngle + toAdd >= 90)
		{
			verticalViewAngle = 89.99f;
		}
		else if (verticalViewAngle + toAdd <= -90)
		{
			verticalViewAngle = -89.99f;
		}
		else
		{
			verticalViewAngle += toAdd;
		}
	}

	void Camera::AddVelocity(Vector3 velocity)
	{
		m_velocity.x += velocity.x;
		m_velocity.y += velocity.y;
		m_velocity.z += velocity.z;
	}

	Vector3& Camera::GetVelocity()
	{
		return m_velocity;
	}

	// Eventually remove the event handling and give Project viewport camera settings to save and load. Use Primary camera here in the meantime
	void Camera::Update()
	{
		Follow();

		if (m_velocity != 0)
		{
			Transform* transform = GetOwningObject()->Get<Transform>();
			Vector3 position = transform->GetPosition();
			transform->SetPosition(position + m_velocity);
			m_velocity.x *= 0.95f;
			m_velocity.y *= 0.95f;
			m_velocity.z *= 0.95f;
		}
	}
}