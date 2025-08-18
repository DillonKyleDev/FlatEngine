#pragma once
#include "Joint.h"



namespace FlatEngine
{
    struct MouseJointProps : public Joint::JointProps {
        float dampingRatio = 0.5f;
        Vector2 target = Vector2(0, 0);
        float hertz = 2.0f;
        float maxForce = 2000.0f;
    };

    class MouseJoint : public Joint
    {
    public:
        MouseJoint();
        ~MouseJoint();
        JointProps* GetJointProps();

    private:
        MouseJointProps m_jointProps;
    };

}