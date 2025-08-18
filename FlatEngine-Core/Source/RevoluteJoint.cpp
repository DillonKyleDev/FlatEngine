#include "RevoluteJoint.h"



namespace FlatEngine
{
	RevoluteJoint::RevoluteJoint()
	{
		m_jointProps = RevoluteJointProps();
		m_jointProps.jointType = JT_Revolute;
		m_jointType = JT_Revolute;
	}

	RevoluteJoint::~RevoluteJoint()
	{
	}

	Joint::JointProps* RevoluteJoint::GetJointProps()
	{
		return &m_jointProps;
	}
}
