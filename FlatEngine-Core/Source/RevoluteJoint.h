#pragma once
#include "Joint.h"



namespace FlatEngine
{
    class RevoluteJoint : public Joint
    {
    public:
		struct RevoluteJointProps : public Joint::JointProps {
			float dampingRatio = 0.5f;
			float drawSize = 3.0f;
			bool b_enableLimit = false;
			bool b_enableMotor = true;
			bool b_enableSpring = true;
			float hertz = 2.0f;
			float lowerAngle = -1.0f;
			float upperAngle = 1.0f;
			float maxMotorTorque = 2000.0f;
			float motorSpeed = 100.0f;
			float referenceAngle = 0.0f;
			float targetAngle = 0.0f;
		};

        RevoluteJoint(BaseProps baseProps, RevoluteJointProps jointProps);
        ~RevoluteJoint();
		json GetJointData();
        RevoluteJointProps& GetJointProps();

    private:
        RevoluteJointProps m_jointProps;
    };
}

