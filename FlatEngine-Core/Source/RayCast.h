#pragma once
#include "Vector2.h"
#include "Body.h"
#include "Vector2.h"

#include <map>


namespace FlatEngine
{
	class RayCast
	{
	public:
		RayCast(Vector2 initialPos, Vector2 direction, float increment, float length, void(*callback)(RayCast*, Body*), long parentID, bool b_visible = false);
		~RayCast();

		void Cast(bool b_visible = false);
		void Update();
		bool IsCasting();
		void OnHit(Body* collidedWith);
		Vector2 GetPoint();
		Vector2 GetTail();
		Body* GetCollidedWith();

	private:
		Vector2 m_initialPos;
		Vector2 m_currentPos;
		Vector2 m_direction;
		float m_increment;
		float m_length;
		bool m_b_casting;
		bool m_b_visible;
		void (*m_callback)(RayCast*, Body*);		
		Body* m_collidedWith;
	};
}