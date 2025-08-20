#pragma once
#include "Joint.h"


namespace FlatEngine
{
	class Body;

    class DistanceJoint : public Joint
    {
    public:
		struct DistanceJointProps : public Joint::JointProps {			
			float dampingRatio = 0.5f;
			bool b_enableLimit = false;
			bool b_enableMotor = true;
			bool b_enableSpring = false;
			float hertz = 2.0f;
			float minLength = 3.0f;
			float maxLength = 5.0f;
			float length = 4.0f;
			float maxMotorForce = 2.0f;
			float motorSpeed = 10.0f;
		};

        DistanceJoint(BaseProps baseProps, DistanceJointProps jointProps);
        ~DistanceJoint();
		json GetJointData();
		DistanceJointProps& GetJointProps();

		void SetJointProps(DistanceJointProps jointProps);
		void SetLength(float length);
		float GetLength();
		void SetLengthRange(float minLength, float maxLength);
		void SetEnableSpring(bool b_enableSpring);
		void SetSpringHertz(float hertz);
		float GetSpringHertz();
		void SetSpringDampingRatio(float springDampingRatio);
		float GetSpringDampingRatio();
		void SetEnableMotor(bool b_enableMotor);
		void SetMotorSpeed(float motorSpeed);
		float GetMotorSpeed();
		void SetMaxMotorForce(float maxMotorForce);
		float GetMotorForce();
		void SetEnableLimit(bool b_enableLimit);

    private:
        DistanceJointProps m_jointProps;
    };
}

