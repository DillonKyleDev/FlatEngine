#include "components/Body2D.h"
#include "components/Transform.h"
#include "managers/Assets.h"
#include "managers/SceneManager.h"
#include "physics/PhysicsManager.h"
#include "physics/Shape.h"
#include "tools/JsonHelper.h"


namespace FlatEngine
{
	Shape::Shape(long ownerID, ShapeType shapeType)
	{		
		type = shapeType;
		m_shapeID = b2_nullShapeId;
		m_chainID = b2_nullChainId;
		m_ownerID = ownerID;
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

	ShapeType Shape::GetType()
	{
		return type;
	}

	json Shape::GetData()
	{
		json jsonData = {
			{ "type", (int)type },
			{ "b_enableContactEvents", b_enableContactEvents },
			{ "b_enableSensorEvents", b_enableSensorEvents },
			{ "b_isSensor", b_isSensor },
			{ "restitution", restitution },
			{ "density", density },
			{ "friction", friction },
			{ "tangentSpeed", tangentSpeed },
			{ "rollingResistance", rollingResistance }	
		};

		jsonData["shapeData"] = std::visit([](auto&& sData) { return sData.GetData(); }, shapeData);

		return jsonData;
	}

	void Shape::PutData(json jsonData, std::string objectName)
	{
		type = (ShapeType)(JsonHelper::CheckJsonInt(jsonData, "type", objectName));
		b_enableContactEvents = JsonHelper::CheckJsonBool(jsonData, "b_enableContactEvents", objectName);
		b_enableSensorEvents = JsonHelper::CheckJsonBool(jsonData, "b_enableSensorEvents", objectName);
		b_isSensor = JsonHelper::CheckJsonBool(jsonData, "b_isSensor", objectName);				
		restitution = JsonHelper::CheckJsonFloat(jsonData, "restitution", objectName);
		density = JsonHelper::CheckJsonFloat(jsonData, "density", objectName);
		friction = JsonHelper::CheckJsonFloat(jsonData, "friction", objectName);
		tangentSpeed = JsonHelper::CheckJsonFloat(jsonData, "tangentSpeed", objectName);
		rollingResistance = JsonHelper::CheckJsonFloat(jsonData, "rollingResistance", objectName);
		
		if (JsonHelper::JsonContains(jsonData, "shapeData", objectName))
		{
			switch (type)
			{
				case ShapeType_Box: { BoxShapeData shape; shape.PutData(jsonData.at("shapeData"), objectName); shapeData = shape; renderShapes.push_back(SceneView::CreateQuadObject()); break; }
				case ShapeType_Circle: { CircleShapeData shape; shape.PutData(jsonData.at("shapeData"), objectName); shapeData = shape; renderShapes.push_back(SceneView::CreateCircleObject());  break; }
				case ShapeType_Polygon: { PolygonShapeData shape; shape.PutData(jsonData.at("shapeData"), objectName); shapeData = shape; break; }
				case ShapeType_Capsule: { CapsuleShapeData shape; shape.PutData(jsonData.at("shapeData"), objectName); shapeData = shape; break; }
				case ShapeType_Chain: { ChainShapeData shape; shape.PutData(jsonData.at("shapeData"), objectName); shapeData = shape; break; }
				default: break;
			}
		}
	}	

	void Shape::SetShapeID(b2ShapeId shapeID)
	{
		m_shapeID = shapeID;
	}

	const b2ShapeId Shape::GetShapeID()
	{
		return m_shapeID;
	}

	void Shape::SetChainID(b2ChainId chainID)
	{
		m_chainID = chainID;
	}

	const b2ChainId Shape::GetChainID()
	{
		return m_chainID;
	}

	const long Shape::GetOwnerID()
	{
		return m_ownerID;
	}

	const b2BodyId Shape::Getb2BodyID()
	{
		return SceneManager::loadedScene.GetObjectByID(m_ownerID)->Get<Body2D>()->GetBodyID();
	}

	void Shape::SetIsSensor(bool b_setIsSensor)
	{
		b_isSensor = b_setIsSensor;
		PhysicsManager::physics2D.RecreateShape(this);
	}

	void Shape::SetEnableSensorEvents(bool b_setEnableSensorEvents)
	{		
		b_enableSensorEvents = b_setEnableSensorEvents;
		PhysicsManager::physics2D.RecreateShape(this);
	}

	void Shape::SetEnableContactEvents(bool b_setEnableContactEvents)
	{
		b_enableContactEvents = b_setEnableContactEvents;
		PhysicsManager::physics2D.RecreateShape(this);
	}

	void Shape::SetDensity(float newDensity)
	{		
		density = newDensity;
		PhysicsManager::physics2D.RecreateShape(this);
	}

	void Shape::SetFriction(float newFriction)
	{		
		friction = newFriction;
		PhysicsManager::physics2D.RecreateShape(this);
	}

	void Shape::SetRestitution(float newRestitution)
	{		
		restitution = newRestitution;
		PhysicsManager::physics2D.RecreateShape(this);
	}

	void Shape::SetTangentSpeed(float newTangentSpeed)
	{		
		if (newTangentSpeed >= 0)
		{
			tangentSpeed = newTangentSpeed;
			PhysicsManager::physics2D.RecreateShape(this);
		}
	}
	void Shape::SetRollingResistance(float newRrollingResistance)
	{		
		if (newRrollingResistance >= 0)
		{
			rollingResistance = newRrollingResistance;
			PhysicsManager::physics2D.RecreateShape(this);
		}
	}

	bool Shape::PointInShape(Vector2 point)
	{
		switch (type)
		{
		case ShapeType::ShapeType_Box:
		{
			b2Polygon box = b2Shape_GetPolygon(m_shapeID);
			return b2PointInPolygon(&box, Vector2::GetB2Vec2(point));
		}
		case ShapeType::ShapeType_Circle:
		{
			b2Circle circle = b2Shape_GetCircle(m_shapeID);
			return b2PointInCircle(&circle, Vector2::GetB2Vec2(point));
		}
		case ShapeType::ShapeType_Capsule:
		{
			b2Capsule capsule = b2Shape_GetCapsule(m_shapeID);
			return b2PointInCapsule(&capsule, Vector2::GetB2Vec2(point));
		}
		case ShapeType::ShapeType_Polygon:
		{
			b2Polygon polygon = b2Shape_GetPolygon(m_shapeID);
			return b2PointInPolygon(&polygon, Vector2::GetB2Vec2(point));
		}
		default:
			return false;
		}
	}

	b2CastOutput Shape::CastRayAt(b2RayCastInput* rayCastInput)
	{
		switch (type)
		{
		case ShapeType::ShapeType_Box:
		{
			b2Polygon box = b2Shape_GetPolygon(m_shapeID);
			return b2RayCastPolygon(&box, rayCastInput);
		}
		case ShapeType::ShapeType_Circle:
		{					
			b2Circle circle = b2Shape_GetCircle(m_shapeID);
			return b2RayCastCircle(&circle, rayCastInput);
		}
		case ShapeType::ShapeType_Capsule:
		{
			b2Capsule capsule = b2Shape_GetCapsule(m_shapeID);
			return b2RayCastCapsule(&capsule, rayCastInput);
		}
		case ShapeType::ShapeType_Polygon:
		{
			b2Polygon polygon = b2Shape_GetPolygon(m_shapeID);
			return b2RayCastPolygon(&polygon, rayCastInput);
		}
		case ShapeType::ShapeType_Chain:
		{
			//return b2RayCastSegment(&b2Shape_GetPolygon(m_shapeID), rayCastInput);
		}
		default:
			return b2CastOutput();
		}
	}

	b2CastOutput Shape::CastShapeAt(b2ShapeCastInput* shapeCastInput)
	{
		switch (type)
		{
		case ShapeType::ShapeType_Box:
		{
			b2Polygon box = b2Shape_GetPolygon(m_shapeID);
			return b2ShapeCastPolygon(&box, shapeCastInput);
		}
		case ShapeType::ShapeType_Circle:
		{
			b2Circle circle = b2Shape_GetCircle(m_shapeID);
			return b2ShapeCastCircle(&circle, shapeCastInput);
		}
		case ShapeType::ShapeType_Capsule:
		{
			b2Capsule capsule = b2Shape_GetCapsule(m_shapeID);
			return b2ShapeCastCapsule(&capsule, shapeCastInput);
		}
		case ShapeType::ShapeType_Polygon:
		{
			b2Polygon polygon = b2Shape_GetPolygon(m_shapeID);
			return b2ShapeCastPolygon(&polygon, shapeCastInput);
		}
		case ShapeType::ShapeType_Chain:
		{
			//return b2ShapeCastSegment(&b2Shape_GetPolygon(m_shapeID), rayCastInput);
		}
		default:
			return b2CastOutput();
		}
	}
}