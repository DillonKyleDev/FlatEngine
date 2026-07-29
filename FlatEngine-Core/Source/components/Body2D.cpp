#include "components/Body2D.h"
#include "joints/DistanceJoint.h"
#include "joints/Joint.h"
#include "managers/LuaManager.h"
#include "tools/Logger.h"
#include "tools/Numbers.h"


namespace FlatEngine
{
	Body2D::Body2D(long myID, long parentObjectID)
	{
		SetID(myID);
		SetParentObjectID(parentObjectID);
		SetType(ComponentType_Body2D);

		m_bodyID = b2BodyId();
		bodyProps = PhysicsManager::BodyProps();		
		m_boxes = std::list<Box>();
		m_circles = std::list<Circle>();
		m_capsules = std::list<Capsule>();
		m_polygons = std::list<Polygon>();
		m_chains = std::list<Chain>();
		// Contacts
		m_beginContactCallback = nullptr;
		m_b_beginContactCallbackSet = false;
		m_endContactCallback = nullptr;
		m_b_endContactCallbackSet = false;
		// Sensors
		m_beginSensorTouchCallback = nullptr;
		m_b_beginSensorTouchCallbackSet = false;
		m_endSensorTouchCallback = nullptr;
		m_b_endSensorTouchCallbackSet = false;

		m_distanceJoints = std::list<DistanceJoint*>();

		CreateBody();
	}

	json Body2D::GetData(bool b_IDOverride)
	{
		json shapesArray = json::array();		

		for (Shape* shape : GetShapes())
		{
			shapesArray.push_back(shape->GetShapeData());
		}

		json jsonData = {
			{ "type", (int)GetType() },
			{ "id", b_IDOverride ? -1 : GetID() },
			{ "b_isCollapsed", IsCollapsed() },
			{ "b_isActive", IsActive() },
			{ "bodyType", (int)bodyProps.type },
			{ "b_lockedRotation", bodyProps.b_lockedRotation },
			{ "b_lockedXAxis", bodyProps.b_lockedXAxis },
			{ "b_lockedYAxis", bodyProps.b_lockedYAxis },
			{ "gravityScale", bodyProps.gravityScale },
			{ "linearDamping", bodyProps.linearDamping },
			{ "angularDamping", bodyProps.angularDamping },
			{ "shapes", shapesArray }
		};
		
		return jsonData;
	}


	void RetrieveShapeProps(Shape::ShapeProps& shapeProps, json componentJson, std::string objectName)
	{
		shapeProps.shape = (Shape::ShapeType)(JsonHelper::CheckJsonInt(componentJson, "shape", objectName));
		shapeProps.b_enableContactEvents = JsonHelper::CheckJsonBool(componentJson, "b_enableContactEvents", objectName);
		shapeProps.b_enableSensorEvents = JsonHelper::CheckJsonBool(componentJson, "b_enableSensorEvents", objectName);
		shapeProps.b_isSensor = JsonHelper::CheckJsonBool(componentJson, "b_isSensor", objectName);
		shapeProps.positionOffset = Vector2(JsonHelper::CheckJsonFloat(componentJson, "xOffset", objectName), JsonHelper::CheckJsonFloat(componentJson, "yOffset", objectName));
		shapeProps.rotationOffset.c = JsonHelper::CheckJsonFloat(componentJson, "rotationOffsetCos", objectName);
		shapeProps.rotationOffset.s = JsonHelper::CheckJsonFloat(componentJson, "rotationOffsetSin", objectName);
		shapeProps.restitution = JsonHelper::CheckJsonFloat(componentJson, "restitution", objectName);
		shapeProps.density = JsonHelper::CheckJsonFloat(componentJson, "density", objectName);
		shapeProps.friction = JsonHelper::CheckJsonFloat(componentJson, "friction", objectName);
		shapeProps.dimensions = Vector2(JsonHelper::CheckJsonFloat(componentJson, "width", objectName), JsonHelper::CheckJsonFloat(componentJson, "height", objectName));
		shapeProps.cornerRadius = JsonHelper::CheckJsonFloat(componentJson, "cornerRadius", objectName);
		shapeProps.radius = JsonHelper::CheckJsonFloat(componentJson, "radius", objectName);
		shapeProps.capsuleLength = JsonHelper::CheckJsonFloat(componentJson, "capsuleLength", objectName);
		shapeProps.b_horizontal = JsonHelper::CheckJsonBool(componentJson, "b_horizontal", objectName);
		shapeProps.b_isLoop = JsonHelper::CheckJsonBool(componentJson, "b_isLoop", objectName);
		shapeProps.tangentSpeed = JsonHelper::CheckJsonFloat(componentJson, "tangentSpeed", objectName);
		shapeProps.rollingResistance = JsonHelper::CheckJsonFloat(componentJson, "rollingResistance", objectName);
		std::vector<Vector2> points = std::vector<Vector2>();

		for (int i = 0; i < componentJson.at("points").size(); i++)
		{
			try
			{
				json pointsJson = componentJson.at("points").at(i);
				Vector2 point = Vector2(JsonHelper::CheckJsonFloat(pointsJson, "xPos", objectName), JsonHelper::CheckJsonFloat(pointsJson, "yPos", objectName));
				points.push_back(point);
			}
			catch (const json::out_of_range& e)
			{
				Logger::log.Err("{}", e.what());
			}
		}

		shapeProps.points = points;
	}

	void RetrieveBodyProps(PhysicsManager::BodyProps& bodyProps, json componentJson, std::string objectName)
	{
		bodyProps.type = (b2BodyType)JsonHelper::CheckJsonInt(componentJson, "bodyType", objectName);
		bodyProps.b_lockedRotation = JsonHelper::CheckJsonBool(componentJson, "b_lockedRotation", objectName);
		bodyProps.b_lockedXAxis = JsonHelper::CheckJsonBool(componentJson, "b_lockedXAxis", objectName);
		bodyProps.b_lockedYAxis = JsonHelper::CheckJsonBool(componentJson, "b_lockedYAxis", objectName);
		bodyProps.gravityScale = JsonHelper::CheckJsonFloat(componentJson, "gravityScale", objectName);
		bodyProps.linearDamping = JsonHelper::CheckJsonFloat(componentJson, "linearDamping", objectName);
		bodyProps.angularDamping = JsonHelper::CheckJsonFloat(componentJson, "angularDamping", objectName);
	}

	void Body2D::PutData(json componentJson, std::string objectName)
	{
        Component::PutData(componentJson, objectName);

		RetrieveBodyProps(bodyProps, componentJson, objectName);
		
		if (JsonHelper::JsonContains(componentJson, "shapes", objectName))
		{
			for (int i = 0; i < componentJson.at("shapes").size(); i++)
			{
				json shapeJson = componentJson.at("shapes").at(i);
				Shape::ShapeProps shapeProps;
				RetrieveShapeProps(shapeProps, shapeJson, objectName);

				switch (shapeProps.shape)
				{
				case Shape::ShapeType_None:												
					break;
				case Shape::ShapeType_Box:
					AddBox(shapeProps);
					break;
				case Shape::ShapeType_Circle:
					AddCircle(shapeProps);
					break;
				case Shape::ShapeType_Capsule:
					AddCapsule(shapeProps);
					break;
				case Shape::ShapeType_Polygon:
					AddPolygon(shapeProps);
					break;
				case Shape::ShapeType_Chain:
					AddChain(shapeProps);
					break;
				}
			}
		}			
    }

	void Body2D::SetActive(bool b_isActive)
	{
		Component::SetActive(b_isActive);

		if (b2Body_IsValid(m_bodyID))
		{
			if (b_isActive)
			{
				b2Body_Enable(m_bodyID);
			}
			else
			{
				b2Body_Disable(m_bodyID);
			}
		}
	}

	// Conversions from local to world space
	Vector2 Body2D::ConvertWorldToLocalPoint(Vector2 worldPoint)
	{
		return b2Body_GetLocalPoint(m_bodyID, b2Vec2(worldPoint.x, worldPoint.y));
	}

	Vector2 Body2D::ConvertLocalToWorldPoint(Vector2 localPoint)
	{
		return b2Body_GetWorldPoint(m_bodyID, b2Vec2(localPoint.x, localPoint.y));
	}

	Vector2 Body2D::ConvertWorldToLocalVector(Vector2 worldVector)
	{
		return b2Body_GetLocalVector(m_bodyID, b2Vec2(worldVector.x, worldVector.y));
	}

	Vector2 Body2D::ConvertLocalToWorldVector(Vector2 localVector)
	{
		return b2Body_GetWorldVector(m_bodyID, b2Vec2(localVector.x, localVector.y));
	}

	void Body2D::SetOnBeginContact(void(*beginContactCallback)(b2Manifold manifold, b2ShapeId myID, b2ShapeId collidedWithID))
	{
		m_beginContactCallback = beginContactCallback;
		m_b_beginContactCallbackSet = true;
	}

	void Body2D::OnBeginContact(b2Manifold manifold, b2ShapeId myID, b2ShapeId collidedWithID)
	{		
		// C++ scripts
		if (m_b_beginContactCallbackSet)
		{
			m_beginContactCallback(manifold, myID, collidedWithID);		
		}
		// Lua scripts
		Body2D* caller = GetBodyFromShapeID(myID);
		Body2D* collidedWith = GetBodyFromShapeID(collidedWithID);
		CallLuaCollisionFunction2D(LuaManager::LuaEventFunction::OnBeginCollision, caller, collidedWith, manifold);
	}

	Body2D* Body2D::GetBodyFromShapeID(b2ShapeId shapeID)
	{
		Shape* shape = static_cast<Shape*>(b2Shape_GetUserData(shapeID));

		if (b2Shape_IsValid(shape->GetShapeID()))
		{
			return shape->GetParentBody();
		}
		else if (b2Chain_IsValid(shape->GetChainID()))
		{
			return shape->GetParentBody();
		}
		return nullptr;
	}

	void Body2D::SetOnEndContact(void(*endContactCallback)(b2ShapeId myID, b2ShapeId collidedWithID))
	{
		m_endContactCallback = endContactCallback;
		m_b_endContactCallbackSet = true;
	}

	void Body2D::OnEndContact(b2ShapeId myID, b2ShapeId collidedWithID)
	{	
		// C++ scripts
		if (m_b_endContactCallbackSet)
		{			
			m_endContactCallback(myID, collidedWithID);	
		}
		// Lua scripts
		Body2D* caller = GetBodyFromShapeID(myID);
		Body2D* collidedWith = GetBodyFromShapeID(collidedWithID);
		CallLuaCollisionFunction2D(LuaManager::LuaEventFunction::OnEndCollision, caller, collidedWith);
	}

	void Body2D::SetOnSensorBeginTouch(void (*beginSensorTouchCallback)(b2ShapeId myID, b2ShapeId touchedID))
	{
		m_beginSensorTouchCallback = beginSensorTouchCallback;
		m_b_beginSensorTouchCallbackSet = true;
	}

	void Body2D::OnSensorBeginTouch(b2ShapeId myID, b2ShapeId touchedID)
	{
		// C++ scripts
		if (m_b_beginSensorTouchCallbackSet)
		{
			m_beginSensorTouchCallback(myID, touchedID);
		}
		// Lua scripts
		Body2D* caller = GetBodyFromShapeID(myID);
		Body2D* touched = GetBodyFromShapeID(touchedID);
		CallLuaSensorFunction2D(LuaManager::LuaEventFunction::OnBeginSensorTouch, caller, touched);
	}

	void Body2D::SetOnSensorEndTouch(void (*endSensorTouchCallback)(b2ShapeId myID, b2ShapeId touchedID))
	{
		m_endSensorTouchCallback = endSensorTouchCallback;
		m_b_endSensorTouchCallbackSet = true;
	}

	void Body2D::OnSensorEndTouch(b2ShapeId myID, b2ShapeId touchedID)
	{
		// C++ scripts
		if (m_b_endSensorTouchCallbackSet)
		{
			m_endSensorTouchCallback(myID, touchedID);
		}
		// Lua scripts
		Body2D* caller = GetBodyFromShapeID(myID);
		Body2D* touched = GetBodyFromShapeID(touchedID);
		CallLuaSensorFunction2D(LuaManager::LuaEventFunction::OnEndSensorTouch, caller, touched);
	}


	PhysicsManager::BodyProps Body2D::GetLiveProps()
	{
		PhysicsManager::BodyProps liveProps;				
		liveProps.type = bodyProps.type;		
		liveProps.position = GetPosition();
		liveProps.rotation = GetB2Rotation();
		liveProps.b_lockedRotation = bodyProps.b_lockedRotation;
		liveProps.b_lockedXAxis = bodyProps.b_lockedXAxis;
		liveProps.b_lockedYAxis = bodyProps.b_lockedYAxis;
		liveProps.gravityScale = bodyProps.gravityScale;
		liveProps.linearDamping = bodyProps.linearDamping;
		liveProps.angularDamping = bodyProps.angularDamping;

		return liveProps;
	}

	void Body2D::SetBodyID(b2BodyId bodyID)
	{
		m_bodyID = bodyID;
	}

	void Body2D::SetPosition(Vector2 position)
	{
		bodyProps = GetLiveProps();
		bodyProps.position = position;	
		b2Body_SetTransform(m_bodyID, Vector2::GetB2Vev2(position), GetB2Rotation());
		RecreateBody();
	}

	Vector2 Body2D::GetPosition()
	{
		b2Vec2 b2Position = b2Body_GetPosition(m_bodyID);
		Vector2 position = Vector2(b2Position.x, b2Position.y);
		return position;
	}

	void Body2D::SetRotation(float rotation)
	{
		bodyProps = GetLiveProps();
		bodyProps.rotation = b2MakeRot(Numbers::DegreesToRadians(rotation));
		RecreateBody();
	}

	void Body2D::SetRotation(b2Rot rotation)
	{
		bodyProps = GetLiveProps();
		bodyProps.rotation = rotation;
		RecreateBody();
	}

	// Returns rotation in degrees between -180 and 180
	float Body2D::GetRotation()
	{
		b2Rot bodyRotation = b2Body_GetRotation(m_bodyID);
		float rotation = Numbers::RadiansToDegrees(b2Rot_GetAngle(bodyRotation));

		return rotation;
	}

	b2Rot Body2D::GetB2Rotation()
	{
		return b2Body_GetRotation(m_bodyID);
	}

	void Body2D::SetLockedRotation(bool b_lockedRotation)
	{
		bodyProps = GetLiveProps();
		bodyProps.b_lockedRotation = b_lockedRotation;
		RecreateBody();
	}

	void Body2D::SetLockedXAxis(bool b_lockedXAxis)
	{
		bodyProps = GetLiveProps();
		bodyProps.b_lockedXAxis = b_lockedXAxis;
		RecreateBody();
	}

	void Body2D::SetLockedYAxis(bool b_lockedYAxis)
	{
		bodyProps = GetLiveProps();
		bodyProps.b_lockedYAxis = b_lockedYAxis;
		RecreateBody();
	}

	void Body2D::SetGravityScale(float gravityScale)
	{
		bodyProps = GetLiveProps();
		bodyProps.gravityScale = gravityScale;
		RecreateBody();
	}

	void Body2D::SetLinearDamping(float linearDamping)
	{
		bodyProps = GetLiveProps();
		bodyProps.linearDamping = linearDamping;
		RecreateBody();
	}

	void Body2D::SetAngularDamping(float angularDamping)
	{
		bodyProps = GetLiveProps();
		bodyProps.angularDamping = angularDamping;
		RecreateBody();
	}

	void Body2D::SetBodyType(b2BodyType type)
	{
		bodyProps = GetLiveProps();
		bodyProps.type = type;
		RecreateBody();

		// If is KINEMATIC, you can drive the body to a specific transform (position and rotation) using:
		// b2Vec2 targetPosition = { x, y };
		// b2Rot targetRotation = b2MakeRot(B2_PI);
		// b2Transform target = {targetPosition, targetRotation};
		// b2Body_SetTargetTransform(m_bodyID, target, timeStep);
	}


	void Body2D::CreateBody()
	{
		PhysicsManager::physics2D.CreateBody(this);
	}

	void Body2D::RecreateBody()
	{
		PhysicsManager::physics2D.RecreateBody(this);
	}

	void Body2D::RecreateLiveBody()
	{
		bodyProps = GetLiveProps();
		RecreateBody();
	}

	void Body2D::RecreateShapes()
	{
		for (Shape* shape : GetShapes())
		{
			shape->CreateBodyShape();
		}
	}

	void Body2D::ApplyForce(Vector2 force, Vector2 worldPoint)
	{
		bool b_wake = true;
		b2Body_ApplyForce(m_bodyID, b2Vec2(force.x, force.y), b2Vec2(worldPoint.x, worldPoint.y), b_wake);
	}

	void Body2D::ApplyLinearInpulse(Vector2 impulse, Vector2 worldPoint)
	{
		bool b_wake = true;
		b2Body_ApplyLinearImpulse(m_bodyID, b2Vec2(impulse.x, impulse.y), b2Vec2(worldPoint.x, worldPoint.y), b_wake);
	}

	void Body2D::ApplyForceToCenter(Vector2 force)
	{
		bool b_wake = true;
		b2Body_ApplyForceToCenter(m_bodyID, b2Vec2(force.x, force.y), b_wake);
	}

	void Body2D::ApplyLinearImpulseToCenter(Vector2 impulse)
	{
		bool b_wake = true;
		b2Body_ApplyLinearImpulseToCenter(m_bodyID, b2Vec2(impulse.x, impulse.y), b_wake);
	}

	void Body2D::ApplyTorque(float torque)
	{
		bool b_wake = true;
		b2Body_ApplyTorque(m_bodyID, torque, b_wake);
	}

	void Body2D::ApplyAngularImpulse(float impulse)
	{
		bool b_wake = true;
		b2Body_ApplyAngularImpulse(m_bodyID, impulse, b_wake);
	}

	Vector2 Body2D::GetLinearVelocity()
	{		
		return Vector2(b2Body_GetLinearVelocity(m_bodyID));
	}

	float Body2D::GetAngularVelocity()
	{
		return b2Body_GetAngularVelocity(m_bodyID);
	}

	const b2BodyId Body2D::GetBodyID()
	{
		return m_bodyID;
	}

	std::vector<Shape*> Body2D::GetShapes()
	{
		std::vector<Shape*> shapes = std::vector<Shape*>();
		
		for (Box& shape : m_boxes)
		{
			shapes.push_back(&shape);
		}
		for (Circle& shape : m_circles)
		{
			shapes.push_back(&shape);
		}
		for (Capsule& shape : m_capsules)
		{
			shapes.push_back(&shape);
		}
		for (Polygon& shape : m_polygons)
		{
			shapes.push_back(&shape);
		}
		for (Chain& shape : m_chains)
		{
			shapes.push_back(&shape);
		}

		return shapes;
	}

	void Body2D::RemoveShape(b2ShapeId shapeID)
	{
		int toDelete = -1;
		int counter = 0;

		for (Box box : m_boxes)
		{
			if (box.GetShapeID().index1 == shapeID.index1)
			{
				box.DestroyShape();
				toDelete = counter;
				break;
			}
		}
		if (toDelete != -1)
		{
			m_boxes.erase(std::next(m_boxes.begin(), toDelete));
			toDelete = -1;
		}

		for (Circle circle : m_circles)
		{
			if (circle.GetShapeID().index1 == shapeID.index1)
			{
				circle.DestroyShape();
				toDelete = counter;
				break;
			}
		}
		if (toDelete != -1)
		{
			m_circles.erase(std::next(m_circles.begin(), toDelete));
			toDelete = -1;
		}

		for (Capsule capsule : m_capsules)
		{
			if (capsule.GetShapeID().index1 == shapeID.index1)
			{
				capsule.DestroyShape();
				toDelete = counter;
				break;
			}
		}
		if (toDelete != -1)
		{
			m_capsules.erase(std::next(m_capsules.begin(), toDelete));
			toDelete = -1;
		}

		for (Polygon polygon : m_polygons)
		{
			if (polygon.GetShapeID().index1 == shapeID.index1)
			{
				polygon.DestroyShape();
				toDelete = counter;
				break;
			}
		}
		if (toDelete != -1)
		{
			m_polygons.erase(std::next(m_polygons.begin(), toDelete));
			toDelete = -1;
		}
	}

	void Body2D::RemoveChain(b2ChainId chainID)
	{
		int toDelete = -1;
		int chainCounter = 0;

		for (Chain chain : m_chains)
		{
			if (chain.GetChainID().index1 == chainID.index1)
			{
				chain.DestroyShape();
				toDelete = chainCounter;
			}
			chainCounter++;
		}
		if (toDelete != -1)
		{
			m_chains.erase(std::next(m_chains.begin(), toDelete));
		}
	}

	std::list<Box>& Body2D::GetBoxes()
	{
		return m_boxes;
	}

	std::list<Circle>& Body2D::GetCircles()
	{
		return m_circles;
	}

	std::list<Capsule>& Body2D::GetCapsules()
	{
		return m_capsules;
	}

	std::list<Polygon>& Body2D::GetPolygons()
	{
		return m_polygons;
	}

	std::list<Chain>& Body2D::GetChains()
	{
		return m_chains;
	}

	void Body2D::Cleanup()
	{
		for (Shape* shape : GetShapes())
		{
			shape->DestroyShape();
		}

		PhysicsManager::physics2D.DestroyBody(m_bodyID);
		m_bodyID = b2_nullBodyId;
	}

	void Body2D::AddBox(Shape::ShapeProps shapeProps)
	{
		Box box = Box(this);
		if (shapeProps.shape != Shape::ShapeType_None)
		{
			box.SetShapeProps(shapeProps);
		}
		m_boxes.push_back(box);
		m_boxes.back().CreateBodyShape();
	}

	void Body2D::AddCircle(Shape::ShapeProps shapeProps)
	{
		Circle circle = Circle(this);
		if (shapeProps.shape != Shape::ShapeType_None)
		{
			circle.SetShapeProps(shapeProps);
		}		
		m_circles.push_back(circle);		
		m_circles.back().CreateBodyShape();
	}

	void Body2D::AddCapsule(Shape::ShapeProps shapeProps)
	{
		Capsule capsule = Capsule(this);
		if (shapeProps.shape != Shape::ShapeType_None)
		{
			capsule.SetShapeProps(shapeProps);
		}
		m_capsules.push_back(capsule);
		m_capsules.back().CreateBodyShape();
	}

	void Body2D::AddPolygon(Shape::ShapeProps shapeProps)
	{
		Polygon polygon = Polygon(this);
		if (shapeProps.shape != Shape::ShapeType_None)
		{
			polygon.SetShapeProps(shapeProps);
		}
		m_polygons.push_back(polygon);
		m_polygons.back().CreateBodyShape();
	}

	void Body2D::AddChain(Shape::ShapeProps shapeProps)
	{
		Chain chain = Chain(this);
		if (shapeProps.shape != Shape::ShapeType_None)
		{
			chain.SetShapeProps(shapeProps);
		}
		m_chains.push_back(chain);
		m_chains.back().CreateBodyShape();
	}

	void Body2D::AddJoint(Joint* joint)
	{
		switch (joint->GetJointType())
		{
		case Joint::JT_Distance:
		{
			m_distanceJoints.push_back(static_cast<DistanceJoint*>(joint));
		}
		}
	}





}