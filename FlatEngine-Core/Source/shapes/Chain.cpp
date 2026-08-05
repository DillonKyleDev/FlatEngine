#include "components/Body2D.h"
#include "shapes/Chain.h"


namespace FlatEngine
{
	Chain::Chain(long parentID) : Shape::Shape(parentID)
	{				
		m_shapeProps.shape = Shape::ShapeType::ShapeType_Chain;
		m_shapeProps.points = { {4.0f, -2.0f}, {4.0f, 0.0f}, {-4.0f, 0.0f}, {-4.0f, -2.0f} };		
		m_shapeProps.b_isLoop = false;
		m_shapeProps.tangentSpeed = 0.0f;
		m_shapeProps.rollingResistance = 0.0f;
		m_shapeProps.b_enableSensorEvents = false;
	}

	void Chain::SetTangentSpeed(float tangentSpeed)
	{		
		if (tangentSpeed >= 0)
		{
			m_shapeProps.tangentSpeed = tangentSpeed;
			RecreateShape();
		}
	}

	void Chain::SetRollingResistance(float rollingResistance)
	{		
		if (rollingResistance >= 0)
		{
			m_shapeProps.rollingResistance = rollingResistance;
			RecreateShape();
		}
	}
}