#include "RevoluteJoint.h"
#include "Body.h"


namespace FlatEngine
{
	RevoluteJoint::RevoluteJoint(BaseProps baseProps, RevoluteJointProps jointProps) : Joint(baseProps)
	{
		m_jointProps = jointProps;
		m_jointString = "Revolute Joint";
		m_baseProps.jointType = JT_Revolute;
	}

	RevoluteJoint::~RevoluteJoint()
	{
	}

	json RevoluteJoint::GetJointData()
	{
		json jointData = json::object();

		json jsonData = {
			{ "dampingRatio", m_jointProps.dampingRatio },
			{ "_enableLimit", m_jointProps.b_enableLimit },
			{ "_enableMotor", m_jointProps.b_enableMotor },
			{ "_enableSpring", m_jointProps.b_enableSpring },
			{ "hertz", m_jointProps.hertz },
			{ "drawSize", m_jointProps.drawSize },
			{ "lowerAngle", m_jointProps.lowerAngle },
			{ "upperAngle", m_jointProps.upperAngle },
			{ "maxMotorTorque", m_jointProps.maxMotorTorque },
			{ "motorSpeed", m_jointProps.motorSpeed },
			{ "referenceAngle", m_jointProps.referenceAngle },
			{ "targetAngle", m_jointProps.targetAngle }
		};

		jointData.emplace("baseProps", GetBasePropsData());
		jointData.emplace("jointProps", jsonData);

		return jointData;
	}

	RevoluteJoint::RevoluteJointProps& RevoluteJoint::GetJointProps()
	{
		return m_jointProps;
	}
}
