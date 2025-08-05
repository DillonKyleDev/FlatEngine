#include "RayCast.h"
#include "FlatEngine.h"


namespace FlatEngine
{
	RayCast CastRay(Vector2 initialPos, Vector2 direction, float increment, float length, void (*callback)(RayCast*, Body*), long parentID, bool b_visible)
	{
		RayCast newRay = RayCast(initialPos, direction, increment, length, callback, parentID);
		return newRay;
	}


	RayCast::RayCast(Vector2 initialPos, Vector2 direction, float increment, float length, void (*callback)(RayCast*, Body*), long parentID, bool b_visible)
	{
		m_initialPos = initialPos;
		m_currentPos = m_initialPos;
		if (direction == Vector2(0, 0))
		{
			direction = Vector2(1, 1);
		}
		m_direction = Vector2::Normalize(direction);
		m_increment = increment;
		m_length = length;
		m_b_casting = false;
		m_b_visible = b_visible;
		m_callback = callback;		
		m_collidedWith = nullptr;
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
		if (m_b_casting)
		{
			if (m_b_visible)
			{
				AddLineToScene(m_initialPos, m_currentPos, GetColor("rayCast"), 1);
			}

			Vector2 difference = m_currentPos - m_initialPos;

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

	void RayCast::OnHit(Body* collidedWith)
	{
		m_b_casting = false;
		m_callback(this, collidedWith);
		m_collidedWith = collidedWith;		
	}

	Vector2 RayCast::GetPoint()
	{
		return m_currentPos;
	}

	Vector2 RayCast::GetTail()
	{
		return m_initialPos;
	}

	Body* RayCast::GetCollidedWith()
	{
		return m_collidedWith;
	}
}