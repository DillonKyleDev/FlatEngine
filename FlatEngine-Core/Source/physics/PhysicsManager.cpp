
#include "components/Body.h"
#include "components/Body2D.h"
#include "GameObject.h"
#include "components/Transform.h"
#include "joints/DistanceJoint.h"
#include "joints/Joint.h"
#include "joints/MotorJoint.h"
#include "joints/MouseJoint.h"
#include "joints/PrismaticJoint.h"
#include "joints/RevoluteJoint.h"
#include "joints/WeldJoint.h"
#include "joints/WheelJoint.h"
#include "managers/SceneManager.h"
#include "physics/PhysicsManager.h"
#include "physics/Shape.h"
#include "render/SceneView.h"
#include "TagList.h"
#include "tools/Logger.h"
#include <id.h>


namespace FlatEngine
{
	namespace PhysicsManager
	{
		// Physics physics = Physics();
		Physics2D physics2D = Physics2D();
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
				SceneView::DrawLineInScene(initialPos, (initialPos + direction) * 10, "rayCast", 2);
			}

			for (Body2D& body : SceneManager::loadedScene.GetAll<Body2D>().GetAll())
			{
				body.GetOwningObject()->GetTagList().UpdateBits();
				if (PhysicsManager::physics2D.CanCollide(tagList, body.GetOwningObject()->GetTagList()))
				{
					for (Shape* shape : body.GetShapes())
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
		}

		void Physics2D::Shutdown()
		{
			b2DestroyWorld(m_worldID);
		}

		void Physics2D::Update(float deltaTime)
		{
			float timeStep = 1.0f / 60.0f;
			int substepCount = 4;

			b2World_Step(m_worldID, timeStep, substepCount);
			HandleCollisions();
			// DrawDebugShapes();
		}

		void Physics2D::HandleCollisions()
		{
			b2ContactEvents contactEvents = b2World_GetContactEvents(m_worldID);
			b2SensorEvents sensorEvents = b2World_GetSensorEvents(m_worldID);

			// Contacts
			for (int i = 0; i < contactEvents.beginCount; ++i)
			{
				b2ContactBeginTouchEvent* beginEvent = contactEvents.beginEvents + i;	
				b2Manifold manifold = b2Contact_GetData(beginEvent->contactId).manifold;	
				GetBodyFromShapeID(beginEvent->shapeIdA)->OnBeginContact(manifold, beginEvent->shapeIdA, beginEvent->shapeIdB);
				GetBodyFromShapeID(beginEvent->shapeIdB)->OnBeginContact(manifold, beginEvent->shapeIdB, beginEvent->shapeIdA);
			}
			for (int i = 0; i < contactEvents.endCount; ++i)
			{
				b2ContactEndTouchEvent* endEvent = contactEvents.endEvents + i;
				if (b2Shape_IsValid(endEvent->shapeIdA) && b2Shape_IsValid(endEvent->shapeIdB))
				{
					GetBodyFromShapeID(endEvent->shapeIdA)->OnEndContact(endEvent->shapeIdA, endEvent->shapeIdB);
					GetBodyFromShapeID(endEvent->shapeIdB)->OnEndContact(endEvent->shapeIdB, endEvent->shapeIdA);
				}
			}

			// Sensors
			for (int i = 0; i < sensorEvents.beginCount; ++i)
			{
				b2SensorBeginTouchEvent* beginTouch = sensorEvents.beginEvents + i;
				GetBodyFromShapeID(beginTouch->sensorShapeId)->OnSensorBeginTouch(beginTouch->sensorShapeId, beginTouch->visitorShapeId);
				GetBodyFromShapeID(beginTouch->visitorShapeId)->OnSensorBeginTouch(beginTouch->visitorShapeId, beginTouch->sensorShapeId);
			}
			for (int i = 0; i < sensorEvents.endCount; ++i)
			{
				b2SensorEndTouchEvent* endTouch = sensorEvents.endEvents + i;
				if (b2Shape_IsValid(endTouch->visitorShapeId))
				{
					GetBodyFromShapeID(endTouch->sensorShapeId)->OnSensorEndTouch(endTouch->sensorShapeId, endTouch->visitorShapeId);
					GetBodyFromShapeID(endTouch->visitorShapeId)->OnSensorEndTouch(endTouch->visitorShapeId, endTouch->sensorShapeId);
				}
			}

			// Hit events
			for (int i = 0; i < contactEvents.hitCount; ++i)
			{
				float hitSpeedForSound = 10.0f;

				b2ContactHitEvent* hitEvent = contactEvents.hitEvents + i;
				if (hitEvent->approachSpeed > hitSpeedForSound)
				{
					// Play sound, etc..
				}
			}
		}

		Body2D* Physics2D::GetBodyFromShapeID(b2ShapeId shapeID)
		{
			Shape* shape = static_cast<Shape*>(b2Shape_GetUserData(shapeID));

			if (b2Shape_IsValid(shape->GetShapeID()))
			{
				return SceneManager::loadedScene.GetObjectByID(shape->GetOwnerID())->Get<Body2D>();
			}
			else if (b2Chain_IsValid(shape->GetChainID()))
			{
				return SceneManager::loadedScene.GetObjectByID(shape->GetOwnerID())->Get<Body2D>();
			}
			return nullptr;
		}

		void Physics2D::CreateBody(Body2D* parentBody)
		{
			b2BodyDef bodyDef = b2DefaultBodyDef();
			b2Vec2 position = b2Vec2(parentBody->position.x, parentBody->position.y);
			bodyDef.isEnabled = parentBody->IsActive();
			bodyDef.isAwake = true;
			bodyDef.enableSleep = true;
			bodyDef.userData = parentBody;
			bodyDef.position = position;
			bodyDef.rotation = parentBody->rotation;
			b2MotionLocks motionLocks;		
			motionLocks.angularZ = parentBody->b_lockedRotation;
			motionLocks.linearX = parentBody->b_lockedXAxis;
			motionLocks.linearY = parentBody->b_lockedYAxis;
			bodyDef.motionLocks = motionLocks;
			bodyDef.gravityScale = parentBody->gravityScale;
			bodyDef.linearDamping = parentBody->linearDamping;
			bodyDef.angularDamping = parentBody->angularDamping;
			bodyDef.type = parentBody->type;		
			b2BodyId bodyID = b2CreateBody(m_worldID, &bodyDef);
			parentBody->SetBodyID(bodyID);
		}

		void Physics2D::CreateShape(Shape* shape, Body2D* parentBody)
		{
			b2BodyId bodyID = parentBody != nullptr ? parentBody->GetBodyID() : b2_nullBodyId;
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
			TagList tagList = parentBody != nullptr ? parentBody->GetOwningObject()->GetTagList() : TagList();
			tagList.UpdateBits();
			filter.categoryBits = tagList.GetCategoryBits();
			filter.maskBits = tagList.GetMaskBits();
			shapeDef.filter = filter;

			b2ShapeId shapeID = b2ShapeId();
			// Vector2 position = parentBody != nullptr ? parentBody->GetPosition() : Vector2();
			// Vector2 offset = shape->positionOffset;

			b2SurfaceMaterial material = b2DefaultSurfaceMaterial();
			material.friction = shape->friction;
			material.restitution = shape->restitution;
			material.rollingResistance = shape->rollingResistance;
			material.tangentSpeed = shape->tangentSpeed;
			
			// shape->renderShape.transform.SetScale(Vector3(shapeProps.radius, shapeProps.radius, 1));
			// shape->renderShape.transform.SetScale(Vector3(shapeProps.dimensions.x, shapeProps.dimensions.y, 0));
			// shape->renderShape.transform.SetPosition(Vector3(position.x + offset.x, position.y + offset.y, SceneManager::loadedScene.Get<Transform>(parentBody->GetOwnerID()) != nullptr ? SceneManager::loadedScene.Get<Transform>(parentBody->GetOwnerID())->GetPosition().z : 0));	

			std::visit([parentBody, bodyID, shapeDef, filter, material, shape](auto&& sData) -> void
			{
				using T = std::decay_t<decltype(sData)>;
				if constexpr (std::is_same_v<T, BoxShapeData>)
				{	
					b2Rot rotationOffset = sData.rotationOffset;
					b2Vec2 center = b2Vec2(sData.offset.x, sData.offset.y);
					shape->polygon = b2MakeOffsetRoundedBox(sData.dimensions.x / 2, sData.dimensions.y / 2, center, rotationOffset, sData.cornerRadius);

					if (parentBody != nullptr)				
						shape->SetShapeID(b2CreatePolygonShape(bodyID, &shapeDef, &shape->polygon));	
				}
				else if constexpr (std::is_same_v<T, CircleShapeData>)
				{
					b2Circle circle;
					b2Vec2 center = b2Vec2(sData.offset.x, sData.offset.y);
					circle.center = center;
					circle.radius = sData.radius;
					shape->circle = circle;

					if (parentBody != nullptr)		
						shape->SetShapeID(b2CreateCircleShape(bodyID, &shapeDef, &circle));	
				}
				else if constexpr (std::is_same_v<T, PolygonShapeData>)
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

							if (parentBody != nullptr)				
								shape->SetShapeID(b2CreatePolygonShape(bodyID, &shapeDef, &shape->polygon));									
						}
					}
				}
				else if constexpr (std::is_same_v<T, CapsuleShapeData>)
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

					if (parentBody != nullptr)				
						shape->SetShapeID(b2CreateCapsuleShape(bodyID, &shapeDef, &capsule));		
				}
				else if constexpr (std::is_same_v<T, ChainShapeData>)
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

			if (b2Shape_IsValid(shapeID))
			{						
				shape->SetShapeID(shapeID);
			}
		}

		void Physics2D::DestroyBody(b2BodyId bodyID)
		{
			b2DestroyBody(bodyID);
		}

		void Physics2D::RecreateBody(Body2D* parentBody)
		{
			DestroyBody(parentBody->GetBodyID());
			CreateBody(parentBody);
			parentBody->RecreateShapes();			
		}

		void Physics2D::DestroyShape(Shape* shape)
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

		void Physics2D::RecreateShape(Shape* shape)
		{
			DestroyShape(shape);
			CreateShape(shape, SceneManager::loadedScene.GetObjectByID(shape->GetOwnerID())->Get<Body2D>());
		}

		void Physics2D::CreateJoint(Body2D* bodyA, Body2D* bodyB, Joint* joint)
		{
			Joint::BaseProps baseProps = joint->GetBaseProps();
			b2JointId jointID = b2_nullJointId;
			b2JointDef jointDef;
			jointDef.userData = joint;
			jointDef.bodyIdA = bodyA->GetBodyID();
			jointDef.bodyIdB = bodyB->GetBodyID();			
			jointDef.localFrameA.p = Vector2::GetB2Vec2(baseProps.anchorA);
			jointDef.localFrameA.q = bodyA->GetB2Rotation();
			jointDef.localFrameB.p = Vector2::GetB2Vec2(baseProps.anchorB);
			jointDef.localFrameB.q = bodyB->GetB2Rotation();
			jointDef.collideConnected = baseProps.b_collideConnected;

			b2Vec2 anchorA = b2Body_GetWorldPoint(jointDef.bodyIdA, Vector2::GetB2Vec2(baseProps.anchorA));
			b2Vec2 anchorB = b2Body_GetWorldPoint(jointDef.bodyIdB, Vector2::GetB2Vec2(baseProps.anchorB));

			switch (joint->GetJointType())
			{
			case Joint::JointType::JT_Distance:
			{
				DistanceJoint::DistanceJointProps distanceProps = static_cast<DistanceJoint*>(joint)->GetJointProps();
				b2DistanceJointDef distanceJointDef = b2DefaultDistanceJointDef();
				distanceJointDef.base = jointDef;
				distanceJointDef.length = distanceProps.length;
				distanceJointDef.enableSpring = distanceProps.b_enableSpring;
				distanceJointDef.enableLimit = distanceProps.b_enableLimit;
				distanceJointDef.enableMotor = distanceProps.b_enableMotor;
				distanceJointDef.dampingRatio = distanceProps.dampingRatio;
				distanceJointDef.hertz = distanceProps.hertz;
				distanceJointDef.minLength = distanceProps.minLength;
				distanceJointDef.maxLength = distanceProps.maxLength;
				distanceJointDef.motorSpeed = distanceProps.motorSpeed;
				distanceJointDef.maxMotorForce = distanceProps.maxMotorForce;
				jointID = b2CreateDistanceJoint(m_worldID, &distanceJointDef);
				break;
			}
			case Joint::JointType::JT_Revolute:
			{
				RevoluteJoint::RevoluteJointProps revoluteProps = static_cast<RevoluteJoint*>(joint)->GetJointProps();
				b2RevoluteJointDef revoluteJointDef = b2DefaultRevoluteJointDef();
				revoluteJointDef.base = jointDef;
				revoluteJointDef.dampingRatio = revoluteProps.dampingRatio;
				revoluteJointDef.enableLimit = revoluteProps.b_enableLimit;
				revoluteJointDef.enableSpring = revoluteProps.b_enableSpring;
				revoluteJointDef.enableMotor = revoluteProps.b_enableMotor;
				revoluteJointDef.dampingRatio = revoluteProps.dampingRatio;
				revoluteJointDef.hertz = revoluteProps.hertz;
				revoluteJointDef.lowerAngle = revoluteProps.lowerAngle;
				revoluteJointDef.upperAngle = revoluteProps.upperAngle;
				revoluteJointDef.maxMotorTorque = revoluteProps.maxMotorTorque;
				revoluteJointDef.motorSpeed = revoluteProps.motorSpeed;
				revoluteJointDef.targetAngle = revoluteProps.targetAngle;			
				jointID = b2CreateRevoluteJoint(m_worldID, &revoluteJointDef);
				break;
			}
			case Joint::JointType::JT_Prismatic:
			{
				PrismaticJoint::PrismaticJointProps prismaticProps = static_cast<PrismaticJoint*>(joint)->GetJointProps();
				b2PrismaticJointDef prismaticJointDef = b2DefaultPrismaticJointDef();
				prismaticJointDef.base = jointDef;
				prismaticJointDef.dampingRatio = prismaticProps.dampingRatio;
				prismaticJointDef.enableLimit = prismaticProps.b_enableLimit;
				prismaticJointDef.enableSpring = prismaticProps.b_enableSpring;
				prismaticJointDef.enableMotor = prismaticProps.b_enableMotor;
				prismaticJointDef.dampingRatio = prismaticProps.dampingRatio;
				prismaticJointDef.hertz = prismaticProps.hertz;
				prismaticJointDef.lowerTranslation = prismaticProps.lowerTranslation;
				prismaticJointDef.upperTranslation = prismaticProps.upperTranslation;
				prismaticJointDef.targetTranslation = prismaticProps.targetTranslation;
				prismaticJointDef.motorSpeed = prismaticProps.motorSpeed;
				prismaticJointDef.maxMotorForce = prismaticProps.maxMotorForce;
				jointID = b2CreatePrismaticJoint(m_worldID, &prismaticJointDef);
				break;
			}
			case Joint::JointType::JT_Mouse:
			{
				MouseJoint::MouseJointProps mouseProps = static_cast<MouseJoint*>(joint)->GetJointProps();
				b2MouseJointDef mouseJointDef = b2DefaultMouseJointDef();
				mouseJointDef.base = jointDef;			
				mouseJointDef.dampingRatio = mouseProps.dampingRatio;
				mouseJointDef.maxForce = mouseProps.maxForce;						
				mouseJointDef.dampingRatio = mouseProps.dampingRatio;
				mouseJointDef.hertz = mouseProps.hertz;			
				jointID = b2CreateMouseJoint(m_worldID, &mouseJointDef);
				break;
			}
			case Joint::JointType::JT_Weld:
			{
				WeldJoint::WeldJointProps weldProps = static_cast<WeldJoint*>(joint)->GetJointProps();
				b2WeldJointDef weldJointDef = b2DefaultWeldJointDef();
				weldJointDef.base = jointDef;
				weldJointDef.angularDampingRatio = weldProps.angularDampingRatio;
				weldJointDef.angularHertz = weldProps.angularHertz;
				weldJointDef.linearDampingRatio = weldProps.linearDampingRatio;
				weldJointDef.linearHertz = weldProps.linearHertz;
				jointID = b2CreateWeldJoint(m_worldID, &weldJointDef);
				break;
			}
			case Joint::JointType::JT_Motor:
			{
				MotorJoint::MotorJointProps motorProps = static_cast<MotorJoint*>(joint)->GetJointProps();
				b2MotorJointDef motorJointDef = b2DefaultMotorJointDef();
				motorJointDef.base = jointDef;
				motorJointDef.angularDampingRatio = motorProps.angularDampingRatio;
				motorJointDef.angularHertz = motorProps.angularHertz;
				motorJointDef.angularVelocity = motorProps.angularVelocity;
				motorJointDef.linearDampingRatio = motorProps.linearDampingRatio;
				motorJointDef.linearHertz = motorProps.linearHertz;
				motorJointDef.linearVelocity = Vector2::GetB2Vec2(motorProps.linearVelocity);
				motorJointDef.maxSpringForce = motorProps.maxSpringForce;
				motorJointDef.maxSpringTorque = motorProps.maxSpringTorque;
				motorJointDef.maxVelocityForce = motorProps.maxVelocityForce;
				motorJointDef.maxVelocityTorque = motorProps.maxVelocityTorque;
				motorJointDef.relativeTransform.p = Vector2::GetB2Vec2(motorProps.relativeTransformPos);
				motorJointDef.relativeTransform.q = b2MakeRot(motorProps.angleBetween);
				jointID = b2CreateMotorJoint(m_worldID, &motorJointDef);
				break;
			}
			case Joint::JointType::JT_Wheel:
			{
				WheelJoint::WheelJointProps wheelProps = static_cast<WheelJoint*>(joint)->GetJointProps();
				b2WheelJointDef wheelJointDef = b2DefaultWheelJointDef();
				wheelJointDef.base = jointDef;
				wheelJointDef.dampingRatio = wheelProps.dampingRatio;
				wheelJointDef.enableLimit = wheelProps.b_enableLimit;
				wheelJointDef.enableMotor = wheelProps.b_enableMotor;
				wheelJointDef.enableSpring = wheelProps.b_enableSpring;
				wheelJointDef.hertz = wheelProps.hertz;
				wheelJointDef.lowerTranslation = wheelProps.lowerTranslation;
				wheelJointDef.upperTranslation = wheelProps.upperTranslation;
				wheelJointDef.maxMotorTorque = wheelProps.maxMotorTorque;
				wheelJointDef.motorSpeed = wheelProps.motorSpeed;
				jointID = b2CreateWheelJoint(m_worldID, &wheelJointDef);
				break;
			}
			default:
				break;
			}

			if (b2Joint_IsValid(jointID))
			{
				joint->SetB2JointID(jointID);
			}
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