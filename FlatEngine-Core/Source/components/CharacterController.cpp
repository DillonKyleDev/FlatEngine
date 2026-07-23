#include "components/CharacterController.h"


namespace FlatEngine 
{
	CharacterController::CharacterController(long myID, long parentObjectID)
	{
		SetType(ComponentType_CharacterController);
		SetID(myID);
		SetParentObjectID(parentObjectID);		
		m_capsule = Capsule();
		m_capsule.CreateShape();
		m_maxAcceleration = 0.5f;
		m_maxSpeed = 10.0f;
		m_airControl = 0.2f;
		m_speedCorrection = 1;
		m_b_isMoving = false;
	}

	std::string CharacterController::GetData()
	{
		json jsonData = {
			{ "type", "CharacterController" },
			{ "id", GetID() },
			{ "_isCollapsed", IsCollapsed() },
			{ "_isActive", IsActive() },			
			{ "maxAcceleration", m_maxAcceleration },
			{ "maxSpeed", m_maxSpeed },
			{ "airControl", m_airControl }
		};

		std::string data = jsonData.dump();
		return data;
	}

	    void CharacterController::PutData(json componentJson)
	{
		std::string objectName = "CharacterController GameObject";
		std::string type = JsonHelper::CheckJsonString(componentJson, "type", objectName);
		long componentID = JsonHelper::CheckJsonLong(componentJson, "id", objectName);
		bool b_isCollapsed = JsonHelper::CheckJsonBool(componentJson, "_isCollapsed", objectName);
		bool b_isActive = JsonHelper::CheckJsonBool(componentJson, "_isActive", objectName);
		SetActive(b_isActive);
		SetCollapsed(b_isCollapsed);
		SetMaxAcceleration(JsonHelper::CheckJsonFloat(componentJson, "maxAcceleration", objectName));
		SetMaxSpeed(JsonHelper::CheckJsonFloat(componentJson, "maxSpeed", objectName));
		SetAirControl(JsonHelper::CheckJsonFloat(componentJson, "airControl", objectName));
    }

	Capsule& CharacterController::GetCapsule()
	{
		return m_capsule;
	}

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