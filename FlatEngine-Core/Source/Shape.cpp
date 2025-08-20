#include "Shape.h"
#include "Body.h"
#include "Transform.h"
#include "FlatEngine.h"
#include "Scene.h"
#include "Physics.h"
#include "Vector4.h"


namespace FlatEngine
{
	// RayCast will only be visible for the frame of the cast if b_visible = true
	b2CastOutput CastRay(Vector2 initialPos, Vector2 direction, float increment, TagList tagList, Body& hit, bool b_visible)
	{
		if (b_visible)
		{
			DrawLineInScene(initialPos, (initialPos + direction) * 10, GetColor("rayCast"), 2);
		}

		for (std::pair<long, Body> bodyPair : GetLoadedScene()->GetBodies())
		{
			bodyPair.second.GetParent()->GetTagList().UpdateBits();
			if (Physics::CanCollide(tagList, bodyPair.second.GetParent()->GetTagList()))
			{
				for (Shape* shape : bodyPair.second.GetShapes())
				{
					b2RayCastInput input = { 0 };
					input.origin = Vector2::GetB2Vev2(initialPos);
					input.translation = Vector2::GetB2Vev2(direction);
					input.maxFraction = increment;

					b2CastOutput output = shape->CastRayAt(&input);

					if (output.hit)
					{
						hit = bodyPair.second;
						return output;
					}
				}
			}
		}

		return b2CastOutput();
	}

	Shape::Shape(Body* parentBody)
	{		
		m_shapeID = b2_nullShapeId;
		m_chainID = b2_nullChainId;
		m_parentBody = parentBody;
		m_b_editingPoints = false;
		m_b_showPoints = false;
		m_b_drawInGame = false;
		m_inGameDrawColor = GetColor("boxColliderActive");
		m_inGameDrawThickness = 2.0f;
	}

	Shape::~Shape()
	{
	}

	Shape::ShapeType Shape::GetShapeType()
	{
		return m_shapeProps.shape;
	}

	json Shape::GetShapeData()
	{
		json pointArray = json::array();

		for (Vector2 point : m_shapeProps.points)
		{
			json pointData = {
				{ "xPos", point.x },
				{ "yPos", point.y }
			};
			pointArray.push_back(pointData);
		}

		json jsonData = {
			{ "shape", (int)m_shapeProps.shape },
			{ "xOffset", m_shapeProps.positionOffset.x },
			{ "yOffset", m_shapeProps.positionOffset.y },
			{ "rotationOffsetCos", m_shapeProps.rotationOffset.c },
			{ "rotationOffsetSin", m_shapeProps.rotationOffset.s },
			{ "_enableContactEvents", m_shapeProps.b_enableContactEvents },
			{ "_enableSensorEvents", m_shapeProps.b_enableSensorEvents },
			{ "_isSensor", m_shapeProps.b_isSensor },
			{ "restitution", m_shapeProps.restitution },
			{ "density", m_shapeProps.density },
			{ "friction", m_shapeProps.friction },
			{ "width", m_shapeProps.dimensions.x },
			{ "height", m_shapeProps.dimensions.y },
			{ "cornerRadius", m_shapeProps.cornerRadius },
			{ "radius", m_shapeProps.radius },
			{ "capsuleLength", m_shapeProps.capsuleLength },
			{ "_horizontal", m_shapeProps.b_horizontal },
			{ "points", pointArray },
			{ "_isLoop", m_shapeProps.b_isLoop },
			{ "tangentSpeed", m_shapeProps.tangentSpeed },
			{ "rollingResistance", m_shapeProps.rollingResistance }
		};

		return jsonData;
	}

	std::string Shape::GetShapeString()
	{
		switch (m_shapeProps.shape)
		{
		case Shape::ShapeType::BS_Box:
		{
			return "Box";			
		}
		case Shape::ShapeType::BS_Circle:
		{
			return "Circle";
		}
		case Shape::ShapeType::BS_Capsule:
		{
			return "Capsule";
		}
		case Shape::ShapeType::BS_Polygon:
		{
			return "Polygon";
		}
		case Shape::ShapeType::BS_Chain:
		{
			return "Chain";
		}
		default:
			return "Null";
		}
	}

	//void Shape::SetShapeUserData(std::shared_ptr<Shape> shapePtr)
	//{
	//	b2Shape_SetUserData(m_shapeID, shapePtr);
	//}

	void Shape::SetShapeID(b2ShapeId shapeID)
	{
		m_shapeID = shapeID;
	}

	b2ShapeId Shape::GetShapeID()
	{
		return m_shapeID;
	}

	void Shape::SetChainID(b2ChainId chainID)
	{
		m_chainID = chainID;
	}

	b2ChainId Shape::GetChainID()
	{
		return m_chainID;
	}

	Body* Shape::GetParentBody()
	{
		return m_parentBody;
	}

	b2BodyId Shape::GetParentBodyID()
	{
		return m_parentBody->GetBodyID();
	}

	void Shape::SetShapeProps(ShapeProps shapeProps)
	{
		m_shapeProps = shapeProps;
	}

	Shape::ShapeProps& Shape::GetShapeProps()
	{
		return m_shapeProps;
	}

	void Shape::CreateShape()
	{
		F_Physics->CreateShape(m_parentBody, this);
	}

	void Shape::RecreateShape()
	{
		DestroyShape();		
		CreateShape();
	}

	void Shape::DestroyShape()
	{
		if (b2Shape_IsValid(m_shapeID))
		{
			F_Physics->DestroyShape(m_shapeID);
		}
		if (b2Chain_IsValid(m_chainID))
		{
			b2DestroyChain(m_chainID);
		}
		m_shapeID = b2_nullShapeId;
		m_chainID = b2_nullChainId;
	}

	bool Shape::PointInShape(Vector2 point)
	{
		switch (m_shapeProps.shape)
		{
		case Shape::ShapeType::BS_Box:
		{
			b2Polygon box = b2Shape_GetPolygon(m_shapeID);
			return b2PointInPolygon(&box, Vector2::GetB2Vev2(point));
		}
		case Shape::ShapeType::BS_Circle:
		{
			b2Circle circle = b2Shape_GetCircle(m_shapeID);
			return b2PointInCircle(&circle, Vector2::GetB2Vev2(point));
		}
		case Shape::ShapeType::BS_Capsule:
		{
			b2Capsule capsule = b2Shape_GetCapsule(m_shapeID);
			return b2PointInCapsule(&capsule, Vector2::GetB2Vev2(point));
		}
		case Shape::ShapeType::BS_Polygon:
		{
			b2Polygon polygon = b2Shape_GetPolygon(m_shapeID);
			return b2PointInPolygon(&polygon, Vector2::GetB2Vev2(point));
		}
		default:
			return false;
		}
	}

	b2CastOutput Shape::CastRayAt(b2RayCastInput* rayCastInput)
	{
		switch (m_shapeProps.shape)
		{
		case Shape::ShapeType::BS_Box:
		{
			b2Polygon box = b2Shape_GetPolygon(m_shapeID);
			return b2RayCastPolygon(&box, rayCastInput);
		}
		case Shape::ShapeType::BS_Circle:
		{					
			b2Circle circle = b2Shape_GetCircle(m_shapeID);
			return b2RayCastCircle(&circle, rayCastInput);
		}
		case Shape::ShapeType::BS_Capsule:
		{
			b2Capsule capsule = b2Shape_GetCapsule(m_shapeID);
			return b2RayCastCapsule(&capsule, rayCastInput);
		}
		case Shape::ShapeType::BS_Polygon:
		{
			b2Polygon polygon = b2Shape_GetPolygon(m_shapeID);
			return b2RayCastPolygon(&polygon, rayCastInput);
		}
		case Shape::ShapeType::BS_Chain:
		{
			//return b2RayCastSegment(&b2Shape_GetPolygon(m_shapeID), rayCastInput);
		}
		default:
			return b2CastOutput();
		}
	}

	b2CastOutput Shape::CastShapeAt(b2ShapeCastInput* shapeCastInput)
	{
		switch (m_shapeProps.shape)
		{
		case Shape::ShapeType::BS_Box:
		{
			b2Polygon box = b2Shape_GetPolygon(m_shapeID);
			return b2ShapeCastPolygon(&box, shapeCastInput);
		}
		case Shape::ShapeType::BS_Circle:
		{
			b2Circle circle = b2Shape_GetCircle(m_shapeID);
			return b2ShapeCastCircle(&circle, shapeCastInput);
		}
		case Shape::ShapeType::BS_Capsule:
		{
			b2Capsule capsule = b2Shape_GetCapsule(m_shapeID);
			return b2ShapeCastCapsule(&capsule, shapeCastInput);
		}
		case Shape::ShapeType::BS_Polygon:
		{
			b2Polygon polygon = b2Shape_GetPolygon(m_shapeID);
			return b2ShapeCastPolygon(&polygon, shapeCastInput);
		}
		case Shape::ShapeType::BS_Chain:
		{
			//return b2ShapeCastSegment(&b2Shape_GetPolygon(m_shapeID), rayCastInput);
		}
		default:
			return b2CastOutput();
		}
	}



	void Shape::SetIsSensor(bool b_isSensor)
	{
		m_shapeProps.b_isSensor = b_isSensor;
		RecreateShape();
	}

	void Shape::SetEnableSensorEvents(bool b_enableSensorEvents)
	{		
		m_shapeProps.b_enableSensorEvents = b_enableSensorEvents;
		RecreateShape();
	}

	void Shape::SetEnableContactEvents(bool b_enableContactEvents)
	{
		m_shapeProps.b_enableContactEvents = b_enableContactEvents;
		RecreateShape();
	}

	void Shape::SetDensity(float density)
	{		
		m_shapeProps.density = density;
		RecreateShape();
	}

	void Shape::SetFriction(float friction)
	{		
		m_shapeProps.friction = friction;
		RecreateShape();
	}

	void Shape::SetRestitution(float restitution)
	{		
		m_shapeProps.restitution = restitution;
		RecreateShape();
	}

	void Shape::SetPositionOffset(Vector2 positionOffset)
	{
		m_shapeProps.positionOffset = positionOffset;
		RecreateShape();
	}

	void Shape::SetRotationOffset(float rotationOffset)
	{
		m_shapeProps.rotationOffset = b2MakeRot(DegreesToRadians(Transform::ClampRotation(rotationOffset)));
		RecreateShape();
	}

	bool Shape::ShowPoints()
	{
		return m_b_showPoints;
	}

	void Shape::SetShowPoints(bool b_showPoints)
	{
		m_b_showPoints = b_showPoints;
	}

	bool Shape::IsEditingPoints()
	{
		return m_b_editingPoints;
	}

	void Shape::SetEditingPoints(bool b_editingPoints)
	{
		m_b_editingPoints = b_editingPoints;
	}

	void Shape::SetDrawInGame(bool b_drawInGame)
	{
		m_b_drawInGame = b_drawInGame;
	}

	bool Shape::DrawInGame()
	{
		return m_b_drawInGame;
	}

	void Shape::SetInGameDrawColor(Vector4 drawColor)
	{
		m_inGameDrawColor = drawColor;
	}

	Vector4 Shape::GetInGameDrawColor()
	{
		return m_inGameDrawColor;
	}

	void Shape::SetInGameDraThickness(float thickness)
	{
		m_inGameDrawThickness = thickness;
	}

	float Shape::GetInGameDraThickness()
	{
		return m_inGameDrawThickness;
	}
}