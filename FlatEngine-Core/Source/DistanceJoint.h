#pragma once
#include "Joint.h"


namespace FlatEngine
{
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

    class DistanceJoint : public Joint
    {
    public:
        DistanceJoint();
        ~DistanceJoint();
        JointProps* GetJointProps();

    private:
        DistanceJointProps m_jointProps;
    };
}

