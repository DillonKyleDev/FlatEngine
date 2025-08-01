#include "CharacterController.h"
#include "FlatEngine.h"
#include "Transform.h"
#include "RigidBody.h"
#include "GameObject.h"


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
		m_b_isMoving = true;

		if (GetParent()->HasComponent("RigidBody"))
		{
			Vector2 pendingForces = Vector2(0, 0);
			Transform* transform = GetParent()->GetTransform();
			float gravity = 1;
			Vector2 velocity = 0;
			float oneOverMass = 1;
			float mass = 1;
			float forceCorrection = 1;

			if (true /*rigidBody != nullptr*/)
			{
				//pendingForces = rigidBody->m_pendingForces;
				//velocity = rigidBody->m_velocity;
				//gravity = rigidBody->m_gravity;				
				//mass = rigidBody->m_mass;
				//oneOverMass = 1 / mass;
				//forceCorrection = rigidBody->m_forceCorrection;
			}
			else
			{
				LogError("No RigidBody connected to this GameObject.");
			}

			// If the object has not hit max speed in negative or positive direction
			if ((direction.x != 0 || direction.y != 0) &&
				(velocity.x >= 0 && velocity.x < m_maxSpeed) || (velocity.x <= 0 && velocity.x > -m_maxSpeed) ||
				// If velocity exceeds positive max speed but x direction is negative
				((velocity.x >= m_maxSpeed && direction.x * m_maxAcceleration < 0) ||
				// If velocity exceeds negative max speed but x direction is positive
				(velocity.x <= -m_maxSpeed && direction.x * m_maxAcceleration > 0)))
			{
				//if (rigidBody != nullptr)
				//{
				//	// Handles changes in direction
				//	float skidForce = 0.5f;
				//	if (rigidBody->IsGrounded() && ((velocity.x < 0 && direction.x > 0) || (velocity.x > 0 && direction.x < 0)))
				//	{
				//		rigidBody->SetPendingForces(Vector2(pendingForces.x * skidForce, pendingForces.y));
				//	}

				//	Vector2 finalForce = direction;
				//
				//	// In air
				//	if ((gravity != 0 && !rigidBody->IsGrounded()))
				//	{
				//		Vector2 inAirForce = Vector2(pendingForces.x + (direction.x * m_maxAcceleration * m_airControl), (pendingForces.y + (direction.y * m_maxAcceleration * m_airControl)));
				//		float modifiers = 1 / (m_maxAcceleration * m_airControl);

				//		if (velocity.x + inAirForce.x > m_maxSpeed)
				//		{
				//			finalForce.x = (m_maxSpeed - pendingForces.x) * modifiers;
				//		}
				//		else if (velocity.x + inAirForce.x < -m_maxSpeed)
				//		{
				//			finalForce.x = (-m_maxSpeed - pendingForces.x) * modifiers;
				//		}

				//		finalForce.x *= m_maxAcceleration * m_airControl;
				//	}
				//	// On the ground
				//	else
				//	{
				//		Vector2 groundedForce = Vector2(pendingForces.x + (direction.x * m_maxAcceleration), pendingForces.y + (direction.y * m_maxAcceleration));
				//		float modifiers = 1 / m_maxAcceleration;

				//		if (velocity.x + groundedForce.x > m_maxSpeed) // maxSpeed = pendingForces.x + (groundedForce.x * m_maxAcceleration)
				//		{
				//			finalForce.x = (m_maxSpeed - pendingForces.x) * modifiers;
				//		}
				//		else if (velocity.x + groundedForce.x < -m_maxSpeed)
				//		{
				//			finalForce.x = (-m_maxSpeed - pendingForces.x) * modifiers;
				//		}

				//		if (gravity == 0)
				//		{
				//			if (velocity.y + groundedForce.y > m_maxSpeed) // maxSpeed = pendingForces.x + (groundedForce.x * m_maxAcceleration)
				//			{
				//				finalForce.y = (m_maxSpeed - pendingForces.y) * modifiers;
				//			}
				//			else if (velocity.y + groundedForce.y < -m_maxSpeed)
				//			{
				//				finalForce.y = (-m_maxSpeed - pendingForces.y) * modifiers;
				//			}

				//			finalForce.y *= m_maxAcceleration;
				//		}

				//		finalForce.x *= m_maxAcceleration;
				//	}

				//	rigidBody->AddVelocity(finalForce);
				//	m_b_isMoving = true;
				//}
			}

			if (direction.x == 0 && ((gravity == 0 && direction.y == 0) || gravity != 0))
			{
				m_b_isMoving = false;
			}
		}
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