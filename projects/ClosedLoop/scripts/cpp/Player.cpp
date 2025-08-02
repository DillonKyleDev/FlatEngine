#include "Player.h"
#include "GameObject.h"
#include "Transform.h"
#include "Vector2.h"
#include "CapsuleBody.h"


namespace FlatEngine
{
	void CallbackFunction(RayCast* rayCast, Collider* collidedWith)
	{

	}


	Player::Player()
	{

	}

	Player::~Player()
	{

	}

	void Player::Awake()
	{

	}

	void Player::Start()
	{		
		m_context = GetMappingContext("MC_Player");
		m_capsule = GetParent()->GetCapsuleBody();
		LogString("Player script started..");

		//RayCast* rayCast = CastRay(GetParent()->GetTransform()->GetPosition(), Vector2(1, 0), 1, 10, CallbackFunction, GetParent()->GetID(), true);
		//std::vector<GameObject*> collidingObjects = rayCast->GetCollidingObjects();
		//Vector2 forceDirection = (rayCast->GetPoint() - rayCast->GetTail());
		//Vector2 impactToCenter = (collidingObjects[0]->GetTransform()->GetTruePosition() - rayCast->GetPoint()).Normalize();
		//float forceInDirectionOfCenter = forceDirection.Dot(impactToCenter);
		//float torque = forceDirection.GetMagnitude() - forceInDirectionOfCenter;
		//float direction = 1;
		//if (forceDirection.CrossKResult(impactToCenter) > 0)
		//{
		//	direction = -1;
		//}
		//collidingObjects[0]->GetRigidBody()->AddTorque(torque, direction);
		//collidingObjects[0]->GetRigidBody()->AddForce(impactToCenter, forceInDirectionOfCenter);
	}

	void Player::Update()
    {
		if (m_context->ActionPressed("IA_MoveRight"))
		{
			LogString("Right");
			m_capsule->ApplyForceToCenter(Vector2(100, 0));
		}
		if (m_context->ActionPressed("IA_MoveLeft"))
		{
			LogString("Left");
			m_capsule->ApplyForceToCenter(Vector2(-100, 0));
		}
		if (m_context->Fired("IA_Jump"))
		{
			LogString("Jump");
			m_capsule->ApplyForceToCenter(Vector2(0, 5000));
		}
	}
}