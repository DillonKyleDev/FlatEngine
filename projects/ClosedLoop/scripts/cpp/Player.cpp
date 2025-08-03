#include "Player.h"
#include "GameObject.h"
#include "Transform.h"
#include "Vector2.h"
#include "CapsuleBody.h"
#include "Body.h"


namespace FlatEngine
{
	void CallbackFunction(b2Manifold manifold, b2ShapeId myID, b2ShapeId collidedWithID)
	{
		GameObject* self = static_cast<Body*>(b2Shape_GetUserData(myID))->GetParent();
		GameObject* collidedWith = static_cast<Body*>(b2Shape_GetUserData(collidedWithID))->GetParent();		
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
		m_capsule->SetOnBeginContact(CallbackFunction);
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