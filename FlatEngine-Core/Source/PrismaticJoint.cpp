#include "PrismaticJoint.h"
#include "Body.h"


namespace FlatEngine
{
	PrismaticJoint::PrismaticJoint(BaseProps baseProps, PrismaticJointProps jointProps) : Joint(baseProps)
	{
		m_jointProps = jointProps;	
		m_jointString = "Prismatic Joint";
		m_baseProps.jointType = JT_Prismatic;
	}

	PrismaticJoint::~PrismaticJoint()
	{
	}

	json PrismaticJoint::GetJointData()
	{
		json jointData = json::object();

		json jsonData = {
			{ "dampingRatio", m_jointProps.dampingRatio },
			{ "_enableLimit", m_jointProps.b_enableLimit },
			{ "_enableMotor", m_jointProps.b_enableMotor },
			{ "_enableSpring", m_jointProps.b_enableSpring },
			{ "hertz", m_jointProps.hertz },
			{ "targetTranslation", m_jointProps.targetTranslation },
			{ "lowerTranslation", m_jointProps.lowerTranslation },
			{ "upperTranslation", m_jointProps.upperTranslation },
			{ "maxMotorForce", m_jointProps.maxMotorForce },
			{ "motorSpeed", m_jointProps.motorSpeed },
			{ "referenceAngle", m_jointProps.referenceAngle },
			{ "localAxisAX", m_jointProps.localAxisA.x },
			{ "localAxisAY", m_jointProps.localAxisA.y }			
		};

		jointData.emplace("baseProps", GetBasePropsData());
		jointData.emplace("jointProps", jsonData);

		return jointData;
	}

	PrismaticJoint::PrismaticJointProps& PrismaticJoint::GetJointProps()
	{
		return m_jointProps;
	}


	void PrismaticJoint::SetJointProps(PrismaticJointProps jointProps)
	{
		m_jointProps = jointProps;
	}

	void PrismaticJoint::SetReferenceAngle(float length)
	{
		//b2PrismaticJoint_
	}

	float PrismaticJoint::GetReferenceAngle()
	{
		return 0;
	}

	void PrismaticJoint::SetTargetTranslation(float targetTranslation)
	{

	}

	float PrismaticJoint::GetTargetTranslation()
	{
		return 0;
	}

	// Translation upper and lower tied to b_enableLimit
	void PrismaticJoint::SetTranslationRange(float lowerTranslation, float upperTranslation)
	{
		if (true) // some constraint on translation range todo)
		{
			m_jointProps.lowerTranslation = lowerTranslation;
			m_jointProps.upperTranslation = upperTranslation;

			if (b2Joint_IsValid(m_b2JointID))
			{
				b2PrismaticJoint_SetLimits(m_b2JointID, lowerTranslation, upperTranslation);
			}
			else
			{
				CreateJoint();
			}
		}
	}

	void PrismaticJoint::SetEnableSpring(bool b_enableSpring)
	{
		m_jointProps.b_enableSpring = b_enableSpring;

		if (b2Joint_IsValid(m_b2JointID))
		{
			b2PrismaticJoint_EnableSpring(m_b2JointID, b_enableSpring);
		}
		else
		{
			CreateJoint();
		}
	}

	void PrismaticJoint::SetSpringHertz(float springHertz)
	{
		if (springHertz >= 0)
		{
			m_jointProps.hertz = springHertz;

			if (b2Joint_IsValid(m_b2JointID))
			{
				b2PrismaticJoint_SetSpringHertz(m_b2JointID, springHertz);
			}
			else
			{
				CreateJoint();
			}
		}
	}

	float PrismaticJoint::GetSpringHertz()
	{
		if (b2Joint_IsValid(m_b2JointID))
		{
			return b2PrismaticJoint_GetSpringHertz(m_b2JointID);
		}
		else
		{
			return m_jointProps.hertz;
		}
	}

	void PrismaticJoint::SetSpringDampingRatio(float springDampingRatio)
	{
		if (springDampingRatio >= 0)
		{
			m_jointProps.dampingRatio = springDampingRatio;

			if (b2Joint_IsValid(m_b2JointID))
			{
				b2PrismaticJoint_SetSpringDampingRatio(m_b2JointID, springDampingRatio);
			}
			else
			{
				CreateJoint();
			}
		}
	}

	float PrismaticJoint::GetSpringDampingRatio()
	{
		if (b2Joint_IsValid(m_b2JointID))
		{
			return b2PrismaticJoint_GetSpringDampingRatio(m_b2JointID);
		}
		else
		{
			return m_jointProps.dampingRatio;
		}
	}

	void PrismaticJoint::SetEnableMotor(bool b_enableMotor)
	{
		m_jointProps.b_enableMotor = b_enableMotor;

		if (b2Joint_IsValid(m_b2JointID))
		{
			b2PrismaticJoint_EnableSpring(m_b2JointID, b_enableMotor);
		}
		else
		{
			CreateJoint();
		}
	}

	void PrismaticJoint::SetMotorSpeed(float motorSpeed)
	{
		if (motorSpeed >= 0)
		{
			m_jointProps.motorSpeed = motorSpeed;

			if (b2Joint_IsValid(m_b2JointID))
			{
				b2PrismaticJoint_SetMotorSpeed(m_b2JointID, motorSpeed);
			}
			else
			{
				CreateJoint();
			}
		}
	}

	float PrismaticJoint::GetMotorSpeed()
	{
		if (b2Joint_IsValid(m_b2JointID))
		{
			return b2PrismaticJoint_GetMotorSpeed(m_b2JointID);
		}
		else
		{
			return m_jointProps.motorSpeed;
		}
	}

	void PrismaticJoint::SetMaxMotorForce(float maxMotorForce)
	{
		if (maxMotorForce >= 0)
		{
			m_jointProps.maxMotorForce = maxMotorForce;

			if (b2Joint_IsValid(m_b2JointID))
			{
				b2PrismaticJoint_SetMaxMotorForce(m_b2JointID, maxMotorForce);
			}
			else
			{
				CreateJoint();
			}
		}
	}

	float PrismaticJoint::GetMotorForce()
	{
		if (b2Joint_IsValid(m_b2JointID))
		{
			return b2PrismaticJoint_GetMotorForce(m_b2JointID);
		}
		else
		{
			return 0;
		}
	}

	void PrismaticJoint::SetEnableLimit(bool b_enableLimit)
	{
		m_jointProps.b_enableLimit = b_enableLimit;

		if (b2Joint_IsValid(m_b2JointID))
		{
			b2PrismaticJoint_EnableLimit(m_b2JointID, b_enableLimit);
		}
		else
		{
			CreateJoint();
		}
	}
}