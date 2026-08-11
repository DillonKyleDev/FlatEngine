#include "components/Body2D.h"
#include "managers/SceneManager.h"
#include "physics/Joint.h"
#include "physics/PhysicsManager.h"


namespace FlatEngine
{
	Joint::Joint(long ownerID, JointType jointType)
	{
		m_jointID = b2_nullJointId;
		m_ownerID = ownerID;		
		type = jointType;

		PhysicsManager::physics2D.CreateJoint(this);
	}

	json Joint::GetData()
	{
		json jsonData = {
			{ "jointType", (int)type },
			{ "bodyAID", bodyAID },
			{ "bodyBID", bodyBID },
			{ "b_collideConnected", b_collideConnected },
			{ "anchorAX", anchorA.x },
			{ "anchorAY", anchorA.y },
			{ "anchorBX", anchorB.x },
			{ "anchorBY", anchorB.y }
		};

		jsonData["jointData"] = std::visit([](auto&& jData) { return jData.GetData(); }, jointData);

		return jsonData;
	}

	void Joint::PutData(json jsonData, std::string name)
	{
		type = (JointType)JsonHelper::CheckJsonInt(jsonData, "jointType", name);
		b_collideConnected = JsonHelper::CheckJsonBool(jsonData, "b_collideConnected", name);
		bodyAID = JsonHelper::CheckJsonLong(jsonData, "bodyAID", name);
		bodyBID = JsonHelper::CheckJsonLong(jsonData, "bodyBID", name);
		anchorA.x = JsonHelper::CheckJsonFloat(jsonData, "anchorAX", name);
		anchorA.y = JsonHelper::CheckJsonFloat(jsonData, "anchorAY", name);
		anchorB.x = JsonHelper::CheckJsonFloat(jsonData, "anchorBX", name);
		anchorB.y = JsonHelper::CheckJsonFloat(jsonData, "anchorAY", name);

		if (JsonHelper::JsonContains(jsonData, "jointData", name))
		{
			json jointJson = jsonData.at("jointData");
			switch (type)
			{
				case JointType_Distance:  { DistanceJointData joint;  joint.PutData(jointJson, name); jointData = joint; renderShapes.push_back(SceneView::CreateQuadObject()); break; }
				case JointType_Revolute:  { PrismaticJointData joint; joint.PutData(jointJson, name); jointData = joint; renderShapes.push_back(SceneView::CreateCircleObject());  break; }
				case JointType_Prismatic: { RevoluteJointData joint;  joint.PutData(jointJson, name); jointData = joint; break; }
				case JointType_Mouse:     { MouseJointData joint;     joint.PutData(jointJson, name); jointData = joint; break; }
				case JointType_Weld:      { WheelJointData joint;     joint.PutData(jointJson, name); jointData = joint; break; }
				case JointType_Wheel:     { MotorJointData joint;     joint.PutData(jointJson, name); jointData = joint; break; }
				case JointType_Motor:     { WeldJointData joint;      joint.PutData(jointJson, name); jointData = joint; break; }
				default: break;
			}
		}
	}

	void Joint::SetOwnerID(long ownerID)
	{
		m_ownerID = ownerID;
	}

	long Joint::GetOwnerID()
	{
		return m_ownerID;
	}

	void Joint::SetJointID(b2JointId jointID)
	{
		m_jointID = jointID;
	}

	b2JointId Joint::GetJointID()
	{
		return m_jointID;
	}

	void Joint::SetBodyAID(long newBodyAID)
	{ 
		if (bodyAID == bodyBID)
		{
			bodyBID = -1;
		}
		
		bodyAID = newBodyAID;
		PhysicsManager::physics2D.RecreateJoint(this);
	}

	void Joint::SetBodyBID(long newBodyBID)
	{			
		if (bodyBID == bodyAID)
		{
			bodyAID = -1;
		}

		bodyBID = newBodyBID;
		PhysicsManager::physics2D.RecreateJoint(this);
	}

	JointType Joint::GetJointType()
	{
		return jointType;
	}

	Body2D* Joint::GetBodyA()
	{
		return SceneManager::loadedScene.Get<Body2D>(bodyAID);
	}

	Body2D* Joint::GetBodyB()
	{
		return SceneManager::loadedScene.Get<Body2D>(bodyBID);
	}

	bool Joint::HasValidBodies()
	{
		return (bodyAID != -1 && bodyBID != -1);
	}

	void Joint::SetAnchorA(Vector2 anchorA)
	{
		anchorA = anchorA;
		PhysicsManager::physics2D.RecreateJoint(this);
	}

	void Joint::SetAnchorB(Vector2 anchorB)
	{
		anchorB = anchorB;
		PhysicsManager::physics2D.RecreateJoint(this);
	}

	bool Joint::CollideConnected()
	{
		return b_collideConnected;
		PhysicsManager::physics2D.RecreateJoint(this);
	}

	Vector2 Joint::GetAnchorA()
	{
		return anchorA;
	}

	Vector2 Joint::GetAnchorB()
	{
		return anchorB;
	}

	Vector2 Joint::GetConstraintForce()
	{
		return b2Joint_GetConstraintForce(m_jointID);
	}

	float Joint::GetConstraintTorque()
	{
		return b2Joint_GetConstraintTorque(m_jointID);
	}
}