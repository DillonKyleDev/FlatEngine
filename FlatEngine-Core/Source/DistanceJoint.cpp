#include "DistanceJoint.h"



namespace FlatEngine
{
	DistanceJoint::DistanceJoint()
	{
		m_jointProps = DistanceJointProps();
		m_jointProps.jointType = JT_Distance;
		m_jointType = JT_Distance;
	}

	DistanceJoint::~DistanceJoint()
	{
	}

	Joint::JointProps* DistanceJoint::GetJointProps()
	{
		return &m_jointProps;
	}
}