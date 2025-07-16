#include "Player.h"
#include "GameObject.h"
#include "RigidBody.h"
#include "Transform.h"
#include "Vector2.h"


namespace FlatEngine
{
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
		m_initialXPos = GetRandInt(0, 10);
		GetParent()->GetTransform()->SetPosition(Vector2(m_initialXPos, 0));
	}

	void Player::Update()
    {
		Vector2 currentPos = GetParent()->GetTransform()->GetPosition();
		GetParent()->GetTransform()->SetPosition(Vector2((float)currentPos.x + 0.01, 0));
	}
}