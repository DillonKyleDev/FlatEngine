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

    class JointMaker : public Component
    {
    public:
        JointMaker(long myID = -1, long parentID = -1);
        ~JointMaker();
        std::string GetData();
        
        void AddDistanceJoint(Joint::BaseProps baseProps = Joint::BaseProps(), DistanceJoint::DistanceJointProps jointProps = DistanceJoint::DistanceJointProps());
        void AddPrismaticJoint(Joint::BaseProps baseProps = Joint::BaseProps(), PrismaticJoint::PrismaticJointProps jointProps = PrismaticJoint::PrismaticJointProps());
        void AddRevoluteJoint(Joint::BaseProps baseProps = Joint::BaseProps(), RevoluteJoint::RevoluteJointProps jointProps = RevoluteJoint::RevoluteJointProps());
        void AddMouseJoint(Joint::BaseProps baseProps = Joint::BaseProps(), MouseJoint::MouseJointProps jointProps = MouseJoint::MouseJointProps());
        void AddMotorJoint(Joint::BaseProps baseProps = Joint::BaseProps(), MotorJoint::MotorJointProps jointProps = MotorJoint::MotorJointProps());
        void AddWeldJoint(Joint::BaseProps baseProps = Joint::BaseProps(), WeldJoint::WeldJointProps jointProps = WeldJoint::WeldJointProps());
        void AddWheelJoint(Joint::BaseProps baseProps = Joint::BaseProps(), WheelJoint::WheelJointProps jointProps = WheelJoint::WheelJointProps());

        std::vector<Joint*> GetJoints();

        void Cleanup();
        void RemoveJoint(long jointIDToDelete);

    private:
        std::list<DistanceJoint> m_distanceJoints;
        std::list<PrismaticJoint> m_prismaticJoints;
        std::list<RevoluteJoint> m_revoluteJoints;
        std::list<MouseJoint> m_mouseJoints;
        std::list<WheelJoint> m_wheelJoints;
        std::list<MotorJoint> m_motorJoints;
        std::list<WeldJoint> m_weldJoints;
        long m_nextJointID;
    };
}
