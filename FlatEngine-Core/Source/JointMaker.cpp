#include "JointMaker.h"


namespace FlatEngine
{
	JointMaker::JointMaker(long myID, long parentID)
	{
		SetID(myID);
		SetParentID(parentID);
		SetType(T_JointMaker);

		m_distanceJoints = std::list<DistanceJoint>();
		m_prismaticJoints = std::list<PrismaticJoint>();
		m_revoluteJoints = std::list<RevoluteJoint>();
		m_mouseJoints = std::list<MouseJoint>();
		m_wheelJoints = std::list<WheelJoint>();
		m_motorJoints = std::list<MotorJoint>();
		m_weldJoints = std::list<WeldJoint>();
		m_nextJointID = 0;
	}

	JointMaker::~JointMaker()
	{
	}

	std::string JointMaker::GetData()
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
			{ "type", "JointMaker"},
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


	void JointMaker::AddDistanceJoint(Joint::BaseProps baseProps, DistanceJoint::DistanceJointProps jointProps)
	{
		DistanceJoint distanceJoint = DistanceJoint(baseProps, jointProps);
		distanceJoint.SetJointID(m_nextJointID);
		m_nextJointID++;
		m_distanceJoints.push_back(distanceJoint);
	}

	void JointMaker::AddPrismaticJoint(Joint::BaseProps baseProps, PrismaticJoint::PrismaticJointProps jointProps)
	{
		PrismaticJoint prismaticJoint = PrismaticJoint(baseProps, jointProps);
		prismaticJoint.SetJointID(m_nextJointID);
		m_nextJointID++;
		m_prismaticJoints.push_back(prismaticJoint);
	}

	void JointMaker::AddRevoluteJoint(Joint::BaseProps baseProps, RevoluteJoint::RevoluteJointProps jointProps)
	{
		RevoluteJoint revoluteJoint = RevoluteJoint(baseProps, jointProps);
		revoluteJoint.SetJointID(m_nextJointID);
		m_nextJointID++;
		m_revoluteJoints.push_back(revoluteJoint);
	}

	void JointMaker::AddMouseJoint(Joint::BaseProps baseProps, MouseJoint::MouseJointProps jointProps)
	{
		MouseJoint mouseJoint = MouseJoint(baseProps, jointProps);
		mouseJoint.SetJointID(m_nextJointID);
		m_nextJointID++;
		m_mouseJoints.push_back(mouseJoint);
	}

	void JointMaker::AddWeldJoint(Joint::BaseProps baseProps, WeldJoint::WeldJointProps jointProps)
	{
		WeldJoint weldJoint = WeldJoint(baseProps, jointProps);
		weldJoint.SetJointID(m_nextJointID);
		m_nextJointID++;
		m_weldJoints.push_back(weldJoint);
	}

	void JointMaker::AddMotorJoint(Joint::BaseProps baseProps, MotorJoint::MotorJointProps jointProps)
	{
		MotorJoint motorJoint = MotorJoint(baseProps, jointProps);
		motorJoint.SetJointID(m_nextJointID);
		m_nextJointID++;
		m_motorJoints.push_back(motorJoint);
	}

	void JointMaker::AddWheelJoint(Joint::BaseProps baseProps, WheelJoint::WheelJointProps jointProps)
	{
		WheelJoint wheelJoint = WheelJoint(baseProps, jointProps);
		wheelJoint.SetJointID(m_nextJointID);
		m_nextJointID++;
		m_wheelJoints.push_back(wheelJoint);
	}

	std::vector<Joint*> JointMaker::GetJoints()
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

	void JointMaker::Cleanup()
	{
		for (Joint* joint : GetJoints())
		{
			joint->DestroyJoint();
		}
	}

	void JointMaker::RemoveJoint(long jointIDToDelete)
	{
		for (std::list<DistanceJoint>::iterator jointIter = m_distanceJoints.begin(); jointIter != m_distanceJoints.end(); jointIter++)
		{
			if (jointIter->GetJointID() == jointIDToDelete)
			{
				jointIter->DestroyJoint();
				m_distanceJoints.erase(jointIter);
				return;
			}
		}

		for (std::list<PrismaticJoint>::iterator jointIter = m_prismaticJoints.begin(); jointIter != m_prismaticJoints.end(); jointIter++)
		{
			if (jointIter->GetJointID() == jointIDToDelete)
			{
				jointIter->DestroyJoint();
				m_prismaticJoints.erase(jointIter);
				return;
			}
		}

		for (std::list<RevoluteJoint>::iterator jointIter = m_revoluteJoints.begin(); jointIter != m_revoluteJoints.end(); jointIter++)
		{
			if (jointIter->GetJointID() == jointIDToDelete)
			{
				jointIter->DestroyJoint();
				m_revoluteJoints.erase(jointIter);
				return;
			}
		}

		for (std::list<WeldJoint>::iterator jointIter = m_weldJoints.begin(); jointIter != m_weldJoints.end(); jointIter++)
		{
			if (jointIter->GetJointID() == jointIDToDelete)
			{
				jointIter->DestroyJoint();
				m_weldJoints.erase(jointIter);
				return;
			}
		}

		for (std::list<MotorJoint>::iterator jointIter = m_motorJoints.begin(); jointIter != m_motorJoints.end(); jointIter++)
		{
			if (jointIter->GetJointID() == jointIDToDelete)
			{
				jointIter->DestroyJoint();
				m_motorJoints.erase(jointIter);
				return;
			}
		}

		for (std::list<WheelJoint>::iterator jointIter = m_wheelJoints.begin(); jointIter != m_wheelJoints.end(); jointIter++)
		{
			if (jointIter->GetJointID() == jointIDToDelete)
			{
				jointIter->DestroyJoint();
				m_wheelJoints.erase(jointIter);
				return;
			}
		}
	}
}