// #include "components/Body2D.h"
// #include "managers/SceneManager.h"
// #include "physics/Joint.h"
// #include "physics/PhysicsManager.h"


// namespace FlatEngine
// {
// 	Joint::Joint(long ownerID, long myID, JointType jointType)
// 	{
// 		m_ID = myID;
// 		m_jointID = b2_nullJointId;
// 		m_ownerID = ownerID;		
// 		type = jointType;

// 		PhysicsManager::physics2D.CreateJoint(this);
// 	}

// 	json Joint::GetData()
// 	{
// 		json jointJson = {
// 			{ "jointType", JointTypeStrings[type] },
// 			{ "bodyAID", bodyAID },
// 			{ "bodyBID", bodyBID },
// 			{ "b_collideConnected", b_collideConnected },
// 			{ "anchorAX", anchorA.x },
// 			{ "anchorAY", anchorA.y },
// 			{ "anchorBX", anchorB.x },
// 			{ "anchorBY", anchorB.y }
// 		};

// 		jointJson["jointData"] = std::visit([](auto&& jData) { return jData.GetData(); }, jointData);

// 		return jointJson;
// 	}

// 	void Joint::PutData(json jointJson, std::string name)
// 	{
// 		json jointDataJson = json::object();
// 		if (!jointJson.empty() && JsonHelper::JsonContains(jointJson, "jointData", name))		
// 			jointDataJson = jointJson.at("jointData");

// 		switch (type)
// 		{
// 			case JointType_Distance:  { jointData = DistanceJointData();  break; }
// 			case JointType_Revolute:  { jointData = PrismaticJointData(); break; }
// 			case JointType_Prismatic: { jointData = RevoluteJointData();  break; }
// 			case JointType_Mouse:     { jointData = MouseJointData();     break; }
// 			case JointType_Weld:      { jointData = WheelJointData();     break; }
// 			case JointType_Wheel:     { jointData = MotorJointData();     break; }
// 			case JointType_Motor:     { jointData = WeldJointData();      break; }
// 			default: break;
// 		}
// 		renderShapes.push_back(SceneView::CreateLineObject());
// 		std::visit([jointDataJson, name](auto&& jData) { jData.PutData(jointDataJson, name); }, jointData);

// 		if (jointJson.empty())
// 			return;

// 		b_collideConnected = JsonHelper::CheckJsonBool(jointJson, "b_collideConnected", name);
// 		bodyAID = JsonHelper::CheckJsonLong(jointJson, "bodyAID", name);
// 		bodyBID = JsonHelper::CheckJsonLong(jointJson, "bodyBID", name);
// 		anchorA.x = JsonHelper::CheckJsonFloat(jointJson, "anchorAX", name);
// 		anchorA.y = JsonHelper::CheckJsonFloat(jointJson, "anchorAY", name);
// 		anchorB.x = JsonHelper::CheckJsonFloat(jointJson, "anchorBX", name);
// 		anchorB.y = JsonHelper::CheckJsonFloat(jointJson, "anchorBY", name);
// 	}

// 	const long Joint::GetID()
// 	{
// 		return m_ID;
// 	}

// 	void Joint::SetOwnerID(long ownerID)
// 	{
// 		m_ownerID = ownerID;
// 	}

// 	long Joint::GetOwnerID()
// 	{
// 		return m_ownerID;
// 	}

// 	void Joint::SetJointID(b2JointId jointID)
// 	{
// 		m_jointID = jointID;
// 		std::visit([jointID](auto&& jData) { jData.jointID = jointID; }, jointData);
// 	}

// 	b2JointId Joint::GetJointID()
// 	{
// 		return m_jointID;
// 	}

// 	void Joint::SetBodyAID(long newBodyAID)
// 	{ 
// 		if (bodyAID == bodyBID)
// 		{
// 			bodyBID = -1;
// 		}
		
// 		bodyAID = newBodyAID;
// 		PhysicsManager::physics2D.RecreateJoint(this);
// 	}

// 	void Joint::SetBodyBID(long newBodyBID)
// 	{			
// 		if (bodyBID == bodyAID)
// 		{
// 			bodyAID = -1;
// 		}

// 		bodyBID = newBodyBID;
// 		PhysicsManager::physics2D.RecreateJoint(this);
// 	}

// 	JointType Joint::GetJointType()
// 	{
// 		return type;
// 	}

// 	Body2D* Joint::GetBodyA()
// 	{
// 		return SceneManager::loadedScene.Get<Body2D>(bodyAID);
// 	}

// 	Body2D* Joint::GetBodyB()
// 	{
// 		return SceneManager::loadedScene.Get<Body2D>(bodyBID);
// 	}

// 	bool Joint::HasValidBodies()
// 	{
// 		return (bodyAID != -1 && bodyBID != -1);
// 	}

// 	void Joint::SetAnchorA(Vector2 setAanchorA)
// 	{
// 		anchorA = setAanchorA;
// 		PhysicsManager::physics2D.RecreateJoint(this);
// 	}

// 	void Joint::SetAnchorB(Vector2 setAnchorB)
// 	{
// 		anchorB = setAnchorB;
// 		PhysicsManager::physics2D.RecreateJoint(this);
// 	}

// 	bool Joint::DoesCollideConnected()
// 	{
// 		return b_collideConnected;		
// 	}

// 	void Joint::SetCollideConnected(bool b_setCollideConnected)
// 	{
// 		b_collideConnected = b_setCollideConnected;
// 		PhysicsManager::physics2D.RecreateJoint(this);
// 	}

// 	Vector2 Joint::GetAnchorA()
// 	{
// 		return anchorA;
// 	}

// 	Vector2 Joint::GetAnchorB()
// 	{
// 		return anchorB;
// 	}

// 	Vector2 Joint::GetConstraintForce()
// 	{
// 		if (!b2Joint_IsValid(m_jointID))
// 			return Vector2();

// 		return b2Joint_GetConstraintForce(m_jointID);
// 	}

// 	float Joint::GetConstraintTorque()
// 	{
// 		if (!b2Joint_IsValid(m_jointID))
// 			return 0;

// 		return b2Joint_GetConstraintTorque(m_jointID);
// 	}
// }