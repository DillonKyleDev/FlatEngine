#include "components/Camera.h"
#include "components/Transform.h"
#include "managers/SceneManager.h"
#include "render/SceneView.h"


namespace FlatEngine 
{
	Camera::Camera(long ownerID)
	{
		SetType(ComponentType_Camera);
		SetOwnerID(ownerID);
		orthoNearClippingDistance = -1000;
		orthoFarClippingDistance = 500;
		nearClippingDistance = 0.1f;
		farClippingDistance = 1000.0f;
		perspectiveAngle = 80.0f;
		b_shouldFollow = false;
		toFollowID = -1;
		followSmoothing = 0.1f;
		b_orthographic = true;		
		gridStep = 20;
		m_b_isPrimaryCamera = false;		
		
		Transform* transform = SceneManager::loadedScene.Get<Transform>(ownerID);
		if (transform != nullptr)
		{	
			SceneView::AddSceneViewCameraGizmo(*transform, ownerID);
			transform->SetRotation(Vector3(0.0f, 180.0f, 0.0f));
		}
	}

	json Camera::GetData(bool b_IDOverride)
	{
		json componentJson = {
			{ "gridStep", gridStep },
			{ "b_orthographic", b_orthographic },
			{ "b_isPrimaryCamera", m_b_isPrimaryCamera },
			{ "b_follow", b_shouldFollow },
			{ "followSmoothing", followSmoothing },
			{ "following", toFollowID },
			{ "perspectiveAngle", perspectiveAngle },
			{ "nearClippingDistance", nearClippingDistance },
			{ "farClippingDistance", farClippingDistance }
		};
		componentJson.update(Component::GetData(b_IDOverride));

		return componentJson;
	}

	void Camera::PutData(json componentJson, std::string objectName)
	{
		if (componentJson.empty())		
			return;	
		
        Component::PutData(componentJson, objectName);
		
		bool b_isPrimaryCamera = JsonHelper::CheckJsonBool(componentJson, "b_isPrimaryCamera", objectName);		
		SetPrimaryCamera(b_isPrimaryCamera);                                 
		b_orthographic = JsonHelper::CheckJsonBool(componentJson, "b_orthographic", objectName);
		gridStep = JsonHelper::CheckJsonFloat(componentJson, "gridStep", objectName);
		perspectiveAngle = JsonHelper::CheckJsonFloat(componentJson, "perspectiveAngle", objectName);
		nearClippingDistance = JsonHelper::CheckJsonFloat(componentJson, "nearClippingDistance", objectName);
		farClippingDistance = JsonHelper::CheckJsonFloat(componentJson, "farClippingDistance", objectName);
		b_shouldFollow = JsonHelper::CheckJsonBool(componentJson, "b_follow", objectName);
		followSmoothing = JsonHelper::CheckJsonFloat(componentJson, "followSmoothing", objectName);
		toFollowID = JsonHelper::CheckJsonLong(componentJson, "following", objectName);
    }

	void Camera::SetPrimaryCamera(bool b_isPrimary)
	{
		m_b_isPrimaryCamera = b_isPrimary;
		if (m_b_isPrimaryCamera)
		{
			SceneManager::loadedScene.SetPrimaryCamera(GetOwnerID());
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

	glm::mat4 Camera::GetProjection()
	{
		glm::mat4 projection;			
		float aspectRatio = 16.0f / 9.0f;

		if (b_orthographic)
		{    		
			float halfWidth  = SceneView::finalImageSize.x / gridStep / 2.0f;
			float halfHeight = SceneView::finalImageSize.y / gridStep / 2.0f;
			projection = glm::ortho(-halfWidth, halfWidth, -halfHeight, halfHeight, orthoNearClippingDistance, orthoFarClippingDistance);			
			projection[1][1] *= -1;
		}
		else
		{
			projection = glm::perspective(glm::radians(perspectiveAngle), aspectRatio, nearClippingDistance, farClippingDistance);
			projection[1][1] *= -1;
		}

		return projection;
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