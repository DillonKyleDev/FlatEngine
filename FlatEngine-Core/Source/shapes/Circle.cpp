#include "components/Body.h"
#include "shapes/Circle.h"


namespace FlatEngine
{
	Circle::Circle(Body* parentBody) : Shape::Shape(parentBody)
	{
		m_shapeProps.shape = Shape::ShapeType::BS_Circle;
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