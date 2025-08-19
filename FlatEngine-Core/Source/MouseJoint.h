#pragma once
#include "Joint.h"



namespace FlatEngine
{
    class MouseJoint : public Joint
    {
    public:
        struct MouseJointProps : public Joint::JointProps {
            float dampingRatio = 0.5f;            
            float hertz = 2.0f;
            float maxForce = 2000.0f;
        };

        MouseJoint(MouseJointProps jointProps);
        ~MouseJoint();
        json GetJointData();
        JointProps* GetJointProps();

        void SetBodyA(Body* bodyA);
        void SetBodyB(Body* bodyB);

    private:
        MouseJointProps m_jointProps;
    };

}