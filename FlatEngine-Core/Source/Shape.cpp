#include "Shape.h"
#include "Body.h"
#include "Transform.h"
#include "FlatEngine.h"


namespace FlatEngine
{
	Shape::Shape(Body* parentBody)
	{		
		m_shapeID = b2_nullShapeId;
		m_chainID = b2_nullChainId;
		m_parentBody = parentBody;
	}

	Shape::~Shape()
	{
	}

	Shape::ShapeType Shape::GetShape()
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

	void Shape::SetProps(ShapeProps shapeProps)
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
}