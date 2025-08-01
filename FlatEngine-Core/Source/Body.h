#pragma once
#include "Component.h"
#include "FlatEngine.h"
#include "Physics.h"
#include "Vector2.h"

#include <vector>


namespace FlatEngine
{
	class Body : public Component
	{
		friend class BoxBody;
		friend class CircleBody;
		friend class CapsuleBody;
		friend class PolygonBody;

	public:
		Body(long myID = -1, long parentID = -1);
		~Body();
		
		Physics::BodyProps GetLiveProps();
		b2BodyId GetBodyID();
		std::vector<b2ShapeId> GetShapeIDs();
		void AddShapeID(b2ShapeId shapeID);
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
		std::vector<b2ShapeId> m_shapeIDs;
		Physics::BodyProps m_bodyProps;		
	};
}
