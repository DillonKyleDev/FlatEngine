#include "ChainBody.h"
#include "GameObject.h"
#include "Transform.h"
#include "FlatEngine.h"

#include <json.hpp>


namespace FlatEngine
{
	ChainBody::ChainBody(long myID, long parentID) : Body::Body(myID, parentID)
	{
		SetType(T_ChainBody);
		m_bodyProps.b_isEnabled = IsActive();
		m_bodyProps.shape = Physics::BodyShape::BS_Chain;		
		m_bodyProps.type = b2_staticBody;
		m_bodyProps.position = GetParent()->GetTransform()->GetPosition();		
		m_bodyProps.points = { {5.0f, -1.0f}, {4.0f, 0.0f}, {-4.0f, 0.0f}, {-5.0f, -1.0f} };		
		m_bodyProps.b_isLoop = false;
		m_bodyProps.tangentSpeed = 0.0f;
		m_bodyProps.rollingResistance = 0.0f;
	}

	ChainBody::~ChainBody()
	{
	}

	std::string ChainBody::GetData()
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
			{ "type", "ChainBody" },
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
			{ "points", pointArray },
			{ "_isLoop", m_bodyProps.b_isLoop },
			{ "tangentSpeed", m_bodyProps.tangentSpeed },
			{ "rollingResistance", m_bodyProps.rollingResistance }			
		};

		std::string data = jsonData.dump();
		// Return dumped json object with required data for saving
		return data;
	}

	void ChainBody::SetPoints(std::vector<Vector2> points)
	{
		m_bodyProps = GetLiveProps();
		m_bodyProps.points = points;		
		RecreateBody();
	}

	void ChainBody::UpdatePoints()
	{
		m_bodyProps = GetLiveProps();		
		RecreateBody();
	}

	void ChainBody::SetIsLoop(bool b_isLoop)
	{
		m_bodyProps = GetLiveProps();
		m_bodyProps.b_isLoop = b_isLoop;		
		RecreateBody();
	}

	void ChainBody::SetTangentSpeed(float tangentSpeed)
	{
		m_bodyProps = GetLiveProps();
		m_bodyProps.tangentSpeed = tangentSpeed;
		RecreateBody();
	}

	void ChainBody::SetRollingResistance(float rollingResistance)
	{
		m_bodyProps = GetLiveProps();
		m_bodyProps.rollingResistance = rollingResistance;
		RecreateBody();
	}

}