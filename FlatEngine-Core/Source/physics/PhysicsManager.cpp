
// #include "components/Body.h"
#include "components/Body2D.h"
#include "GameObject.h"
#include "components/Transform.h"
#include "managers/SceneManager.h"
#include "physics/PhysicsManager.h"
#include "physics/Joint2D.h"
#include "physics/Shape2D.h"
#include "render/SceneView.h"
#include "TagList.h"
#include "tools/Logger.h"
#include <box2d.h>
#include <id.h>


namespace FlatEngine
{
	namespace PhysicsManager
	{
		// Physics physics = Physics();
		Physics2D gamePhysics2D = Physics2D();
		// std::vector<std::pair<Collider*, Collider*>> F_ColliderPairs = std::vector<std::pair<Collider*, Collider*>>();

		Physics2D::Physics2D()
		{
			m_worldID = b2_nullWorldId;
		}

		// RayCast will only be visible for the frame of the cast if b_visible = true
		b2CastOutput CastRay(Vector2 initialPos, Vector2 direction, float increment, TagList tagList, Body2D& hit, bool b_visible)
		{
			if (b_visible)
			{
				Vector3 start = Vector3(initialPos.x, initialPos.y, 0);
				Vector2 endPos2 = (initialPos + direction) * 10;
				Vector3 endPos = Vector3(endPos2.x, endPos2.y, 0);
				SceneView::DebugDrawLine(start, endPos, "rayCast");
			}

			for (Body2D& body : SceneManager::loadedScene.GetAll<Body2D>().GetAll())
			{
				body.GetOwningObject()->GetTagList().UpdateBits();
				if (PhysicsManager::gamePhysics2D.CanCollide(tagList, body.GetOwningObject()->GetTagList()))
				{
					for (Shape2D* shape : body.GetShapes())
					{
						b2RayCastInput input = { 0 };
						input.origin = Vector2::GetB2Vec2(initialPos);
						input.translation = Vector2::GetB2Vec2(direction);
						input.maxFraction = increment;

						b2CastOutput output = shape->CastRayAt(&input);

						if (output.hit)
						{
							hit = body;
							return output;
						}
					}
				}
			}

			return b2CastOutput();
		}

		bool Physics2D::CanCollide(TagList tagList1, TagList tagList2)
		{
			uint64_t catA = tagList1.GetCategoryBits();
			uint64_t maskA = tagList1.GetMaskBits();
			uint64_t catB = tagList2.GetCategoryBits();
			uint64_t maskB = tagList2.GetMaskBits();

			return ((catA & maskB) != 0 && (catB & maskA) != 0);
		}

		void Physics2D::Init()
		{
			b2WorldDef worldDef = b2DefaultWorldDef();
			worldDef.gravity = b2Vec2{ 0.0f, -10.0f };
			m_worldID = b2CreateWorld(&worldDef);	
			// b2World_EnableSleeping(m_worldID, false);
		}

		void Physics2D::Shutdown()
		{
			if (b2World_IsValid(m_worldID))
				b2DestroyWorld(m_worldID);
		}

		void Physics2D::Update(float deltaTime)
		{
			float timeStep = 1.0f / 60.0f;
			int substepCount = 4;

			b2World_Step(m_worldID, timeStep, substepCount);
			HandleCollisions();			
		}

		void Physics2D::HandleCollisions()
		{
			b2ContactEvents contactEvents = b2World_GetContactEvents(m_worldID);
			b2SensorEvents sensorEvents = b2World_GetSensorEvents(m_worldID);

			// // Contacts
			// for (int i = 0; i < contactEvents.beginCount; ++i)
			// {
			// 	b2ContactBeginTouchEvent* beginEvent = contactEvents.beginEvents + i;	
			// 	b2Manifold manifold = b2Contact_GetData(beginEvent->contactId).manifold;	
			// 	if (GetBodyFromShapeID(beginEvent->shapeIdA) != nullptr)
			// 		GetBodyFromShapeID(beginEvent->shapeIdA)->OnBeginContact(manifold, beginEvent->shapeIdA, beginEvent->shapeIdB);				 	
			// 	if (GetBodyFromShapeID(beginEvent->shapeIdB))
			// 		GetBodyFromShapeID(beginEvent->shapeIdB)->OnBeginContact(manifold, beginEvent->shapeIdB, beginEvent->shapeIdA);
			// 	else
			// 	 	Logger::log.Trace("Body not found from shapeID: {}, {}. Suspected Chain, must add userdata to each link in the chain.", beginEvent->shapeIdB.index1, beginEvent->shapeIdB.generation);
			// }
			// for (int i = 0; i < contactEvents.endCount; ++i)
			// {
			// 	b2ContactEndTouchEvent* endEvent = contactEvents.endEvents + i;
			// 	if (b2Shape_IsValid(endEvent->shapeIdA) && b2Shape_IsValid(endEvent->shapeIdB))
			// 	{
			// 		if (GetBodyFromShapeID(endEvent->shapeIdA) != nullptr)
			// 			GetBodyFromShapeID(endEvent->shapeIdA)->OnEndContact(endEvent->shapeIdA, endEvent->shapeIdB);
			// 		else
			// 			Logger::log.Trace("Body not found from shapeID: {}, {}. Suspected Chain, must add userdata to each link in the chain.", endEvent->shapeIdA.index1, endEvent->shapeIdA.generation);
			// 		if (GetBodyFromShapeID(endEvent->shapeIdB))
			// 			GetBodyFromShapeID(endEvent->shapeIdB)->OnEndContact(endEvent->shapeIdB, endEvent->shapeIdA);
			// 		else
			// 			Logger::log.Trace("Body not found from shapeID: {}, {}. Suspected Chain, must add userdata to each link in the chain.", endEvent->shapeIdB.index1, endEvent->shapeIdB.generation);
			// 	}
			// }

			// // Sensors
			// for (int i = 0; i < sensorEvents.beginCount; ++i)
			// {
			// 	b2SensorBeginTouchEvent* beginTouch = sensorEvents.beginEvents + i;
			// 	GetBodyFromShapeID(beginTouch->sensorShapeId)->OnSensorBeginTouch(beginTouch->sensorShapeId, beginTouch->visitorShapeId);
			// 	GetBodyFromShapeID(beginTouch->visitorShapeId)->OnSensorBeginTouch(beginTouch->visitorShapeId, beginTouch->sensorShapeId);
			// }
			// for (int i = 0; i < sensorEvents.endCount; ++i)
			// {
			// 	b2SensorEndTouchEvent* endTouch = sensorEvents.endEvents + i;
			// 	if (b2Shape_IsValid(endTouch->visitorShapeId))
			// 	{
			// 		GetBodyFromShapeID(endTouch->sensorShapeId)->OnSensorEndTouch(endTouch->sensorShapeId, endTouch->visitorShapeId);
			// 		GetBodyFromShapeID(endTouch->visitorShapeId)->OnSensorEndTouch(endTouch->visitorShapeId, endTouch->sensorShapeId);
			// 	}
			// }

			// // Hit events
			// for (int i = 0; i < contactEvents.hitCount; ++i)
			// {
			// 	float hitSpeedForSound = 10.0f;

			// 	b2ContactHitEvent* hitEvent = contactEvents.hitEvents + i;
			// 	if (hitEvent->approachSpeed > hitSpeedForSound)
			// 	{
			// 		// Play sound, etc..
			// 	}
			// }
		}

		Body2D* Physics2D::GetBodyFromShapeID(b2ShapeId shapeID)
		{
			Shape2D* shape = static_cast<Shape2D*>(b2Shape_GetUserData(shapeID));
			if (shape != nullptr && b2Shape_IsValid(shape->GetShapeID()))
			{				
				return SceneManager::loadedScene.Get<Body2D>(shape->GetOwnerID());
			}
			else
			{
				Logger::log.Debug("user data not found for: index: {}, world: {}, generation: {}", shapeID.index1, shapeID.world0, shapeID.generation);
				return nullptr;
			}
		}

		void Physics2D::CreateBody(Body2D* body)
		{
			Transform* transform = SceneManager::loadedScene.Get<Transform>(body->GetOwnerID());
			// Vector3 transformPos = transform->GetCleanPosition();
			b2BodyDef bodyDef = b2DefaultBodyDef();			
			bodyDef.isEnabled = body->IsActive();
			bodyDef.isAwake = true;
			bodyDef.enableSleep = true;
			bodyDef.userData = body; //reinterpret_cast<void*>(body->GetOwnerID());
			bodyDef.position = b2Vec2(body->position.x, body->position.y);
			bodyDef.rotation = body->rotation;
			b2MotionLocks motionLocks;		
			motionLocks.angularZ = body->b_lockedRotation;
			motionLocks.linearX = body->b_lockedXAxis;
			motionLocks.linearY = body->b_lockedYAxis;
			bodyDef.motionLocks = motionLocks;
			bodyDef.gravityScale = body->gravityScale;
			bodyDef.linearDamping = body->linearDamping;
			bodyDef.angularDamping = body->angularDamping;
			bodyDef.type = body->type;		
			b2BodyId bodyID = b2CreateBody(m_worldID, &bodyDef);
			body->SetBodyID(bodyID);
		}

		void Physics2D::CreateShape(Shape2D* shape, Body2D* body)
		{
			b2BodyId bodyID = body != nullptr ? body->GetBodyID() : b2_nullBodyId;
			b2ShapeDef shapeDef = b2DefaultShapeDef();
			shapeDef.userData = shape;
			shapeDef.enableContactEvents = shape->b_enableContactEvents;
			shapeDef.enableSensorEvents = shape->b_enableSensorEvents;
			shapeDef.isSensor = shape->b_isSensor;
			shapeDef.density = shape->density;
			shapeDef.material.friction = shape->friction;
			shapeDef.material.restitution = shape->restitution;		

			b2Filter filter = b2DefaultFilter();
			filter.categoryBits = 0;
			filter.maskBits = 0;
			TagList tagList = body != nullptr ? body->GetOwningObject()->GetTagList() : TagList();
			tagList.UpdateBits();
			filter.categoryBits = tagList.GetCategoryBits();
			filter.maskBits = tagList.GetMaskBits();
			shapeDef.filter = filter;

			b2SurfaceMaterial material = b2DefaultSurfaceMaterial();
			material.friction = shape->friction;
			material.restitution = shape->restitution;
			material.rollingResistance = shape->rollingResistance;
			material.tangentSpeed = shape->tangentSpeed;

			std::visit([body, bodyID, &shapeDef, filter, material, shape](auto&& sData) -> void
			{
				using T = std::decay_t<decltype(sData)>;
				if constexpr (std::is_same_v<T, BoxShape2DData>)
				{	
					b2Rot rotationOffset = sData.rotationOffset;
					b2Vec2 center = b2Vec2(sData.offset.x, sData.offset.y);
					shape->polygon = b2MakeOffsetRoundedBox(sData.dimensions.x / 2, sData.dimensions.y / 2, center, rotationOffset, sData.cornerRadius);

					if (body != nullptr)	
					{	
						b2ShapeId id = b2CreatePolygonShape(bodyID, &shapeDef, &shape->polygon);
						shape->SetShapeID(id);	
					}
				}
				else if constexpr (std::is_same_v<T, CircleShape2DData>)
				{
					b2Circle circle;
					b2Vec2 center = b2Vec2(sData.offset.x, sData.offset.y);
					circle.center = center;
					circle.radius = sData.radius;
					shape->circle = circle;

					if (body != nullptr)	
					{	
						b2ShapeId id = b2CreateCircleShape(bodyID, &shapeDef, &shape->circle);						
						shape->SetShapeID(id);	
					}
				}
				else if constexpr (std::is_same_v<T, PolygonShape2DData>)
				{
					std::vector<b2Vec2> points;
					float cornerRadius = sData.cornerRadius;

					for (Vector2 point : sData.points)
					{
						points.push_back(b2Vec2(point.x, point.y));
					}

					if (points.size() > 0)
					{
						b2Hull hull = b2ComputeHull(&points[0], (int)points.size());

						if (hull.count == 0)
						{
							Logger::log.Err("Hull not successfully created.");
						}
						else
						{					
							shape->polygon = b2MakePolygon(&hull, cornerRadius);	

							if (body != nullptr)				
								shape->SetShapeID(b2CreatePolygonShape(bodyID, &shapeDef, &shape->polygon));									
						}
					}
				}
				else if constexpr (std::is_same_v<T, CapsuleShape2DData>)
				{
					b2Capsule capsule;
					b2Vec2 center = b2Vec2(sData.offset.x, sData.offset.y);
					float center1Value = ((sData.length / 2) - sData.radius) * -1;
					float center2Value = (sData.length / 2) - sData.radius;
					b2Vec2 center1 = b2Vec2(0, 0);
					b2Vec2 center2 = b2Vec2(0, 0);

					if (sData.b_horizontal)
					{
						center1.x = center1Value;
						center2.x = center2Value;
					}
					else
					{
						center1.y = center1Value;
						center2.y = center2Value;
					}

					capsule.center1 = center1 + center;
					capsule.center2 = center2 + center;
					capsule.radius = sData.radius;
					shape->capsule = capsule;

					if (body != nullptr)				
						shape->SetShapeID(b2CreateCapsuleShape(bodyID, &shapeDef, &shape->capsule));		
				}
				else if constexpr (std::is_same_v<T, ChainShape2DData>)
				{
					b2ChainDef chainDef = b2DefaultChainDef();
					chainDef.userData = shape;
					chainDef.filter = filter;
					chainDef.enableSensorEvents = shape->b_enableSensorEvents;
					chainDef.isLoop = sData.b_isLoop;
					chainDef.materialCount = 1;
					chainDef.materials = &material;

					std::vector<b2Vec2> points;
					for (Vector2 point : sData.points)
					{
						points.push_back(b2Vec2(point.x, point.y));
					}

					chainDef.points = &points[0];
					chainDef.count = (int)points.size();

					b2ChainId chainID = b2CreateChain(bodyID, &chainDef);
					if (b2Chain_IsValid(chainID))
					{				
						shape->SetChainID(chainID);
					}
				}
			}, shape->shapeData);
		}

		void Physics2D::DestroyBody(b2BodyId bodyID)
		{
			b2DestroyBody(bodyID);			
		}

		void Physics2D::RecreateBody(Body2D* body)
		{
			DestroyBody(body->GetBodyID());
			body->SetBodyID(b2_nullBodyId);
			CreateBody(body);

			for (Shape2D* shape : body->GetShapes())
			{
				RecreateShape(shape);			
			}

			for (Joint2D* joint : body->GetJoints())
			{				
				RecreateJoint(joint);
			}

			for (auto& jointPair : body->GetConnectedJoints())
			{
				CreateJoint(jointPair.second);
			}
		}

		void Physics2D::DestroyShape(Shape2D* shape)
		{
			if (b2Shape_IsValid(shape->GetShapeID()))
			{				
				b2DestroyShape(shape->GetShapeID(), true);
			}
			if (b2Chain_IsValid(shape->GetChainID()))
			{
				b2DestroyChain(shape->GetChainID());
			}
			shape->SetShapeID(b2_nullShapeId);
			shape->SetChainID(b2_nullChainId);
		}

		void Physics2D::RecreateShape(Shape2D* shape)
		{
			DestroyShape(shape);
			CreateShape(shape, SceneManager::loadedScene.GetObjectByID(shape->GetOwnerID())->Get<Body2D>());
		}

		void Physics2D::CreateJoint(Joint2D* joint, Body2D* bodyA, Body2D* bodyB)
		{	
			if (bodyA == nullptr)
				bodyA = joint->GetBodyA();
			if (bodyB == nullptr)
				bodyB = joint->GetBodyB();
			
			if (bodyA == bodyB || bodyA == nullptr || bodyB == nullptr)
				return;			

			// bodyA->AddConnectedJoint(joint);
			bodyB->AddConnectedJoint(joint);			
			
			b2JointDef jointDef;
			jointDef.userData = joint;
			jointDef.bodyIdA = bodyA->GetBodyID();
			jointDef.bodyIdB = bodyB->GetBodyID();			
			jointDef.localFrameA.p = Vector2::GetB2Vec2(joint->anchorA);
			jointDef.localFrameA.q = bodyA->GetB2Rotation();
			jointDef.localFrameB.p = Vector2::GetB2Vec2(joint->anchorB);
			jointDef.localFrameB.q = bodyB->GetB2Rotation();
			jointDef.collideConnected = joint->b_collideConnected;

			std::visit([jointDef, joint, this](auto&& jData) -> void
			{
				using T = std::decay_t<decltype(jData)>;
				if constexpr (std::is_same_v<T, DistanceJoint2DData>)
				{	
					b2DistanceJointDef distanceJointDef = b2DefaultDistanceJointDef();
					distanceJointDef.base          		= jointDef;
					distanceJointDef.length        		= jData.length;
					distanceJointDef.enableSpring 		= jData.b_enableSpring;
					distanceJointDef.enableLimit   		= jData.b_enableLimit;
					distanceJointDef.enableMotor   		= jData.b_enableMotor;
					distanceJointDef.dampingRatio  		= jData.dampingRatio;
					distanceJointDef.hertz         		= jData.hertz;
					distanceJointDef.minLength     		= jData.minLength;
					distanceJointDef.maxLength     		= jData.maxLength;
					distanceJointDef.motorSpeed    		= jData.motorSpeed;
					distanceJointDef.maxMotorForce 		= jData.maxMotorForce;
					joint->SetJointID(b2CreateDistanceJoint(this->m_worldID, &distanceJointDef));					
				}
				else if constexpr (std::is_same_v<T, RevoluteJoint2DData>)
				{
					b2RevoluteJointDef revoluteJointDef = b2DefaultRevoluteJointDef();
					revoluteJointDef.base           	= jointDef;
					revoluteJointDef.dampingRatio   	= jData.dampingRatio;
					revoluteJointDef.enableLimit    	= jData.b_enableLimit;
					revoluteJointDef.enableSpring   	= jData.b_enableSpring;
					revoluteJointDef.enableMotor    	= jData.b_enableMotor;
					revoluteJointDef.dampingRatio   	= jData.dampingRatio;
					revoluteJointDef.hertz          	= jData.hertz;
					revoluteJointDef.lowerAngle     	= jData.lowerAngle;
					revoluteJointDef.upperAngle     	= jData.upperAngle;
					revoluteJointDef.maxMotorTorque 	= jData.maxMotorTorque;
					revoluteJointDef.motorSpeed     	= jData.motorSpeed;
					revoluteJointDef.targetAngle    	= jData.targetAngle;			
					joint->SetJointID(b2CreateRevoluteJoint(this->m_worldID, &revoluteJointDef));					
				}
				else if constexpr (std::is_same_v<T, PrismaticJoint2DData>)
				{
					b2PrismaticJointDef prismaticJointDef = b2DefaultPrismaticJointDef();
					prismaticJointDef.base                = jointDef;
					prismaticJointDef.dampingRatio        = jData.dampingRatio;
					prismaticJointDef.enableLimit         = jData.b_enableLimit;
					prismaticJointDef.enableSpring        = jData.b_enableSpring;
					prismaticJointDef.enableMotor         = jData.b_enableMotor;
					prismaticJointDef.dampingRatio        = jData.dampingRatio;
					prismaticJointDef.hertz               = jData.hertz;
					prismaticJointDef.lowerTranslation    = jData.lowerTranslation;
					prismaticJointDef.upperTranslation    = jData.upperTranslation;
					prismaticJointDef.targetTranslation   = jData.targetTranslation;
					prismaticJointDef.motorSpeed          = jData.motorSpeed;
					prismaticJointDef.maxMotorForce       = jData.maxMotorForce;
					joint->SetJointID(b2CreatePrismaticJoint(this->m_worldID, &prismaticJointDef));
				}
				else if constexpr (std::is_same_v<T, MouseJoint2DData>)
				{
					b2MouseJointDef mouseJointDef = b2DefaultMouseJointDef();
					mouseJointDef.base            = jointDef;			
					mouseJointDef.dampingRatio    = jData.dampingRatio;
					mouseJointDef.maxForce        = jData.maxForce;						
					mouseJointDef.dampingRatio    = jData.dampingRatio;
					mouseJointDef.hertz           = jData.hertz;			
					joint->SetJointID(b2CreateMouseJoint(this->m_worldID, &mouseJointDef));
				}
				else if constexpr (std::is_same_v<T, WeldJoint2DData>)
				{
					b2WeldJointDef weldJointDef      = b2DefaultWeldJointDef();
					weldJointDef.base                = jointDef;
					weldJointDef.angularDampingRatio = jData.angularDampingRatio;
					weldJointDef.angularHertz        = jData.angularHertz;
					weldJointDef.linearDampingRatio  = jData.linearDampingRatio;
					weldJointDef.linearHertz         = jData.linearHertz;
					joint->SetJointID(b2CreateWeldJoint(this->m_worldID, &weldJointDef));
				}
				else if constexpr (std::is_same_v<T, MotorJoint2DData>)
				{
					b2MotorJointDef motorJointDef     = b2DefaultMotorJointDef();
					motorJointDef.base                = jointDef;
					motorJointDef.angularDampingRatio = jData.angularDampingRatio;
					motorJointDef.angularHertz        = jData.angularHertz;
					motorJointDef.angularVelocity     = jData.angularVelocity;
					motorJointDef.linearDampingRatio  = jData.linearDampingRatio;
					motorJointDef.linearHertz         = jData.linearHertz;
					motorJointDef.linearVelocity      = Vector2::GetB2Vec2(jData.linearVelocity);
					motorJointDef.maxSpringForce      = jData.maxSpringForce;
					motorJointDef.maxSpringTorque     = jData.maxSpringTorque;
					motorJointDef.maxVelocityForce    = jData.maxVelocityForce;
					motorJointDef.maxVelocityTorque   = jData.maxVelocityTorque;
					motorJointDef.relativeTransform.p = Vector2::GetB2Vec2(jData.relativeTransformPos);
					motorJointDef.relativeTransform.q = b2MakeRot(jData.angleBetween);
					joint->SetJointID(b2CreateMotorJoint(this->m_worldID, &motorJointDef));
				}
				else if constexpr (std::is_same_v<T, WheelJoint2DData>)
				{
					b2WheelJointDef wheelJointDef  = b2DefaultWheelJointDef();
					wheelJointDef.base             = jointDef;
					wheelJointDef.dampingRatio     = jData.dampingRatio;
					wheelJointDef.enableLimit      = jData.b_enableLimit;
					wheelJointDef.enableMotor      = jData.b_enableMotor;
					wheelJointDef.enableSpring     = jData.b_enableSpring;
					wheelJointDef.hertz            = jData.hertz;
					wheelJointDef.lowerTranslation = jData.lowerTranslation;
					wheelJointDef.upperTranslation = jData.upperTranslation;
					wheelJointDef.maxMotorTorque   = jData.maxMotorTorque;
					wheelJointDef.motorSpeed       = jData.motorSpeed;
					joint->SetJointID(b2CreateWheelJoint(this->m_worldID, &wheelJointDef));
				}
			}, joint->jointData);
		}

		void Physics2D::DestroyJoint(FL::Joint2D* joint)
		{
			if (joint != nullptr)
			{
				if (b2Joint_IsValid(joint->m_jointID))
					b2DestroyJoint(joint->m_jointID);	
				
				joint->SetJointID(b2_nullJointId);							
			}
		}

		void Physics2D::RecreateJoint(Joint2D* joint)
		{
			DestroyJoint(joint);
			CreateJoint(joint);
		}

		// For Mouse button collisions - Vector4 objectA(top, right, bottom, left), Vector4 objectB(top, right, bottom, left)
		bool Physics2D::AreCollidingViewport(Vector4 ObjectA, Vector4 ObjectB)
		{
			float A_TopEdge = ObjectA.z;
			float A_RightEdge = ObjectA.y;
			float A_BottomEdge = ObjectA.x;
			float A_LeftEdge = ObjectA.w;

			float B_TopEdge = ObjectB.x;
			float B_RightEdge = ObjectB.y;
			float B_BottomEdge = ObjectB.z;
			float B_LeftEdge = ObjectB.w;

			return (A_LeftEdge < B_RightEdge && A_RightEdge > B_LeftEdge && A_TopEdge > B_BottomEdge && A_BottomEdge < B_TopEdge);
		}
	}
}