#include "components/Body2D.h"
#include "shapes/Polygon.h"


namespace FlatEngine
{
	Polygon::Polygon(Body2D* parentBody) : Shape::Shape(parentBody)
	{		
		m_shapeProps.shape = Shape::ShapeType::ShapeType_Polygon;
		m_shapeProps.points = { {3.0f, -2.0f}, {0.0f, 2.0f}, {-3.0f, -2.0f} };
	}

	void Polygon::SetCornerRadius(float cornerRadius)
	{		
		if (cornerRadius >= 0)
		{
			m_shapeProps.cornerRadius = cornerRadius;
			RecreateShape();
		}
	}
}