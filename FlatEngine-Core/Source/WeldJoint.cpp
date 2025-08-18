#include "WeldJoint.h"



namespace FlatEngine
{
	WeldJoint::WeldJoint()
	{
		m_jointProps = WeldJointProps();
		m_jointProps.jointType = JT_Weld;
		m_jointType = JT_Weld;
	}

	WeldJoint::~WeldJoint()
	{
	}

	Joint::JointProps* WeldJoint::GetJointProps()
	{
		return &m_jointProps;
	}
}