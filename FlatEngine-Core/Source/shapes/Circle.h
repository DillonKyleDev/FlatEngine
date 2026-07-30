#pragma once
#include "shapes/Shape.h"


namespace FlatEngine
{
	class Body2D;
	
	class Circle : public Shape
	{
	public:
		Circle(Body2D* parentBody = nullptr);

		void SetRadius(float radius);
	};
}