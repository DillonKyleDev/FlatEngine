#include "Physics.h"
#include <memory>


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

		BodyProps staticBodyProps;
		staticBodyProps.type = b2_staticBody;		
		staticBodyProps.position = Vector2(0, 0);
		staticBodyProps.dimensions = Vector2(50.0f, 10.0f);
		
		BodyProps bodyProps;
		bodyProps.type = b2_dynamicBody;		
		bodyProps.position = Vector2(0, 10.0f);
		bodyProps.dimensions = Vector2(1.0f, 1.0f);
		bodyProps.density = 1.0f;
		bodyProps.friction = 0.3f;
		
		//b2BodyId staticID;
		//b2BodyId dynamicID;

		//CreateBody(staticBodyProps, staticID);
		//CreateBody(bodyProps, dynamicID);

		//m_dynamicID = dynamicID;
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

	void Physics::CreateBody(BodyProps bodyProps, b2BodyId& bodyID)
	{
		b2BodyDef bodyDef = b2DefaultBodyDef();
		bodyDef.position = b2Vec2(bodyProps.position.x, bodyProps.position.y);
		bodyDef.rotation = b2MakeRot(bodyProps.rotation);
		bodyDef.type = bodyProps.type;		
		bodyID = b2CreateBody(m_worldID, &bodyDef);
		b2Polygon box = b2MakeBox(bodyProps.dimensions.x / 2, bodyProps.dimensions.y / 2);
		b2ShapeDef shapeDef = b2DefaultShapeDef();
		shapeDef.density = bodyProps.density;
		shapeDef.material.friction = bodyProps.friction;
		b2CreatePolygonShape(bodyID, &shapeDef, &box);
	}

	void Physics::CreateBox(BodyProps bodyProps, b2BodyId& bodyID)
	{
		bodyProps.type = b2_staticBody;		
		bodyProps.density = 1.0f;
		bodyProps.friction = 0.3f;

		CreateBody(bodyProps, bodyID);
	}

	void Physics::DestroyBody(b2BodyId bodyID)
	{
		b2DestroyBody(bodyID);
	}

	void Physics::RecreateBody(BodyProps bodyProps, b2BodyId& bodyID)
	{
		DestroyBody(bodyID);
		CreateBody(bodyProps, bodyID);
	}



	//b2Vec2 position = b2Body_GetPosition(F_Physics->m_dynamicID);
	//b2Rot rotation = b2Body_GetRotation(F_Physics->m_dynamicID);
	//LogVector2(Vector2(position.x, position.y), "Position: ");
	//LogFloat(b2Rot_GetAngle(rotation), "Rotation: ");
}