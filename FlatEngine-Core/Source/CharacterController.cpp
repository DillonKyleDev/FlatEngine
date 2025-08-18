#include "CharacterController.h"
#include "FlatEngine.h"
#include "Transform.h"
#include "GameObject.h"
#include "box2d.h"


namespace FlatEngine 
{
	CharacterController::CharacterController(long myID, long parentID)
	{
		SetType(T_CharacterController);
		SetID(myID);
		SetParentID(parentID);
		m_maxAcceleration = 0.5f;
		m_maxSpeed = 10.0f;
		m_airControl = 0.2f;
		m_speedCorrection = 1;
		m_b_isMoving = false;
	}

	CharacterController::~CharacterController()
	{
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

	void CharacterController::MoveToward(Vector2 direction)
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