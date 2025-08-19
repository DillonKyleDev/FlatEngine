#pragma once
#include "Component.h"
#include "DistanceJoint.h"
#include "PrismaticJoint.h"
#include "RevoluteJoint.h"
#include "MouseJoint.h"
#include "WheelJoint.h"
#include "MotorJoint.h"
#include "WeldJoint.h"

#include <string>
#include <list>

namespace FlatEngine
{
    class Body;

    class JointManager : public Component
    {
    public:
        JointManager(long myID = -1, long parentID = -1);
        ~JointManager();
        std::string GetData();
        
        void AddDistanceJoint(DistanceJoint::DistanceJointProps jointProps = DistanceJoint::DistanceJointProps());
        void AddPrismaticJoint(PrismaticJoint::PrismaticJointProps jointProps = PrismaticJoint::PrismaticJointProps());
        void AddRevoluteJoint(RevoluteJoint::RevoluteJointProps jointProps = RevoluteJoint::RevoluteJointProps());
        void AddMouseJoint(MouseJoint::MouseJointProps jointProps = MouseJoint::MouseJointProps());
        void AddMotorJoint(MotorJoint::MotorJointProps jointProps = MotorJoint::MotorJointProps());
        void AddWeldJoint(WeldJoint::WeldJointProps jointProps = WeldJoint::WeldJointProps());
        void AddWheelJoint(WheelJoint::WheelJointProps jointProps = WheelJoint::WheelJointProps());

        std::vector<Joint*> GetJoints();

        void RemoveJoint(b2JointId jointToDelete);

    private:
        std::list<DistanceJoint> m_distanceJoints;
        std::list<PrismaticJoint> m_prismaticJoints;
        std::list<RevoluteJoint> m_revoluteJoints;
        std::list<MouseJoint> m_mouseJoints;
        std::list<WheelJoint> m_wheelJoints;
        std::list<MotorJoint> m_motorJoints;
        std::list<WeldJoint> m_weldJoints;
    };
}
