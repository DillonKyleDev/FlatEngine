#include "PrismaticJoint.h"


namespace FlatEngine
{
	PrismaticJoint::PrismaticJoint()
	{
		m_jointProps = PrismaticJointProps();
		m_jointProps.jointType = JT_Prismatic;
		m_jointType = JT_Prismatic;
	}

	PrismaticJoint::~PrismaticJoint()
	{
	}

	Joint::JointProps* PrismaticJoint::GetJointProps()
	{
		return &m_jointProps;
	}
}