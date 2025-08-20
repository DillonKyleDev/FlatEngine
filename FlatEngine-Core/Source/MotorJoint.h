#pragma once
#include "Joint.h"



namespace FlatEngine
{
    class MotorJoint : public Joint
    {
    public:
		struct MotorJointProps : public Joint::JointProps {
			float angularDampingRatio = 0.0f;
			float angularHertz = 2.0f;
			float angularVelocity = 10.0f;
			float linearDampingRatio = 0.5f;
			float linearHertz = 2.0f;
			Vector2 linearVelocity = Vector2(0.0f, 0.0f);
			float maxSpringForce = 20.0f;
			float maxSpringTorque = 2000.0f;
			float maxVelocityForce = 100.0f;
			float maxVelocityTorque = 2000.0f;
			Vector2 relativeTransformPos = Vector2(2.0f, 2.0f);
			float angleBetween = 0.0f;
		};

        MotorJoint(BaseProps baseProps, MotorJointProps jointProps);
        ~MotorJoint();
		json GetJointData();
		MotorJointProps& GetJointProps();

    private:
        MotorJointProps m_jointProps;
    };

}