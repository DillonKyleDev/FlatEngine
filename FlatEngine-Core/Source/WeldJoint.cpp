#include "WeldJoint.h"
#include "Body.h"


namespace FlatEngine
{
	WeldJoint::WeldJoint(WeldJointProps jointProps) : Joint(&jointProps)
	{
		m_jointProps = jointProps;
		m_jointString = "Weld Joint";
		m_jointProps.jointType = JT_Weld;
		m_jointType = JT_Weld;
	}

	WeldJoint::~WeldJoint()
	{
	}

	json WeldJoint::GetJointData()
	{
		json jsonData = {
			// Base Joint Props
			{ "jointType", (int)m_jointType },
			{ "bodyAID", m_bodyAID },
			{ "bodyBID", m_bodyBID },
			{ "_collideConnected", m_b_collideConnected },
			{ "anchorAX", m_anchorA.x },
			{ "anchorAY", m_anchorA.y },
			{ "anchorBX", m_anchorB.x },
			{ "anchorBY", m_anchorB.y },
			// Distance Joint Props
			{ "angularDampingRatio", m_jointProps.angularDampingRatio },
			{ "angularHertz", m_jointProps.angularHertz },
			{ "linearDampingRatio", m_jointProps.linearDampingRatio },
			{ "linearHertz", m_jointProps.linearHertz },
			{ "referenceAngle", m_jointProps.referenceAngle }
		};

		return jsonData;
	}

	Joint::JointProps* WeldJoint::GetJointProps()
	{
		return &m_jointProps;
	}

	void WeldJoint::SetBodyA(Body* bodyA)
	{
		Joint::SetBodyA(bodyA);
		m_jointProps.bodyAID = bodyA->GetParentID();
	}

	void WeldJoint::SetBodyB(Body* bodyB)
	{
		Joint::SetBodyB(bodyB);
		m_jointProps.bodyBID = bodyB->GetParentID();
	}
}