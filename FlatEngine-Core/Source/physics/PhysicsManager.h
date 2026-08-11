#pragma once
#include "TagList.h"
#include "tools/Vector4.h"
#include "tools/Vector2.h"

#include <types.h>

namespace FL = FlatEngine;


namespace FlatEngine
{	
	class Body;
	class Body2D;
	class Shape;
	class Joint;

	namespace PhysicsManager
	{
		class Physics2D
		{
		public:
			Physics2D();			

			static bool CanCollide(TagList tagList1, TagList tagList2);
			static bool AreCollidingViewport(Vector4 ObjectA, Vector4 ObjectB);
			static b2CastOutput CastRay(Vector2 initialPos, Vector2 direction, float increment, TagList tagList, Body& hit, bool b_visible = false);

			void Init();
			void Shutdown();
			void Update(float deltaTime);

			Body2D* GetBodyFromShapeID(b2ShapeId shapeID);
			void CreateBody(FL::Body2D* parentBody);
			void DestroyBody(b2BodyId bodyID);
			void RecreateBody(FL::Body2D* parentBody);

			void CreateShape(FL::Shape* shape, FL::Body2D* parentBody = nullptr);
			void DestroyShape(FL::Shape* shape);
			void RecreateShape(FL::Shape* shape);

			void CreateJoint(FL::Joint* joint, FL::Body2D* bodyA = nullptr, FL::Body2D* bodyB = nullptr);
			void DestroyJoint(FL::Joint* joint);
			void RecreateJoint(FL::Joint* joint);

		private:
			void HandleCollisions();

			b2WorldId m_worldID;
		};

		class Physics
		{
		public:
			// Physics();			

			// static bool CanCollide(TagList tagList1, TagList tagList2);
			// static bool AreCollidingViewport(Vector4 ObjectA, Vector4 ObjectB);
			// static b2CastOutput CastRay(Vector2 initialPos, Vector2 direction, float increment, TagList tagList, Body& hit, bool b_visible = false);

			// void Init();
			// void Shutdown();
			// void Update(float deltaTime);
			// void HandleCollisions();

			// void CreateBody(FL::Body* parentBody);
			// void DestroyBody(b2BodyId bodyID);
			// void RecreateBody(FL::Body* parentBody);

			// void CreateShape(FL::Shape* shape, FL::Body2D* parentBody = nullptr);
			// void DestroyShape(b2ShapeId shapeID);
			// void RecreateShape(FL::Shape* shape);

			// void CreateJoint(FL::Body* bodyA, FL::Body* bodyB, FL::Joint* joint);
			// void DestroyJoint(b2JointId jointID);

		private:
			// b2WorldId m_worldID;
		};

		// 2D Physics for Scene
		extern Physics2D physics2D;
	}
}
