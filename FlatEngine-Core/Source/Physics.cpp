#include "Physics.h"
#include "GameObject.h"
#include "TagList.h"
#include "FlatEngine.h"
#include "Body.h"
#include "Shape.h"

#include <memory>
#include <string>


namespace FlatEngine
{
	std::unique_ptr<Physics> F_Physics = std::make_unique<Physics>();


	Physics::Physics()
	{

	}

	Physics::~Physics()
	{
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
		// Category tags
		std::map<std::string, bool> hasTags = tagList.GetTagsMap();
		for (std::map<std::string, bool>::iterator iterator = hasTags.begin(); iterator != hasTags.end(); iterator++)
		{
			if (iterator->second)
			{
				uint64_t bit = 1;
				int index = (int)std::distance(hasTags.begin(), iterator);

				for (int i = 0; i < index; i++)
				{
					bit *= 2;
				}
				if (index == 0)
				{
					bit = 2;
				}

				filter.categoryBits |= bit;
			}
		}
		// Masked tags
		std::map<std::string, bool> collidesTags = tagList.GetCollidesTagsMap();
		for (std::map<std::string, bool>::iterator iterator = collidesTags.begin(); iterator != collidesTags.end(); iterator++)
		{
			if (iterator->second)
			{
				uint64_t bit = 1;
				int index = (int)std::distance(collidesTags.begin(), iterator);

				for (int i = 0; i < index; i++)
				{
					bit *= 2;
				}
				if (index == 0)
				{
					bit = 2;
				}

				filter.maskBits |= bit;
			}
		}

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
		DestroyShape(shape->GetShapeID());
		CreateShape(shape->GetParentBody(), shape);
	}
}