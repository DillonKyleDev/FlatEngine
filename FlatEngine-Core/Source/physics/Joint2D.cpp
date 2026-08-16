#include "components/Body2D.h"
#include "managers/SceneManager.h"
#include "physics/Joint2D.h"
#include "physics/PhysicsManager.h"


namespace FlatEngine
{
	Joint2D::Joint2D(long ownerID, long myID, JointType2D jointType)
	{
		m_ID = myID;
		m_jointID = b2_nullJointId;
		m_ownerID = ownerID;		
		type = jointType;
		b_isCollapsed = false;
		bodyAID = ownerID;
		bodyBID = -1;		
		b_collideConnected = false;
	}

	json Joint2D::GetData()
	{
		json jointJson = {
			{ "jointType", JointType2DStrings[type] },
			{ "bodyBID", bodyBID },
			{ "b_collideConnected", b_collideConnected },
			{ "b_isCollapsed", b_isCollapsed },
			{ "anchorAX", anchorA.x },
			{ "anchorAY", anchorA.y },
			{ "anchorBX", anchorB.x },
			{ "anchorBY", anchorB.y }
		};

		jointJson["jointData"] = std::visit([](auto&& jData) { return jData.GetData(); }, jointData);

		return jointJson;
	}

	void Joint2D::PutData(json jointJson, std::string name)
	{
		json jointDataJson = json::object();
		if (!jointJson.empty() && JsonHelper::JsonContains(jointJson, "jointData", name))		
			jointDataJson = jointJson.at("jointData");

		switch (type)
		{
			case JointType2D_Distance:  { jointData = DistanceJoint2DData();  break; }
			case JointType2D_Revolute:  { jointData = PrismaticJoint2DData(); break; }
			case JointType2D_Prismatic: { jointData = RevoluteJoint2DData();  break; }
			case JointType2D_Mouse:     { jointData = MouseJoint2DData();     break; }
			case JointType2D_Weld:      { jointData = WheelJoint2DData();     break; }
			case JointType2D_Wheel:     { jointData = MotorJoint2DData();     break; }
			case JointType2D_Motor:     { jointData = WeldJoint2DData();      break; }
			default: break;
		}
		renderShapes.push_back(SceneView::CreateLineObject());
		std::visit([jointDataJson, name](auto&& jData) { jData.PutData(jointDataJson, name); }, jointData);

		if (jointJson.empty())
			return;

		b_collideConnected = JsonHelper::CheckJsonBool(jointJson, "b_collideConnected", name);
		b_isCollapsed = JsonHelper::CheckJsonBool(jointJson, "b_isCollapsed", name);		
		bodyBID = JsonHelper::CheckJsonLong(jointJson, "bodyBID", name);
		anchorA.x = JsonHelper::CheckJsonFloat(jointJson, "anchorAX", name);
		anchorA.y = JsonHelper::CheckJsonFloat(jointJson, "anchorAY", name);
		anchorB.x = JsonHelper::CheckJsonFloat(jointJson, "anchorBX", name);
		anchorB.y = JsonHelper::CheckJsonFloat(jointJson, "anchorBY", name);
	}

	const long Joint2D::GetID()
	{
		return m_ID;
	}

	void Joint2D::SetOwnerID(long ownerID)
	{
		m_ownerID = ownerID;
	}

	long Joint2D::GetOwnerID()
	{
		return m_ownerID;
	}

	void Joint2D::SetJointID(b2JointId jointID)
	{
		m_jointID = jointID;
		std::visit([jointID](auto&& jData) { jData.jointID = jointID; }, jointData);
	}

	b2JointId Joint2D::GetJointID()
	{
		return m_jointID;
	}

	void Joint2D::SetBodyAID(long newBodyAID)
	{ 
		if (bodyAID == bodyBID)
		{
			bodyBID = -1;
		}
		
		bodyAID = newBodyAID;
		PhysicsManager::gamePhysics2D.RecreateJoint(this);
	}

	void Joint2D::SetBodyBID(long newBodyBID)
	{			
		if (newBodyBID != bodyAID)
		{
			bodyBID = newBodyBID;
			PhysicsManager::gamePhysics2D.RecreateJoint(this);
		}
	}

	JointType2D Joint2D::GetType()
	{
		return type;
	}

	Body2D* Joint2D::GetBodyA()
	{
		return SceneManager::loadedScene.Get<Body2D>(bodyAID);
	}

	Body2D* Joint2D::GetBodyB()
	{
		return SceneManager::loadedScene.Get<Body2D>(bodyBID);
	}

	bool Joint2D::HasValidBodies()
	{
		return (bodyAID != -1 && bodyBID != -1);
	}

	void Joint2D::SetAnchorA(Vector2 setAanchorA)
	{
		anchorA = setAanchorA;
		PhysicsManager::gamePhysics2D.RecreateJoint(this);
	}

	void Joint2D::SetAnchorB(Vector2 setAnchorB)
	{
		anchorB = setAnchorB;
		PhysicsManager::gamePhysics2D.RecreateJoint(this);
	}

	bool Joint2D::DoesCollideConnected()
	{
		return b_collideConnected;		
	}

	void Joint2D::SetCollideConnected(bool b_setCollideConnected)
	{
		b_collideConnected = b_setCollideConnected;
		PhysicsManager::gamePhysics2D.RecreateJoint(this);
	}

	Vector2 Joint2D::GetAnchorA()
	{
		return anchorA;
	}

	Vector2 Joint2D::GetAnchorB()
	{
		return anchorB;
	}

	Vector2 Joint2D::GetConstraintForce()
	{
		if (!b2Joint_IsValid(m_jointID))
			return Vector2();

		return b2Joint_GetConstraintForce(m_jointID);
	}

	float Joint2D::GetConstraintTorque()
	{
		if (!b2Joint_IsValid(m_jointID))
			return 0;

		return b2Joint_GetConstraintTorque(m_jointID);
	}
}