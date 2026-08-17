#pragma once
#include "TagList.h"
#include "tools/Vector4.h"
#include "tools/Vector2.h"

#include <list>
#include <types.h>

namespace FL = FlatEngine;


namespace FlatEngine
{	
	class Body;
	class Body2D;
	class Shape2D;
	class Joint2D;

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
			void CreateBody(FL::Body2D* body);
			void DestroyBody(b2BodyId bodyID);
			void RecreateBody(FL::Body2D* body);

			void CreateShape(FL::Shape2D* shape, FL::Body2D* body = nullptr);
			void DestroyShape(FL::Shape2D* shape);
			void RecreateShape(FL::Shape2D* shape);

			void CreateJoint(FL::Joint2D* joint, FL::Body2D* bodyA = nullptr, FL::Body2D* bodyB = nullptr);
			void DestroyJoint(FL::Joint2D* joint);
			void RecreateJoint(FL::Joint2D* joint);

			b2WorldId m_worldID;
			
		private:
			std::list<Joint2D> m_distanceJoints;
			std::list<Joint2D> m_prismaticJoints;
			std::list<Joint2D> m_revoluteJoints;
			std::list<Joint2D> m_mouseJoints;
			std::list<Joint2D> m_wheelJoints;
			std::list<Joint2D> m_motorJoints;
			std::list<Joint2D> m_weldJoints;  
			void HandleCollisions();			
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

			// void CreateBody(FL::Body* body);
			// void DestroyBody(b2BodyId bodyID);
			// void RecreateBody(FL::Body* body);

			// void CreateShape(FL::Shape2D* shape, FL::Body2D* body = nullptr);
			// void DestroyShape(b2ShapeId shapeID);
			// void RecreateShape(FL::Shape2D* shape);

			// void CreateJoint(FL::Body* bodyA, FL::Body* bodyB, FL::Joint* joint);
			// void DestroyJoint(b2JointId jointID);

		private:
			// b2WorldId m_worldID;
		};

		// 2D Physics for Scene
		extern Physics2D gamePhysics2D;
	}
}
