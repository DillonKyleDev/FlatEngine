#include "Player.h"
#include "GameObject.h"
#include "Transform.h"
#include "Vector2.h"
#include "Body.h"


namespace FlatEngine
{
	void SensorCallbackFunction(b2ShapeId myID, b2ShapeId collidedWithID)
	{
		GameObject* self = Body::GetBodyFromShapeID(myID)->GetParent();
		GameObject* collidedWith = Body::GetBodyFromShapeID(collidedWithID)->GetParent();
		LogString("Sensor began touching from C++");
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
		m_body = GetParent()->GetBody();
		m_body->SetOnSensorBeginTouch(SensorCallbackFunction);
	}

	void Player::Update()
    {
		if (m_context->ActionPressed("IA_MoveRight"))
		{
			LogString("Right");
			m_body->ApplyForceToCenter(Vector2(100, 0));
		}
		if (m_context->ActionPressed("IA_MoveLeft"))
		{
			LogString("Left");
			m_body->ApplyForceToCenter(Vector2(-100, 0));
		}
		if (m_context->Fired("IA_Jump"))
		{
			LogString("Jump");
			m_body->ApplyForceToCenter(Vector2(0, 5000));
		}
	}
}