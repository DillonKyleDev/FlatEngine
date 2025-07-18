#include "Player.h"
#include "GameObject.h"
#include "RigidBody.h"
#include "Transform.h"
#include "Vector2.h"


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

	}
}