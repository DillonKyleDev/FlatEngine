#pragma once
#include "box2d.h"
#include "Vector2.h"

#include <vector>


namespace FlatEngine
{
	class GameObject;
	class Body;

	class Physics
	{
	public:

		enum BodyShape {
			BS_None,
			BS_Box,
			BS_Circle,
			BS_Capsule,
			BS_Polygon
		};
		struct BodyProps {
			bool b_isEnabled = true;
			bool b_enableContactEvents = true;			
			BodyShape shape = BS_None;
			b2BodyType type = b2_dynamicBody;
			Vector2 position = Vector2(0,0);	
			b2Rot rotation = b2MakeRot(0);
			bool b_lockedRotation = false;
			bool b_lockedXAxis = false;
			bool b_lockedYAxis = false;
			float gravityScale = 1.0f;
			float linearDamping = 0.0f;
			float angularDamping = 0.0f;
			float restitution = 0.3f;
			float density = 1.0f;
			float friction = 0.3f;
			Vector2 dimensions = Vector2(1.0f, 1.0f);
			float radius = 1.0f;
			float capsuleLength = 4.0f;
			bool b_horizontal = false;
			std::vector<Vector2> points = std::vector<Vector2>();
		};

		Physics();
		~Physics();

		void Init();
		void Shutdown();
		void Update(float deltaTime);
		void HandleCollisions();

		void CreateBody(Body* parentBody, BodyProps bodyProps, b2BodyId& bodyID, std::vector<b2ShapeId>& shapeIDs);
		void DestroyBody(b2BodyId bodyID, std::vector<b2ShapeId>& shapeIDs);
		void RecreateBody(Body* parentBody, BodyProps bodyProps, b2BodyId& bodyID, std::vector<b2ShapeId>& shapeIDs);

	private:
		b2WorldId m_worldID;
	};
}
