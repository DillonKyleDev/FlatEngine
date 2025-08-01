#include "Physics.h"
#include "FlatEngine.h"

#include <memory>
#include <vector>


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
		bodyDef.position = b2Vec2(bodyProps.position.x, bodyProps.position.y);
		float rotation = bodyProps.rotation;
		
		if (rotation < -179.99f)
		{
			bodyDef.rotation = b2MakeRot(DegreesToRadians(179.99f));
		}
		else if (rotation > 179.99f)
		{
			bodyDef.rotation = b2MakeRot(DegreesToRadians(-179.99f));
		}
		else
		{
			bodyDef.rotation = b2MakeRot(DegreesToRadians(rotation));
		}

		bodyDef.type = bodyProps.type;		
		bodyID = b2CreateBody(m_worldID, &bodyDef);
		b2ShapeDef shapeDef = b2DefaultShapeDef();
		shapeDef.density = bodyProps.density;
		shapeDef.material.friction = bodyProps.friction;

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
			circle.center = b2Vec2(bodyProps.position.x, bodyProps.position.y);
			circle.radius = bodyProps.radius;
			shapeID = b2CreateCircleShape(bodyID, &shapeDef, &circle);
			break;
		}
		case Physics::BodyShape::BS_Capsule:
		{
			b2Capsule capsule;
			capsule.center1 = b2Vec2(bodyProps.position.x, bodyProps.position.y);
			capsule.center2 = b2Vec2(bodyProps.position.x, bodyProps.position.y + bodyProps.capsuleLength - bodyProps.radius);
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
		bodyProps.type = b2_staticBody;		
		bodyProps.density = 1.0f;
		bodyProps.friction = 0.3f;

		//CreateBody(bodyProps, bodyID);
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