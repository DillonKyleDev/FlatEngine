#pragma once
#include "components/Component.h"
#include "physics/joints/DistanceJoint.h"
#include "physics/joints/PrismaticJoint.h"
#include "physics/joints/RevoluteJoint.h"
#include "physics/joints/MotorJoint.h"
#include "physics/joints/MouseJoint.h"
#include "physics/joints/WeldJoint.h"
#include "physics/joints/WheelJoint.h"

#include <list>


namespace FlatEngine
{
    class Body;

    class JointMaker : public Component
    {
    public:
        JointMaker(long ownerID = -1);        
        json GetData(bool b_IDOverride = false);
        void PutData(json componentJson, std::string objectName);
        
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
