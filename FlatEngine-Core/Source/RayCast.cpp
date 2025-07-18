#include "RayCast.h"
#include "FlatEngine.h"
#include "Scene.h"


namespace FlatEngine
{
	RayCast CastRay(Vector2 initialPos, Vector2 direction, float increment, float length, void (*callback)(RayCast*, Collider*), long parentID, bool b_visible)
	{
		RayCast newRay = RayCast(initialPos, direction, increment, length, callback, parentID);
		RayCast* rayPtr = GetLoadedScene()->AddRayCast(newRay, newRay.GetID());		
		rayPtr->Cast(b_visible);
		newRay = RayCast(*rayPtr);
		GetLoadedScene()->RemoveComponent(rayPtr, newRay.GetID());
		return newRay;
	}


	RayCast::RayCast(Vector2 initialPos, Vector2 direction, float increment, float length, void (*callback)(RayCast*, Collider*), long parentID, bool b_visible)
	{
		SetID(GetNextComponentID());
		SetType(T_RayCast);
		SetParentID(parentID);
		m_initialPos = initialPos;
		m_currentPos = m_initialPos;
		m_direction = direction.Normalize();
		m_increment = increment;
		m_length = length;
		m_b_casting = false;
		m_b_visible = b_visible;
		m_callback = callback;
		SetActiveRadiusGrid(1);		
		m_collidedWith = nullptr;
	}

	RayCast::~RayCast()
	{
		
	}

	void RayCast::Cast(bool b_visible)
	{
		m_b_casting = true;
		m_b_visible = b_visible;

		std::map<long, std::map<long, BoxCollider>>& sceneBoxColliders = GetLoadedScene()->GetBoxColliders();
		for (std::map<long, std::map<long, BoxCollider>>::iterator outerIter = sceneBoxColliders.begin(); outerIter != sceneBoxColliders.end();)
		{
			for (std::map<long, BoxCollider>::iterator innerIter = outerIter->second.begin(); innerIter != outerIter->second.end();)
			{
				if (innerIter->second.GetParentID() != this->GetParentID())
				{
					while (m_b_casting)
					{
						CheckForCollisionBoxRayCastSAT(&innerIter->second, this);
						Update();
					}
				}

				innerIter++;
			}
			outerIter++;
		}
	}

	void RayCast::Update()
	{
		if (m_b_casting)
		{
			if (m_b_visible)
			{
				DrawLineInScene(m_initialPos, m_currentPos, GetColor("rayCast"), 1);
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
		m_b_casting = false;

		if (AddCollidingObject(collidedWith))
		{
			m_callback(this, collidedWith);
			m_collidedWith = collidedWith;
		}
	}

	Vector2 RayCast::GetPoint()
	{
		return m_currentPos;
	}

	Vector2 RayCast::GetTail()
	{
		return m_initialPos;
	}

	Collider* RayCast::GetCollidedWith()
	{
		return m_collidedWith;
	}
}