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
		m_bodyProps.position = GetParent()->GetTransform()->GetPosition();
		m_bodyProps.points = { {4.0f, -2.0f}, {4.0f, 0.0f}, {-4.0f, 0.0f}, {-4.0f, -2.0f} };
	}

	PolygonBody::~PolygonBody()
	{
	}

	std::string PolygonBody::GetData()
	{
		json pointArray = json::array();

		for (Vector2 point : m_bodyProps.points)
		{
			json pointData = {
				{ "xPos", point.x },
				{ "yPos", point.y }
			};
			pointArray.push_back(pointData);
		}

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
			{ "points", pointArray }					
		};

		std::string data = jsonData.dump();
		// Return dumped json object with required data for saving
		return data;
	}


	void PolygonBody::SetPoints(std::vector<Vector2> points)
	{
		m_bodyProps = GetLiveProps();
		m_bodyProps.points = points;
		RecreateBody();
	}

	void PolygonBody::UpdatePoints()
	{
		m_bodyProps = GetLiveProps();
		RecreateBody();
	}

	void PolygonBody::SetCornerRadius(float cornerRadius)
	{
		m_bodyProps = GetLiveProps();
		m_bodyProps.cornerRadius = cornerRadius;
		RecreateBody();
	}
}