#include "Joint.h"
#include "FlatEngine.h"
#include "Physics.h"
#include "Body.h"


namespace FlatEngine
{
	Joint::Joint(BaseProps baseProps)
	{
		m_b2JointID = b2_nullJointId;
		m_jointID = 0;
		m_jointString = "";
		m_baseProps = baseProps;
		CreateJoint();
	}

	Joint::~Joint()
	{
	}

	json Joint::GetBasePropsData()
	{
		json jsonData = {
			{ "jointType", (int)m_baseProps.jointType },
			{ "bodyAID", m_baseProps.bodyAID },
			{ "bodyBID", m_baseProps.bodyBID },
			{ "_collideConnected", m_baseProps.b_collideConnected },
			{ "anchorAX", m_baseProps.anchorA.x },
			{ "anchorAY", m_baseProps.anchorA.y },
			{ "anchorBX", m_baseProps.anchorB.x },
			{ "anchorBY", m_baseProps.anchorB.y }
		};

		return jsonData;
	}


	void Joint::SetJointID(long jointID)
	{
		m_jointID = jointID;
	}

	long Joint::GetJointID()
	{
		return m_jointID;
	}

	void Joint::SetB2JointID(b2JointId jointID)
	{
		m_b2JointID = jointID;
	}

	b2JointId Joint::GetB2JointID()
	{
		return m_b2JointID;
	}

	Joint::BaseProps Joint::GetBaseProps()
	{
		return m_baseProps;
	}

	void Joint::SetBodyAID(long bodyAID)
	{ 
		if (bodyAID == m_baseProps.bodyBID)
		{
			m_baseProps.bodyBID = -1;
		}
		
		m_baseProps.bodyAID = bodyAID;
		RecreateJoint();
	}

	void Joint::SetBodyBID(long bodyBID)
	{			
		if (bodyBID == m_baseProps.bodyAID)
		{
			m_baseProps.bodyAID = -1;
		}

		m_baseProps.bodyBID = bodyBID;
		RecreateJoint();
	}

	Joint::JointType Joint::GetJointType()
	{
		return m_baseProps.jointType;
	}

	std::string Joint::GetJointString()
	{
		return m_jointString;
	}

	Body* Joint::GetBodyA()
	{
		if (GetObjectByID(m_baseProps.bodyAID) != nullptr)
		{
			return GetObjectByID(m_baseProps.bodyAID)->GetBody();
		}
		else
		{
			return nullptr;
		}
	}

	Body* Joint::GetBodyB()
	{
		if (GetObjectByID(m_baseProps.bodyBID) != nullptr)
		{
			return GetObjectByID(m_baseProps.bodyBID)->GetBody();
		}
		else
		{
			return nullptr;
		}
	}

	bool Joint::HasValidBodies()
	{
		return (m_baseProps.bodyAID != -1 && m_baseProps.bodyBID != -1);
	}

	void Joint::SetAnchorA(Vector2 anchorA)
	{
		m_baseProps.anchorA = anchorA;
		RecreateJoint();
	}

	void Joint::SetAnchorB(Vector2 anchorB)
	{
		m_baseProps.anchorB = anchorB;
		RecreateJoint();
	}

	bool Joint::CollideConnected()
	{
		return m_baseProps.b_collideConnected;
		RecreateJoint();
	}

	Vector2 Joint::GetAnchorA()
	{
		return m_baseProps.anchorA;
	}

	Vector2 Joint::GetAnchorB()
	{
		return m_baseProps.anchorB;
	}

	void Joint::CreateJoint()
	{
		DestroyJoint();

		GameObject* objectA = GetObjectByID(m_baseProps.bodyAID);
		GameObject* objectB = GetObjectByID(m_baseProps.bodyBID);

		if (objectA != nullptr && objectB != nullptr && objectA->HasComponent("Body") && objectB->HasComponent("Body"))
		{
			F_Physics->CreateJoint(GetObjectByID(m_baseProps.bodyAID)->GetBody(), GetObjectByID(m_baseProps.bodyBID)->GetBody(), this);
		}
	}

	void Joint::CreateJoint(Body* bodyA, Body* bodyB)
	{
		if (bodyA != nullptr && bodyB != nullptr)
		{
			F_Physics->CreateJoint(bodyA, bodyB, this);
		}
	}

	void Joint::RecreateJoint()
	{
		DestroyJoint();
		CreateJoint();
	}

	void Joint::DestroyJoint()
	{
		if (b2Joint_IsValid(m_b2JointID))
		{
			b2DestroyJoint(m_b2JointID);
			m_b2JointID = b2_nullJointId;
		}
	}

	Vector2 Joint::GetConstraintForce()
	{
		return b2Joint_GetConstraintForce(m_b2JointID);
	}

	float Joint::GetConstraintTorque()
	{
		return b2Joint_GetConstraintTorque(m_b2JointID);
	}
}