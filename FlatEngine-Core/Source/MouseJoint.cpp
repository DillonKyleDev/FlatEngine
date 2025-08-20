#include "MouseJoint.h"
#include "Body.h"


namespace FlatEngine
{
	MouseJoint::MouseJoint(BaseProps baseProps, MouseJointProps jointProps) : Joint(baseProps)
	{
		m_jointProps = jointProps;
		m_jointString = "Mouse Joint";
		m_baseProps.jointType = JT_Mouse;
	}

	MouseJoint::~MouseJoint()
	{
	}

	json MouseJoint::GetJointData()
	{
		json jointData = json::object();

		json jsonData = {
			{ "dampingRatio", m_jointProps.dampingRatio },
			{ "hertz", m_jointProps.hertz },
			{ "maxForce", m_jointProps.maxForce }
		};

		jointData.emplace("baseProps", GetBasePropsData());
		jointData.emplace("jointProps", jsonData);

		return jointData;
	}

	MouseJoint::MouseJointProps& MouseJoint::GetJointProps()
	{
		return m_jointProps;
	}
}