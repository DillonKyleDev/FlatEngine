#include "RigidBody.h"
#include "GameObject.h"
#include "Transform.h"
#include "CharacterController.h"
#include "BoxCollider.h"
#include "Sprite.h"


namespace FlatEngine 
{
	RigidBody::RigidBody(long myID, long parentID)
	{
		//SetType(T_RigidBody);
		SetID(myID);
		SetParentID(parentID);
		m_mass = 10;
		m_1overMass = 1 / m_mass;		
		m_pendingForces = Vector2(0, 0);
		m_velocity = Vector2(0, 0);	
		m_acceleration = Vector2(0, 0);
		m_friction = 0.01f;
		m_linearDrag = 0.99f; // 0.999f for pool

		// Rotational
		m_I = 1;
		m_1overI = 1;
		m_pendingTorques = 0;
		m_angularVelocity = 0;
		m_angularAcceleration = 0;
		m_angularDrag = 0.85f; // 0.82f;
		m_b_allowTorques = true;

		m_restitution = 1;
		m_equilibriumForce = 2;				
		m_b_isGrounded = false;
		m_b_isStatic = false;
		m_gravity = 1;
		m_fallingGravity = m_gravity;
		m_terminalVelocity = 3.0f;
		m_windResistance = 0.9f;  // Lower value = more resistance
	}

	RigidBody::~RigidBody()
	{
	}

	std::string RigidBody::GetData()
	{
		json jsonData = {
			{ "type", "RigidBody" },
			{ "id", GetID() },
			{ "_isCollapsed", IsCollapsed() },
			{ "_isActive", IsActive() },
			{ "mass", m_mass},
			{ "angularDrag", m_angularDrag },
			{ "gravity", m_gravity },
			{ "fallingGravity", m_fallingGravity },
			{ "friction", m_friction },
			{ "equilibriumForce", m_equilibriumForce },
			{ "terminalVelocity", m_terminalVelocity },
			{ "windResistance", m_windResistance },
			{ "_isStatic", m_b_isStatic },
			{ "_allowTorques", m_b_allowTorques },
		};

		std::string data = jsonData.dump();
		// Return dumped json object with required data for saving
		return data;
	}

	void RigidBody::CalculatePhysics()
	{
		ApplyGravity();
		//ApplyFriction();
		//ApplyEquilibriumForce();
	}

	void RigidBody::ApplyPhysics(float deltaTime)
	{
		Transform* transform = GetParent()->GetTransform();
		Vector2 position = transform->GetPosition();		
		m_acceleration = m_pendingForces * m_1overMass * deltaTime; // Makes the calculations more accurate
		m_velocity = (m_velocity + m_acceleration) * m_linearDrag;	// linear drag dampens velocity so it becomes 0 eventually	
		transform->SetPosition(position + m_velocity);	
		m_pendingForces = Vector2(0, 0);

		if (m_b_allowTorques)
		{
			m_angularAcceleration = m_pendingTorques * m_1overI * deltaTime;
			m_angularVelocity += m_angularAcceleration;
			m_angularVelocity *= m_angularDrag;
			transform->SetRotation((float)fmod(transform->GetRotation() + m_angularVelocity, 360));
			m_angularAcceleration = 0;
		}
	}

	void RigidBody::AddVelocity(Vector2 vel)
	{
		if (!m_b_isStatic)
		{
			m_velocity = m_velocity + vel;
		}
	}

	void RigidBody::ApplyGravity()
	{
		if (m_gravity > 0 && !m_b_isStatic)
		{
			if (m_velocity.y > -m_terminalVelocity)
			{
				if (m_velocity.y < 0)
				{
					m_velocity.y -= m_fallingGravity * 0.5f * GetDeltaTime();
				}
				else
				{
					m_velocity.y -= m_gravity * 0.5f * GetDeltaTime();
				}
			}
		}
		else if (m_gravity < 0 && !m_b_isStatic)
		{
			if (m_velocity.y < m_terminalVelocity)
			{
				if (m_velocity.y > 0)
				{
					m_velocity.y -= m_fallingGravity * 0.5f * GetDeltaTime();
				}
				else
				{
					m_velocity.y -= m_gravity * 0.5f * GetDeltaTime();
				}
			}
		}
	}

	void RigidBody::ApplyFriction()
	{
		// Wind resistance
		if (m_gravity != 0 && !m_b_isGrounded)
		{
			Vector2 dampenedVelocity = Vector2(m_pendingForces.x * m_windResistance, m_pendingForces.y * m_windResistance);
			m_pendingForces = dampenedVelocity;
		}

		// Rotational drag
		if (m_b_allowTorques)
		{
			m_pendingTorques *= m_angularDrag;
		}

		// Get Character Controller for b_isMoving
		CharacterController* characterController = nullptr;

		if (GetParent() != nullptr && GetParent()->HasComponent("CharacterController"))
		{
			characterController = GetParent()->GetCharacterController();
		}

		bool b_isMoving = false;
		if (characterController != nullptr)
		{
			b_isMoving = characterController->IsMoving();
		}

		// Ground m_friction
		if (!b_isMoving && (m_gravity != 0 && m_b_isGrounded))
		{
			Vector2 dampenedVelocity = Vector2(m_pendingForces.x * m_friction, m_pendingForces.y);
			m_pendingForces = dampenedVelocity;
		}	
		else if (m_gravity == 0)
		{
			Vector2 dampenedVelocity = Vector2(m_pendingForces.x * m_friction, m_pendingForces.y * m_friction);
			m_pendingForces = dampenedVelocity;
		}
	}

	void RigidBody::ApplyEquilibriumForce()
	{
		float maxSpeed = 1;
		CharacterController* characterController = nullptr;

		if (GetParent() != nullptr && GetParent()->HasComponent("CharacterController"))
		{
			characterController = GetParent()->GetCharacterController();
		}

		if (characterController != nullptr)
		{
			maxSpeed = characterController->GetMaxSpeed();
		}

		if (characterController != nullptr || m_friction != 1)
		{
			// Horizontal speed control
			if (characterController != nullptr && m_velocity.x > maxSpeed || m_velocity.x > m_terminalVelocity)
			{
				m_pendingForces.x -= m_equilibriumForce;
			}
			else if (characterController != nullptr && m_velocity.x < -maxSpeed || m_velocity.x < -m_terminalVelocity)
			{
				m_pendingForces.x += m_equilibriumForce;
			}
			// Vertical speed control
			//if (m_velocity.y > maxSpeed)
			//	m_pendingForces.y -= m_equilibriumForce;
			//else if (m_velocity.y < -maxSpeed)
			//	m_pendingForces.y += m_equilibriumForce;		
		}
	}

	void RigidBody::ApplyCollisionForces()
	{
		//std::vector<BoxCollider*> boxColliders = GetParent()->GetBoxColliders();
		//std::vector<CircleCollider*> circleColliders = GetParent()->GetCircleColliders();
		//
		//for (BoxCollider* boxCollider : boxColliders)
		//{
		//	if (boxCollider != nullptr)
		//	{
		//		Vector2 scale = Vector2(1, 1);
		//		Transform* transform = boxCollider->GetParent()->GetTransform();
		//		if (transform != nullptr)
		//		{
		//			scale = transform->GetScale();
		//		}

		//		float halfWidth = boxCollider->GetActiveWidth() / 2 * scale.x;
		//		float halfHeight = boxCollider->GetActiveHeight() / 2 * scale.y;

		//		ApplyCollisionForce(boxCollider, halfWidth, halfHeight);
		//	}
		//}		
		//for (CircleCollider* circleCollider : circleColliders)
		//{
		//	if (circleCollider != nullptr)
		//	{
		//		float activeRadius = circleCollider->GetActiveRadiusGrid();
		//		ApplyCollisionForce(circleCollider, activeRadius, activeRadius);
		//	}
		//}
	}

	void RigidBody::ApplyCollisionForce(Collider* collider, float halfWidth, float halfHeight)
	{
		
	}

	void RigidBody::AddForce(Vector2 direction, float magnitude)
	{
		if (!m_b_isStatic && direction != Vector2())
		{			
			Vector2 addedForce = Vector2::Normalize(direction) * magnitude;
			m_pendingForces = m_pendingForces + addedForce;
		}
	}

	void RigidBody::AddTorque(float torque, float direction)
	{
		if (m_b_allowTorques)
		{
			float addedTorque = torque * direction;
			m_pendingTorques += addedTorque;
		}
	}

	Vector2 RigidBody::GetNextPosition()
	{
		Vector2 nextVelocity = m_velocity + m_acceleration;
		Transform* transform = GetParent()->GetTransform();
		Vector2 position = transform->GetPosition();
		return position + nextVelocity;
	}

	void RigidBody::SetMass(float mass)
	{
		if (mass != 0)
		{
			m_mass = mass;
			UpdateI();
		}
	}

	float RigidBody::GetMass()
	{
		return m_mass;
	}

	float RigidBody::GetMassInv()
	{
		return m_1overMass;
	}

	float RigidBody::GetI()
	{
		return m_I;
	}

	float RigidBody::GetIInv()
	{
		return m_1overI;
	}

	void RigidBody::UpdateI()
	{		
		//if (GetParent() != nullptr)
		//{
		//	Vector2 scale = GetParent()->GetTransform()->GetScale();
		//	BoxCollider* boxCollider = GetParent()->GetBoxCollider();
		//	CircleCollider* circleCollider = GetParent()->GetCircleCollider();

		//	if (boxCollider != nullptr)
		//	{
		//		float width = (float)boxCollider->GetActiveWidth() * scale.x / 10 ;
		//		float height = (float)boxCollider->GetActiveHeight() * scale.y / 10;
		//		if (width != 0 && height != 0)
		//		{
		//			m_I = (1 / 12) * m_mass * ((height * height) + (width * width));
		//		}
		//	}
		//	else if (circleCollider != nullptr)
		//	{
		//		float radius = (float)circleCollider->GetActiveRadiusGrid();
		//		if (radius != 0)
		//		{
		//			m_I = m_mass * 0.5f * radius * radius;
		//		}
		//	}
		//}

		//if (!m_b_isStatic)
		//{
		//	m_1overMass = 1 / m_mass;

		//	if (m_I != 0)
		//	{
		//		m_1overI = 1 / m_I;
		//	}
		//	else
		//	{
		//		m_1overI = m_1overMass;
		//	}
		//}
		//else
		//{
		//	m_1overI = 0;
		//	m_1overMass = 0;
		//}
	}

	void RigidBody::SetTorquesAllowed(bool b_allowed)
	{
		m_b_allowTorques = b_allowed;
	}

	bool RigidBody::TorquesAllowed()
	{
		return m_b_allowTorques;
	}

	void RigidBody::SetAngularDrag(float angularDrag)
	{
		m_angularDrag = angularDrag;
	}

	float RigidBody::GetAngularDrag()
	{
		return m_angularDrag;
	}

	void RigidBody::SetGravity(float gravity)
	{
		m_gravity = gravity;
	}

	float RigidBody::GetGravity()
	{
		return m_gravity;
	}

	void RigidBody::SetFallingGravity(float fallingGravity)
	{
		m_fallingGravity = fallingGravity;
	}

	float RigidBody::GetFallingGravity()
	{
		return m_fallingGravity;
	}

	void RigidBody::SetVelocity(Vector2 velocity)
	{
		m_velocity = velocity;
	}

	void RigidBody::SetTerminalVelocity(float terminalVelocity)
	{
		m_terminalVelocity = terminalVelocity;
	}

	float RigidBody::GetTerminalVelocity()
	{
		return m_terminalVelocity;
	}

	void RigidBody::SetEquilibriumForce(float equilibriumForce)
	{
		m_equilibriumForce = equilibriumForce;
	}

	Vector2 RigidBody::GetVelocity()
	{
		return m_velocity;
	}

	Vector2 RigidBody::GetAcceleration()
	{
		return m_acceleration;
	}

	Vector2 RigidBody::GetPendingForces()
	{
		return m_pendingForces;
	}

	float RigidBody::GetAngularVelocity()
	{
		return m_angularVelocity;
	}

	float RigidBody::GetAngularVelocityRadians()
	{
		return m_angularVelocity / 57.29578f;
	}

	float RigidBody::GetAngularAcceleration()
	{
		return m_angularAcceleration;
	}

	float RigidBody::GetPendingTorques()
	{
		return m_pendingTorques;
	}

	void RigidBody::SetIsStatic(bool b_static)
	{
		m_b_isStatic = b_static;
	}

	bool RigidBody::IsStatic()
	{
		return m_b_isStatic;
	}

	void RigidBody::SetIsGrounded(bool b_grounded)
	{
		m_b_isGrounded = b_grounded;
	}

	bool RigidBody::IsGrounded()
	{
		return m_b_isGrounded;
	}

	float RigidBody::GetRestitution()
	{
		return m_restitution;
	}

	void RigidBody::SetRestitution(float restitution)
	{
		m_restitution = restitution;
	}
	
	void RigidBody::SetPendingForces(Vector2 pendingForces)
	{
		m_pendingForces = pendingForces;
	}

	void RigidBody::SetAngularVelocity(float angularVelocity)
	{
		m_angularVelocity = angularVelocity;
	}

	void RigidBody::AddAngularVelocity(float angularVelocity)
	{
		m_angularVelocity += angularVelocity;
	}

	void RigidBody::SetWindResistance(float windResistance)
	{
		m_windResistance = windResistance;
	}

	float RigidBody::GetWindResistance()
	{
		return m_windResistance;
	}

	float RigidBody::GetFriction()
	{
		return m_friction;
	}

	void RigidBody::SetFriction(float friction)
	{
		m_friction = friction;
	}
	
	float RigidBody::AdjustedFriction()
	{		
		return 1 - m_friction;
	}

	float RigidBody::GetEquilibriumForce()
	{
		return m_equilibriumForce;
	}
}