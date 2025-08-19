#include "JointManager.h"


namespace FlatEngine
{
	JointManager::JointManager(long myID, long parentID)
	{
		SetID(myID);
		SetParentID(parentID);
		SetType(T_JointManager);
	}

	JointManager::~JointManager()
	{
	}

	std::string JointManager::GetData()
	{
		json distanceJoints = json::array();
		json prismaticJoints = json::array();
		json revoluteJoints = json::array();
		json mouseJoints = json::array();
		json weldJoints = json::array();
		json motorJoints = json::array();
		json wheelJoints = json::array();

		for (DistanceJoint joint : m_distanceJoints)
		{
			distanceJoints.push_back(joint.GetJointData());
		}
		for (PrismaticJoint joint : m_prismaticJoints)
		{
			prismaticJoints.push_back(joint.GetJointData());
		}
		for (RevoluteJoint joint : m_revoluteJoints)
		{
			revoluteJoints.push_back(joint.GetJointData());
		}
		for (MouseJoint joint : m_mouseJoints)
		{
			mouseJoints.push_back(joint.GetJointData());
		}
		for (WeldJoint joint : m_weldJoints)
		{
			weldJoints.push_back(joint.GetJointData());
		}
		for (MotorJoint joint : m_motorJoints)
		{
			motorJoints.push_back(joint.GetJointData());
		}
		for (WheelJoint joint : m_wheelJoints)
		{
			wheelJoints.push_back(joint.GetJointData());
		}

		json jsonData = {
			{ "type", "JointManager"},
			{ "id", GetID() },
			{ "_isCollapsed", IsCollapsed() },
			{ "_isActive", IsActive() },
			{ "distanceJoints", distanceJoints },
			{ "prismaticJoints", prismaticJoints },
			{ "revoluteJoints", revoluteJoints },
			{ "mouseJoints", mouseJoints },
			{ "weldJoints", weldJoints },
			{ "motorJoints", motorJoints },
			{ "wheelJoints", wheelJoints }
		};

		std::string data = jsonData.dump();
		// Return dumped json object with required data for saving
		return data;
	}


	void JointManager::AddDistanceJoint(DistanceJoint::DistanceJointProps jointProps)
	{
		DistanceJoint distanceJoint = DistanceJoint(jointProps);	
		m_distanceJoints.push_back(distanceJoint);
	}

	void JointManager::AddPrismaticJoint(PrismaticJoint::PrismaticJointProps jointProps)
	{
		PrismaticJoint prismaticJoint = PrismaticJoint(jointProps);
		m_prismaticJoints.push_back(prismaticJoint);
	}

	void JointManager::AddRevoluteJoint(RevoluteJoint::RevoluteJointProps jointProps)
	{
		RevoluteJoint revoluteJoint = RevoluteJoint(jointProps);
		m_revoluteJoints.push_back(revoluteJoint);
	}

	void JointManager::AddMouseJoint(MouseJoint::MouseJointProps jointProps)
	{
		MouseJoint mouseJoint = MouseJoint(jointProps);
		m_mouseJoints.push_back(mouseJoint);
	}

	void JointManager::AddWeldJoint(WeldJoint::WeldJointProps jointProps)
	{
		WeldJoint weldJoint = WeldJoint(jointProps);
		m_weldJoints.push_back(weldJoint);
	}

	void JointManager::AddMotorJoint(MotorJoint::MotorJointProps jointProps)
	{
		MotorJoint motorJoint = MotorJoint(jointProps);
		m_motorJoints.push_back(motorJoint);
	}

	void JointManager::AddWheelJoint(WheelJoint::WheelJointProps jointProps)
	{
		WheelJoint wheelJoint = WheelJoint(jointProps);
		m_wheelJoints.push_back(wheelJoint);
	}

	std::vector<Joint*> JointManager::GetJoints()
	{
		std::vector<Joint*> joints = std::vector<Joint*>();

		for (DistanceJoint& joint : m_distanceJoints)
		{
			joints.push_back(&joint);
		}
		for (PrismaticJoint& joint : m_prismaticJoints)
		{
			joints.push_back(&joint);
		}
		for (RevoluteJoint& joint : m_revoluteJoints)
		{
			joints.push_back(&joint);
		}
		for (MouseJoint& joint : m_mouseJoints)
		{
			joints.push_back(&joint);
		}
		for (WheelJoint& joint : m_wheelJoints)
		{
			joints.push_back(&joint);
		}
		for (MotorJoint& joint : m_motorJoints)
		{
			joints.push_back(&joint);
		}
		for (WeldJoint& joint : m_weldJoints)
		{
			joints.push_back(&joint);
		}

		return joints;
	}

	void JointManager::RemoveJoint(b2JointId jointToDelete)
	{
		Joint* joint = static_cast<Joint*>(b2Joint_GetUserData(jointToDelete));

		switch (joint->GetJointType())
		{
		case Joint::JT_Distance:
		{
			for (std::list<DistanceJoint>::iterator jointIter = m_distanceJoints.begin(); jointIter != m_distanceJoints.end(); jointIter++)
			{
				if (jointIter->GetJointID().index1 == jointToDelete.index1)
				{
					m_distanceJoints.erase(jointIter);
					return;
				}
			}
			break;
		}
		}
	}
}