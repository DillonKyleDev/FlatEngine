#pragma once
#include "Collider.h"
#include "Vector2.h"
#include <map>


namespace FlatEngine
{
	class RayCast : public Collider
	{
	public:
		RayCast(Vector2 initialPos, Vector2 direction, float increment, float length, void(*callback)(RayCast*, Collider*), long parentID, bool b_visible = false);
		~RayCast();

		void Cast(bool b_visible = false);
		void Update();
		bool IsCasting();
		void OnHit(Collider* collidedWith);
		Vector2 GetPoint();
		Vector2 GetTail();
		Collider* GetCollidedWith();

	private:
		Vector2 m_initialPos;
		Vector2 m_currentPos;
		Vector2 m_direction;
		float m_increment;
		float m_length;
		bool m_b_casting;
		bool m_b_visible;
		void (*m_callback)(RayCast*, Collider*);		
		Collider* m_collidedWith;
	};
}