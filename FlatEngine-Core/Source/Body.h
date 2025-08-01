#pragma once
#include "Component.h"
#include "FlatEngine.h"
#include "Physics.h"
#include "Vector2.h"


namespace FlatEngine
{
	class Body : public Component
	{
		friend class BoxBody;

	public:
		Body(long myID = -1, long parentID = -1);
		~Body();
		
		b2BodyId GetBodyID();
		void SetPosition(Vector2 position);
		Vector2 GetPosition();
		void SetRotation(float rotation);
		float GetRotation();
		void SetBodyType(b2BodyType type);
		void SetDensity(float mass);
		void SetFriction(float friction);
		void RecreateBody();
		Physics::BodyProps GetBodyProps();

	private:
		b2BodyId m_bodyID;
		Physics::BodyProps m_bodyProps;		
	};
}
