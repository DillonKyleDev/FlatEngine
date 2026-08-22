#include "components/Body2D.h"
#include "components/Transform.h"
#include "managers/Assets.h"
#include "managers/SceneManager.h"
#include "physics/PhysicsManager.h"
#include "physics/Shape2D.h"
#include "tools/JsonHelper.h"
#include <box2d.h>


namespace FlatEngine
{
	Shape2D::Shape2D(long ownerID, ShapeType2D shapeType)
	{		
		m_ownerID = ownerID;
		type = shapeType;
		m_shapeID = b2_nullShapeId;
		m_chainID = b2_nullChainId;
		b_isCollapsed = false;
		b_enableContactEvents = true;
		b_enableSensorEvents = true;
		b_isSensor = false;
		restitution = 0.3f;
		density = 1.0f;
		friction = 0.3f;
		tangentSpeed = 0.0f;
		rollingResistance = 0.0f;
		b_drawInGame = false;
		inGameDrawColor = Assets::assetManager.GetColor("boxColliderActive");
		inGameDrawThickness = 2.0f;
	}

	ShapeType2D Shape2D::GetType()
	{
		return type;
	}

	json Shape2D::GetData()
	{
		json shapeJson = {
			{ "shapeType", ShapeType2DStrings[(int)type] },
			{ "b_isCollapsed", b_isCollapsed },
			{ "b_enableContactEvents", b_enableContactEvents },
			{ "b_enableSensorEvents", b_enableSensorEvents },
			{ "b_isSensor", b_isSensor },
			{ "restitution", restitution },
			{ "density", density },
			{ "friction", friction },
			{ "tangentSpeed", tangentSpeed },
			{ "rollingResistance", rollingResistance }	
		};
		shapeJson["shapeData"] = std::visit([](auto&& sData) { return sData.GetData(); }, shapeData);

		return shapeJson;
	}

	void Shape2D::PutData(json shapeJson, std::string name)
	{
		json shapeDataJson = json::object();
		if (!shapeJson.empty() && JsonHelper::JsonContains(shapeJson, "shapeData", name))		
			shapeDataJson = shapeJson.at("shapeData");

		switch (type)
		{
			case ShapeType2D_Box:     shapeData = BoxShape2DData();     renderShapes.push_back(SceneView::CreateQuadObject()); break; 
			case ShapeType2D_Circle:  shapeData = CircleShape2DData();  renderShapes.push_back(SceneView::CreateCircleObject());  break; 
			case ShapeType2D_Capsule: shapeData = CapsuleShape2DData(); renderShapes = SceneView::CreateCapsuleObject(); break; 
			case ShapeType2D_Polygon: shapeData = PolygonShape2DData(); renderShapes = SceneView::CreatePolygonObject(); break; 			
			case ShapeType2D_Chain:   shapeData = ChainShape2DData();   renderShapes = SceneView::CreateChainObject(); break;
			default: break;
		}
		std::visit([shapeDataJson, name](auto&& sData) { sData.PutData(shapeDataJson, name); }, shapeData);

		if (shapeJson.empty())
			return;

		b_isCollapsed = JsonHelper::CheckJsonBool(shapeJson, "b_isCollapsed", name);
		b_enableContactEvents = JsonHelper::CheckJsonBool(shapeJson, "b_enableContactEvents", name);
		b_enableSensorEvents = JsonHelper::CheckJsonBool(shapeJson, "b_enableSensorEvents", name);
		b_isSensor = JsonHelper::CheckJsonBool(shapeJson, "b_isSensor", name);				
		restitution = JsonHelper::CheckJsonFloat(shapeJson, "restitution", name);
		density = JsonHelper::CheckJsonFloat(shapeJson, "density", name);
		friction = JsonHelper::CheckJsonFloat(shapeJson, "friction", name);
		tangentSpeed = JsonHelper::CheckJsonFloat(shapeJson, "tangentSpeed", name);
		rollingResistance = JsonHelper::CheckJsonFloat(shapeJson, "rollingResistance", name);
	}	

	void Shape2D::Cleanup()
	{
		PhysicsManager::gamePhysics2D.DestroyShape(this);
	}

	void Shape2D::SetShapeID(b2ShapeId shapeID)
	{
		m_shapeID = shapeID;
		std::visit([shapeID](auto&& sData) { sData.shapeID = shapeID; }, shapeData);
	}

	const b2ShapeId Shape2D::GetShapeID()
	{
		return m_shapeID;
	}

	void Shape2D::SetChainID(b2ChainId chainID)
	{
		m_chainID = chainID;
	}

	const b2ChainId Shape2D::GetChainID()
	{
		return m_chainID;
	}

	const long Shape2D::GetOwnerID()
	{
		return m_ownerID;
	}

	const b2BodyId Shape2D::Getb2BodyID()
	{
		return SceneManager::loadedScene.GetObjectByID(m_ownerID)->Get<Body2D>()->GetBodyID();
	}

	void Shape2D::SetIsSensor(bool b_setIsSensor)
	{
		b_isSensor = b_setIsSensor;
		PhysicsManager::gamePhysics2D.RecreateShape(this);
	}

	void Shape2D::SetEnableSensorEvents(bool b_setEnableSensorEvents)
	{		
		b_enableSensorEvents = b_setEnableSensorEvents;
		PhysicsManager::gamePhysics2D.RecreateShape(this);
	}

	void Shape2D::SetEnableContactEvents(bool b_setEnableContactEvents)
	{
		b_enableContactEvents = b_setEnableContactEvents;
		PhysicsManager::gamePhysics2D.RecreateShape(this);
	}

	void Shape2D::SetDensity(float newDensity)
	{		
		density = newDensity;
		PhysicsManager::gamePhysics2D.RecreateShape(this);
	}

	void Shape2D::SetFriction(float newFriction)
	{		
		friction = newFriction;
		PhysicsManager::gamePhysics2D.RecreateShape(this);
	}

	void Shape2D::SetRestitution(float newRestitution)
	{		
		restitution = newRestitution;
		PhysicsManager::gamePhysics2D.RecreateShape(this);
	}

	void Shape2D::SetTangentSpeed(float newTangentSpeed)
	{		
		if (newTangentSpeed >= 0)
		{
			tangentSpeed = newTangentSpeed;
			PhysicsManager::gamePhysics2D.RecreateShape(this);
		}
	}
	void Shape2D::SetRollingResistance(float newRrollingResistance)
	{		
		if (newRrollingResistance >= 0)
		{
			rollingResistance = newRrollingResistance;
			PhysicsManager::gamePhysics2D.RecreateShape(this);
		}
	}

	bool Shape2D::PointInShape(Vector2 point)
	{
		if (!b2Shape_IsValid(m_shapeID))
			return false;

		switch (type)
		{
		case ShapeType2D::ShapeType2D_Box:
		{
			b2Polygon box = b2Shape_GetPolygon(m_shapeID);
			return b2PointInPolygon(&box, Vector2::GetB2Vec2(point));
		}
		case ShapeType2D::ShapeType2D_Circle:
		{
			b2Circle circle = b2Shape_GetCircle(m_shapeID);
			return b2PointInCircle(&circle, Vector2::GetB2Vec2(point));
		}
		case ShapeType2D::ShapeType2D_Capsule:
		{
			b2Capsule capsule = b2Shape_GetCapsule(m_shapeID);
			return b2PointInCapsule(&capsule, Vector2::GetB2Vec2(point));
		}
		case ShapeType2D::ShapeType2D_Polygon:
		{
			b2Polygon polygon = b2Shape_GetPolygon(m_shapeID);
			return b2PointInPolygon(&polygon, Vector2::GetB2Vec2(point));
		}
		default:
			return false;
		}
	}

	b2CastOutput Shape2D::CastRayAt(b2RayCastInput* rayCastInput)
	{
		if (!b2Shape_IsValid(m_shapeID))
			return b2CastOutput();

		switch (type)
		{
		case ShapeType2D::ShapeType2D_Box:
		{
			b2Polygon box = b2Shape_GetPolygon(m_shapeID);
			return b2RayCastPolygon(&box, rayCastInput);
		}
		case ShapeType2D::ShapeType2D_Circle:
		{					
			b2Circle circle = b2Shape_GetCircle(m_shapeID);
			return b2RayCastCircle(&circle, rayCastInput);
		}
		case ShapeType2D::ShapeType2D_Capsule:
		{
			b2Capsule capsule = b2Shape_GetCapsule(m_shapeID);
			return b2RayCastCapsule(&capsule, rayCastInput);
		}
		case ShapeType2D::ShapeType2D_Polygon:
		{
			b2Polygon polygon = b2Shape_GetPolygon(m_shapeID);
			return b2RayCastPolygon(&polygon, rayCastInput);
		}
		case ShapeType2D::ShapeType2D_Chain:
		{
			//return b2RayCastSegment(&b2Shape_GetPolygon(m_shapeID), rayCastInput);
		}
		default:
			return b2CastOutput();
		}
	}

	b2CastOutput Shape2D::CastShapeAt(b2ShapeCastInput* shapeCastInput)
	{
		if (!b2Shape_IsValid(m_shapeID))
			return b2CastOutput();
		
		switch (type)
		{
		case ShapeType2D::ShapeType2D_Box:
		{
			b2Polygon box = b2Shape_GetPolygon(m_shapeID);
			return b2ShapeCastPolygon(&box, shapeCastInput);
		}
		case ShapeType2D::ShapeType2D_Circle:
		{
			b2Circle circle = b2Shape_GetCircle(m_shapeID);
			return b2ShapeCastCircle(&circle, shapeCastInput);
		}
		case ShapeType2D::ShapeType2D_Capsule:
		{
			b2Capsule capsule = b2Shape_GetCapsule(m_shapeID);
			return b2ShapeCastCapsule(&capsule, shapeCastInput);
		}
		case ShapeType2D::ShapeType2D_Polygon:
		{
			b2Polygon polygon = b2Shape_GetPolygon(m_shapeID);
			return b2ShapeCastPolygon(&polygon, shapeCastInput);
		}
		case ShapeType2D::ShapeType2D_Chain:
		{
			//return b2ShapeCastSegment(&b2Shape_GetPolygon(m_shapeID), rayCastInput);
		}
		default:
			return b2CastOutput();
		}
	}
}