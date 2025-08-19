#include "RevoluteJoint.h"
#include "Body.h"


namespace FlatEngine
{
	RevoluteJoint::RevoluteJoint(RevoluteJointProps jointProps) : Joint(&jointProps)
	{
		m_jointProps = jointProps;
		m_jointString = "Revolute Joint";
		m_jointProps.jointType = JT_Revolute;
		m_jointType = JT_Revolute;
	}

	RevoluteJoint::~RevoluteJoint()
	{
	}

	json RevoluteJoint::GetJointData()
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
			{ "_enableLimit", m_jointProps.b_enableLimit },
			{ "_enableMotor", m_jointProps.b_enableMotor },
			{ "_enableSpring", m_jointProps.b_enableSpring },
			{ "hertz", m_jointProps.hertz },
			{ "drawSize", m_jointProps.drawSize },
			{ "lowerAngle", m_jointProps.lowerAngle },
			{ "upperAngle", m_jointProps.upperAngle },
			{ "maxMotorTorque", m_jointProps.maxMotorTorque },
			{ "motorSpeed", m_jointProps.motorSpeed },
			{ "referenceAngle", m_jointProps.referenceAngle },
			{ "targetAngle", m_jointProps.targetAngle }
		};

		return jsonData;
	}

	Joint::JointProps* RevoluteJoint::GetJointProps()
	{
		return &m_jointProps;
	}

	void RevoluteJoint::SetBodyA(Body* bodyA)
	{
		Joint::SetBodyA(bodyA);
		m_jointProps.bodyAID = bodyA->GetParentID();
	}

	void RevoluteJoint::SetBodyB(Body* bodyB)
	{
		Joint::SetBodyB(bodyB);
		m_jointProps.bodyBID = bodyB->GetParentID();
	}
}
