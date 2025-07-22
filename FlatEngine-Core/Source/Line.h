#pragma once
#include "Vector2.h"
#include "Vector4.h"

#include <vector>


namespace FlatEngine
{
	class Line
	{
	public:
		Line(Vector2 startingPoint, Vector2 endingPoint, Vector4 color, float thickness);
		~Line();

		Vector2 m_start;
		Vector2 m_end;
		Vector4 m_color;
		float m_thickness;
	private:

	};
}