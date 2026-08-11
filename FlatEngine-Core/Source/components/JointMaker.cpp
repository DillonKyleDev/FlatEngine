#include "components/JointMaker.h"
#include "physics/Joint.h"
#include "physics/PhysicsManager.h"
#include "render/SceneView.h"
#include "tools/JsonHelper.h"
#include <id.h>


namespace FlatEngine
{
	JointMaker::JointMaker(long ownerID)
	{
		SetOwnerID(ownerID);
		SetType(ComponentType_JointMaker);

		m_distanceJoints = std::list<Joint>();
		m_prismaticJoints = std::list<Joint>();
		m_revoluteJoints = std::list<Joint>();
		m_mouseJoints = std::list<Joint>();
		m_wheelJoints = std::list<Joint>();
		m_motorJoints = std::list<Joint>();
		m_weldJoints = std::list<Joint>();
		m_nextJointID = 0;
	}

	json JointMaker::GetData(bool b_IDOverride)
	{
		json joints = json::array();

		for (Joint* joint : GetJoints())
		{
			joints.push_back(joint->GetData());
		}

		json jsonData = {
			{ "type", (int)GetType() },
			{ "b_isCollapsed", IsCollapsed() },
			{ "b_isActive", IsActive() },
			{ "joints", joints }
		};

		return jsonData;
	}

	void JointMaker::PutData(json componentJson, std::string objectName)
	{
        Component::PutData(componentJson, objectName);

		if (JsonHelper::JsonContains(componentJson, "joints", objectName))
		{		
			for (int i = 0; i < componentJson.at("joints").size(); i++)
			{
				json jointJson = componentJson.at("joints").at(i);
				JointType type = (JsonHelper::CheckJsonInt(jointJson, "jointType", objectName)) != -1 ? (JointType)(JsonHelper::CheckJsonInt(jointJson, "jointType", objectName)) : JointType_None;				
				Joint joint = Joint(GetOwnerID(), type);
				joint.PutData(jointJson, objectName);

				switch (type)
				{				
					case JointType_Distance:  m_distanceJoints.push_back(joint);  PhysicsManager::physics2D.CreateJoint(&m_distanceJoints.back()); break;
					case JointType_Revolute:  m_revoluteJoints.push_back(joint);  PhysicsManager::physics2D.CreateJoint(&m_revoluteJoints.back()); break;
					case JointType_Prismatic: m_prismaticJoints.push_back(joint); PhysicsManager::physics2D.CreateJoint(&m_prismaticJoints.back()); break;
					case JointType_Mouse:	  m_mouseJoints.push_back(joint);     PhysicsManager::physics2D.CreateJoint(&m_mouseJoints.back()); break;
					case JointType_Weld:      m_weldJoints.push_back(joint);      PhysicsManager::physics2D.CreateJoint(&m_weldJoints.back()); break;
					case JointType_Wheel:     m_wheelJoints.push_back(joint);     PhysicsManager::physics2D.CreateJoint(&m_wheelJoints.back()); break;
					case JointType_Motor:     m_motorJoints.push_back(joint);     PhysicsManager::physics2D.CreateJoint(&m_motorJoints.back()); break;
					default: break;					
				}
			}
		}
    }

	void JointMaker::AddJoint(JointType type)
	{
		Joint joint = Joint(GetOwnerID(), type);
		switch (type)
		{
			case JointType_Distance:  { DistanceJointData jointData;  joint.jointData = jointData; joint.renderShapes.push_back(SceneView::CreateQuadObject());   m_distanceJoints.push_back(joint); break; }
			case JointType_Prismatic: { PrismaticJointData jointData; joint.jointData = jointData; joint.renderShapes.push_back(SceneView::CreateCircleObject()); m_prismaticJoints.push_back(joint); break; }
			case JointType_Revolute:  { RevoluteJointData jointData;  joint.jointData = jointData; joint.renderShapes.push_back(SceneView::CreateLineObject());   m_revoluteJoints.push_back(joint); break; }
			case JointType_Mouse: 	  { MouseJointData jointData;     joint.jointData = jointData; joint.renderShapes.push_back(SceneView::CreateLineObject());   m_mouseJoints.push_back(joint); break; }			
			case JointType_Wheel: 	  { WheelJointData jointData;     joint.jointData = jointData; joint.renderShapes.push_back(SceneView::CreateLineObject());   m_wheelJoints.push_back(joint); break; }
			case JointType_Motor:     { MotorJointData jointData; 	  joint.jointData = jointData; joint.renderShapes.push_back(SceneView::CreateLineObject());   m_motorJoints.push_back(joint); break; }
			case JointType_Weld:      { WeldJointData jointData; 	  joint.jointData = jointData; joint.renderShapes.push_back(SceneView::CreateLineObject());   m_weldJoints.push_back(joint); break; }
			default: break;
		}
	}

	std::vector<Joint*> JointMaker::GetJoints()
	{
		std::vector<Joint*> joints = std::vector<Joint*>();

		for (Joint& joint : m_distanceJoints)
		{
			joints.push_back(&joint);
		}
		for (Joint& joint : m_prismaticJoints)
		{
			joints.push_back(&joint);
		}
		for (Joint& joint : m_revoluteJoints)
		{
			joints.push_back(&joint);
		}
		for (Joint& joint : m_mouseJoints)
		{
			joints.push_back(&joint);
		}
		for (Joint& joint : m_wheelJoints)
		{
			joints.push_back(&joint);
		}
		for (Joint& joint : m_motorJoints)
		{
			joints.push_back(&joint);
		}
		for (Joint& joint : m_weldJoints)
		{
			joints.push_back(&joint);
		}

		return joints;
	}

	void JointMaker::Cleanup()
	{
		for (Joint* joint : GetJoints())
		{
			PhysicsManager::physics2D.DestroyJoint(joint);
		}
	}

	void JointMaker::RemoveJoint(Joint* jointToDelete)
	{
		for (std::list<Joint>::iterator jointIter = m_distanceJoints.begin(); jointIter != m_distanceJoints.end(); jointIter++)
		{
			if (jointIter->GetJointID().index1 == jointToDelete->GetJointID().index1 && jointIter->GetJointID().world0 == jointToDelete->GetJointID().world0)
			{
				PhysicsManager::physics2D.DestroyJoint(&(*jointIter));
				m_distanceJoints.erase(jointIter);
				return;
			}
		}

		for (std::list<Joint>::iterator jointIter = m_prismaticJoints.begin(); jointIter != m_prismaticJoints.end(); jointIter++)
		{
			if (jointIter->GetJointID().index1 == jointToDelete->GetJointID().index1 && jointIter->GetJointID().world0 == jointToDelete->GetJointID().world0)
			{
				PhysicsManager::physics2D.DestroyJoint(&(*jointIter));
				m_prismaticJoints.erase(jointIter);
				return;
			}
		}

		for (std::list<Joint>::iterator jointIter = m_revoluteJoints.begin(); jointIter != m_revoluteJoints.end(); jointIter++)
		{
			if (jointIter->GetJointID().index1 == jointToDelete->GetJointID().index1 && jointIter->GetJointID().world0 == jointToDelete->GetJointID().world0)
			{
				PhysicsManager::physics2D.DestroyJoint(&(*jointIter));
				m_revoluteJoints.erase(jointIter);
				return;
			}
		}

		for (std::list<Joint>::iterator jointIter = m_weldJoints.begin(); jointIter != m_weldJoints.end(); jointIter++)
		{
			if (jointIter->GetJointID().index1 == jointToDelete->GetJointID().index1 && jointIter->GetJointID().world0 == jointToDelete->GetJointID().world0)
			{
				PhysicsManager::physics2D.DestroyJoint(&(*jointIter));
				m_weldJoints.erase(jointIter);
				return;
			}
		}

		for (std::list<Joint>::iterator jointIter = m_motorJoints.begin(); jointIter != m_motorJoints.end(); jointIter++)
		{
			if (jointIter->GetJointID().index1 == jointToDelete->GetJointID().index1 && jointIter->GetJointID().world0 == jointToDelete->GetJointID().world0)
			{
				PhysicsManager::physics2D.DestroyJoint(&(*jointIter));
				m_motorJoints.erase(jointIter);
				return;
			}
		}

		for (std::list<Joint>::iterator jointIter = m_wheelJoints.begin(); jointIter != m_wheelJoints.end(); jointIter++)
		{
			if (jointIter->GetJointID().index1 == jointToDelete->GetJointID().index1 && jointIter->GetJointID().world0 == jointToDelete->GetJointID().world0)
			{
				PhysicsManager::physics2D.DestroyJoint(&(*jointIter));
				m_wheelJoints.erase(jointIter);
				return;
			}
		}
	}
}