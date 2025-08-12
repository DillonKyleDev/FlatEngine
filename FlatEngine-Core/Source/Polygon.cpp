#include "Polygon.h"
#include "Body.h"


namespace FlatEngine
{
	Polygon::Polygon(Body* parentBody) : Shape::Shape(parentBody)
	{		
		m_shapeProps.shape = Shape::ShapeType::BS_Polygon;
		m_shapeProps.points = { {3.0f, -2.0f}, {0.0f, 2.0f}, {-3.0f, -2.0f} };
	}

	Polygon::~Polygon()
	{
	}

	void Polygon::SetPoints(std::vector<Vector2> points)
	{		
		m_shapeProps.points = points;
		RecreateShape();
	}

	void Polygon::UpdatePoints()
	{		
		RecreateShape();
	}

	void Polygon::SetCornerRadius(float cornerRadius)
	{		
		m_shapeProps.cornerRadius = cornerRadius;
		RecreateShape();
	}
}