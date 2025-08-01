#pragma once
#include "box2d.h"
#include "Vector2.h"

#include <vector>


namespace FlatEngine
{
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
			BodyShape shape = BS_None;
			b2BodyType type = b2_staticBody;
			Vector2 position = Vector2(0,0);	
			float rotation = 0;
			Vector2 dimensions = Vector2(1.0f, 1.0f);
			float radius = 1.0f;
			float capsuleLength = 4.0f;
			std::vector<Vector2> points = std::vector<Vector2>();
			float density = 1;
			float friction = 0.3f;		
		};

		Physics();
		~Physics();

		void Init();
		void Shutdown();
		void Update(float deltaTime);

		void CreateBody(BodyProps bodyProps, b2BodyId& bodyID, std::vector<b2ShapeId>& shapeIDs);
		void CreateBox(BodyProps bodyProps, b2BodyId& bodyID);
		void DestroyBody(b2BodyId bodyID);
		void RecreateBody(BodyProps bodyProps, b2BodyId& bodyID, std::vector<b2ShapeId> shapeIDs);

	private:
		b2WorldId m_worldID;
	};
}
