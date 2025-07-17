#include "Player.h"
#include "GameObject.h"
#include "RigidBody.h"
#include "Transform.h"
#include "Vector2.h"


namespace FlatEngine
{
	void CallbackFunction(RayCast* rayCast, GameObject* collidedWith)
	{
		Vector2 forceDirection = (rayCast->GetPoint() - rayCast->GetTail());
		Vector2 impactToCenter = (collidedWith->GetTransform()->GetTruePosition() - rayCast->GetPoint()).Normalize();
		float forceInDirectionOfCenter = forceDirection.Dot(impactToCenter);
		float torque = forceDirection.GetMagnitude() - forceInDirectionOfCenter;
		collidedWith->GetRigidBody()->AddTorque(torque, 1);
		collidedWith->GetRigidBody()->AddForce(impactToCenter, forceInDirectionOfCenter);
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
		CastRay(GetParent()->GetTransform()->GetPosition(), Vector2(1, 0), 1, 10, CallbackFunction, true);
	}

	void Player::Update()
    {

	}
}