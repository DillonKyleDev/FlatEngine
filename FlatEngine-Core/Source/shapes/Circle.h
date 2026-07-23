#pragma once
#include "shapes/Shape.h"


namespace FlatEngine
{
	class Body;
	
	class Circle : public Shape
	{
	public:
		Circle(Body* parentBody = nullptr);

		void SetRadius(float radius);

	private:

	};
}


