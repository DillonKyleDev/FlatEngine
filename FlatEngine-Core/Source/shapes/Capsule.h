#pragma once
#include "shapes/Shape.h"


namespace FlatEngine
{
	class Body2D;

	class Capsule : public Shape
	{		
	public:
		Capsule(long parentID = -1);		

		void SetCapsuleLength(float length);
		void SetRadius(float radius);			
	};
}
