#include "components/Body2D.h"
#include "components/Transform.h"
#include "managers/SceneManager.h"
#include "physics/Shape.h"
#include "physics/Joint.h"
#include "managers/LuaManager.h"
#include "physics/PhysicsManager.h"
#include "render/SceneView.h"
#include "tools/Numbers.h"
#include "tools/Vector3.h"


namespace FlatEngine
{
	Body2D::Body2D(long ownerID)
	{
		SetOwnerID(ownerID);
		SetType(ComponentType_Body2D);

		m_bodyID = b2BodyId();
		type = b2_dynamicBody;
		rotation = b2MakeRot(0);
		b_lockedRotation = false;
		b_lockedXAxis = false;
		b_lockedYAxis = false;
		gravityScale = 1.0f;
		linearDamping = 0.0f;
		angularDamping = 0.0f;

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
	}

	json Body2D::GetData(bool b_IDOverride)
	{
		json shapesArray = json::array();		

		for (Shape* shape : GetShapes())
		{
			shapesArray.push_back(shape->GetData());
		}

		json jsonData = {
			{ "type", (int)GetType() },
			{ "b_isCollapsed", IsCollapsed() },
			{ "b_isActive", IsActive() },
			{ "bodyType", (int)type },
			{ "b_lockedRotation", b_lockedRotation },
			{ "b_lockedXAxis", b_lockedXAxis },
			{ "b_lockedYAxis", b_lockedYAxis },
			{ "gravityScale", gravityScale },
			{ "linearDamping", linearDamping },
			{ "angularDamping", angularDamping },
			{ "shapes", shapesArray }
		};
		
		return jsonData;
	}

	void Body2D::PutData(json jsonData, std::string objectName)
	{
        Component::PutData(jsonData, objectName);

		type = (b2BodyType)JsonHelper::CheckJsonInt(jsonData, "bodyType", objectName);
		b_lockedRotation = JsonHelper::CheckJsonBool(jsonData, "b_lockedRotation", objectName);
		b_lockedXAxis = JsonHelper::CheckJsonBool(jsonData, "b_lockedXAxis", objectName);
		b_lockedYAxis = JsonHelper::CheckJsonBool(jsonData, "b_lockedYAxis", objectName);
		gravityScale = JsonHelper::CheckJsonFloat(jsonData, "gravityScale", objectName);
		linearDamping = JsonHelper::CheckJsonFloat(jsonData, "linearDamping", objectName);
		angularDamping = JsonHelper::CheckJsonFloat(jsonData, "angularDamping", objectName);

		Transform* transform = SceneManager::loadedScene.Get<Transform>(GetOwnerID());
		Vector3 pos = transform->GetCleanPosition(); 
		Vector3 rot = transform->GetCleanRotation();
		position = Vector2(pos.x, pos.y);		
		rotation = b2MakeRot(rot.z);

		PhysicsManager::physics2D.CreateBody(this);
		
		if (JsonHelper::JsonContains(jsonData, "shapes", objectName))
		{
			for (int i = 0; i < jsonData.at("shapes").size(); i++)
			{
				json shapeJson = jsonData.at("shapes").at(i);
				ShapeType shapeType = (JsonHelper::CheckJsonInt(jsonData, "type", objectName)) != -1 ? (ShapeType)(JsonHelper::CheckJsonInt(jsonData, "type", objectName)) : ShapeType_None;				
				Shape shape = Shape(GetOwnerID(), shapeType);
				shape.PutData(shapeJson, objectName);

				switch (shape.GetType())
				{
					case ShapeType_Box:	    boxes.push_back(shape);    PhysicsManager::physics2D.CreateShape(&boxes.back(), this); break;
					case ShapeType_Circle:  circles.push_back(shape);  PhysicsManager::physics2D.CreateShape(&circles.back(), this); break;
					case ShapeType_Capsule: capsules.push_back(shape); PhysicsManager::physics2D.CreateShape(&capsules.back(), this); break;
					case ShapeType_Polygon:	polygons.push_back(shape); PhysicsManager::physics2D.CreateShape(&polygons.back(), this); break;
					case ShapeType_Chain:   chains.push_back(shape);   PhysicsManager::physics2D.CreateShape(&chains.back(), this); break;
					default: break;
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
		Body2D* caller = PhysicsManager::physics2D.GetBodyFromShapeID(myID);
		Body2D* collidedWith = PhysicsManager::physics2D.GetBodyFromShapeID(collidedWithID);
		CallLuaCollisionFunction2D(LuaManager::LuaEventFunction::OnBeginCollision, caller, collidedWith, manifold);
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
		Body2D* caller = PhysicsManager::physics2D.GetBodyFromShapeID(myID);
		Body2D* collidedWith = PhysicsManager::physics2D.GetBodyFromShapeID(collidedWithID);
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
		Body2D* caller = PhysicsManager::physics2D.GetBodyFromShapeID(myID);
		Body2D* touched = PhysicsManager::physics2D.GetBodyFromShapeID(touchedID);
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
		Body2D* caller = PhysicsManager::physics2D.GetBodyFromShapeID(myID);
		Body2D* touched = PhysicsManager::physics2D.GetBodyFromShapeID(touchedID);
		CallLuaSensorFunction2D(LuaManager::LuaEventFunction::OnEndSensorTouch, caller, touched);
	}

	void Body2D::SetBodyID(b2BodyId bodyID)
	{
		m_bodyID = bodyID;
	}

	void Body2D::UpdateRenderShapes()
	{
		if (SceneManager::loadedScene.Get<Transform>(GetOwnerID()) == nullptr)
			return;

		Transform* ownerTransform = SceneManager::loadedScene.Get<Transform>(GetOwnerID());		

		for (Shape* shape : GetShapes())
		{
			if (shape->renderShapes.size() == 0)
				return;

			std::visit([this, shape, ownerTransform](auto&& sData)
			{
				using T = std::decay_t<decltype(sData)>;

				if constexpr (std::is_same_v<T, BoxShapeData>)
				{	
					Transform renderTransform;
					Vector3 ownerPos = ownerTransform->GetPosition();					
					renderTransform.SetPosition(Vector3(ownerPos.x + sData.offset.x, ownerPos.y + sData.offset.y, ownerPos.z));
					renderTransform.SetScale(Vector3(sData.dimensions.x, sData.dimensions.y, 1));
					shape->renderShapes[0].transform = renderTransform;
				}
				else if constexpr (std::is_same_v<T, CircleShapeData>)
				{
					Transform renderTransform;
					Vector3 ownerPos = ownerTransform->GetPosition();	
					renderTransform.SetPosition(Vector3(ownerPos.x + sData.offset.x, ownerPos.y + sData.offset.y, ownerPos.z));
					renderTransform.SetScale(Vector3(sData.radius, sData.radius, 1));
					shape->renderShapes[0].transform = renderTransform;
				}
				else if constexpr (std::is_same_v<T, CapsuleShapeData>)
				{
					// shape->renderShapes[0].transform = ComposeTransform(ownerTransform, sData.point1);
					// shape->renderShapes[1].transform = ComposeTransform(ownerTransform, sData.point2);
					// shape->renderShapes[2].transform = ComposeCapsuleBodyTransform(ownerTransform, sData.point1,sData.point2);
				}
				else if constexpr (std::is_same_v<T, PolygonShapeData>)
				{
					// for (size_t i = 0; i < sData.vertices.size(); i++)
					// {
					// 	size_t next = (i + 1) % sData.vertices.size();
					// 	shape->renderShapes[i].transform = ComposeSegmentTransform(bodyTransform, sData.vertices[i], sData.vertices[next]);
					// }
				}
				else if constexpr (std::is_same_v<T, ChainShapeData>)
				{
					// for (size_t i = 0; i < sData.vertices.size(); i++)
					// {
					// 	size_t next = (i + 1) % sData.vertices.size();
					// 	shape->renderShapes[i].transform = ComposeSegmentTransform(bodyTransform, sData.vertices[i], sData.vertices[next]);
					// }
				}
			}, shape->shapeData);
		}
	}

	std::vector<SceneView::SceneRenderObject> CreateCapsuleRenderObjects()
	{
		return std::vector<SceneView::SceneRenderObject>();
	}

	void Body2D::AddShape(ShapeType type)
	{
		Shape shape = Shape(GetOwnerID(), type);
		switch (type)
		{
			case ShapeType_Box:     { BoxShapeData shapeData;     shape.shapeData = shapeData; shape.renderShapes.push_back(SceneView::CreateQuadObject()); boxes.push_back(shape); PhysicsManager::physics2D.CreateShape(&boxes.back(), this); break; }
			case ShapeType_Circle:  { CircleShapeData shapeData;  shape.shapeData = shapeData; shape.renderShapes.push_back(SceneView::CreateCircleObject()); circles.push_back(shape); PhysicsManager::physics2D.CreateShape(&circles.back(), this); break; }
			case ShapeType_Capsule: { CapsuleShapeData shapeData; shape.shapeData = shapeData; shape.renderShapes.push_back(SceneView::CreateLineObject()); capsules.push_back(shape); PhysicsManager::physics2D.CreateShape(&capsules.back(), this); break; }
			case ShapeType_Polygon: { PolygonShapeData shapeData; shape.shapeData = shapeData; shape.renderShapes = CreateCapsuleRenderObjects(); polygons.push_back(shape); PhysicsManager::physics2D.CreateShape(&polygons.back(), this); break; }			
			case ShapeType_Chain:   { ChainShapeData shapeData;   shape.shapeData = shapeData; shape.renderShapes.push_back(SceneView::CreateLineObject()); chains.push_back(shape); PhysicsManager::physics2D.CreateShape(&chains.back(), this); break; }
			default: break;
		}
	}

	void Body2D::SetPosition(Vector2 newPosition)
	{
		position = newPosition;	
		b2Body_SetTransform(m_bodyID, Vector2::GetB2Vec2(position), GetB2Rotation());
		PhysicsManager::physics2D.RecreateBody(this);
	}

	Vector2 Body2D::GetPosition()
	{
		b2Vec2 b2Position = b2Body_GetPosition(m_bodyID);		
		return Vector2(b2Position.x, b2Position.y);
	}

	void Body2D::SetRotation(float newRotation)
	{
		rotation = b2MakeRot(Numbers::DegreesToRadians(newRotation));
		PhysicsManager::physics2D.RecreateBody(this);
	}

	void Body2D::SetRotation(b2Rot newRotation)
	{
		rotation = newRotation;
		PhysicsManager::physics2D.RecreateBody(this);
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

	void Body2D::SetLockedRotation(bool b_lockRotation)
	{
		b_lockedRotation = b_lockRotation;
		PhysicsManager::physics2D.RecreateBody(this);
	}

	void Body2D::SetLockedXAxis(bool b_lockXAxis)
	{
		b_lockedXAxis = b_lockXAxis;
		PhysicsManager::physics2D.RecreateBody(this);
	}

	void Body2D::SetLockedYAxis(bool b_lockYAxis)
	{
		b_lockedYAxis = b_lockYAxis;
		PhysicsManager::physics2D.RecreateBody(this);
	}

	void Body2D::SetGravityScale(float newGravityScale)
	{
		gravityScale = newGravityScale;
		PhysicsManager::physics2D.RecreateBody(this);
	}

	void Body2D::SetLinearDamping(float newLinearDamping)
	{
		if (linearDamping >= 0)
		{
			linearDamping = newLinearDamping;
			PhysicsManager::physics2D.RecreateBody(this);
		}
	}

	void Body2D::SetAngularDamping(float newAngularDamping)
	{
		if (angularDamping >= 0)
		{
			angularDamping = newAngularDamping;
			PhysicsManager::physics2D.RecreateBody(this);
		}
	}

	void Body2D::SetBodyType(b2BodyType newType)
	{
		type = newType;
		PhysicsManager::physics2D.RecreateBody(this);

		// If is KINEMATIC, you can drive the body to a specific transform (position and rotation) using:
		// b2Vec2 targetPosition = { x, y };
		// b2Rot targetRotation = b2MakeRot(B2_PI);
		// b2Transform target = {targetPosition, targetRotation};
		// b2Body_SetTargetTransform(m_bodyID, target, timeStep);
	}

	void Body2D::RecreateShapes()
	{
		for (Shape* shape : GetShapes())
		{
			PhysicsManager::physics2D.RecreateShape(shape);			
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
		
		for (Shape& shape : boxes)
		{
			shapes.push_back(&shape);
		}
		for (Shape& shape : circles)
		{
			shapes.push_back(&shape);
		}
		for (Shape& shape : capsules)
		{
			shapes.push_back(&shape);
		}
		for (Shape& shape : polygons)
		{
			shapes.push_back(&shape);
		}
		for (Shape& shape : chains)
		{
			shapes.push_back(&shape);
		}

		return shapes;
	}

	void Body2D::RemoveShape(b2ShapeId shapeID)
	{
		int toDelete = -1;
		int counter = 0;

		for (Shape& shape : boxes)
		{
			if (shape.GetShapeID().index1 == shapeID.index1)
			{
				PhysicsManager::physics2D.DestroyShape(&shape);
				toDelete = counter;
				break;
			}
		}
		if (toDelete != -1)
		{
			boxes.erase(std::next(boxes.begin(), toDelete));
			toDelete = -1;
		}
		for (Shape& shape : circles)
		{
			if (shape.GetShapeID().index1 == shapeID.index1)
			{
				PhysicsManager::physics2D.DestroyShape(&shape);
				toDelete = counter;
				break;
			}
		}
		if (toDelete != -1)
		{
			circles.erase(std::next(circles.begin(), toDelete));
			toDelete = -1;
		}
		for (Shape& shape : polygons)
		{
			if (shape.GetShapeID().index1 == shapeID.index1)
			{
				PhysicsManager::physics2D.DestroyShape(&shape);
				toDelete = counter;
				break;
			}
		}
		if (toDelete != -1)
		{
			polygons.erase(std::next(polygons.begin(), toDelete));
			toDelete = -1;
		}
		for (Shape& shape : capsules)
		{
			if (shape.GetShapeID().index1 == shapeID.index1)
			{
				PhysicsManager::physics2D.DestroyShape(&shape);
				toDelete = counter;
				break;
			}
		}
		if (toDelete != -1)
		{
			capsules.erase(std::next(capsules.begin(), toDelete));
			toDelete = -1;
		}
	}

	void Body2D::RemoveChain(b2ChainId chainID)
	{
		int toDelete = -1;
		int shapeCounter = 0;

		for (Shape& shape : chains)
		{
			if (shape.GetChainID().index1 == chainID.index1)
			{
				PhysicsManager::physics2D.DestroyShape(&shape);
				toDelete = shapeCounter;
			}
			shapeCounter++;
		}
		if (toDelete != -1)
		{
			chains.erase(std::next(chains.begin(), toDelete));
		}
	}

	void Body2D::Cleanup()
	{
		// for (Shape* shape : GetShapes())
		// {
		// 	PhysicsManager::physics2D.DestroyShape(shape);
		// }

		boxes.clear();
		circles.clear();
		polygons.clear();
		capsules.clear();
		chains.clear();

		PhysicsManager::physics2D.DestroyBody(m_bodyID);
		m_bodyID = b2_nullBodyId;
	}

	void Body2D::AddJoint(Joint* joint)
	{
		switch (joint->GetJointType())
		{
			case JointType_Distance: m_distanceJoints.push_back(joint);
			default: break;
		}
	}
}