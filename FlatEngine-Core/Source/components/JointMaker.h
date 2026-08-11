#pragma once
#include "components/Component.h"
#include "physics/Joint.h"

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
        void AddJoint(JointType type);
        std::vector<Joint*> GetJoints();
        void Cleanup();
        void RemoveJoint(Joint* jointToDelete);

    private:
        std::list<Joint> m_distanceJoints;
        std::list<Joint> m_prismaticJoints;
        std::list<Joint> m_revoluteJoints;
        std::list<Joint> m_mouseJoints;
        std::list<Joint> m_wheelJoints;
        std::list<Joint> m_motorJoints;
        std::list<Joint> m_weldJoints;
        long m_nextJointID;
    };
}
