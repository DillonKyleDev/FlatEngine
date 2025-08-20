#include "WeldJoint.h"
#include "Body.h"


namespace FlatEngine
{
	WeldJoint::WeldJoint(BaseProps baseProps, WeldJointProps jointProps) : Joint(baseProps)
	{
		m_jointProps = jointProps;
		m_jointString = "Weld Joint";
		m_baseProps.jointType = JT_Weld;
	}

	WeldJoint::~WeldJoint()
	{
	}

	json WeldJoint::GetJointData()
	{
		json jointData = json::object();

		json jsonData = {
			{ "angularDampingRatio", m_jointProps.angularDampingRatio },
			{ "angularHertz", m_jointProps.angularHertz },
			{ "linearDampingRatio", m_jointProps.linearDampingRatio },
			{ "linearHertz", m_jointProps.linearHertz },
			{ "referenceAngle", m_jointProps.referenceAngle }
		};

		jointData.emplace("baseProps", GetBasePropsData());
		jointData.emplace("jointProps", jsonData);

		return jointData;
	}

	WeldJoint::WeldJointProps& WeldJoint::GetJointProps()
	{
		return m_jointProps;
	}
}