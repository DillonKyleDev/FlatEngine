#include "CircleBody.h"
#include "GameObject.h"
#include "Transform.h"


namespace FlatEngine
{
	CircleBody::CircleBody(long myID, long parentID) : Body::Body(myID, parentID)
	{
		SetType(T_CircleBody);		
		m_bodyProps.shape = Physics::BodyShape::BS_Circle;
		m_bodyProps.position = GetParent()->GetTransform()->GetPosition();
		m_bodyProps.radius = 1.0f;	
	}

	CircleBody::~CircleBody()
	{
	}

	std::string CircleBody::GetData()
	{
		json jsonData = {
			{ "type", "CircleBody" },
			{ "id", GetID() },
			{ "_isCollapsed", IsCollapsed() },
			{ "_isActive", IsActive() },			
			{ "bodyType", (int)m_bodyProps.type },	
			{ "_lockedRotation", m_bodyProps.b_lockedRotation },
			{ "_lockedXAxis", m_bodyProps.b_lockedXAxis },
			{ "_lockedYAxis", m_bodyProps.b_lockedYAxis },
			{ "gravityScale", m_bodyProps.gravityScale },
			{ "linearDamping", m_bodyProps.linearDamping },
			{ "angularDamping", m_bodyProps.angularDamping },
			{ "restitution", m_bodyProps.restitution },
			{ "density", m_bodyProps.density },
			{ "friction", m_bodyProps.friction },
			{ "radius", m_bodyProps.radius },
		};

		std::string data = jsonData.dump();
		// Return dumped json object with required data for saving
		return data;
	}


	void CircleBody::SetRadius(float radius)
	{
		if (radius > 0)
		{
			m_bodyProps = GetLiveProps();
			m_bodyProps.radius = radius;
			RecreateBody();
		}
	}
}