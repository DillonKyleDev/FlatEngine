#include "DistanceJoint.h"
#include "Body.h"
#include "FlatEngine.h"


namespace FlatEngine
{
	DistanceJoint::DistanceJoint(BaseProps baseProps, DistanceJointProps jointProps) : Joint(baseProps)
	{
		m_jointProps = jointProps;
		m_jointString = "Distance Joint";
		m_baseProps.jointType = JT_Distance;		
	}

	DistanceJoint::~DistanceJoint()
	{
	}

	json DistanceJoint::GetJointData()
	{	
		json jointData = json::object();

		json jsonData = {			
			{ "dampingRatio", m_jointProps.dampingRatio },
			{ "_enableLimit", m_jointProps.b_enableLimit },
			{ "_enableMotor", m_jointProps.b_enableMotor },
			{ "_enableSpring", m_jointProps.b_enableSpring },
			{ "hertz", m_jointProps.hertz },
			{ "minLength", m_jointProps.minLength },
			{ "maxLength", m_jointProps.maxLength },
			{ "length", m_jointProps.length },
			{ "maxMotorForce", m_jointProps.maxMotorForce },
			{ "motorSpeed", m_jointProps.motorSpeed }
		};

		jointData.emplace("baseProps", GetBasePropsData());
		jointData.emplace("jointProps", jsonData);

		return jointData;
	}

	DistanceJoint::DistanceJointProps& DistanceJoint::GetJointProps()
	{
		return m_jointProps;
	}

	void DistanceJoint::SetJointProps(DistanceJointProps jointProps)
	{
		m_jointProps = jointProps;
	}


	void DistanceJoint::SetLength(float length)
	{
		if (length > 0)
		{
			m_jointProps.length = length;

			if (b2Joint_IsValid(m_b2JointID))
			{
				b2DistanceJoint_SetLength(m_b2JointID, length);
			}
			else
			{
				CreateJoint();
			}
		}
	}

	float DistanceJoint::GetLength()
	{
		if (b2Joint_IsValid(m_b2JointID))
		{
			return b2DistanceJoint_GetLength(m_b2JointID);
		}
		else
		{
			return m_jointProps.length;
		}
	}

	void DistanceJoint::SetLengthRange(float minLength, float maxLength)
	{
		if (minLength >= 0 && maxLength >= minLength)
		{
			m_jointProps.minLength = minLength;
			m_jointProps.maxLength = maxLength;

			if (b2Joint_IsValid(m_b2JointID))
			{
				b2DistanceJoint_SetLengthRange(m_b2JointID, minLength, maxLength);				
			}
			else
			{
				CreateJoint();
			}
		}
	}

	void DistanceJoint::SetEnableSpring(bool b_enableSpring)
	{
		m_jointProps.b_enableSpring = b_enableSpring;

		if (b2Joint_IsValid(m_b2JointID))
		{
			b2DistanceJoint_EnableSpring(m_b2JointID, b_enableSpring);
		}
		else
		{
			CreateJoint();
		}
	}

	void DistanceJoint::SetSpringHertz(float springHertz)
	{
		if (springHertz >= 0)
		{
			m_jointProps.hertz = springHertz;

			if (b2Joint_IsValid(m_b2JointID))
			{
				b2DistanceJoint_SetSpringHertz(m_b2JointID, springHertz);
			}
			else
			{
				CreateJoint();
			}
		}
	}

	float DistanceJoint::GetSpringHertz()
	{
		if (b2Joint_IsValid(m_b2JointID))
		{
			return b2DistanceJoint_GetSpringHertz(m_b2JointID);
		}
		else
		{
			return m_jointProps.hertz;
		}
	}

	void DistanceJoint::SetSpringDampingRatio(float springDampingRatio)
	{
		if (springDampingRatio >= 0)
		{
			m_jointProps.dampingRatio = springDampingRatio;

			if (b2Joint_IsValid(m_b2JointID))
			{				
				b2DistanceJoint_SetSpringDampingRatio(m_b2JointID, springDampingRatio);
			}
			else
			{
				CreateJoint();
			}
		}
	}

	float DistanceJoint::GetSpringDampingRatio()
	{
		if (b2Joint_IsValid(m_b2JointID))
		{
			return b2DistanceJoint_GetSpringDampingRatio(m_b2JointID);
		}
		else
		{
			return m_jointProps.dampingRatio;
		}
	}

	void DistanceJoint::SetEnableMotor(bool b_enableMotor)
	{
		m_jointProps.b_enableMotor = b_enableMotor;

		if (b2Joint_IsValid(m_b2JointID))
		{
			b2DistanceJoint_EnableSpring(m_b2JointID, b_enableMotor);
		}
		else
		{
			CreateJoint();
		}
	}

	void DistanceJoint::SetMotorSpeed(float motorSpeed)
	{
		if (motorSpeed >= 0)
		{
			m_jointProps.motorSpeed = motorSpeed;

			if (b2Joint_IsValid(m_b2JointID))
			{
				b2DistanceJoint_SetMotorSpeed(m_b2JointID, motorSpeed);
			}
			else
			{
				CreateJoint();
			}
		}
	}

	float DistanceJoint::GetMotorSpeed()
	{
		if (b2Joint_IsValid(m_b2JointID))
		{
			return b2DistanceJoint_GetMotorSpeed(m_b2JointID);
		}
		else
		{
			return m_jointProps.motorSpeed;
		}		
	}

	void DistanceJoint::SetMaxMotorForce(float maxMotorForce)
	{
		if (maxMotorForce >= 0)
		{
			m_jointProps.maxMotorForce = maxMotorForce;

			if (b2Joint_IsValid(m_b2JointID))
			{
				b2DistanceJoint_SetMaxMotorForce(m_b2JointID, maxMotorForce);
			}
			else
			{
				CreateJoint();
			}
		}
	}

	float DistanceJoint::GetMotorForce()
	{
		if (b2Joint_IsValid(m_b2JointID))
		{
			return b2DistanceJoint_GetMotorForce(m_b2JointID);
		}
		else
		{
			return 0;
		}
	}

	void DistanceJoint::SetEnableLimit(bool b_enableLimit)
	{
		m_jointProps.b_enableLimit = b_enableLimit;

		if (b2Joint_IsValid(m_b2JointID))
		{
			b2DistanceJoint_EnableLimit(m_b2JointID, b_enableLimit);
		}
		else
		{
			CreateJoint();
		}
	}
}