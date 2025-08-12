#include "Chain.h"
#include "FlatEngine.h"


namespace FlatEngine
{
	Chain::Chain(Body* parentBody) : Shape::Shape(parentBody)
	{				
		m_shapeProps.shape = Shape::ShapeType::BS_Chain;
		m_shapeProps.points = { {4.0f, -2.0f}, {4.0f, 0.0f}, {-4.0f, 0.0f}, {-4.0f, -2.0f} };		
		m_shapeProps.b_isLoop = false;
		m_shapeProps.tangentSpeed = 0.0f;
		m_shapeProps.rollingResistance = 0.0f;
		m_shapeProps.b_enableSensorEvents = false;
	}

	Chain::~Chain()
	{
	}


	void Chain::SetPoints(std::vector<Vector2> points)
	{		
		m_shapeProps.points = points;		
		RecreateShape();
	}

	void Chain::UpdatePoints()
	{			
		RecreateShape();
	}

	void Chain::SetIsLoop(bool b_isLoop)
	{		
		m_shapeProps.b_isLoop = b_isLoop;		
		RecreateShape();
	}

	void Chain::SetTangentSpeed(float tangentSpeed)
	{		
		m_shapeProps.tangentSpeed = tangentSpeed;
		RecreateShape();
	}

	void Chain::SetRollingResistance(float rollingResistance)
	{		
		m_shapeProps.rollingResistance = rollingResistance;
		RecreateShape();
	}
}