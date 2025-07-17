#include "RayCast.h"
#include "FlatEngine.h"
#include "Scene.h"


namespace FlatEngine
{
	RayCast* CastRay(Vector2 initialPos, Vector2 direction, float increment, float length, void (*callback)(RayCast*, GameObject*), bool b_visible)
	{
		RayCast newRay = RayCast(initialPos, direction, increment, length, callback);
		newRay.Cast(b_visible);
		RayCast* ray = GetLoadedScene()->AddRayCast(newRay, newRay.GetID());		
		UpdateColliderPairs();
		return ray;
	}


	RayCast::RayCast(Vector2 initialPos, Vector2 direction, float increment, float length, void (*callback)(RayCast*, GameObject*), bool b_visible)
	{
		SetID(GetNextComponentID());
		SetType(T_RayCast);
		m_initialPos = initialPos;
		m_currentPos = m_initialPos;
		m_direction = direction.Normalize();
		m_increment = increment;
		m_length = length;
		m_collidedWith = nullptr;
		m_b_casting = false;
		m_b_visible = b_visible;
		m_callback = callback;
		SetActiveRadiusGrid(1);
	}

	RayCast::~RayCast()
	{

	}

	void RayCast::Cast(bool b_visible)
	{
		m_b_casting = true;
		m_b_visible = b_visible;
	}

	void RayCast::Update()
	{
		SetCenterGrid(m_currentPos);

		if (m_b_casting)
		{
			if (m_b_visible)
			{
				DrawLineInScene(m_initialPos, m_currentPos, GetColor("rayCast"), 2);
			}

			Vector2 difference = Vector2(m_currentPos.x - m_initialPos.x, m_currentPos.y - m_initialPos.y);

			if (difference.GetMagnitude() < m_length)
			{
				m_currentPos.x += (m_direction.x * m_increment);
				m_currentPos.y += (m_direction.y * m_increment);
			}
			else
			{
				m_b_casting = false;
			}
		}
	}

	bool RayCast::IsCasting()
	{
		return m_b_casting;
	}

	void RayCast::OnHit(Collider* collidedWith)
	{
		AddCollidingObject(collidedWith);
		m_callback(this, collidedWith->GetParent());
		m_b_casting = false;
	}

	Vector2 RayCast::GetPoint()
	{
		return m_currentPos;
	}
	Vector2 RayCast::GetTail()
	{
		return m_initialPos;
	}
}