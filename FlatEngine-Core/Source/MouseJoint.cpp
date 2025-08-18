#include "MouseJoint.h"



namespace FlatEngine
{
	MouseJoint::MouseJoint()
	{
		m_jointProps = MouseJointProps();
		m_jointProps.jointType = JT_Mouse;
		m_jointType = JT_Mouse;
	}

	MouseJoint::~MouseJoint()
	{
	}

	Joint::JointProps* MouseJoint::GetJointProps()
	{
		return &m_jointProps;
	}
}