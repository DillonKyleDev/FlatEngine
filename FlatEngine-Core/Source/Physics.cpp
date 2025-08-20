#include "Physics.h"
#include "GameObject.h"
#include "TagList.h"
#include "FlatEngine.h"
#include "Body.h"
#include "Shape.h"
#include "Joint.h"
#include "DistanceJoint.h"
#include "PrismaticJoint.h"
#include "RevoluteJoint.h"
#include "MouseJoint.h"
#include "WheelJoint.h"
#include "MotorJoint.h"
#include "WeldJoint.h"

#include <memory>
#include <string>
#include <map>


namespace FlatEngine
{
	std::unique_ptr<Physics> F_Physics = std::make_unique<Physics>();


	Physics::Physics()
	{
		m_worldID = b2_nullWorldId;
	}

	Physics::~Physics()
	{
	}

	bool Physics::CanCollide(TagList tagList1, TagList tagList2)
	{
		uint64_t catA = tagList1.GetCategoryBits();
		uint64_t maskA = tagList1.GetMaskBits();
		uint64_t catB = tagList2.GetCategoryBits();
		uint64_t maskB = tagList2.GetMaskBits();

		return ((catA & maskB) != 0 && (catB & maskA) != 0);
	}

	void Physics::Init()
	{
		b2WorldDef worldDef = b2DefaultWorldDef();
		worldDef.gravity = b2Vec2{ 0.0f, -10.0f };
		m_worldID = b2CreateWorld(&worldDef);	
	}

	void Physics::Shutdown()
	{
		b2DestroyWorld(m_worldID);
	}

	void Physics::Update(float deltaTime)
	{
		float timeStep = 1.0f / 60.0f;
		int substepCount = 4;

		b2World_Step(m_worldID, timeStep, substepCount);
		HandleCollisions();
	}

	void Physics::HandleCollisions()
	{
		b2ContactEvents contactEvents = b2World_GetContactEvents(m_worldID);
		b2SensorEvents sensorEvents = b2World_GetSensorEvents(m_worldID);

		// Contacts
		for (int i = 0; i < contactEvents.beginCount; ++i)
		{
			b2ContactBeginTouchEvent* beginEvent = contactEvents.beginEvents + i;	
			b2Manifold manifold = b2Contact_GetData(beginEvent->contactId).manifold;	
			Body::GetBodyFromShapeID(beginEvent->shapeIdA)->OnBeginContact(manifold, beginEvent->shapeIdA, beginEvent->shapeIdB);
			Body::GetBodyFromShapeID(beginEvent->shapeIdB)->OnBeginContact(manifold, beginEvent->shapeIdB, beginEvent->shapeIdA);
		}
		for (int i = 0; i < contactEvents.endCount; ++i)
		{
			b2ContactEndTouchEvent* endEvent = contactEvents.endEvents + i;
			if (b2Shape_IsValid(endEvent->shapeIdA) && b2Shape_IsValid(endEvent->shapeIdB))
			{
				Body::GetBodyFromShapeID(endEvent->shapeIdA)->OnEndContact(endEvent->shapeIdA, endEvent->shapeIdB);
				Body::GetBodyFromShapeID(endEvent->shapeIdB)->OnEndContact(endEvent->shapeIdB, endEvent->shapeIdA);
			}
		}

		// Sensors
		for (int i = 0; i < sensorEvents.beginCount; ++i)
		{
			b2SensorBeginTouchEvent* beginTouch = sensorEvents.beginEvents + i;
			Body::GetBodyFromShapeID(beginTouch->sensorShapeId)->OnSensorBeginTouch(beginTouch->sensorShapeId, beginTouch->visitorShapeId);
			Body::GetBodyFromShapeID(beginTouch->visitorShapeId)->OnSensorBeginTouch(beginTouch->visitorShapeId, beginTouch->sensorShapeId);
		}
		for (int i = 0; i < sensorEvents.endCount; ++i)
		{
			b2SensorEndTouchEvent* endTouch = sensorEvents.endEvents + i;
			if (b2Shape_IsValid(endTouch->visitorShapeId))
			{
				Body::GetBodyFromShapeID(endTouch->sensorShapeId)->OnSensorEndTouch(endTouch->sensorShapeId, endTouch->visitorShapeId);
				Body::GetBodyFromShapeID(endTouch->visitorShapeId)->OnSensorEndTouch(endTouch->visitorShapeId, endTouch->sensorShapeId);
			}
		}

		// Hit events
		for (int i = 0; i < contactEvents.hitCount; ++i)
		{
			float hitSpeedForSound = 10.0f;

			b2ContactHitEvent* hitEvent = contactEvents.hitEvents + i;
			if (hitEvent->approachSpeed > hitSpeedForSound)
			{
				// Play sound
			}
		}
	}

	void Physics::CreateBody(Body* parentBody)
	{
		BodyProps bodyProps = parentBody->GetBodyProps();
		b2BodyDef bodyDef = b2DefaultBodyDef();
		b2Vec2 position = b2Vec2(bodyProps.position.x, bodyProps.position.y);
		bodyDef.isEnabled = parentBody->IsActive();
		bodyDef.isAwake = true;
		bodyDef.enableSleep = true;
		bodyDef.userData = parentBody;
		bodyDef.position = position;
		bodyDef.rotation = bodyProps.rotation;
		b2MotionLocks motionLocks;		
		motionLocks.angularZ = bodyProps.b_lockedRotation;
		motionLocks.linearX = bodyProps.b_lockedXAxis;
		motionLocks.linearY = bodyProps.b_lockedYAxis;
		bodyDef.motionLocks = motionLocks;
		bodyDef.gravityScale = bodyProps.gravityScale;
		bodyDef.linearDamping = bodyProps.linearDamping;
		bodyDef.angularDamping = bodyProps.angularDamping;
		bodyDef.type = bodyProps.type;		
		b2BodyId bodyID = b2CreateBody(m_worldID, &bodyDef);
		parentBody->SetBodyID(bodyID);
	}

	void Physics::CreateShape(Body* parentBody, Shape* shape)
	{
		b2BodyId bodyID = parentBody->GetBodyID();			
		Shape::ShapeProps shapeProps = shape->GetShapeProps();
		b2ShapeDef shapeDef = b2DefaultShapeDef();
		shapeDef.userData = shape;
		shapeDef.enableContactEvents = shapeProps.b_enableContactEvents;
		shapeDef.enableSensorEvents = shapeProps.b_enableSensorEvents;
		shapeDef.isSensor = shapeProps.b_isSensor;
		shapeDef.density = shapeProps.density;
		shapeDef.material.friction = shapeProps.friction;
		shapeDef.material.restitution = shapeProps.restitution;		

		b2Filter filter = b2DefaultFilter();
		filter.categoryBits = 0;
		filter.maskBits = 0;

		TagList tagList = parentBody->GetParent()->GetTagList();
		tagList.UpdateBits();
		filter.categoryBits = tagList.GetCategoryBits();
		filter.maskBits = tagList.GetMaskBits();

		shapeDef.filter = filter;

		b2ShapeId shapeID = b2ShapeId();
		b2Vec2 center = b2Vec2(shapeProps.positionOffset.x, shapeProps.positionOffset.y);
		b2Rot rotationOffset = shapeProps.rotationOffset;
		float cornerRadius = shapeProps.cornerRadius;

		switch (shapeProps.shape)
		{
		case Shape::ShapeType::BS_Box:
		{			
			b2Polygon box;

			box = b2MakeOffsetRoundedBox(shapeProps.dimensions.x / 2, shapeProps.dimensions.y / 2, center, rotationOffset, cornerRadius);
			shapeID = b2CreatePolygonShape(bodyID, &shapeDef, &box);

			break;
		}
		case Shape::ShapeType::BS_Circle:
		{
			b2Circle circle;
			circle.center = center;
			circle.radius = shapeProps.radius;
			shapeID = b2CreateCircleShape(bodyID, &shapeDef, &circle);

			break;
		}
		case Shape::ShapeType::BS_Capsule:
		{
			b2Capsule capsule;
			float center1Value = ((shapeProps.capsuleLength / 2) - shapeProps.radius) * -1;
			float center2Value = (shapeProps.capsuleLength / 2) - shapeProps.radius;
			b2Vec2 center1 = b2Vec2(0, 0);
			b2Vec2 center2 = b2Vec2(0, 0);

			if (shapeProps.b_horizontal)
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
			capsule.radius = shapeProps.radius;
			shapeID = b2CreateCapsuleShape(bodyID, &shapeDef, &capsule);
			break;
		}
		case Shape::ShapeType::BS_Polygon:
		{
			std::vector<b2Vec2> points;
			float cornerRadius = shapeProps.cornerRadius;

			for (Vector2 point : shapeProps.points)
			{
				points.push_back(b2Vec2(point.x, point.y));
			}

			if (points.size() > 0)
			{
				b2Hull hull = b2ComputeHull(&points[0], (int)points.size());

				if (hull.count == 0)
				{
					LogError("Hull not successfully created.");
				}
				else
				{
					b2Polygon polygon = b2MakePolygon(&hull, cornerRadius);
					shapeID = b2CreatePolygonShape(bodyID, &shapeDef, &polygon);
				}
			}

			break;
		}
		case Shape::ShapeType::BS_Chain:
		{
			b2SurfaceMaterial material = b2DefaultSurfaceMaterial();
			material.friction = shapeProps.friction;
			material.restitution = shapeProps.restitution;
			material.rollingResistance = shapeProps.rollingResistance;
			material.tangentSpeed = shapeProps.tangentSpeed;

			b2ChainDef chainDef = b2DefaultChainDef();
			chainDef.userData = shape;
			chainDef.filter = filter;
			chainDef.enableSensorEvents = shapeProps.b_enableSensorEvents;
			chainDef.isLoop = shapeProps.b_isLoop;
			chainDef.materialCount = 1;
			chainDef.materials = &material;

			std::vector<b2Vec2> points;

			for (Vector2 point : shapeProps.points)
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

			break;
		}
		default:
			break;
		}

		if (b2Shape_IsValid(shapeID))
		{						
			shape->SetShapeID(shapeID);
		}
	}

	void Physics::DestroyBody(b2BodyId bodyID)
	{
		b2DestroyBody(bodyID);
	}

	void Physics::RecreateBody(Body* parentBody)
	{
		DestroyBody(parentBody->GetBodyID());
		CreateBody(parentBody);
		parentBody->RecreateShapes();
		
	}

	void Physics::DestroyShape(b2ShapeId shapeID)
	{
		b2DestroyShape(shapeID, true);
	}

	void Physics::RecreateShape(Shape* shape)
	{
		//DestroyShape(shape->GetShapeID());
		//CreateShape(shape->GetParentBody(), shape);
	}

	void Physics::CreateJoint(Body* bodyA, Body* bodyB, Joint* joint)
	{
		Joint::BaseProps baseProps = joint->GetBaseProps();
		b2JointId jointID = b2_nullJointId;
		b2JointDef jointDef;
		jointDef.userData = joint;
		jointDef.bodyIdA = bodyA->GetBodyID();
		jointDef.bodyIdB = bodyB->GetBodyID();			
		jointDef.localFrameA.p = Vector2::GetB2Vev2(baseProps.anchorA);
		jointDef.localFrameA.q = bodyA->GetB2Rotation();
		jointDef.localFrameB.p = Vector2::GetB2Vev2(baseProps.anchorB);
		jointDef.localFrameB.q = bodyB->GetB2Rotation();
		jointDef.collideConnected = baseProps.b_collideConnected;

		b2Vec2 anchorA = b2Body_GetWorldPoint(jointDef.bodyIdA, Vector2::GetB2Vev2(baseProps.anchorA));
		b2Vec2 anchorB = b2Body_GetWorldPoint(jointDef.bodyIdB, Vector2::GetB2Vev2(baseProps.anchorB));

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
			motorJointDef.linearVelocity = Vector2::GetB2Vev2(motorProps.linearVelocity);
			motorJointDef.maxSpringForce = motorProps.maxSpringForce;
			motorJointDef.maxSpringTorque = motorProps.maxSpringTorque;
			motorJointDef.maxVelocityForce = motorProps.maxVelocityForce;
			motorJointDef.maxVelocityTorque = motorProps.maxVelocityTorque;
			motorJointDef.relativeTransform.p = Vector2::GetB2Vev2(motorProps.relativeTransformPos);
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
}