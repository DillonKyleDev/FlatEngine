#pragma once
#include "shapes/Shape.h"


namespace FlatEngine
{
	class Body2D;

	class Capsule : public Shape
	{		
	public:
		Capsule(Body2D* parentBody = nullptr);		

		void SetCapsuleLength(float length);
		void SetRadius(float radius);			
	};
}
