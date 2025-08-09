#pragma once
#include "Shape.h"


namespace FlatEngine
{
	class Circle : public Shape
	{
	public:
		Circle(Body* parentBody);
		~Circle();		

		void SetRadius(float radius);

	private:

	};
}


