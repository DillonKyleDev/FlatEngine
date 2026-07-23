#pragma once
#include "components/Component.h"
#include "joints/DistanceJoint.h"
#include "joints/PrismaticJoint.h"
#include "joints/RevoluteJoint.h"
#include "joints/MotorJoint.h"
#include "joints/MouseJoint.h"
#include "joints/WeldJoint.h"
#include "joints/WheelJoint.h"
#include "tools/JsonHelper.h"

#include <list>
#include <string>


namespace FlatEngine
{
    class Body;

    class JointMaker : public Component
    {
    public:
        JointMaker(long myID = -1, long parentObjectID = -1);        
        std::string GetData();
        void PutData(json componentJson);
        
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
