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

        MouseJoint(BaseProps baseProps, MouseJointProps jointProps);
        ~MouseJoint();
        json GetJointData();
        MouseJointProps& GetJointProps();

    private:
        MouseJointProps m_jointProps;
    };

}