#include "CircleBody.h"
#include "GameObject.h"
#include "Transform.h"


namespace FlatEngine
{
	CircleBody::CircleBody(long myID, long parentID) : Body::Body(myID, parentID)
	{
		SetType(T_CircleBody);
		m_bodyProps.shape = Physics::BodyShape::BS_Circle;
		m_bodyProps.type = b2_staticBody;
		m_bodyProps.position = GetParent()->GetTransform()->GetTruePosition();
		m_bodyProps.dimensions = Vector2(2.0f, 2.0f);
		m_bodyProps.radius = 1.0f;
		m_bodyProps.density = 1.0f;
		m_bodyProps.friction = 0.3f;
		F_Physics->CreateBody(m_bodyProps, m_bodyID, m_shapeIDs);
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
			{ "radius", m_bodyProps.radius },
			{ "density", m_bodyProps.density },
			{ "friction", m_bodyProps.friction },
			//{ "angularDrag", m_angularDrag },
			//{ "gravity", m_gravity },
			//{ "fallingGravity", m_fallingGravity },
			//{ "terminalVelocity", m_terminalVelocity },
			//{ "windResistance", m_windResistance },			
			//{ "_allowTorques", m_b_allowTorques },
		};

		std::string data = jsonData.dump();
		// Return dumped json object with required data for saving
		return data;
	}


	void CircleBody::SetRadius(float radius)
	{
		if (radius > 0)
		{
			m_bodyProps.radius = radius;
		}
	}
}