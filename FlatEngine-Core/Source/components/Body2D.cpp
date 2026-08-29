#include "components/Body2D.h"
#include "components/Transform.h"
#include "managers/SceneManager.h"
#include "physics/Shape.h"
#include "physics/Joint.h"
#include "managers/Assets.h"
#include "managers/LuaManager.h"
#include "physics/PhysicsManager.h"
#include "physics/Shape2D.h"
#include "render/SceneView.h"
#include "structs/SceneRenderObject.h"
#include "tools/Numbers.h"
#include "tools/Vector2.h"
#include "tools/Vector3.h"
#include "Types.h"

#include <box2d.h>
#include <math_functions.h>
#include <unordered_map>
#include <vector>


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

		for (Shape2D* shape : GetShapes())
		{
			shapesArray.push_back(shape->GetData());
		}

		json joints = json::array();

		for (Joint2D* joint : GetJoints())
		{
			joints.push_back(joint->GetData());
		}

		json componentJson = {
			{ "bodyType", (int)type },
			{ "b_lockedRotation", b_lockedRotation },
			{ "b_lockedXAxis", b_lockedXAxis },
			{ "b_lockedYAxis", b_lockedYAxis },
			{ "gravityScale", gravityScale },
			{ "linearDamping", linearDamping },
			{ "angularDamping", angularDamping },
			{ "shapes", shapesArray },
			{ "joints", joints }
		};
		componentJson.update(Component::GetData(b_IDOverride));
		
		return componentJson;
	}

	void Body2D::PutData(json componentJson, std::string objectName)
	{
		if (componentJson.empty())
		{	
			PhysicsManager::gamePhysics2D.CreateBody(this);
			return;
		}

        Component::PutData(componentJson, objectName);

		type = (b2BodyType)JsonHelper::CheckJsonInt(componentJson, "bodyType", objectName);
		b_lockedRotation = JsonHelper::CheckJsonBool(componentJson, "b_lockedRotation", objectName);
		b_lockedXAxis = JsonHelper::CheckJsonBool(componentJson, "b_lockedXAxis", objectName);
		b_lockedYAxis = JsonHelper::CheckJsonBool(componentJson, "b_lockedYAxis", objectName);
		gravityScale = JsonHelper::CheckJsonFloat(componentJson, "gravityScale", objectName);
		linearDamping = JsonHelper::CheckJsonFloat(componentJson, "linearDamping", objectName);
		angularDamping = JsonHelper::CheckJsonFloat(componentJson, "angularDamping", objectName);

		Transform* transform = SceneManager::loadedScene.Get<Transform>(GetOwnerID());
		Vector3 pos = transform->GetCleanPosition(); 
		Vector3 rot = transform->GetCleanRotation();
		position = Vector2(pos.x, pos.y);		
		rotation = b2MakeRot(Numbers::DegreesToRadians(rot.z));

		PhysicsManager::gamePhysics2D.CreateBody(this);
		
		if (JsonHelper::JsonContains(componentJson, "shapes", objectName))
		{
			for (int i = 0; i < componentJson.at("shapes").size(); i++)
			{
				json shapeJson = componentJson.at("shapes").at(i);
				ShapeType2D shapeType = GetTypeFromString<ShapeType2D>(ShapeType2DFromString, JsonHelper::CheckJsonString(shapeJson, "shapeType", objectName));
				AddShape(shapeType, shapeJson, objectName);
			}
		}			

		if (JsonHelper::JsonContains(componentJson, "joints", objectName))
		{		
			for (int i = 0; i < componentJson.at("joints").size(); i++)
			{
				json jointJson = componentJson.at("joints").at(i);
				JointType2D jointType = GetTypeFromString<JointType2D>(JointType2DFromString, JsonHelper::CheckJsonString(jointJson, "jointType", objectName));
				AddJoint(jointType, jointJson, objectName);
			}
		}
    }

	void Body2D::AddShape(ShapeType2D type, json componentJson, std::string name)
	{
		if (type == ShapeType2D_None)
			return;
		
		Shape2D shape = Shape2D(GetOwnerID(), type);
		shape.PutData(componentJson, name);

		switch (type)
		{
			case ShapeType2D_Box:     m_boxes.push_back(std::move(shape));    PhysicsManager::gamePhysics2D.CreateShape(&m_boxes.back(), this); break; 
			case ShapeType2D_Circle:  m_circles.push_back(std::move(shape));  PhysicsManager::gamePhysics2D.CreateShape(&m_circles.back(), this); break; 
			case ShapeType2D_Capsule: m_capsules.push_back(std::move(shape)); PhysicsManager::gamePhysics2D.CreateShape(&m_capsules.back(), this); break; 
			case ShapeType2D_Polygon: m_polygons.push_back(std::move(shape)); PhysicsManager::gamePhysics2D.CreateShape(&m_polygons.back(), this); break; 			
			case ShapeType2D_Chain:   m_chains.push_back(std::move(shape));   PhysicsManager::gamePhysics2D.CreateShape(&m_chains.back(), this); break; 
			default: break;
		}
	}

	void Body2D::AddJoint(JointType2D type, json componentJson, std::string name)
	{
		Joint2D joint = Joint2D(GetOwnerID(), SceneManager::loadedScene.GetNextJoint2DID(), type);
		joint.PutData(componentJson, name);

		switch (type)
		{
			case JointType2D_Distance:  m_distanceJoints.push_back(std::move(joint));  PhysicsManager::gamePhysics2D.CreateJoint(&m_distanceJoints.back()); break; 
			case JointType2D_Prismatic: m_prismaticJoints.push_back(std::move(joint)); PhysicsManager::gamePhysics2D.CreateJoint(&m_prismaticJoints.back()); break; 
			case JointType2D_Revolute:  m_revoluteJoints.push_back(std::move(joint));  PhysicsManager::gamePhysics2D.CreateJoint(&m_revoluteJoints.back()); break; 
			case JointType2D_Mouse: 	m_mouseJoints.push_back(std::move(joint));     PhysicsManager::gamePhysics2D.CreateJoint(&m_mouseJoints.back()); break; 		
			case JointType2D_Wheel: 	m_wheelJoints.push_back(std::move(joint));     PhysicsManager::gamePhysics2D.CreateJoint(&m_wheelJoints.back()); break; 
			case JointType2D_Motor:     m_motorJoints.push_back(std::move(joint));     PhysicsManager::gamePhysics2D.CreateJoint(&m_motorJoints.back()); break; 
			case JointType2D_Weld:      m_weldJoints.push_back(std::move(joint));      PhysicsManager::gamePhysics2D.CreateJoint(&m_weldJoints.back()); break; 
			default: break;
		}		
	}

	// This Body2D is a BodyB for Joint2D* joint and needs to be able to reference that joint when it is recreated or destroyed.
	void Body2D::AddConnectedJoint(Joint2D* joint)
	{
		if (joint == nullptr)
			return;

		if (m_jointsConnected.count(joint->GetID()) == 0)
		{
			m_jointsConnected.emplace(joint->GetID(), joint);
		}
	}
	void Body2D::RemoveConnectedJoint(Joint2D* joint)
	{
		if (joint == nullptr)
			return;

		if (m_jointsConnected.count(joint->GetID()))
		{
			m_jointsConnected.erase(joint->GetID());
		}
	}
	std::unordered_map<long, Joint2D*> Body2D::GetConnectedJoints()
	{
		return m_jointsConnected;
	}

	void Body2D::Cleanup()
	{
		m_boxes.clear();
		m_circles.clear();
		m_polygons.clear();
		m_capsules.clear();
		m_chains.clear();

		for (Shape2D* shape : GetShapes())
		{
			if (shape != nullptr)
				shape->Cleanup();
		}
		for (Joint2D* joint : GetJoints())
		{			
			if (joint != nullptr && joint->GetBodyB() != nullptr)
				joint->GetBodyB()->RemoveJoint(joint->GetID());
		}
		if (m_jointsConnected.size())
		{
			for (auto& joint : m_jointsConnected)
			{
				if (joint.second != nullptr && joint.second->GetBodyA() != nullptr)
					joint.second->GetBodyA()->RemoveJoint(joint.second->GetID());
			}
		}

		m_jointsConnected.clear();
		m_distanceJoints.clear();
        m_prismaticJoints.clear();
        m_revoluteJoints.clear();
        m_mouseJoints.clear();
        m_wheelJoints.clear();
        m_motorJoints.clear();
        m_weldJoints.clear();  

		PhysicsManager::gamePhysics2D.DestroyBody(m_bodyID);
		m_bodyID = b2_nullBodyId;
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
		if (!b2Body_IsValid(m_bodyID))
			return Vector2();

		return b2Body_GetLocalPoint(m_bodyID, b2Vec2(worldPoint.x, worldPoint.y));
	}

	Vector2 Body2D::ConvertLocalToWorldPoint(Vector2 localPoint)
	{
		if (!b2Body_IsValid(m_bodyID))
			return Vector2();

		return b2Body_GetWorldPoint(m_bodyID, b2Vec2(localPoint.x, localPoint.y));
	}

	Vector2 Body2D::ConvertWorldToLocalVector(Vector2 worldVector)
	{
		if (!b2Body_IsValid(m_bodyID))
			return Vector2();

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
		Body2D* caller = PhysicsManager::gamePhysics2D.GetBodyFromShapeID(myID);
		Body2D* collidedWith = PhysicsManager::gamePhysics2D.GetBodyFromShapeID(collidedWithID);
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
		Body2D* caller = PhysicsManager::gamePhysics2D.GetBodyFromShapeID(myID);
		Body2D* collidedWith = PhysicsManager::gamePhysics2D.GetBodyFromShapeID(collidedWithID);
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
		Body2D* caller = PhysicsManager::gamePhysics2D.GetBodyFromShapeID(myID);
		Body2D* touched = PhysicsManager::gamePhysics2D.GetBodyFromShapeID(touchedID);
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
		Body2D* caller = PhysicsManager::gamePhysics2D.GetBodyFromShapeID(myID);
		Body2D* touched = PhysicsManager::gamePhysics2D.GetBodyFromShapeID(touchedID);
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
		
		if (ownerTransform == nullptr)
			return;

		Vector3 ownerPos = ownerTransform->GetPosition();
		Vector3 ownerRot = ownerTransform->GetRotation();

		for (Shape2D* shape : GetShapes())
		{
			if (shape->renderShapes.size() == 0)
				return;
			
			std::string colliderString = shape->b_isSensor ? "sensor" : "Collider";
			std::string activeString = IsActive() ? "Active" : "Inactive";
			std::string postfix = colliderString + activeString;

			std::visit([this, shape, ownerPos, ownerRot, postfix](auto&& sData)
			{
				using T = std::decay_t<decltype(sData)>;

				if constexpr (std::is_same_v<T, BoxShape2DData>)
				{	
					Transform renderTransform;									
					renderTransform.SetPosition(Vector3(ownerPos.x + sData.offset.x, ownerPos.y + sData.offset.y, ownerPos.z));
					renderTransform.SetRotation(ownerRot);
					renderTransform.SetScale(Vector3(sData.dimensions.x, sData.dimensions.y, 1));
					shape->renderShapes[0].transform = renderTransform;
				}
				else if constexpr (std::is_same_v<T, CircleShape2DData>)
				{
					Transform renderTransform;						
					renderTransform.SetPosition(Vector3(ownerPos.x + sData.offset.x, ownerPos.y + sData.offset.y, ownerPos.z));
					renderTransform.SetScale(Vector3(sData.radius, sData.radius, 1));
					shape->renderShapes[0].transform = renderTransform;
				}
				else if constexpr (std::is_same_v<T, CapsuleShape2DData>)
				{
					if (shape->renderShapes.size() != 8)											
						return;					
									
					float center1Value = ((sData.length / 2) - sData.radius) * -1;
					float center2Value = (sData.length / 2) - sData.radius;
					b2Vec2 offset = b2Vec2(sData.offset.x, sData.offset.y);
					Vector2 center1 = b2Vec2(sData.b_horizontal ? center1Value : 0, sData.b_horizontal ? 0 : center1Value);
					Vector2 center2 = b2Vec2(sData.b_horizontal ? center2Value : 0, sData.b_horizontal ? 0 : center2Value);				
					Vector2 center1World = Vector2(b2Body_GetWorldPoint(m_bodyID, offset + b2Vec2(center1.x, center1.y)));
					Vector2 center2World = Vector2(b2Body_GetWorldPoint(m_bodyID, offset + b2Vec2(center2.x, center2.y)));
					Vector2 difference = center2World - center1World;
					Vector2 diffN = Vector2::Normalize(difference);
					Vector2 diffNR = diffN * sData.radius;
					Vector2 diffPerp = Vector2::Rotate(diffNR, 90);
					Vector2 flippedDiffPerp = Vector2::Rotate(diffNR, -90);
					
					// circles
					shape->renderShapes[0].transform.SetPosition(Vector3(center1World, ownerPos.z));
					shape->renderShapes[0].transform.SetScale(Vector3(sData.radius, sData.radius, 1));
					shape->renderShapes[1].transform.SetPosition(Vector3(center2World, ownerPos.z));
					shape->renderShapes[1].transform.SetScale(Vector3(sData.radius, sData.radius, 1));
					// Lines
					shape->renderShapes[2].transform = SceneView::GetLineTransformForStartEndPos(Vector3(center1World - diffNR, ownerPos.z), Vector3(center1World + diffNR, ownerPos.z));				
					shape->renderShapes[3].transform = SceneView::GetLineTransformForStartEndPos(Vector3(center2World - diffNR, ownerPos.z), Vector3(center2World + diffNR, ownerPos.z));				
					shape->renderShapes[4].transform = SceneView::GetLineTransformForStartEndPos(Vector3(center1World - diffPerp, ownerPos.z), Vector3(center1World + diffPerp, ownerPos.z));				
					shape->renderShapes[5].transform = SceneView::GetLineTransformForStartEndPos(Vector3(center2World - diffPerp, ownerPos.z), Vector3(center2World + diffPerp, ownerPos.z));				
					shape->renderShapes[6].transform = SceneView::GetLineTransformForStartEndPos(Vector3(center1World + diffPerp, ownerPos.z), Vector3(center1World + diffPerp + difference, ownerPos.z));												
					shape->renderShapes[7].transform = SceneView::GetLineTransformForStartEndPos(Vector3(center1World + flippedDiffPerp, ownerPos.z), Vector3(center1World + flippedDiffPerp + difference, ownerPos.z));									

					std::string shapeString = "capsule";
					Vector4 color = Assets::assetManager.GetColor(shapeString + postfix);
					Vector4 colorLight = Assets::assetManager.GetColor(shapeString + postfix + "Light");

					shape->renderShapes[0].mesh.SetUBOVec4("color", color);
					shape->renderShapes[1].mesh.SetUBOVec4("color", color);
					shape->renderShapes[2].mesh.SetUBOVec4("color", color);
					shape->renderShapes[3].mesh.SetUBOVec4("color", color);
					shape->renderShapes[4].mesh.SetUBOVec4("color", color);
					shape->renderShapes[5].mesh.SetUBOVec4("color", color);
					shape->renderShapes[6].mesh.SetUBOVec4("color", color);
					shape->renderShapes[7].mesh.SetUBOVec4("color", color);		
				}
				else if constexpr (std::is_same_v<T, PolygonShape2DData>)
				{
					if (shape->renderShapes.size() < sData.points.size())
					{
						int diff = sData.points.size() - shape->renderShapes.size();
						for (int i = 0; i < diff; i++)
						{
							shape->renderShapes.push_back(CreateLineObject());
						}
					}

					for (int p = 0; p < sData.points.size(); p++)
					{
						int pNext = p == sData.points.size() - 1 ? 0 : p + 1;
						b2Vec2 pointStart = b2Body_GetWorldPoint(m_bodyID, b2Vec2(sData.points[p].x, sData.points[p].y));
						Vector3 startPos = Vector3(pointStart, ownerPos.z);
						b2Vec2 pointEnd = b2Body_GetWorldPoint(m_bodyID, b2Vec2(sData.points[pNext].x, sData.points[pNext].y));
						Vector3 endPos = Vector3(pointEnd, ownerPos.z);

						shape->renderShapes[p].transform = SceneView::GetLineTransformForStartEndPos(startPos, endPos);
					}
				}
				else if constexpr (std::is_same_v<T, ChainShape2DData>)
				{
					if (shape->renderShapes.size() < sData.points.size())
					{
						int diff = sData.points.size() - shape->renderShapes.size();
						for (int i = 0; i < diff; i++)
						{
							shape->renderShapes.push_back(CreateLineObject());
						}
					}

					for (int p = 0; p < sData.points.size(); p++)
					{
						if (p == sData.points.size() - 1)
						{
							if (sData.b_isLoop)
							{
								shape->renderShapes[p].mesh.SetActive(true);
							}
							else 
							{
								shape->renderShapes[p].mesh.SetActive(false);
								return;
							}
						}

						int pNext = p == sData.points.size() - 1 ? 0 : p + 1;
						b2Vec2 pointStart = b2Body_GetWorldPoint(m_bodyID, b2Vec2(sData.points[p].x, sData.points[p].y));
						Vector3 startPos = Vector3(pointStart, ownerPos.z);
						b2Vec2 pointEnd = b2Body_GetWorldPoint(m_bodyID, b2Vec2(sData.points[pNext].x, sData.points[pNext].y));
						Vector3 endPos = Vector3(pointEnd, ownerPos.z);
						shape->renderShapes[p].transform = SceneView::GetLineTransformForStartEndPos(startPos, endPos);

						std::string color = "chain";
						if ((p == 0 || p == shape->renderShapes.size() - 2) && !sData.b_isLoop)
							color = "chainEndSegments";
						shape->renderShapes[p].mesh.SetUBOVec4("color", Assets::assetManager.GetColor(color + postfix));
					}
				}
			}, shape->shapeData);
		}

		for (Joint2D* joint : GetJoints())
		{
			if (joint->renderShapes.size() == 0)
				return;
			
			std::string postfix = IsActive() ? "Active" : "Inactive";			

			// Common to all Joints
			std::visit([this, joint, ownerPos, postfix](auto&& jData) 
			{
				if (joint->GetBodyB() == nullptr)
					return;
				
				Vector3 bodyAWorld = Vector3(b2Body_GetWorldPoint(m_bodyID, b2Vec2(joint->anchorA.x, joint->anchorA.y)), ownerPos.z);
				Vector3 bodyBWorld = Vector3(b2Body_GetWorldPoint(joint->GetBodyB()->GetBodyID(), b2Vec2(joint->anchorB.x, joint->anchorB.y)), ownerPos.z);
				joint->renderShapes[0].transform = SceneView::GetLineTransformForStartEndPos(bodyAWorld, bodyBWorld);
			}, joint->jointData);

			// std::visit([this, joint, ownerPos, postfix](auto&& jData)
			// {
			// 	using T = std::decay_t<decltype(jData)>;

			// 	if constexpr (std::is_same_v<T, DistanceJointData>)
			// 	{	
			// 	}
			// 	else if constexpr (std::is_same_v<T, RevoluteJointData>)
			// 	{
			// 	}
			// 	else if constexpr (std::is_same_v<T, PrismaticJointData>)
			// 	{		
			// 	}
			// 	else if constexpr (std::is_same_v<T, MouseJointData>)
			// 	{
			// 	}
			// 	else if constexpr (std::is_same_v<T, WeldJointData>)
			// 	{
			// 	}
			// 	else if constexpr (std::is_same_v<T, MotorJointData>)
			// 	{
			// 	}
			// 	else if constexpr (std::is_same_v<T, WheelJointData>)
			// 	{
			// 	}
			// }, joint->jointData);
		}
	}

	void Body2D::SetPosition(Vector2 newPosition)
	{
		position = newPosition;	
		Wake();
		b2Body_SetTransform(m_bodyID, Vector2::GetB2Vec2(position), GetB2Rotation());
	}

	Vector2 Body2D::GetPosition()
	{
		if (!b2Body_IsValid(m_bodyID))
			return Vector2();

		b2Vec2 b2Position = b2Body_GetPosition(m_bodyID);		
		return Vector2(b2Position.x, b2Position.y);
	}

	void Body2D::Wake()
	{
		b2Body_SetAwake(m_bodyID, true);
	}

	void Body2D::SetRotation(float newRotation)
	{
		Wake();
		b2Body_SetTransform(m_bodyID, Vector2::GetB2Vec2(position), b2MakeRot(Numbers::DegreesToRadians(newRotation)));		
	}

	void Body2D::SetRotation(b2Rot newRotation)
	{
		rotation = newRotation;
		RecreateBody();
	}

	// Returns rotation in degrees between -180 and 180
	float Body2D::GetRotation()
	{
		if (!b2Body_IsValid(m_bodyID))
			return 0;

		b2Rot bodyRotation = b2Body_GetRotation(m_bodyID);
		float rotation = Numbers::RadiansToDegrees(b2Rot_GetAngle(bodyRotation));

		return rotation;
	}

	b2Rot Body2D::GetB2Rotation()
	{
		if (!b2Body_IsValid(m_bodyID))
			return b2Rot(0);

		return b2Body_GetRotation(m_bodyID);
	}

	void Body2D::RecreateBody()
	{
		position = GetPosition();
		rotation = GetB2Rotation();
		PhysicsManager::gamePhysics2D.RecreateBody(this);
	}

	void Body2D::SetLockedRotation(bool b_lockRotation)
	{
		b_lockedRotation = b_lockRotation;
		RecreateBody();
	}

	void Body2D::SetLockedXAxis(bool b_lockXAxis)
	{
		b_lockedXAxis = b_lockXAxis;
		RecreateBody();
	}

	void Body2D::SetLockedYAxis(bool b_lockYAxis)
	{
		b_lockedYAxis = b_lockYAxis;
		RecreateBody();
	}

	void Body2D::SetGravityScale(float newGravityScale)
	{
		gravityScale = newGravityScale;
		RecreateBody();
	}

	void Body2D::SetLinearDamping(float newLinearDamping)
	{
		if (linearDamping >= 0)
		{
			linearDamping = newLinearDamping;
			RecreateBody();
		}
	}

	void Body2D::SetAngularDamping(float newAngularDamping)
	{
		if (angularDamping >= 0)
		{
			angularDamping = newAngularDamping;
			RecreateBody();
		}
	}

	void Body2D::SetBodyType(b2BodyType newType)
	{
		type = newType;
		RecreateBody();

		// If is KINEMATIC, you can drive the body to a specific transform (position and rotation) using:
		// b2Vec2 targetPosition = { x, y };
		// b2Rot targetRotation = b2MakeRot(B2_PI);
		// b2Transform target = {targetPosition, targetRotation};
		// b2Body_SetTargetTransform(m_bodyID, target, timeStep);
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
		if (!b2Body_IsValid(m_bodyID))
			return 0;

		return b2Body_GetAngularVelocity(m_bodyID);
	}

	const b2BodyId Body2D::GetBodyID()
	{
		return m_bodyID;
	}

	std::vector<Shape2D*> Body2D::GetShapes()
	{
		std::vector<Shape2D*> shapes = std::vector<Shape2D*>();
		
		for (Shape2D& shape : m_boxes)
		{
			shapes.push_back(&shape);
		}
		for (Shape2D& shape : m_circles)
		{
			shapes.push_back(&shape);
		}
		for (Shape2D& shape : m_capsules)
		{
			shapes.push_back(&shape);
		}
		for (Shape2D& shape : m_polygons)
		{
			shapes.push_back(&shape);
		}
		for (Shape2D& shape : m_chains)
		{
			shapes.push_back(&shape);
		}

		return shapes;
	}

	void Body2D::RemoveShape(b2ShapeId shapeID)
	{
		int toDelete = -1;
		int counter = 0;

		for (Shape2D& shape : m_boxes)
		{
			if (shape.GetShapeID().index1 == shapeID.index1)
			{
				PhysicsManager::gamePhysics2D.DestroyShape(&shape);
				toDelete = counter;
				break;
			}
		}
		if (toDelete != -1)
		{
			m_boxes.erase(std::next(m_boxes.begin(), toDelete));
			toDelete = -1;
		}
		for (Shape2D& shape : m_circles)
		{
			if (shape.GetShapeID().index1 == shapeID.index1)
			{
				PhysicsManager::gamePhysics2D.DestroyShape(&shape);
				toDelete = counter;
				break;
			}
		}
		if (toDelete != -1)
		{
			m_circles.erase(std::next(m_circles.begin(), toDelete));
			toDelete = -1;
		}
		for (Shape2D& shape : m_polygons)
		{
			if (shape.GetShapeID().index1 == shapeID.index1)
			{
				PhysicsManager::gamePhysics2D.DestroyShape(&shape);
				toDelete = counter;
				break;
			}
		}
		if (toDelete != -1)
		{
			m_polygons.erase(std::next(m_polygons.begin(), toDelete));
			toDelete = -1;
		}
		for (Shape2D& shape : m_capsules)
		{
			if (shape.GetShapeID().index1 == shapeID.index1)
			{
				PhysicsManager::gamePhysics2D.DestroyShape(&shape);
				toDelete = counter;
				break;
			}
		}
		if (toDelete != -1)
		{
			m_capsules.erase(std::next(m_capsules.begin(), toDelete));
			toDelete = -1;
		}
	}

	void Body2D::RemoveChain(b2ChainId chainID)
	{
		int toDelete = -1;
		int shapeCounter = 0;

		for (Shape2D& shape : m_chains)
		{
			if (shape.GetChainID().index1 == chainID.index1)
			{
				PhysicsManager::gamePhysics2D.DestroyShape(&shape);
				toDelete = shapeCounter;
			}
			shapeCounter++;
		}
		if (toDelete != -1)
		{
			m_chains.erase(std::next(m_chains.begin(), toDelete));
		}
	}

	std::vector<Joint2D*> Body2D::GetJoints()
	{
		std::vector<Joint2D*> joints = std::vector<Joint2D*>();

		for (Joint2D& joint : m_distanceJoints)
		{
			joints.push_back(&joint);
		}
		for (Joint2D& joint : m_prismaticJoints)
		{
			joints.push_back(&joint);
		}
		for (Joint2D& joint : m_revoluteJoints)
		{
			joints.push_back(&joint);
		}
		for (Joint2D& joint : m_mouseJoints)
		{
			joints.push_back(&joint);
		}
		for (Joint2D& joint : m_wheelJoints)
		{
			joints.push_back(&joint);
		}
		for (Joint2D& joint : m_motorJoints)
		{
			joints.push_back(&joint);
		}
		for (Joint2D& joint : m_weldJoints)
		{
			joints.push_back(&joint);
		}

		return joints;
	}

	// Todo: Simplify this somehow.
	void Body2D::RemoveJoint(long jointID)
	{
		if (m_jointsConnected.count(jointID))
		{
			m_jointsConnected.at(jointID)->Cleanup();				
			m_jointsConnected.erase(jointID);
		}

		for (std::list<Joint2D>::iterator jointIter = m_distanceJoints.begin(); jointIter != m_distanceJoints.end(); jointIter++)
		{
			if (jointIter->GetID() == jointID && jointIter->GetID() == jointID)
			{				
				jointIter->Cleanup();				
				m_distanceJoints.erase(jointIter);
				SceneManager::loadedScene.AddFreedJoint2DID(jointID);
				return;
			}
		}

		for (std::list<Joint2D>::iterator jointIter = m_prismaticJoints.begin(); jointIter != m_prismaticJoints.end(); jointIter++)
		{
			if (jointIter->GetID() == jointID && jointIter->GetID() == jointID)
			{
				if (jointIter->GetBodyB())
					jointIter->GetBodyB()->RemoveConnectedJoint(&(*jointIter));

				jointIter->Cleanup();	
				m_prismaticJoints.erase(jointIter);
				SceneManager::loadedScene.AddFreedJoint2DID(jointID);
				return;
			}
		}

		for (std::list<Joint2D>::iterator jointIter = m_revoluteJoints.begin(); jointIter != m_revoluteJoints.end(); jointIter++)
		{
			if (jointIter->GetID() == jointID && jointIter->GetID() == jointID)
			{
				if (jointIter->GetBodyB())
					jointIter->GetBodyB()->RemoveConnectedJoint(&(*jointIter));
				
				jointIter->Cleanup();	
				m_revoluteJoints.erase(jointIter);
				SceneManager::loadedScene.AddFreedJoint2DID(jointID);
				return;
			}
		}

		for (std::list<Joint2D>::iterator jointIter = m_weldJoints.begin(); jointIter != m_weldJoints.end(); jointIter++)
		{
			if (jointIter->GetID() == jointID && jointIter->GetID() == jointID)
			{
				if (jointIter->GetBodyB())
					jointIter->GetBodyB()->RemoveConnectedJoint(&(*jointIter));
				
				jointIter->Cleanup();	
				m_weldJoints.erase(jointIter);
				SceneManager::loadedScene.AddFreedJoint2DID(jointID);
				return;
			}
		}

		for (std::list<Joint2D>::iterator jointIter = m_motorJoints.begin(); jointIter != m_motorJoints.end(); jointIter++)
		{
			if (jointIter->GetID() == jointID && jointIter->GetID() == jointID)
			{
				if (jointIter->GetBodyB())
					jointIter->GetBodyB()->RemoveConnectedJoint(&(*jointIter));
				
				jointIter->Cleanup();	
				m_motorJoints.erase(jointIter);
				SceneManager::loadedScene.AddFreedJoint2DID(jointID);
				return;
			}
		}

		for (std::list<Joint2D>::iterator jointIter = m_wheelJoints.begin(); jointIter != m_wheelJoints.end(); jointIter++)
		{
			if (jointIter->GetID() == jointID && jointIter->GetID() == jointID)
			{
				if (jointIter->GetBodyB())
					jointIter->GetBodyB()->RemoveConnectedJoint(&(*jointIter));
				
				jointIter->Cleanup();	
				m_wheelJoints.erase(jointIter);
				SceneManager::loadedScene.AddFreedJoint2DID(jointID);
				return;
			}
		}

		for (std::list<Joint2D>::iterator jointIter = m_mouseJoints.begin(); jointIter != m_mouseJoints.end(); jointIter++)
		{
			if (jointIter->GetID() == jointID && jointIter->GetID() == jointID)
			{
				if (jointIter->GetBodyB())
					jointIter->GetBodyB()->RemoveConnectedJoint(&(*jointIter));
				
				jointIter->Cleanup();	
				m_mouseJoints.erase(jointIter);
				SceneManager::loadedScene.AddFreedJoint2DID(jointID);
				return;
			}
		}
	}

	void Body2D::RecreateJoints()
	{
		for (Joint2D& joint : m_distanceJoints)
		{
			PhysicsManager::gamePhysics2D.RecreateJoint(&joint);
		}
	}
}