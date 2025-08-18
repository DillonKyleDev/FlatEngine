#pragma once
#include "Joint.h"



namespace FlatEngine
{
    struct WeldJointProps : public Joint::JointProps {
        float angularDampingRatio = 0.5f;
        float linearDampingRatio = 0.5f;
        float angularHertz = 2.0f;
        float linearHertz = 2.0f;
        float referenceAngle = 0.0f;
    };

    class WeldJoint : public Joint
    {
    public:
        WeldJoint();
        ~WeldJoint();
        JointProps* GetJointProps();

    private:
        WeldJointProps m_jointProps;
    };
}