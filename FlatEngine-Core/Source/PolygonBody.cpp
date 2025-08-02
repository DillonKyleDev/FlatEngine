#include "PolygonBody.h"
#include "GameObject.h"
#include "Transform.h"


namespace FlatEngine
{
	PolygonBody::PolygonBody(long myID, long parentID) : Body::Body(myID, parentID)
	{
		SetType(T_PolygonBody);
		m_bodyProps.shape = Physics::BodyShape::BS_Polygon;
		m_bodyProps.type = b2_dynamicBody;
		m_bodyProps.position = GetParent()->GetTransform()->GetTruePosition();
		m_bodyProps.dimensions = Vector2(2.0f, 2.0f);
		m_bodyProps.radius = 1.0f;
		m_bodyProps.capsuleLength = 4.0f;
		m_bodyProps.density = 1.0f;
		m_bodyProps.friction = 0.3f;
		F_Physics->CreateBody(m_bodyProps, m_bodyID, m_shapeIDs);
	}

	PolygonBody::~PolygonBody()
	{
	}

	std::string PolygonBody::GetData()
	{
		json jsonData = {
			{ "type", "CapsuleBody" },
			{ "id", GetID() },
			{ "_isCollapsed", IsCollapsed() },
			{ "_isActive", IsActive() },
			{ "bodyType", (int)m_bodyProps.type },
			
			// Use animation component as reference for vectors of data in GetData for bodyProps.points member

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
}