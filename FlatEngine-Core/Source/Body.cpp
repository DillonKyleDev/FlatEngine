#include "Body.h"



namespace FlatEngine
{
	Body::Body(long myID, long parentID)
	{
		SetID(myID);
		SetParentID(parentID);

		m_bodyID = b2BodyId();
		m_bodyProps = Physics::BodyProps();
	}

	Body::~Body()
	{
	}


	Physics::BodyProps Body::GetLiveProps()
	{
		Physics::BodyProps liveProps;
		liveProps.type = m_bodyProps.type;
		liveProps.position = GetPosition();
		liveProps.rotation = GetRotation();
		liveProps.dimensions = m_bodyProps.dimensions;
		liveProps.density = m_bodyProps.density;
		liveProps.friction = m_bodyProps.friction;
		return liveProps;
	}

	void Body::SetPosition(Vector2 position)
	{
		m_bodyProps = GetLiveProps();
		m_bodyProps.position = position;				
		RecreateBody();
	}

	Vector2 Body::GetPosition()
	{
		b2Vec2 b2Position = b2Body_GetPosition(m_bodyID);
		Vector2 position = Vector2(b2Position.x, b2Position.y);
		return position;
	}

	void Body::SetRotation(float rotation)
	{
		m_bodyProps = GetLiveProps();
		m_bodyProps.rotation = rotation;		
		RecreateBody();
	}

	// Returns rotation in degrees between -180 and 180
	float Body::GetRotation()
	{
		b2Rot bodyRotation = b2Body_GetRotation(m_bodyID);
		float rotation = RadiansToDegrees(b2Rot_GetAngle(bodyRotation));

		return rotation;
	}

	void Body::SetBodyType(b2BodyType type)
	{
		m_bodyProps = GetLiveProps();
		m_bodyProps.type = type;
		RecreateBody();
	}

	void Body::SetDensity(float density)
	{
		m_bodyProps = GetLiveProps();
		m_bodyProps.density = density;
		RecreateBody();
	}

	void Body::SetFriction(float friction)
	{
		m_bodyProps = GetLiveProps();
		m_bodyProps.friction = friction;
		RecreateBody();
	}

	void Body::RecreateBody()
	{
		F_Physics->RecreateBody(m_bodyProps, m_bodyID, m_shapeIDs);
	}

	Physics::BodyProps Body::GetBodyProps()
	{
		return m_bodyProps;
	}

	b2BodyId Body::GetBodyID()
	{
		return m_bodyID;
	}

	std::vector<b2ShapeId> Body::GetShapeIDs()
	{
		return m_shapeIDs;
	}

	void Body::AddShapeID(b2ShapeId shapeID)
	{
		m_shapeIDs.push_back(shapeID);
	}
}