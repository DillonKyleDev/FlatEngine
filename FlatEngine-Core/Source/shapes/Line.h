#pragma once
#include "tools/Vector2.h"
#include "tools/Vector4.h"


namespace FlatEngine
{
	class Line
	{
	public:
		Line(Vector2 startingPoint, Vector2 endingPoint, Vector4 color, float thickness);		

		Vector2 m_start;
		Vector2 m_end;
		Vector4 m_color;
		float m_thickness;
	private:

	};
}