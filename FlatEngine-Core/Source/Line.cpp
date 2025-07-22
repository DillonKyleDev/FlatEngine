#include "Line.h"


namespace FlatEngine
{
	Line::Line(Vector2 startingPoint, Vector2 endingPoint, Vector4 color, float thickness)
	{
		m_start = startingPoint;
		m_end = endingPoint;
		m_color = color;
		m_thickness = thickness;
	}

	Line::~Line()
	{
	}
}