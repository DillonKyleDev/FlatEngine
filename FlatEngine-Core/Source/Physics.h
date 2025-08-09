#pragma once
#include "box2d.h"
#include "Vector2.h"
#include "Shape.h"

#include <vector>


namespace FlatEngine
{
	class GameObject;
	class Body;
	class Shape;

	class Physics
	{
	public:

		struct BodyProps {
			b2BodyType type = b2_dynamicBody;
			Vector2 position = Vector2(0,0);	
			b2Rot rotation = b2MakeRot(0);
			bool b_lockedRotation = false;
			bool b_lockedXAxis = false;
			bool b_lockedYAxis = false;
			float gravityScale = 1.0f;
			float linearDamping = 0.0f;
			float angularDamping = 0.0f;
		};

		Physics();
		~Physics();

		void Init();
		void Shutdown();
		void Update(float deltaTime);
		void HandleCollisions();

		void CreateBody(Body* parentBody);
		void DestroyBody(b2BodyId bodyID);
		void RecreateBody(Body* parentBody);

		void CreateShape(Body* parentBody, Shape* shape);
		void DestroyShape(b2ShapeId shapeID);
		void RecreateShape(Shape* shape);

	private:
		b2WorldId m_worldID{};
	};
}
