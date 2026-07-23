#pragma once
#include "shapes/Shape.h"


namespace FlatEngine
{
	class Body;

	class Capsule : public Shape
	{		
	public:
		Capsule(Body* parentBody = nullptr);		

		void SetCapsuleLength(float length);
		void SetRadius(float radius);
		void SetHorizontal(bool b_horizontal);

	private:

	};
}
