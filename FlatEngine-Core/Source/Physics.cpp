#include "Physics.h"
#include "GameObject.h"
#include "TagList.h"
#include "FlatEngine.h"
#include "Body.h"

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

		for (int i = 0; i < contactEvents.beginCount; ++i)
		{
			b2ContactBeginTouchEvent* beginEvent = contactEvents.beginEvents + i;	
			b2Manifold manifold = b2Contact_GetData(beginEvent->contactId).manifold;			
			static_cast<Body*>(b2Shape_GetUserData(beginEvent->shapeIdA))->OnBeginContact(manifold, beginEvent->shapeIdA, beginEvent->shapeIdB);
			static_cast<Body*>(b2Shape_GetUserData(beginEvent->shapeIdB))->OnBeginContact(manifold, beginEvent->shapeIdB, beginEvent->shapeIdA);		
		}

		for (int i = 0; i < contactEvents.endCount; ++i)
		{
			b2ContactEndTouchEvent* endEvent = contactEvents.endEvents + i;
			if (b2Shape_IsValid(endEvent->shapeIdA) && b2Shape_IsValid(endEvent->shapeIdB))
			{
				static_cast<Body*>(b2Shape_GetUserData(endEvent->shapeIdA))->OnEndContact(endEvent->shapeIdA, endEvent->shapeIdB);
				static_cast<Body*>(b2Shape_GetUserData(endEvent->shapeIdB))->OnEndContact(endEvent->shapeIdB, endEvent->shapeIdA);
			}
		}

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

		b2ShapeDef shapeDef = b2DefaultShapeDef();
		shapeDef.userData = parentBody;
		shapeDef.enableContactEvents = bodyProps.b_enableContactEvents;
		shapeDef.density = bodyProps.density;
		shapeDef.material.friction = bodyProps.friction;
		shapeDef.material.restitution = bodyProps.restitution;

		b2Filter filter = b2DefaultFilter();
		filter.categoryBits = 0;
		filter.maskBits = 0;

		TagList tagList = static_cast<Body*>(bodyDef.userData)->GetParent()->GetTagList();
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

		switch (bodyProps.shape)
		{
		case Physics::BodyShape::BS_Box:
		{
			float cornerRadius = bodyProps.cornerRadius;
			b2Polygon box;
			
			if (cornerRadius == 0.0f)
			{
				box = b2MakeBox(bodyProps.dimensions.x / 2, bodyProps.dimensions.y / 2);
			}
			else
			{
				box = b2MakeRoundedBox(bodyProps.dimensions.x / 2, bodyProps.dimensions.y / 2, cornerRadius);
			}

			shapeID = b2CreatePolygonShape(bodyID, &shapeDef, &box);			
			break;
		}
		case Physics::BodyShape::BS_Circle:
		{
			b2Circle circle;
			circle.center = b2Vec2(0,0);
			circle.radius = bodyProps.radius;
			shapeID = b2CreateCircleShape(bodyID, &shapeDef, &circle);
			break;
		}
		case Physics::BodyShape::BS_Capsule:
		{
			b2Capsule capsule;		
			float center1Value = ((bodyProps.capsuleLength / 2) - bodyProps.radius) * -1;
			float center2Value = (bodyProps.capsuleLength / 2) - bodyProps.radius;
			b2Vec2 center1 = b2Vec2(0,0);
			b2Vec2 center2 = b2Vec2(0,0);

			if (bodyProps.b_horizontal)
			{				
				center1.x = center1Value;
				center2.x = center2Value;
			}
			else
			{
				center1.y = center1Value;
				center2.y = center2Value;
			}		

			capsule.center1 = center1;
			capsule.center2 = center2;
			capsule.radius = bodyProps.radius;
			shapeID = b2CreateCapsuleShape(bodyID, &shapeDef, &capsule);
			break;
		}
		case Physics::BodyShape::BS_Polygon:
		{
			std::vector<b2Vec2> points;
			float cornerRadius = bodyProps.cornerRadius;

			for (Vector2 point : bodyProps.points)
			{
				points.push_back(b2Vec2(point.x, point.y));
			}

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

			break;
		}
		case Physics::BodyShape::BS_Chain:
		{												
			b2ChainDef chainDef = b2DefaultChainDef();
			chainDef.userData = parentBody;
			chainDef.filter = filter;
			chainDef.isLoop = bodyProps.b_isLoop;

			b2SurfaceMaterial material = b2DefaultSurfaceMaterial();
			material.friction = bodyProps.friction;
			material.restitution = bodyProps.restitution;
			material.rollingResistance = bodyProps.rollingResistance;
			material.tangentSpeed = bodyProps.tangentSpeed;
			chainDef.materialCount = 1;
			chainDef.materials = &material;

			std::vector<b2Vec2> points;

			for (Vector2 point : bodyProps.points)
			{
				points.push_back(b2Vec2(point.x, point.y));
			}
			
			chainDef.points = &points[0];
			chainDef.count = (int)points.size();

			b2ChainId chainID = b2CreateChain(bodyID, &chainDef);
			if (b2Chain_IsValid(chainID))
			{
				parentBody->SetChainID(chainID);
			}
			
			break;
		}
		default:		
			break;
		}

		if (b2Shape_IsValid(shapeID))
		{
			b2Shape_SetUserData(shapeID, parentBody);			
			parentBody->AddShapeID(shapeID);
		}
	}

	void Physics::DestroyBody(b2BodyId bodyID)
	{
		b2DestroyBody(bodyID);
	}

	void Physics::RecreateBody(Body* parentBody)
	{
		DestroyBody(parentBody->GetBodyID());
		parentBody->CleanupIDs();
		CreateBody(parentBody);
	}
}