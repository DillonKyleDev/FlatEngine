#include "Joint.h"
#include "FlatEngine.h"
#include "Physics.h"
#include "Body.h"


namespace FlatEngine
{
	Joint::Joint()
	{

	}

	Joint::~Joint()
	{

	}

	Joint::JointType Joint::GetJointType()
	{
		return m_jointType;
	}

	std::string Joint::GetJointString()
	{
		return std::string();
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

	void Joint::SetAnchorA(Vector2 anchorA)
	{

	}

	void Joint::SetAnchorB(Vector2 anchorB)
	{

	}

	void Joint::CreateJoint(Body* bodyA, Body* bodyB)
	{
		bodyA->AddJoint(this);
		bodyB->AddJoint(this);
		m_bodyA = bodyA;
		m_bodyB = bodyB;
		F_Physics->CreateJoint(bodyA, bodyB, this);
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