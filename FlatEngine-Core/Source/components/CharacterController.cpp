#include "components/CharacterController.h"


namespace FlatEngine 
{
	CharacterController::CharacterController(long ownerID)
	{
		SetType(ComponentType_CharacterController);
		SetOwnerID(ownerID);
		// m_capsule = Shape(-1);
		// PhysicsManager::physics2D.CreateShape(&m_capsule);		
		m_maxAcceleration = 0.5f;
		m_maxSpeed = 10.0f;
		m_airControl = 0.2f;
		m_speedCorrection = 1;
		m_b_isMoving = false;
	}

	json CharacterController::GetData(bool b_IDOverride)
	{
		json componentJson = {	
			{ "maxAcceleration", m_maxAcceleration },
			{ "maxSpeed", m_maxSpeed },
			{ "airControl", m_airControl }
		};
		componentJson.update(Component::GetData(b_IDOverride));

		return componentJson;
	}

	void CharacterController::PutData(json componentJson, std::string objectName)
	{
		if (componentJson.empty())		
			return;	
		
        Component::PutData(componentJson, objectName);

		SetMaxAcceleration(JsonHelper::CheckJsonFloat(componentJson, "maxAcceleration", objectName));
		SetMaxSpeed(JsonHelper::CheckJsonFloat(componentJson, "maxSpeed", objectName));
		SetAirControl(JsonHelper::CheckJsonFloat(componentJson, "airControl", objectName));
    }

	// Shape& CharacterController::GetCapsule()
	// {
	// 	// return m_capsule;
	// }

	void CharacterController::Move(Vector2 direction)
	{

	}

	void CharacterController::MoveToward(Vector2 position)
	{

	}

	void CharacterController::SetMaxAcceleration(float newMaxAcceleration)
	{
		m_maxAcceleration = newMaxAcceleration;
	}

	float CharacterController::GetMaxAcceleration()
	{
		return m_maxAcceleration;
	}

	void CharacterController::SetMaxSpeed(float newMaxSpeed)
	{
		m_maxSpeed = newMaxSpeed;
	}

	float CharacterController::GetMaxSpeed()
	{
		return m_maxSpeed;
	}

	void CharacterController::SetAirControl(float newAirControl)
	{
		m_airControl = newAirControl;
	}

	float CharacterController::GetAirControl()
	{
		return m_airControl;
	}

	void CharacterController::SetMoving(bool b_isMoving)
	{
		m_b_isMoving = b_isMoving;
	}

	bool CharacterController::IsMoving()
	{
		return m_b_isMoving;
	}
}