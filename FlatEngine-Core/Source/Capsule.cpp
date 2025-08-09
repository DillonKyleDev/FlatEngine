#include "Capsule.h"
#include "GameObject.h"
#include "Transform.h"


namespace FlatEngine
{
	Capsule::Capsule(Body* parentBody) : Shape::Shape(parentBody)
	{		
		m_shapeProps.shape = Shape::ShapeType::BS_Capsule;
		m_shapeProps.radius = 1.0f;
		m_shapeProps.capsuleLength = 4.0f;
	}

	Capsule::~Capsule()
	{
	}

	void Capsule::SetCapsuleLength(float length)
	{
		if (length > m_shapeProps.radius * 2)
		{			
			m_shapeProps.capsuleLength = length;
			RecreateShape();
		}
	}

	void Capsule::SetRadius(float radius)
	{
		if (radius > 0 && radius < m_shapeProps.capsuleLength / 2)
		{			
			m_shapeProps.radius = radius;
			RecreateShape();
		}
	}

	void Capsule::SetHorizontal(bool b_horizontal)
	{		
		m_shapeProps.b_horizontal = b_horizontal;
		RecreateShape();
	}
}