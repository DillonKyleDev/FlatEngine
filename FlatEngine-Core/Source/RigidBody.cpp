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
		SetType(T_RigidBody);
		SetID(myID);
		SetParentID(parentID);
		m_mass = 1;
		m_1overMass = 1 / m_mass;		
		m_pendingForces = Vector2(0, 0);
		m_velocity = Vector2(0, 0);	
		m_acceleration = Vector2(0, 0);
		m_friction = 0.86f;  // 1 = no m_friction. 0 = m_velocity = 0
		// Rotational
		m_I = 1; // Moment of inertia = 2mass / 5 * (r^2) (based on sprite dimensions)
		m_1overI = 1;
		m_pendingTorques = 0;
		m_angularVelocity = 0;
		m_angularAcceleration = 0;
		m_angularDrag = 1;
		m_b_allowTorques = true;

		m_equilibriumForce = 2;				
		m_b_isGrounded = false;
		m_b_isStatic = false;
		m_forceCorrection = 0.01f;
		m_gravity = 1;
		m_fallingGravity = m_gravity * 1.2f;
		m_terminalVelocity = m_gravity * 1;
		m_windResistance = 1.0f;  // Lower value = more resistance
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
		ApplyCollisionForces();
		ApplyGravity();
		ApplyFriction();
		ApplyEquilibriumForce();
	
		// Linear
		if (m_mass == 0)
		{
			m_acceleration = Vector2(m_pendingForces.x, m_pendingForces.y);
		}
		else
		{
			m_acceleration = Vector2(m_pendingForces.x * m_1overMass * m_forceCorrection, m_pendingForces.y * m_1overMass * m_forceCorrection);
		}

		// Rotational - Torque = I * angularAcceleration
		if (m_I == 0)
		{
			m_angularAcceleration = m_pendingTorques;
		}
		else
		{
			m_angularAcceleration = m_pendingTorques * m_1overI;
		}
	}

	void RigidBody::ApplyPhysics(float deltaTime)
	{
		// In reality, we should be adding acceleration to velocity, but it is difficult to simulate opposing forces that would actually cause objects to slow down (ie. give them negative values in the opposing direction at all times)
		m_velocity = Vector2(m_acceleration.x, m_acceleration.y);
		Transform* transform = GetParent()->GetTransform();
		Vector2 position = transform->GetPosition();

		transform->SetPosition(Vector2(position.x + m_velocity.x, position.y + m_velocity.y));

		if (m_b_allowTorques)
		{
			m_angularVelocity = m_angularAcceleration;
		}

		float rotation = transform->GetRotation();
		transform->SetRotation((float)fmod(rotation + m_angularVelocity, 360));
	}

	Vector2 RigidBody::AddVelocity(Vector2 vel)
	{
		// Make sure not colliding in that direction before adding the m_velocity
		std::vector<BoxCollider*> boxColliders = GetParent()->GetBoxColliders();
		for (BoxCollider* boxCollider : boxColliders)
		{
			if (boxCollider != nullptr && (vel.x > 0 && (!boxCollider->m_b_isCollidingRight || !boxCollider->m_b_rightCollisionStatic) || vel.x < 0 && (!boxCollider->m_b_isCollidingLeft || !boxCollider->m_b_leftCollisionStatic)))
			{
				m_pendingForces.x += vel.x;
			}
		}
			
		m_pendingForces.y += vel.y;
		return m_pendingForces;
	}

	void RigidBody::ApplyGravity()
	{
		if (m_gravity > 0 && m_mass != 0)
		{
			if (!m_b_isGrounded && m_velocity.y > -m_terminalVelocity)
			{
				if (m_velocity.y < 0)
				{
					m_pendingForces.y -= m_fallingGravity;
				}
				else
				{
					m_pendingForces.y -= m_gravity;
				}
			}
		}
		else if (m_gravity < 0 && m_mass != 0)
		{
			if (!m_b_isGrounded && m_velocity.y < m_terminalVelocity)
			{
				if (m_velocity.y > 0)
				{
					m_pendingForces.y -= m_fallingGravity;
				}
				else
				{
					m_pendingForces.y -= m_gravity;
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
		std::vector<BoxCollider*> boxColliders = GetParent()->GetBoxColliders();
		std::vector<CircleCollider*> circleColliders = GetParent()->GetCircleColliders();
		
		for (BoxCollider* boxCollider : boxColliders)
		{
			if (boxCollider != nullptr)
			{
				Vector2 scale = Vector2(1, 1);
				Transform* transform = boxCollider->GetParent()->GetTransform();
				if (transform != nullptr)
				{
					scale = transform->GetScale();
				}

				float halfWidth = boxCollider->GetActiveWidth() / 2 * scale.x;
				float halfHeight = boxCollider->GetActiveHeight() / 2 * scale.y;

				ApplyCollisionForce(boxCollider, halfWidth, halfHeight);
			}
		}		
		for (CircleCollider* circleCollider : circleColliders)
		{
			if (circleCollider != nullptr)
			{
				float activeRadius = circleCollider->GetActiveRadiusGrid();
				ApplyCollisionForce(circleCollider, activeRadius, activeRadius);
			}
		}
	}

	void RigidBody::ApplyCollisionForce(Collider* collider, float halfWidth, float halfHeight)
	{
		Transform* transform = GetParent()->GetTransform();
		Vector2 position = GetParent()->GetTransform()->GetPosition();				
		float newYPos;
		float newXPos;
		float cornerm_friction = .01f;
		float cornerLerp = 0.3f;
		
		if (collider->GetTypeString() == "CircleCollider")
		{
			// Moving down
			if (m_pendingForces.y < 0)
			{
				if (collider->m_b_isCollidingBottom && collider->m_b_bottomCollisionSolid && 
					(!collider->m_b_isCollidingBottomLeft || (collider->m_b_isCollidingBottomLeft && !collider->m_b_bottomLeftCollisionSolid)) && 
					(!collider->m_b_isCollidingBottomRight || (collider->m_b_isCollidingBottomRight && !collider->m_b_bottomRightCollisionSolid)))
				{
					m_pendingForces.y = 0;
					newYPos = collider->m_bottomCollidedPosition.y;
					transform->SetPosition(Vector2(position.x, newYPos));
				}
				else if (collider->m_b_isCollidingBottomLeft && collider->m_b_bottomLeftCollisionSolid)
				{
					newXPos = collider->m_bottomLeftCollidedPosition.x;
					newYPos = collider->m_bottomLeftCollidedPosition.y;
					transform->SetPosition(Vector2(newXPos, newYPos));
				}
				else if (collider->m_b_isCollidingBottomRight && collider->m_b_bottomRightCollisionSolid)
				{
					newXPos = position.x - cornerm_friction * -m_pendingForces.y; // Give some slide away from the edge based on m_pendingForces
					newYPos = collider->m_bottomRightCollidedPosition.y;
					//m_pendingForces.y = 0;
					transform->SetPosition(Lerp(position, Vector2(newXPos, newYPos), cornerLerp));
				}
			}	

			// Normal m_gravity
			if (m_gravity > 0 && (collider->m_b_isCollidingBottom && collider->m_b_bottomCollisionSolid) ||
				(collider->m_b_isCollidingBottomLeft && collider->m_b_bottomLeftCollisionSolid) ||
				(collider->m_b_isCollidingBottomRight && collider->m_b_bottomRightCollisionSolid))
			{
				m_b_isGrounded = true;
			}
			// Inverted m_gravity
			else if (m_gravity < 0 && (collider->m_b_isCollidingTop && collider->m_b_topCollisionSolid) || 
				(collider->m_b_isCollidingTopLeft && !collider->m_b_topLeftCollisionSolid) ||
				(collider->m_b_isCollidingTopRight && !collider->m_b_topRightCollisionSolid))
			{
				m_b_isGrounded = true;
			}
			else
			{
				m_b_isGrounded = false;
			}

			// Moving up
			if (m_pendingForces.y > 0)
			{
				if (collider->m_b_isCollidingTop && collider->m_b_topCollisionSolid &&
					(!collider->m_b_isCollidingTopLeft || (collider->m_b_isCollidingTopLeft && !collider->m_b_topLeftCollisionSolid)) &&
					(!collider->m_b_isCollidingTopRight || (collider->m_b_isCollidingTopRight && !collider->m_b_topRightCollisionSolid)))
				{
					// Inverted m_gravity
					if (m_gravity < 0)
						m_b_isGrounded = true;

					m_pendingForces.y = 0;
					newYPos = collider->m_topCollidedPosition.y;
					transform->SetPosition(Vector2(position.x, newYPos));
				}
				else if (collider->m_b_isCollidingTopLeft && collider->m_b_topLeftCollisionSolid)
				{
					if (m_gravity < 0)
					{
						m_b_isGrounded = true;
					}

					newXPos = position.x - cornerm_friction * -m_pendingForces.y; // Give some slide away from the edge based on m_pendingForces
					newYPos = collider->m_topLeftCollidedPosition.y;
					m_pendingForces.y = 0;
					transform->SetPosition(Lerp(position, Vector2(newXPos, newYPos), cornerLerp));
				}
				else if (collider->m_b_isCollidingTopRight && collider->m_b_topRightCollisionSolid)
				{
					if (m_gravity < 0)
					{
						m_b_isGrounded = true;
					}

					newXPos = position.x + cornerm_friction * -m_pendingForces.y; // Give some slide away from the edge based on m_pendingForces
					newYPos = collider->m_topRightCollidedPosition.y;
					m_pendingForces.y = 0;
					transform->SetPosition(Lerp(position, Vector2(newXPos, newYPos), cornerLerp));
				}
				else if (m_gravity < 0)
				{
					m_b_isGrounded = false;
				}
			}
			// Moving left
			if (m_pendingForces.x < 0)
			{
				if (collider->m_b_isCollidingLeft && collider->m_b_leftCollisionSolid && 
					(!collider->m_b_isCollidingBottomLeft || (collider->m_b_isCollidingBottomLeft && !collider->m_b_bottomLeftCollisionSolid)) &&
					(!collider->m_b_isCollidingTopLeft || (collider->m_b_isCollidingTopLeft && !collider->m_b_topLeftCollisionSolid)))
				{
					m_pendingForces.x = 0;
					newXPos = collider->m_leftCollidedPosition.x;
					transform->SetPosition(Vector2(newXPos, position.y));
				}
				else if (collider->m_b_isCollidingBottomLeft && collider->m_b_bottomLeftCollisionSolid)
				{
					newYPos = position.y + cornerm_friction * -m_pendingForces.x; // Give some slide away from the edge based on m_pendingForces
					newXPos = collider->m_bottomLeftCollidedPosition.x;
					m_pendingForces.x = 0;				
					transform->SetPosition(Lerp(position, Vector2(newXPos, newYPos), cornerLerp));
				}
				else if (collider->m_b_isCollidingTopLeft && collider->m_b_topLeftCollisionSolid)
				{
					newYPos = position.y - cornerm_friction * -m_pendingForces.x; // Give some slide away from the edge based on m_pendingForces
					newXPos = collider->m_topLeftCollidedPosition.x;
					m_pendingForces.x = 0;					
					transform->SetPosition(Lerp(position, Vector2(newXPos, newYPos), cornerLerp));
				}
			}
			// Moving right
			if (m_pendingForces.x > 0)
			{
				if (collider->m_b_isCollidingRight && collider->m_b_rightCollisionSolid && 
					(!collider->m_b_isCollidingBottomRight || (collider->m_b_isCollidingBottomRight && !collider->m_b_bottomRightCollisionSolid)) &&
					(!collider->m_b_isCollidingTopRight || (collider->m_b_isCollidingTopRight && !collider->m_b_topRightCollisionSolid)))
				{
					m_pendingForces.x = 0;
					newXPos = collider->m_rightCollidedPosition.x;
					transform->SetPosition(Vector2(newXPos, position.y));
				}
				else if (collider->m_b_isCollidingBottomRight && collider->m_b_bottomRightCollisionSolid)
				{
					newYPos = position.y + cornerm_friction * m_pendingForces.x; // Give some slide away from the edge based on m_pendingForces
					newXPos = collider->m_bottomRightCollidedPosition.x;
					m_pendingForces.x = 0;
					transform->SetPosition(Lerp(position, Vector2(newXPos, newYPos), cornerLerp));
				}
				else if (collider->m_b_isCollidingTopRight && collider->m_b_topRightCollisionSolid)
				{
					newYPos = position.y - cornerm_friction * m_pendingForces.x; // Give some slide away from the edge based on m_pendingForces
					newXPos = collider->m_topRightCollidedPosition.x;
					m_pendingForces.x = 0;
					transform->SetPosition(Lerp(position, Vector2(newXPos, newYPos), cornerLerp));
				}
			}
		}		
		else if (collider->GetTypeString() == "BoxCollider")
		{
			// "Floor" Collision Forces
			// 
			// Check if grounded normal m_gravity
			if ((collider->m_b_isCollidingBottom && collider->m_b_bottomCollisionSolid) || 
				(!collider->m_b_isCollidingBottom && 
				((collider->m_b_isCollidingBottomLeft && collider->m_b_bottomLeftCollisionStatic) || 
				(collider->m_b_isCollidingBottomRight && collider->m_b_bottomRightCollisionStatic))))				
			{
				if (m_gravity > 0)
				{
					m_b_isGrounded = true;
				}
				//else if (m_gravity == 0 && m_pendingForces.y < 0)
				//{
				//	m_pendingForces.y = 0;
				//	newYPos = collider->m_bottomCollision + halfHeight + 0.001f;
				//	transform->SetPosition(Vector2(position.x, newYPos));
				//}
			}
			// Check if grounded inverted m_gravity
			else if (m_gravity != 0 && collider->m_b_isCollidingBottom && collider->m_b_bottomCollisionStatic && collider->m_b_bottomCollisionSolid && m_gravity < 0)
			{
				m_b_isGrounded = true;
			}
			else
			{
				m_b_isGrounded = false;
			}

			if (m_b_isGrounded)
			{
				// If grounded and normal m_gravity or inverted m_gravity
				if ((m_pendingForces.y < 0 && m_gravity > 0) || (m_pendingForces.y > 0 && m_gravity < 0))
				{
					m_pendingForces.y = 0;
				}

				if (collider->m_b_isCollidingBottomRight)
				{
					newYPos = collider->m_bottomRightCollidedPosition.y;
					transform->SetPosition(Vector2(position.x, newYPos));
				}
				else if (collider->m_b_isCollidingBottomLeft)
				{
					newYPos = collider->m_bottomLeftCollidedPosition.y;
					transform->SetPosition(Vector2(position.x, newYPos));
				}
				else if (collider->m_b_isCollidingBottom)
				{
					newYPos = collider->m_bottomCollidedPosition.y;
					transform->SetPosition(Vector2(position.x, newYPos));
				}
			}

			// zero gravity collisions
			if (m_pendingForces.y < 0 && collider->m_b_isCollidingBottom && collider->m_b_bottomCollisionSolid)
			{
				Vector2 currentPos = transform->GetPosition();
				m_pendingForces.y = 0;
				newYPos = collider->m_bottomCollision + halfHeight - 0.001f;
				transform->SetPosition(Vector2(currentPos.x, newYPos));
			}
			// "Ceiling" Collision Forces
			if (m_pendingForces.y > 0 && collider->m_b_isCollidingTop && collider->m_b_topCollisionSolid)
			{
				Vector2 currentPos = transform->GetPosition();
				m_pendingForces.y = 0;
				newYPos = collider->m_topCollision - halfHeight + 0.001f;
				transform->SetPosition(Vector2(currentPos.x, newYPos));

				/*if (m_gravity > 0)
				{
					transform->SetPosition(Vector2(position.x, collider->m_topCollidedPosition.y));
				}
				else if (m_gravity < 0)
				{
					newYPos = collider->m_topCollision - halfHeight + 0.001f;
					transform->SetPosition(Vector2(position.x, newYPos));
				}*/
			}
			if (m_pendingForces.x > 0 && collider->m_b_isCollidingRight && collider->m_b_rightCollisionSolid)
			{
				Vector2 currentPos = transform->GetPosition();
				m_pendingForces.x = 0;
				newXPos = collider->m_rightCollision - halfWidth + 0.001f;
				transform->SetPosition(Vector2(newXPos, currentPos.y));
			}
			if (m_pendingForces.x < 0 && collider->m_b_isCollidingLeft && collider->m_b_leftCollisionSolid)
			{
				Vector2 currentPos = transform->GetPosition();
				m_pendingForces.x = 0;
				newXPos = collider->m_leftCollision + halfWidth - 0.001f;
				transform->SetPosition(Vector2(newXPos, currentPos.y));
			}

			// Horizontal Collision Forces
			if (collider->GetTypeString() == "BoxCollider")
			{
				// Collision on right side when moving to the right
				if (collider->m_b_isCollidingRight && collider->m_b_rightCollisionSolid && m_velocity.x > 0)
				{
					m_pendingForces.x = 0;
					transform->SetPosition(Vector2(collider->m_rightCollidedPosition.x + 0.001f, position.y));
				}
				// Collision on left side when moving to the left
				else if (collider->m_b_isCollidingLeft && collider->m_b_leftCollisionSolid && m_velocity.x < 0)
				{
					m_pendingForces.x = 0;
					transform->SetPosition(Vector2(collider->m_leftCollidedPosition.x - 0.001f, position.y));
				}
			}
			// If not BoxCollider and moving left up a corner while not colliding on bottom
			else if (m_velocity.x < 0 && collider->m_b_isCollidingBottomLeft)
			{
				//transform->SetPosition(Vector2(position.x, newYPos));
				transform->SetPosition(collider->m_bottomLeftCollidedPosition);
			}
			// Collision on right side when moving to the right
			else if (collider->m_b_isCollidingRight && collider->m_b_rightCollisionStatic && collider->m_b_rightCollisionSolid && m_velocity.x > 0)
			{
				m_pendingForces.x = 0;
				newXPos = collider->m_rightCollidedPosition.x;
				transform->SetPosition(Vector2(newXPos, position.y));
			}
			// Collision on left side when moving to the left
			else if (collider->m_b_isCollidingLeft && collider->m_b_leftCollisionStatic && collider->m_b_leftCollisionSolid && m_velocity.x < 0)
			{
				m_pendingForces.x = 0;
				newXPos = collider->m_leftCollidedPosition.x;
				transform->SetPosition(Vector2(newXPos, position.y));
			}
		}
	}

	void RigidBody::AddForce(Vector2 force, float multiplier)
	{
		// Normalize the force first, then apply the power factor to the force
		force.Normalize();
		Vector2 addedForce = Vector2(force.x * multiplier, force.y * multiplier);
		m_pendingForces.x += addedForce.x;
		m_pendingForces.y += addedForce.y;		
	}

	void RigidBody::AddTorque(float torque, float multiplier)
	{
		if (m_b_allowTorques)
		{
			float addedTorque = torque * multiplier;
			m_pendingTorques += addedTorque;
		}
	}

	Vector2 RigidBody::GetNextPosition()
	{
		Vector2 nextVelocity = Vector2(m_acceleration.x, m_acceleration.y);
		Transform* transform = GetParent()->GetTransform();
		Vector2 position = transform->GetTruePosition();
		return Vector2(position.x + nextVelocity.x * 2, position.y + nextVelocity.y * 2);
	}

	void RigidBody::SetMass(float mass)
	{
		m_mass = mass;
		m_1overMass = 1 / m_mass;
		m_I = 2* m_mass / 5 * 9;

		// Get new moment of inertia
		if (GetParent() != nullptr)
		{
			Sprite* sprite = GetParent()->GetSprite();
			if (sprite != nullptr)
			{
				float width = (float)sprite->GetTextureWidth() / 10;
				float height = (float)sprite->GetTextureHeight() / 10;
				float radius = (width + height) / 2; // rough estimate
				if (width != 0 && height != 0)
				{
					m_I = 2 * m_mass / 5 * radius * radius;
				}
			}
		}

		m_1overI = 1 / m_I;
	}

	float RigidBody::GetMass()
	{
		return m_mass;
	}

	float RigidBody::GetI()
	{
		return m_I;
	}

	void RigidBody::UpdateI()
	{		
		if (GetParent() != nullptr)
		{
			Sprite* sprite = GetParent()->GetSprite();
			if (sprite != nullptr)
			{
				float width = (float)sprite->GetTextureWidth() / 10;
				float height = (float)sprite->GetTextureHeight() / 10;
				float radius = (width + height) / 2; // rough estimate
				if (width != 0 && height != 0)
				{
					m_I = 2 * m_mass / 5 * radius * radius;
				}
			}
		}

		m_1overI = 1 / m_I;
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
	
	void RigidBody::SetPendingForces(Vector2 pendingForces)
	{
		m_pendingForces = pendingForces;
	}

	void RigidBody::SetAngularVelocity(float angularVelocity)
	{
		m_angularVelocity = angularVelocity;
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

	float RigidBody::GetEquilibriumForce()
	{
		return m_equilibriumForce;
	}
}