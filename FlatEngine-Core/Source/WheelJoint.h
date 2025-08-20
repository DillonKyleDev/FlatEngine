#pragma once
#include "Joint.h"



namespace FlatEngine
{
    class WheelJoint : public Joint
    {
    public:
		struct WheelJointProps : public Joint::JointProps {
			float dampingRatio = 0.5f;
			bool b_enableLimit = false;
			bool b_enableMotor = true;
			bool b_enableSpring = true;
			float hertz = 2.0f;
			Vector2 localAxisA = Vector2(0, 0);
			float lowerTranslation = 1.0f;
			float upperTranslation = 2.0f;
			float maxMotorTorque = 2000.0f;
			float motorSpeed = 10.0f;
		};

        WheelJoint(BaseProps baseProps, WheelJointProps jointProps);
        ~WheelJoint();
		json GetJointData();
		WheelJointProps& GetJointProps();

    private:
        WheelJointProps m_jointProps;
    };
}