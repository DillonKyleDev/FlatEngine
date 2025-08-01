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

	void Body::SetPosition(Vector2 position)
	{
		m_bodyProps.position = position;
		//b2Body_SetTransform(m_bodyID, b2Vec2(position.x, position.y), b2Rot(m_bodyProps.rotation));
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
		m_bodyProps.rotation;
		//b2Body_SetTransform(m_bodyID, b2Vec2(m_bodyProps.position.x, m_bodyProps.position.y), b2Rot(rotation));
		RecreateBody();
	}

	float Body::GetRotation()
	{
		b2Rot bodyRotation = b2Body_GetRotation(m_bodyID);
		float rotation = b2Rot_GetAngle(bodyRotation);

		return rotation;
	}

	void Body::SetBodyType(b2BodyType type)
	{
		m_bodyProps.type = type;
		RecreateBody();
	}

	void Body::SetDensity(float density)
	{
		m_bodyProps.density = density;
		RecreateBody();
	}

	void Body::SetFriction(float friction)
	{
		m_bodyProps.friction = friction;
		RecreateBody();
	}

	void Body::RecreateBody()
	{
		F_Physics->RecreateBody(m_bodyProps, m_bodyID);
	}

	Physics::BodyProps Body::GetBodyProps()
	{
		return m_bodyProps;
	}

	b2BodyId Body::GetBodyID()
	{
		return m_bodyID;
	}
}