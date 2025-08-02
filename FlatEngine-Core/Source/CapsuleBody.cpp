#include "CapsuleBody.h"
#include "GameObject.h"
#include "Transform.h"


namespace FlatEngine
{
	CapsuleBody::CapsuleBody(long myID, long parentID) : Body::Body(myID, parentID)
	{
		SetType(T_CapsuleBody);
		m_bodyProps.shape = Physics::BodyShape::BS_Capsule;
		m_bodyProps.type = b2_dynamicBody;
		m_bodyProps.position = GetParent()->GetTransform()->GetPosition();
		m_bodyProps.dimensions = Vector2(2.0f, 2.0f);	
		m_bodyProps.radius = 1.0f;
		m_bodyProps.capsuleLength = 4.0f;
		m_bodyProps.density = 1.0f;
		m_bodyProps.friction = 0.3f;

		F_Physics->CreateBody(m_bodyProps, m_bodyID, m_shapeIDs);
	}

	CapsuleBody::~CapsuleBody()
	{
	}

	std::string CapsuleBody::GetData()
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
			{ "radius", m_bodyProps.radius },
			{ "capsuleLength", m_bodyProps.capsuleLength },
			{ "_horizontal", m_bodyProps.b_horizontal },
		};

		std::string data = jsonData.dump();
		// Return dumped json object with required data for saving
		return data;
	}


	void CapsuleBody::SetLength(float length)
	{
		if (length > m_bodyProps.radius * 2)
		{
			m_bodyProps = GetLiveProps();
			m_bodyProps.capsuleLength = length;
			RecreateBody();
		}
		else
		{
			LogError("CapsuleBody length must be less than CapsuleBody radius * 2");
		}
	}

	void CapsuleBody::SetRadius(float radius)
	{
		if (radius > 0 && radius < m_bodyProps.capsuleLength / 2)
		{
			m_bodyProps = GetLiveProps();
			m_bodyProps.radius = radius;
			RecreateBody();
		}
		else
		{
			LogError("CapsuleBody radii must be greater than 0 and less than CapsuleBody length / 2");
		}
	}

	void CapsuleBody::SetHorizontal(bool b_horizontal)
	{
		m_bodyProps = GetLiveProps();
		m_bodyProps.b_horizontal = b_horizontal;
		RecreateBody();
	}
}