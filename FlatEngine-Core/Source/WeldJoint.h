#pragma once
#include "Joint.h"



namespace FlatEngine
{
    class WeldJoint : public Joint
    {
    public:
        struct WeldJointProps : public Joint::JointProps {
            float angularDampingRatio = 0.5f;
            float linearDampingRatio = 0.5f;
            float angularHertz = 2.0f;
            float linearHertz = 2.0f;
            float referenceAngle = 0.0f;
        };

        WeldJoint(WeldJointProps jointProps);
        ~WeldJoint();
        json GetJointData();
        JointProps* GetJointProps();

        void SetBodyA(Body* bodyA);
        void SetBodyB(Body* bodyB);

    private:
        WeldJointProps m_jointProps;
    };
}