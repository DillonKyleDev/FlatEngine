#include "WheelJoint.h"
#include "Body.h"



namespace FlatEngine
{
	WheelJoint::WheelJoint(BaseProps baseProps, WheelJointProps jointProps) : Joint(baseProps)
	{
		m_jointProps = jointProps;
		m_jointString = "Wheel Joint";
		m_baseProps.jointType = JT_Wheel;		
	}

	WheelJoint::~WheelJoint()
	{
	}

	json WheelJoint::GetJointData()
	{
		json jointData = json::object();

		json jsonData = {
			{ "dampingRatio", m_jointProps.dampingRatio },
			{ "_enableLimit", m_jointProps.b_enableLimit },
			{ "_enableMotor", m_jointProps.b_enableMotor },
			{ "_enableSpring", m_jointProps.b_enableSpring },
			{ "hertz", m_jointProps.hertz },
			{ "localAxisAX", m_jointProps.localAxisA.x },
			{ "localAxisAY", m_jointProps.localAxisA.y },
			{ "lowerTranslation", m_jointProps.lowerTranslation },
			{ "upperTranslation", m_jointProps.upperTranslation },
			{ "maxMotorTorque", m_jointProps.maxMotorTorque },
			{ "motorSpeed", m_jointProps.motorSpeed }			
		};

		jointData.emplace("baseProps", GetBasePropsData());
		jointData.emplace("jointProps", jsonData);

		return jointData;
	}

	WheelJoint::WheelJointProps& WheelJoint::GetJointProps()
	{
		return m_jointProps;
	}
}