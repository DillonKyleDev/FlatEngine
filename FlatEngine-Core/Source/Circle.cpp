#include "Circle.h"
#include "GameObject.h"
#include "Transform.h"


namespace FlatEngine
{
	Circle::Circle(Body* parentBody) : Shape::Shape(parentBody)
	{
		m_shapeProps.shape = Shape::ShapeType::BS_Circle;
		m_shapeProps.radius = 1.0f;
	}

	Circle::~Circle()
	{
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