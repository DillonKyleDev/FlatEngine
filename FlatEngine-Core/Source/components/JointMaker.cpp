#include "components/JointMaker.h"
#include "physics/PhysicsManager.h"
#include "tools/Logger.h"


namespace FlatEngine
{
	JointMaker::JointMaker(long ownerID)
	{
		SetOwnerID(ownerID);
		SetType(ComponentType_JointMaker);

		m_distanceJoints = std::list<DistanceJoint>();
		m_prismaticJoints = std::list<PrismaticJoint>();
		m_revoluteJoints = std::list<RevoluteJoint>();
		m_mouseJoints = std::list<MouseJoint>();
		m_wheelJoints = std::list<WheelJoint>();
		m_motorJoints = std::list<MotorJoint>();
		m_weldJoints = std::list<WeldJoint>();
		m_nextJointID = 0;
	}

	json JointMaker::GetData(bool b_IDOverride)
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
			{ "type", (int)GetType() },
			{ "b_isCollapsed", IsCollapsed() },
			{ "b_isActive", IsActive() },
			{ "distanceJoints", distanceJoints },
			{ "prismaticJoints", prismaticJoints },
			{ "revoluteJoints", revoluteJoints },
			{ "mouseJoints", mouseJoints },
			{ "weldJoints", weldJoints },
			{ "motorJoints", motorJoints },
			{ "wheelJoints", wheelJoints }
		};

		return jsonData;
	}

	void RetrieveBaseJointProps(Joint::BaseProps& jointProps, json jointJson, std::string objectName)
	{
		jointProps.jointType = (Joint::JointType)JsonHelper::CheckJsonInt(jointJson, "jointType", objectName);
		jointProps.b_collideConnected = JsonHelper::CheckJsonBool(jointJson, "b_collideConnected", objectName);
		jointProps.bodyAID = JsonHelper::CheckJsonLong(jointJson, "bodyAID", objectName);
		jointProps.bodyBID = JsonHelper::CheckJsonLong(jointJson, "bodyBID", objectName);
		jointProps.anchorA.x = JsonHelper::CheckJsonFloat(jointJson, "anchorAX", objectName);
		jointProps.anchorA.y = JsonHelper::CheckJsonFloat(jointJson, "anchorAY", objectName);
		jointProps.anchorB.x = JsonHelper::CheckJsonFloat(jointJson, "anchorBX", objectName);
		jointProps.anchorB.y = JsonHelper::CheckJsonFloat(jointJson, "anchorAY", objectName);
	}

	void RetrieveDistanceJointProps(DistanceJoint::DistanceJointProps& jointProps, json jointJson, std::string objectName)
	{
		jointProps.b_enableLimit = JsonHelper::CheckJsonBool(jointJson, "b_enableLimit", objectName);
		jointProps.b_enableMotor = JsonHelper::CheckJsonBool(jointJson, "b_enableMotor", objectName);
		jointProps.b_enableSpring = JsonHelper::CheckJsonBool(jointJson, "b_enableSpring", objectName);
		jointProps.dampingRatio = JsonHelper::CheckJsonFloat(jointJson, "dampingRatio", objectName);
		jointProps.hertz = JsonHelper::CheckJsonFloat(jointJson, "hertz", objectName);
		jointProps.minLength = JsonHelper::CheckJsonFloat(jointJson, "minLength", objectName);
		jointProps.maxLength = JsonHelper::CheckJsonFloat(jointJson, "maxLength", objectName);		
		jointProps.length = JsonHelper::CheckJsonFloat(jointJson, "length", objectName);
		jointProps.maxMotorForce = JsonHelper::CheckJsonFloat(jointJson, "maxMotorForce", objectName);
		jointProps.motorSpeed = JsonHelper::CheckJsonFloat(jointJson, "motorSpeed", objectName);
	}

	void RetrievePrismaticJointProps(PrismaticJoint::PrismaticJointProps& jointProps, json jointJson, std::string objectName)
	{
		jointProps.b_enableLimit = JsonHelper::CheckJsonBool(jointJson, "b_enableLimit", objectName);
		jointProps.b_enableMotor = JsonHelper::CheckJsonBool(jointJson, "b_enableMotor", objectName);
		jointProps.b_enableSpring = JsonHelper::CheckJsonBool(jointJson, "b_enableSpring", objectName);
		jointProps.dampingRatio = JsonHelper::CheckJsonFloat(jointJson, "dampingRatio", objectName);
		jointProps.hertz = JsonHelper::CheckJsonFloat(jointJson, "hertz", objectName);
		jointProps.localAxisA = Vector2(JsonHelper::CheckJsonFloat(jointJson, "localAxisAX", objectName), JsonHelper::CheckJsonFloat(jointJson, "localAxisAY", objectName));
		jointProps.lowerTranslation = JsonHelper::CheckJsonFloat(jointJson, "lowerTranslation", objectName);
		jointProps.upperTranslation = JsonHelper::CheckJsonFloat(jointJson, "upperTranslation", objectName);
		jointProps.targetTranslation = JsonHelper::CheckJsonFloat(jointJson, "targetTranslation", objectName);
		jointProps.maxMotorForce = JsonHelper::CheckJsonFloat(jointJson, "maxMotorForce", objectName);
		jointProps.motorSpeed = JsonHelper::CheckJsonFloat(jointJson, "motorSpeed", objectName);
	}

	void RetrieveRevoluteJointProps(RevoluteJoint::RevoluteJointProps& jointProps, json jointJson, std::string objectName)
	{
		jointProps.b_enableLimit = JsonHelper::CheckJsonBool(jointJson, "b_enableLimit", objectName);
		jointProps.b_enableMotor = JsonHelper::CheckJsonBool(jointJson, "b_enableMotor", objectName);
		jointProps.b_enableSpring = JsonHelper::CheckJsonBool(jointJson, "b_enableSpring", objectName);
		jointProps.dampingRatio = JsonHelper::CheckJsonFloat(jointJson, "dampingRatio", objectName);
		jointProps.hertz = JsonHelper::CheckJsonFloat(jointJson, "hertz", objectName);
		jointProps.drawSize = JsonHelper::CheckJsonFloat(jointJson, "drawSize", objectName);
		jointProps.lowerAngle = JsonHelper::CheckJsonFloat(jointJson, "lowerAngle", objectName);
		jointProps.upperAngle = JsonHelper::CheckJsonFloat(jointJson, "upperAngle", objectName);
		jointProps.maxMotorTorque = JsonHelper::CheckJsonFloat(jointJson, "maxMotorForce", objectName);
		jointProps.motorSpeed = JsonHelper::CheckJsonFloat(jointJson, "motorSpeed", objectName);
		jointProps.referenceAngle = JsonHelper::CheckJsonFloat(jointJson, "referenceAngle", objectName);
		jointProps.targetAngle = JsonHelper::CheckJsonFloat(jointJson, "targetAngle", objectName);		
	}

	void RetrieveMouseJointProps(MouseJoint::MouseJointProps& jointProps, json jointJson, std::string objectName)
	{
		jointProps.dampingRatio = JsonHelper::CheckJsonFloat(jointJson, "dampingRatio", objectName);
		jointProps.hertz = JsonHelper::CheckJsonFloat(jointJson, "hertz", objectName);
		jointProps.maxForce = JsonHelper::CheckJsonFloat(jointJson, "maxForce", objectName);
	}

	void RetrieveWeldJointProps(WeldJoint::WeldJointProps& jointProps, json jointJson, std::string objectName)
	{
		jointProps.angularDampingRatio = JsonHelper::CheckJsonBool(jointJson, "angularDampingRatio", objectName);
		jointProps.angularHertz = JsonHelper::CheckJsonBool(jointJson, "angularHertz", objectName);
		jointProps.linearDampingRatio = JsonHelper::CheckJsonBool(jointJson, "linearDampingRatio", objectName);
		jointProps.linearHertz = JsonHelper::CheckJsonFloat(jointJson, "linearHertz", objectName);
		jointProps.referenceAngle = JsonHelper::CheckJsonFloat(jointJson, "referenceAngle", objectName);
	}

	void RetrieveMotorJointProps(MotorJoint::MotorJointProps& jointProps, json jointJson, std::string objectName)
	{
		jointProps.angleBetween = JsonHelper::CheckJsonBool(jointJson, "angleBetween", objectName);
		jointProps.angularDampingRatio = JsonHelper::CheckJsonBool(jointJson, "angularDampingRatio", objectName);
		jointProps.angularHertz = JsonHelper::CheckJsonBool(jointJson, "angularHertz", objectName);
		jointProps.angularVelocity = JsonHelper::CheckJsonFloat(jointJson, "angularVelocity", objectName);
		jointProps.linearDampingRatio = JsonHelper::CheckJsonFloat(jointJson, "linearDampingRatio", objectName);
		jointProps.linearHertz = JsonHelper::CheckJsonFloat(jointJson, "linearHertz", objectName);
		//jointProps.linearVelocity = JsonHelper::CheckJsonFloat(jointJson, "linearVelocity", objectName);
		jointProps.maxSpringForce = JsonHelper::CheckJsonFloat(jointJson, "maxSpringForce", objectName);
		jointProps.maxVelocityForce = JsonHelper::CheckJsonFloat(jointJson, "maxVelocityForce", objectName);
		jointProps.maxVelocityTorque = JsonHelper::CheckJsonFloat(jointJson, "maxVelocityTorque", objectName);
		jointProps.relativeTransformPos = Vector2(JsonHelper::CheckJsonFloat(jointJson, "relativeTransformPosX", objectName), JsonHelper::CheckJsonFloat(jointJson, "relativeTransformPosY", objectName));		
	}

	void RetrieveWheelJointProps(WheelJoint::WheelJointProps& jointProps, json jointJson, std::string objectName)
	{
		jointProps.b_enableLimit = JsonHelper::CheckJsonBool(jointJson, "b_enableLimit", objectName);
		jointProps.b_enableMotor = JsonHelper::CheckJsonBool(jointJson, "b_enableMotor", objectName);
		jointProps.b_enableSpring = JsonHelper::CheckJsonBool(jointJson, "b_enableSpring", objectName);
		jointProps.dampingRatio = JsonHelper::CheckJsonFloat(jointJson, "dampingRatio", objectName);
		jointProps.hertz = JsonHelper::CheckJsonFloat(jointJson, "hertz", objectName);
		jointProps.localAxisA = Vector2(JsonHelper::CheckJsonFloat(jointJson, "localAxisAX", objectName), JsonHelper::CheckJsonFloat(jointJson, "localAxisAY", objectName));
		jointProps.lowerTranslation = JsonHelper::CheckJsonFloat(jointJson, "lowerTranslation", objectName);
		jointProps.upperTranslation = JsonHelper::CheckJsonFloat(jointJson, "upperTranslation", objectName);
		jointProps.maxMotorTorque = JsonHelper::CheckJsonFloat(jointJson, "maxMotorForce", objectName);
		jointProps.motorSpeed = JsonHelper::CheckJsonFloat(jointJson, "motorSpeed", objectName);
	}

	void JointMaker::PutData(json componentJson, std::string objectName)
	{
        Component::PutData(componentJson, objectName);

		if (JsonHelper::JsonContains(componentJson, "distanceJoints", objectName))
		{
			for (int i = 0; i < componentJson.at("distanceJoints").size(); i++)
			{
				try
				{											
					json basePropsJson = componentJson.at("distanceJoints").at(i)["baseProps"];
					json jointPropsJson = componentJson.at("distanceJoints").at(i)["jointProps"];
					DistanceJoint::DistanceJointProps jointProps = DistanceJoint::DistanceJointProps();
					Joint::BaseProps baseProps = Joint::BaseProps();
					RetrieveBaseJointProps(baseProps, basePropsJson, objectName);
					RetrieveDistanceJointProps(jointProps, jointPropsJson, objectName);
					AddDistanceJoint(baseProps, jointProps);
				}
				catch (const json::out_of_range& e)
				{
					Logger::log.Err("{}", e.what());
				}
			}
		}
		if (JsonHelper::JsonContains(componentJson, "prismaticJoints", objectName))
		{
			for (int i = 0; i < componentJson.at("prismaticJoints").size(); i++)
			{
				try
				{												
					json basePropsJson = componentJson.at("prismaticJoints").at(i)["baseProps"];
					json jointPropsJson = componentJson.at("prismaticJoints").at(i)["jointProps"];
					PrismaticJoint::PrismaticJointProps jointProps = PrismaticJoint::PrismaticJointProps();
					Joint::BaseProps baseProps = Joint::BaseProps();
					RetrieveBaseJointProps(baseProps, basePropsJson, objectName);
					RetrievePrismaticJointProps(jointProps, jointPropsJson, objectName);
					AddPrismaticJoint(baseProps, jointProps);
				}
				catch (const json::out_of_range& e)
				{
					Logger::log.Err("{}", e.what());
				}
			}
		}
		if (JsonHelper::JsonContains(componentJson, "revoluteJoints", objectName))
		{
			for (int i = 0; i < componentJson.at("revoluteJoints").size(); i++)
			{
				try
				{											
					json basePropsJson = componentJson.at("revoluteJoints").at(i)["baseProps"];
					json jointPropsJson = componentJson.at("revoluteJoints").at(i)["jointProps"];
					RevoluteJoint::RevoluteJointProps jointProps = RevoluteJoint::RevoluteJointProps();
					Joint::BaseProps baseProps = Joint::BaseProps();
					RetrieveBaseJointProps(baseProps, basePropsJson, objectName);
					RetrieveRevoluteJointProps(jointProps, jointPropsJson, objectName);
					AddRevoluteJoint(baseProps, jointProps);
				}
				catch (const json::out_of_range& e)
				{
					Logger::log.Err("{}", e.what());
				}
			}
		}
		if (JsonHelper::JsonContains(componentJson, "mouseJoints", objectName))
		{
			for (int i = 0; i < componentJson.at("mouseJoints").size(); i++)
			{
				try
				{											
					json basePropsJson = componentJson.at("mouseJoints").at(i)["baseProps"];
					json jointPropsJson = componentJson.at("mouseJoints").at(i)["jointProps"];
					MouseJoint::MouseJointProps jointProps = MouseJoint::MouseJointProps();
					Joint::BaseProps baseProps = Joint::BaseProps();
					RetrieveBaseJointProps(baseProps, basePropsJson, objectName);
					RetrieveMouseJointProps(jointProps, jointPropsJson, objectName);
					AddMouseJoint(baseProps, jointProps);
				}
				catch (const json::out_of_range& e)
				{
					Logger::log.Err("{}", e.what());
				}
			}
		}
		if (JsonHelper::JsonContains(componentJson, "weldJoints", objectName))
		{
			for (int i = 0; i < componentJson.at("weldJoints").size(); i++)
			{
				try
				{											
					json basePropsJson = componentJson.at("weldJoints").at(i)["baseProps"];
					json jointPropsJson = componentJson.at("weldJoints").at(i)["jointProps"];
					WeldJoint::WeldJointProps jointProps = WeldJoint::WeldJointProps();
					Joint::BaseProps baseProps = Joint::BaseProps();
					RetrieveBaseJointProps(baseProps, basePropsJson, objectName);
					RetrieveWeldJointProps(jointProps, jointPropsJson, objectName);
					AddWeldJoint(baseProps, jointProps);
				}
				catch (const json::out_of_range& e)
				{
					Logger::log.Err("{}", e.what());
				}
			}
		}
		if (JsonHelper::JsonContains(componentJson, "motorJoints", objectName))
		{
			for (int i = 0; i < componentJson.at("motorJoints").size(); i++)
			{
				try
				{											
					json basePropsJson = componentJson.at("motorJoints").at(i)["baseProps"];
					json jointPropsJson = componentJson.at("motorJoints").at(i)["jointProps"];
					MotorJoint::MotorJointProps jointProps = MotorJoint::MotorJointProps();
					Joint::BaseProps baseProps = Joint::BaseProps();
					RetrieveBaseJointProps(baseProps, basePropsJson, objectName);
					RetrieveMotorJointProps(jointProps, jointPropsJson, objectName);
					AddMotorJoint(baseProps, jointProps);
				}
				catch (const json::out_of_range& e)
				{
					Logger::log.Err("{}", e.what());
				}
			}
		}
		if (JsonHelper::JsonContains(componentJson, "wheelJoints", objectName))
		{
			for (int i = 0; i < componentJson.at("wheelJoints").size(); i++)
			{
				try
				{											
					json basePropsJson = componentJson.at("wheelJoints").at(i)["baseProps"];
					json jointPropsJson = componentJson.at("wheelJoints").at(i)["jointProps"];
					WheelJoint::WheelJointProps jointProps = WheelJoint::WheelJointProps();
					Joint::BaseProps baseProps = Joint::BaseProps();
					RetrieveBaseJointProps(baseProps, basePropsJson, objectName);
					RetrieveWheelJointProps(jointProps, jointPropsJson, objectName);
					AddWheelJoint(baseProps, jointProps);
				}
				catch (const json::out_of_range& e)
				{
					Logger::log.Err("{}", e.what());
				}
			}
		}
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