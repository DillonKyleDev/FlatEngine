#include "WheelJoint.h"


namespace FlatEngine
{
	WheelJoint::WheelJoint()
	{
		m_jointProps = WheelJointProps();
		m_jointProps.jointType = JT_Wheel;
		m_jointType = JT_Wheel;
	}

	WheelJoint::~WheelJoint()
	{
	}

	Joint::JointProps* WheelJoint::GetJointProps()
	{
		return &m_jointProps;
	}
}