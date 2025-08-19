#include "MouseJoint.h"
#include "Body.h"


namespace FlatEngine
{
	MouseJoint::MouseJoint(MouseJointProps jointProps) : Joint(&jointProps)
	{
		m_jointProps = jointProps;
		m_jointString = "Mouse Joint";
		m_jointProps.jointType = JT_Mouse;
		m_jointType = JT_Mouse;
	}

	MouseJoint::~MouseJoint()
	{
	}

	json MouseJoint::GetJointData()
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
			{ "dampingRatio", m_jointProps.dampingRatio },
			{ "hertz", m_jointProps.hertz },
			{ "maxForce", m_jointProps.maxForce }
		};

		return jsonData;
	}

	Joint::JointProps* MouseJoint::GetJointProps()
	{
		return &m_jointProps;
	}

	void MouseJoint::SetBodyA(Body* bodyA)
	{
		Joint::SetBodyA(bodyA);
		m_jointProps.bodyAID = bodyA->GetParentID();
	}

	void MouseJoint::SetBodyB(Body* bodyB)
	{
		Joint::SetBodyB(bodyB);
		m_jointProps.bodyBID = bodyB->GetParentID();
	}
}