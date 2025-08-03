#include "PolygonBody.h"
#include "GameObject.h"
#include "Transform.h"


namespace FlatEngine
{
	PolygonBody::PolygonBody(long myID, long parentID) : Body::Body(myID, parentID)
	{
		SetType(T_PolygonBody);		
		m_bodyProps.shape = Physics::BodyShape::BS_Polygon;
		m_bodyProps.position = GetParent()->GetTransform()->GetTruePosition();
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
			{ "_lockedRotation", m_bodyProps.b_lockedRotation },
			{ "_lockedXAxis", m_bodyProps.b_lockedXAxis },
			{ "_lockedYAxis", m_bodyProps.b_lockedYAxis },
			{ "gravityScale", m_bodyProps.gravityScale },
			{ "linearDamping", m_bodyProps.linearDamping },
			{ "angularDamping", m_bodyProps.angularDamping },
			{ "restitution", m_bodyProps.restitution },
			{ "density", m_bodyProps.density },
			{ "friction", m_bodyProps.friction },
			
			// Use animation component as reference for vectors of data in GetData for bodyProps.points member
		};

		std::string data = jsonData.dump();
		// Return dumped json object with required data for saving
		return data;
	}
}