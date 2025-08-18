#include "MotorJoint.h"



namespace FlatEngine
{
	MotorJoint::MotorJoint()
	{
		m_jointProps = MotorJointProps();
		m_jointProps.jointType = JT_Motor;
		m_jointType = JT_Motor;
	}

	MotorJoint::~MotorJoint()
	{
	}

	Joint::JointProps* MotorJoint::GetJointProps()
	{
		return &m_jointProps;
	}
}