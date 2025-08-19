#include "WheelJoint.h"
#include "Body.h"



namespace FlatEngine
{
	WheelJoint::WheelJoint(WheelJointProps jointProps) : Joint(&jointProps)
	{
		m_jointProps = jointProps;
		m_jointString = "Wheel Joint";
		m_jointProps.jointType = JT_Wheel;
		m_jointType = JT_Wheel;
	}

	WheelJoint::~WheelJoint()
	{
	}

	json WheelJoint::GetJointData()
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
			{ "localAxisAX", m_jointProps.localAxisA.x },
			{ "localAxisAY", m_jointProps.localAxisA.y },
			{ "lowerTranslation", m_jointProps.lowerTranslation },
			{ "upperTranslation", m_jointProps.upperTranslation },
			{ "maxMotorTorque", m_jointProps.maxMotorTorque },
			{ "motorSpeed", m_jointProps.motorSpeed }			
		};

		return jsonData;
	}

	Joint::JointProps* WheelJoint::GetJointProps()
	{
		return &m_jointProps;
	}

	void WheelJoint::SetBodyA(Body* bodyA)
	{
		Joint::SetBodyA(bodyA);
		m_jointProps.bodyAID = bodyA->GetParentID();
	}

	void WheelJoint::SetBodyB(Body* bodyB)
	{
		Joint::SetBodyB(bodyB);
		m_jointProps.bodyBID = bodyB->GetParentID();
	}
}