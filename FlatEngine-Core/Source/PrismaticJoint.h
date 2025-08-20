#pragma once
#include "Joint.h"


namespace FlatEngine
{
    class PrismaticJoint : public Joint
    {
    public:
		struct PrismaticJointProps : public Joint::JointProps {
			Vector2 localAxisA = Vector2(0, 0);
			float referenceAngle = 0.0f;
			float lowerTranslation = 1.0f;
			float upperTranslation = 2.0f;
			float maxMotorForce = 2000.0f;
			float motorSpeed = 10.0f;
			float dampingRatio = 0.5f;
			bool b_enableLimit = false;
			bool b_enableMotor = true;
			bool b_enableSpring = true;			
			float targetTranslation = 0.0f;
			float hertz = 2.0f;
		};

        PrismaticJoint(BaseProps baseProps, PrismaticJointProps jointProps);
        ~PrismaticJoint();
		json GetJointData();
		PrismaticJointProps& GetJointProps();

		void SetJointProps(PrismaticJointProps jointProps);
		void SetReferenceAngle(float length);
		float GetReferenceAngle();
		void SetTranslationRange(float lowerTranslation, float upperTranslation);
		void SetTargetTranslation(float targetTranslation);
		float GetTargetTranslation();
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
        PrismaticJointProps m_jointProps;
    };
}