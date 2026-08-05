#include "components/Body2D.h"
#include "shapes/Circle.h"


namespace FlatEngine
{
	Circle::Circle(long parentID) : Shape::Shape(parentID)
	{
		m_shapeProps.shape = Shape::ShapeType::ShapeType_Circle;
		m_shapeProps.radius = 1.0f;
	}

	void Circle::SetRadius(float radius)
	{
		if (radius > 0)
		{			
			m_shapeProps.radius = radius;
			RecreateShape();
		}
	}
}