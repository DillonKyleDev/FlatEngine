#pragma once
#include "shapes/Shape.h"


namespace FlatEngine
{
	class Body2D;
	
	class Circle : public Shape
	{
	public:
		Circle(long parentID = -1);

		void SetRadius(float radius);
	};
}