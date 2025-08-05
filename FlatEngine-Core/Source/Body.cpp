#include "Body.h"



namespace FlatEngine
{
	Body::Body(long myID, long parentID)
	{
		SetID(myID);
		SetParentID(parentID);

		m_bodyID = b2BodyId();
		m_bodyProps = Physics::BodyProps();
		m_beginContactCallback = nullptr;
		m_b_beginContactCallbackSet = false;
		m_endContactCallback = nullptr;
		m_b_endContactCallbackSet = false;
	}

	Body::~Body()
	{
	}


	void Body::SetOnBeginContact(void(*beginContactCallback)(b2Manifold manifold, b2ShapeId myID, b2ShapeId collidedWithID))
	{
		m_beginContactCallback = beginContactCallback;
		m_b_beginContactCallbackSet = true;
	}

	void Body::OnBeginContact(b2Manifold manifold, b2ShapeId myID, b2ShapeId collidedWithID)
	{		
		// C++ scripts
		if (m_b_beginContactCallbackSet)
		{
			m_beginContactCallback(manifold, myID, collidedWithID);		
		}
		// Lua scripts
		Body* caller = GetBodyFromShapeID(myID);
		Body* collidedWith = GetBodyFromShapeID(collidedWithID);
		CallLuaCollisionFunction(LuaEventFunction::OnBeginCollision, caller, collidedWith, manifold);
	}

	Body* Body::GetBodyFromShapeID(b2ShapeId shapeID)
	{
		return static_cast<Body*>(b2Shape_GetUserData(shapeID));
	}

	void Body::SetOnEndContact(void(*endContactCallback)(b2ShapeId myID, b2ShapeId collidedWithID))
	{
		m_endContactCallback = endContactCallback;
		m_b_endContactCallbackSet = true;
	}

	void Body::OnEndContact(b2ShapeId myID, b2ShapeId collidedWithID)
	{	
		// C++ scripts
		if (m_b_endContactCallbackSet)
		{			
			m_endContactCallback(myID, collidedWithID);	
		}
		// Lua scripts
		Body* caller = GetBodyFromShapeID(myID);
		Body* collidedWith = GetBodyFromShapeID(collidedWithID);
		CallLuaCollisionFunction(LuaEventFunction::OnEndCollision, caller, collidedWith);
	}

	Physics::BodyProps Body::GetLiveProps()
	{
		Physics::BodyProps liveProps;		
		liveProps.b_isEnabled = IsActive();
		liveProps.shape = m_bodyProps.shape;
		liveProps.type = m_bodyProps.type;
		liveProps.position = GetPosition();
		liveProps.rotation = GetB2Rotation();
		liveProps.b_lockedRotation = m_bodyProps.b_lockedRotation;
		liveProps.b_lockedXAxis = m_bodyProps.b_lockedXAxis;
		liveProps.b_lockedYAxis = m_bodyProps.b_lockedYAxis;
		liveProps.gravityScale = m_bodyProps.gravityScale;
		liveProps.linearDamping = m_bodyProps.linearDamping;
		liveProps.angularDamping = m_bodyProps.angularDamping;
		liveProps.restitution = m_bodyProps.restitution;
		liveProps.density = m_bodyProps.density;
		liveProps.friction = m_bodyProps.friction;
		liveProps.dimensions = m_bodyProps.dimensions;
		liveProps.radius = m_bodyProps.radius;
		liveProps.capsuleLength = m_bodyProps.capsuleLength;
		liveProps.b_horizontal = m_bodyProps.b_horizontal;
		liveProps.points = m_bodyProps.points;	
		liveProps.b_isLoop = m_bodyProps.b_isLoop;
		liveProps.tangentSpeed = m_bodyProps.tangentSpeed;
		liveProps.rollingResistance = m_bodyProps.rollingResistance;

		return liveProps;
	}

	void Body::SetBodyID(b2BodyId bodyID)
	{
		m_bodyID = bodyID;
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
		m_bodyProps.rotation = b2MakeRot(DegreesToRadians(rotation));
		RecreateBody();
	}

	void Body::SetRotation(b2Rot rotation)
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

	b2Rot Body::GetB2Rotation()
	{
		return b2Body_GetRotation(m_bodyID);
	}

	void Body::SetLockedRotation(bool b_lockedRotation)
	{
		m_bodyProps = GetLiveProps();
		m_bodyProps.b_lockedRotation = b_lockedRotation;
		RecreateBody();
	}

	void Body::SetLockedXAxis(bool b_lockedXAxis)
	{
		m_bodyProps = GetLiveProps();
		m_bodyProps.b_lockedXAxis = b_lockedXAxis;
		RecreateBody();
	}

	void Body::SetLockedYAxis(bool b_lockedYAxis)
	{
		m_bodyProps = GetLiveProps();
		m_bodyProps.b_lockedYAxis = b_lockedYAxis;
		RecreateBody();
	}

	void Body::SetGravityScale(float gravityScale)
	{
		m_bodyProps = GetLiveProps();
		m_bodyProps.gravityScale = gravityScale;
		RecreateBody();
	}

	void Body::SetLinearDamping(float linearDamping)
	{
		m_bodyProps = GetLiveProps();
		m_bodyProps.linearDamping = linearDamping;
		RecreateBody();
	}

	void Body::SetAngularDamping(float angularDamping)
	{
		m_bodyProps = GetLiveProps();
		m_bodyProps.angularDamping = angularDamping;
		RecreateBody();
	}

	void Body::SetRestitution(float restitution)
	{
		m_bodyProps = GetLiveProps();
		m_bodyProps.restitution = restitution;
		RecreateBody();
	}

	void Body::SetBodyType(b2BodyType type)
	{
		m_bodyProps = GetLiveProps();
		m_bodyProps.type = type;
		RecreateBody();

		// If is KINEMATIC, you can drive the body to a specific transform (position and rotation) using:
		// b2Vec2 targetPosition = { x, y };
		// b2Rot targetRotation = b2MakeRot(B2_PI);
		// b2Transform target = {targetPosition, targetRotation};
		// b2Body_SetTargetTransform(m_bodyID, target, timeStep);
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

	void Body::CreateBody()
	{
		F_Physics->CreateBody(this);
	}

	void Body::RecreateBody()
	{
		F_Physics->RecreateBody(this);
	}

	void Body::RecreateLiveBody()
	{
		m_bodyProps = GetLiveProps();
		RecreateBody();
	}

	// Conversions from local to world space:
	// b2Vec2 worldPoint = b2Body_GetWorldPoint(m_bodyID, localPoint);
	// b2Vec2 worldVector = b2Body_GetWorldVector(m_bodyID, localVector);
	// b2Vec2 localPoint = b2Body_GetLocalPoint(m_bodyID, worldPoint);
	// b2Vec2 localVector = b2Body_GetLocalVector(m_bodyID, worldVector);
	void Body::ApplyForce(Vector2 force, Vector2 worldPoint)
	{
		bool b_wake = true;
		b2Body_ApplyForce(m_bodyID, b2Vec2(force.x, force.y), b2Vec2(worldPoint.x, worldPoint.y), b_wake);
	}

	void Body::ApplyLinearInpulse(Vector2 impulse, Vector2 worldPoint)
	{
		bool b_wake = true;
		b2Body_ApplyLinearImpulse(m_bodyID, b2Vec2(impulse.x, impulse.y), b2Vec2(worldPoint.x, worldPoint.y), b_wake);
	}

	void Body::ApplyForceToCenter(Vector2 force)
	{
		bool b_wake = true;
		b2Body_ApplyForceToCenter(m_bodyID, b2Vec2(force.x, force.y), b_wake);
	}

	void Body::ApplyLinearImpulseToCenter(Vector2 impulse)
	{
		bool b_wake = true;
		b2Body_ApplyLinearImpulseToCenter(m_bodyID, b2Vec2(impulse.x, impulse.y), b_wake);
	}

	void Body::ApplyTorque(float torque)
	{
		bool b_wake = true;
		b2Body_ApplyTorque(m_bodyID, torque, b_wake);
	}

	void Body::ApplyAngularImpulse(float impulse)
	{
		bool b_wake = true;
		b2Body_ApplyAngularImpulse(m_bodyID, impulse, b_wake);
	}

	Vector2 Body::GetLinearVelocity()
	{		
		return Vector2(b2Body_GetLinearVelocity(m_bodyID));
	}

	float Body::GetAngularVelocity()
	{
		return b2Body_GetAngularVelocity(m_bodyID);
	}

	void Body::SetBodyProps(Physics::BodyProps bodyProps)
	{
		m_bodyProps = bodyProps;		
	}

	Physics::BodyProps& Body::GetBodyProps()
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

	void Body::SetChainID(b2ChainId chainID)
	{
		m_chainID = chainID;
	}

	void Body::AddShapeID(b2ShapeId shapeID)
	{
		m_shapeIDs.push_back(shapeID);
	}

	void Body::CleanupIDs()
	{
		m_bodyID = b2_nullBodyId;
		m_shapeIDs.clear();
		m_chainID = b2_nullChainId;
	}
}