#pragma once
#include "TagList.h"
#include "tools/Vector2.h"

#include "box2d.h"

namespace FL = FlatEngine;


namespace FlatEngine
{	
	class Body;
	class Shape;
	class Joint;

	namespace PhysicsManager
	{
		struct BodyProps {
			b2BodyType type = b2_dynamicBody;
			FL::Vector2 position = FL::Vector2();	
			b2Rot rotation = b2MakeRot(0);
			bool b_lockedRotation = false;
			bool b_lockedXAxis = false;
			bool b_lockedYAxis = false;
			float gravityScale = 1.0f;
			float linearDamping = 0.0f;
			float angularDamping = 0.0f;
		};

		class Physics
		{
		public:
			Physics();			

			static bool CanCollide(TagList tagList1, TagList tagList2);

			void Init();
			void Shutdown();
			void Update(float deltaTime);
			void HandleCollisions();

			void CreateBody(FL::Body* parentBody);
			void DestroyBody(b2BodyId bodyID);
			void RecreateBody(FL::Body* parentBody);

			void CreateShape(Shape* shape);
			void CreateBodyShape(FL::Body* parentBody, FL::Shape* shape);
			void DestroyShape(b2ShapeId shapeID);
			void RecreateShape(FL::Shape* shape);

			void CreateJoint(FL::Body* bodyA, FL::Body* bodyB, FL::Joint* joint);
			void DestroyJoint(b2JointId jointID);
			void RecreateJoint();

		private:
			b2WorldId m_worldID;
		};

		extern Physics physics;
	}
}
