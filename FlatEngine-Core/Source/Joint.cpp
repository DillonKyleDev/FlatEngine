#include "Joint.h"
#include "FlatEngine.h"
#include "Physics.h"
#include "Body.h"


namespace FlatEngine
{
	Joint::Joint(JointProps* jointProps)
	{
		m_jointID = b2_nullJointId;		
		m_bodyA = nullptr;
		m_bodyB = nullptr;
		m_b2BodyAID = b2_nullBodyId;
		m_b2BodyBID = b2_nullBodyId;
		m_bodyAID = jointProps->bodyAID;
		m_bodyBID = jointProps->bodyBID;
		m_b_collideConnected = jointProps->b_collideConnected;
		m_anchorA = jointProps->anchorA;
		m_anchorB = jointProps->anchorB;
	}

	Joint::~Joint()
	{
	}

	void Joint::SetBodyA(Body* bodyA) 
	{ 
		m_bodyA = bodyA; 
		m_bodyAID = bodyA->GetParentID(); 
	};

	void Joint::SetBodyB(Body* bodyB)
	{
		m_bodyB = bodyB;
		m_bodyBID = bodyB->GetParentID();
	};

	Joint::JointType Joint::GetJointType()
	{
		return m_jointType;
	}

	std::string Joint::GetJointString()
	{
		return m_jointString;
	}

	void Joint::SetJointID(b2JointId jointID)
	{
		m_jointID = jointID;
	}

	b2JointId Joint::GetJointID()
	{
		return m_jointID;
	}

	Body* Joint::GetBodyA()
	{
		return m_bodyA;
	}

	Body* Joint::GetBodyB()
	{
		return m_bodyB;
	}

	bool Joint::HasValidBodies()
	{
		return (m_bodyAID != -1 && m_bodyBID != -1);
	}

	void Joint::SetAnchorA(Vector2 anchorA)
	{
		m_anchorA = anchorA;
	}

	void Joint::SetAnchorB(Vector2 anchorB)
	{
		m_anchorB = anchorB;
	}

	bool Joint::CollideConnected()
	{
		return m_b_collideConnected;
	}

	Vector2 Joint::GetAnchorA()
	{
		return m_anchorA;
	}

	Vector2 Joint::GetAnchorB()
	{
		return m_anchorB;
	}

	void Joint::CreateJoint()
	{
		if (m_bodyAID != -1 && m_bodyBID != -1)
		{
			m_bodyA = GetObjectByID(m_bodyAID)->GetBody();
			m_bodyB = GetObjectByID(m_bodyBID)->GetBody();
		}

		if (m_bodyA != nullptr && m_bodyB != nullptr)
		{
			F_Physics->CreateJoint(m_bodyA, m_bodyB, this);
		}
		else
		{
			LogError("Could not create Joint in class Joint: BodyA and/or BodyB were nullptr.");
		}
	}

	void Joint::CreateJoint(Body* bodyA, Body* bodyB)
	{
		if (bodyA != nullptr && bodyB != nullptr)
		{
			m_bodyA = bodyA;
			m_bodyB = bodyB;
		}

		if (m_bodyA != nullptr && m_bodyB != nullptr)
		{
			F_Physics->CreateJoint(m_bodyA, m_bodyB, this);
		}
		else
		{
			LogError("Could not create Joint in class Joint: BodyA and/or BodyB were nullptr.");
		}
	}

	void Joint::DestroyJoint()
	{
		b2DestroyJoint(m_jointID);
	}

	Vector2 Joint::GetConstraintForce()
	{
		return b2Joint_GetConstraintForce(m_jointID);
	}

	float Joint::GetConstraintTorque()
	{
		return b2Joint_GetConstraintTorque(m_jointID);
	}
}