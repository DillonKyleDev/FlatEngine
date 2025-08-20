#include "MotorJoint.h"
#include "Body.h"



namespace FlatEngine
{
	MotorJoint::MotorJoint(BaseProps baseProps, MotorJointProps jointProps) : Joint(baseProps)
	{
		m_jointProps = jointProps;
		m_jointString = "Motor Joint";
		m_baseProps.jointType = JT_Motor;		
	}

	MotorJoint::~MotorJoint()
	{
	}

	json MotorJoint::GetJointData()
	{
		json jointData = json::object();

		json jsonData = {
			{ "angleBetween", m_jointProps.angleBetween },
			{ "angularDampingRatio", m_jointProps.angularDampingRatio },
			{ "angularHertz", m_jointProps.angularHertz },
			{ "angularVelocity", m_jointProps.angularVelocity },
			{ "linearDampingRatio", m_jointProps.linearDampingRatio },
			{ "linearHertz", m_jointProps.linearHertz },
			{ "linearVelocityX", m_jointProps.linearVelocity.x },
			{ "linearVelocityY", m_jointProps.linearVelocity.y },
			{ "maxSpringForce", m_jointProps.maxSpringForce },
			{ "maxSpringTorque", m_jointProps.maxSpringTorque },
			{ "maxVelocityForce", m_jointProps.maxVelocityForce },
			{ "relativeTransformPosX", m_jointProps.relativeTransformPos.x },
			{ "relativeTransformPosY", m_jointProps.relativeTransformPos.y }
		};

		jointData.emplace("baseProps", GetBasePropsData());
		jointData.emplace("jointProps", jsonData);

		return jointData;
	}

	MotorJoint::MotorJointProps& MotorJoint::GetJointProps()
	{
		return m_jointProps;
	}
}