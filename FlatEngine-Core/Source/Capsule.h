#pragma once
#include "Shape.h"
#include "Vector2.h"

#include <string>


namespace FlatEngine
{
	class Body;

	class Capsule : public Shape
	{		
	public:
		Capsule(Body* parentBody);
		~Capsule();

		void SetCapsuleLength(float length);
		void SetRadius(float radius);
		void SetHorizontal(bool b_horizontal);

	private:

	};
}
