#include "components/Body2D.h"
#include "shapes/Capsule.h"
#include "tools/Logger.h"


namespace FlatEngine
{
	Capsule::Capsule(Body2D* parentBody) : Shape::Shape(parentBody)
	{		
		m_shapeProps.shape = Shape::ShapeType::ShapeType_Capsule;
		m_shapeProps.radius = 1.0f;
		m_shapeProps.capsuleLength = 4.0f;
	}

	void Capsule::SetCapsuleLength(float length)
	{
		if (length <= 0)
		{
			Logger::log.Err("Capsule::SetCapsuleLength() : length must be greater than 0.0f."); 
			return;
		}

		m_shapeProps.capsuleLength = length;	

		if (length < m_shapeProps.radius * 2)		
			m_shapeProps.radius = length / 2;		
		
		RecreateShape();
	}

	void Capsule::SetRadius(float radius)
	{
		if (radius <= 0)
		{
			Logger::log.Err("Capsule::SetRadius() : radius must be greater than 0.0f."); 
			return; 
		}

		m_shapeProps.radius = radius;	

		if (radius > m_shapeProps.capsuleLength / 2)	
			m_shapeProps.capsuleLength = radius * 2;	

		RecreateShape();
	}
}