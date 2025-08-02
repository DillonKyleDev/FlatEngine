#include "Physics.h"
#include "GameObject.h"
#include "TagList.h"
#include "FlatEngine.h"

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
	}

	void Physics::CreateBody(BodyProps bodyProps, b2BodyId& bodyID, std::vector<b2ShapeId>& shapeIDs)
	{
		b2BodyDef bodyDef = b2DefaultBodyDef();
		bodyDef.isEnabled = bodyProps.b_isEnabled;
		bodyDef.userData = bodyProps.userData;
		bodyDef.position = b2Vec2(bodyProps.position.x, bodyProps.position.y);
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
		bodyID = b2CreateBody(m_worldID, &bodyDef);

		b2ShapeDef shapeDef = b2DefaultShapeDef();
		shapeDef.density = bodyProps.density;
		shapeDef.material.friction = bodyProps.friction;
		shapeDef.material.restitution = bodyProps.restitution;

		TagList tagList = static_cast<GameObject*>(bodyDef.userData)->GetTagList();
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

				shapeDef.filter.categoryBits |= bit;
			}
		}
		// Masked tags
		std::map<std::string, bool> ignoreTags = tagList.GetIgnoreTagsMap();
		for (std::map<std::string, bool>::iterator iterator = ignoreTags.begin(); iterator != ignoreTags.end(); iterator++)
		{
			if (iterator->second)
			{
				uint64_t bit = 1;
				int index = (int)std::distance(ignoreTags.begin(), iterator);

				for (int i = 0; i < index; i++)
				{
					bit *= 2;
				}

				shapeDef.filter.maskBits |= bit;
			}
		}

		b2ShapeId shapeID;

		switch (bodyProps.shape)
		{
		case Physics::BodyShape::BS_Box:
		{
			b2Polygon box = b2MakeBox(bodyProps.dimensions.x / 2, bodyProps.dimensions.y / 2);			
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
		default:
			shapeID = b2ShapeId();
			break;
		}

		shapeIDs.push_back(shapeID);
	}

	void Physics::CreateBox(BodyProps bodyProps, b2BodyId& bodyID)
	{
		bodyProps.shape = BS_Box;
		bodyProps.type = b2_dynamicBody;	
		bodyProps.position = Vector2(0, 0);
		bodyProps.density = 1.0f;
		bodyProps.friction = 0.3f;
		std::vector<b2ShapeId> shapeId;

		CreateBody(bodyProps, bodyID, shapeId);
	}

	void Physics::DestroyBody(b2BodyId bodyID)
	{
		b2DestroyBody(bodyID);
	}

	void Physics::RecreateBody(BodyProps bodyProps, b2BodyId& bodyID, std::vector<b2ShapeId> shapeIDs)
	{
		DestroyBody(bodyID);
		CreateBody(bodyProps, bodyID, shapeIDs);
	}



	//b2Vec2 position = b2Body_GetPosition(F_Physics->m_dynamicID);
	//b2Rot rotation = b2Body_GetRotation(F_Physics->m_dynamicID);
	//LogVector2(Vector2(position.x, position.y), "Position: ");
	//LogFloat(b2Rot_GetAngle(rotation), "Rotation: ");
}