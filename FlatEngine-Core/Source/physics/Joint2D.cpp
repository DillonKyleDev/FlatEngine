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
			case JointType2D_Distance:  jointData = DistanceJoint2DData();  break; 
			case JointType2D_Revolute:  jointData = PrismaticJoint2DData(); break; 
			case JointType2D_Prismatic: jointData = RevoluteJoint2DData();  break; 
			case JointType2D_Mouse:     jointData = MouseJoint2DData();     break; 
			case JointType2D_Weld:      jointData = WheelJoint2DData();     break; 
			case JointType2D_Wheel:     jointData = MotorJoint2DData();     break; 
			case JointType2D_Motor:     jointData = WeldJoint2DData();      break; 
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

	long Joint2D::GetID()
	{
		return m_ID;
	}

	void Joint2D::Cleanup()
	{
		PhysicsManager::gamePhysics2D.DestroyJoint(this);
		for (SceneView::SceneRenderObject renderShape : renderShapes)
		{
			renderShape.mesh.Cleanup();
		}

		renderShapes.clear();
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

	void Joint2D::WakeBodies()
	{
		GetBodyA()->Wake();
		GetBodyB()->Wake();	
	}


	// Distance Joint	 
	void DistanceJoint2DData::SetLength(float setLength)
	{
		if (setLength > 0)
		{
			length = setLength;

			if (b2Joint_IsValid(jointID))
			{
				static_cast<Joint2D*>(b2Joint_GetUserData(jointID))->WakeBodies();				
				b2DistanceJoint_SetLength(jointID, length);
			}
			else
			{
				PhysicsManager::gamePhysics2D.CreateJoint(static_cast<Joint2D*>(b2Joint_GetUserData(jointID)));
			}
		}
	}

	float DistanceJoint2DData::GetLength()
	{
		if (b2Joint_IsValid(jointID))
		{
			return b2DistanceJoint_GetLength(jointID);
		}
		else
		{
			return length;
		}
	}

	void DistanceJoint2DData::SetLengthRange(float setMinLength, float setMaxLength)
	{
		if (setMinLength >= 0 && setMaxLength >= setMinLength)
		{
			minLength = setMinLength;
			maxLength = setMaxLength;

			if (b2Joint_IsValid(jointID))
			{
				static_cast<Joint2D*>(b2Joint_GetUserData(jointID))->WakeBodies();
				b2DistanceJoint_SetLengthRange(jointID, minLength, maxLength);				
			}
		}
	}

	void DistanceJoint2DData::SetEnableSpring(bool b_setEnableSpring)
	{
		b_enableSpring = b_setEnableSpring;

		if (b2Joint_IsValid(jointID))
		{
			static_cast<Joint2D*>(b2Joint_GetUserData(jointID))->WakeBodies();
			b2DistanceJoint_EnableSpring(jointID, b_enableSpring);
		}
	}

	void DistanceJoint2DData::SetSpringHertz(float springHertz)
	{
		if (springHertz >= 0)
		{
			hertz = springHertz;

			if (b2Joint_IsValid(jointID))
			{
				static_cast<Joint2D*>(b2Joint_GetUserData(jointID))->WakeBodies();
				b2DistanceJoint_SetSpringHertz(jointID, springHertz);
			}
		}
	}

	float DistanceJoint2DData::GetSpringHertz()
	{
		if (b2Joint_IsValid(jointID))
		{
			return b2DistanceJoint_GetSpringHertz(jointID);
		}
		else
		{
			return hertz;
		}
	}

	void DistanceJoint2DData::SetSpringDampingRatio(float springDampingRatio)
	{
		if (springDampingRatio >= 0)
		{
			dampingRatio = springDampingRatio;

			if (b2Joint_IsValid(jointID))
			{				
				static_cast<Joint2D*>(b2Joint_GetUserData(jointID))->WakeBodies();
				b2DistanceJoint_SetSpringDampingRatio(jointID, springDampingRatio);
			}
		}
	}

	float DistanceJoint2DData::GetSpringDampingRatio()
	{
		if (b2Joint_IsValid(jointID))
		{
			return b2DistanceJoint_GetSpringDampingRatio(jointID);
		}
		else
		{
			return dampingRatio;
		}
	}

	void DistanceJoint2DData::SetEnableMotor(bool b_setEnableMotor)
	{
		b_enableMotor = b_setEnableMotor;

		if (b2Joint_IsValid(jointID))
		{
			static_cast<Joint2D*>(b2Joint_GetUserData(jointID))->WakeBodies();
			b2DistanceJoint_EnableSpring(jointID, b_enableMotor);
		}
	}

	void DistanceJoint2DData::SetMotorSpeed(float setMotorSpeed)
	{
		if (setMotorSpeed >= 0)
		{
			motorSpeed = setMotorSpeed;

			if (b2Joint_IsValid(jointID))
			{
				static_cast<Joint2D*>(b2Joint_GetUserData(jointID))->WakeBodies();
				b2DistanceJoint_SetMotorSpeed(jointID, motorSpeed);
			}
		}
	}

	float DistanceJoint2DData::GetMotorSpeed()
	{
		if (b2Joint_IsValid(jointID))
		{
			return b2DistanceJoint_GetMotorSpeed(jointID);
		}
		else
		{
			return motorSpeed;
		}		
	}

	void DistanceJoint2DData::SetMaxMotorForce(float setMaxMotorForce)
	{
		if (setMaxMotorForce >= 0)
		{
			maxMotorForce = setMaxMotorForce;

			if (b2Joint_IsValid(jointID))
			{
				static_cast<Joint2D*>(b2Joint_GetUserData(jointID))->WakeBodies();
				b2DistanceJoint_SetMaxMotorForce(jointID, maxMotorForce);
			}
		}
	}

	float DistanceJoint2DData::GetMotorForce()
	{
		if (b2Joint_IsValid(jointID))
		{
			return b2DistanceJoint_GetMotorForce(jointID);
		}
		else
		{
			return 0;
		}
	}

	void DistanceJoint2DData::SetEnableLimit(bool b_setEnableLimit)
	{
		b_enableLimit = b_setEnableLimit;

		if (b2Joint_IsValid(jointID))
		{
			static_cast<Joint2D*>(b2Joint_GetUserData(jointID))->WakeBodies();
			b2DistanceJoint_EnableLimit(jointID, b_enableLimit);
		}
	}

	// Prismatic Joint
	void PrismaticJoint2DData::SetReferenceAngle(float length)
	{
		static_cast<Joint2D*>(b2Joint_GetUserData(jointID))->WakeBodies();
	}

	float PrismaticJoint2DData::GetReferenceAngle()
	{
		return 0;
	}

	void PrismaticJoint2DData::SetTargetTranslation(float targetTranslation)
	{
		static_cast<Joint2D*>(b2Joint_GetUserData(jointID))->WakeBodies();
	}

	float PrismaticJoint2DData::GetTargetTranslation()
	{
		return 0;
	}

	// Translation upper and lower tied to b_enableLimit
	void PrismaticJoint2DData::SetTranslationRange(float setLowerTranslation, float setUpperTranslation)
	{
		if (true) // some constraint on translation range todo)
		{
			lowerTranslation = setLowerTranslation;
			upperTranslation = setUpperTranslation;

			if (b2Joint_IsValid(jointID))
			{
				static_cast<Joint2D*>(b2Joint_GetUserData(jointID))->WakeBodies();
				b2PrismaticJoint_SetLimits(jointID, lowerTranslation, upperTranslation);
			}
			else
			{
				PhysicsManager::gamePhysics2D.CreateJoint(static_cast<Joint2D*>(b2Joint_GetUserData(jointID)));
			}
		}
	}

	void PrismaticJoint2DData::SetEnableSpring(bool b_setEnableSpring)
	{
		b_enableSpring = b_setEnableSpring;

		if (b2Joint_IsValid(jointID))
		{
			static_cast<Joint2D*>(b2Joint_GetUserData(jointID))->WakeBodies();
			b2PrismaticJoint_EnableSpring(jointID, b_enableSpring);
		}
		else
		{
			PhysicsManager::gamePhysics2D.CreateJoint(static_cast<Joint2D*>(b2Joint_GetUserData(jointID)));
		}
	}

	void PrismaticJoint2DData::SetSpringHertz(float springHertz)
	{
		if (springHertz >= 0)
		{
			hertz = springHertz;

			if (b2Joint_IsValid(jointID))
			{
				static_cast<Joint2D*>(b2Joint_GetUserData(jointID))->WakeBodies();
				b2PrismaticJoint_SetSpringHertz(jointID, springHertz);
			}
			else
			{
				PhysicsManager::gamePhysics2D.CreateJoint(static_cast<Joint2D*>(b2Joint_GetUserData(jointID)));
			}
		}
	}

	float PrismaticJoint2DData::GetSpringHertz()
	{
		if (b2Joint_IsValid(jointID))
		{
			return b2PrismaticJoint_GetSpringHertz(jointID);
		}
		else
		{
			return hertz;
		}
	}

	void PrismaticJoint2DData::SetSpringDampingRatio(float springDampingRatio)
	{
		if (springDampingRatio >= 0)
		{
			dampingRatio = springDampingRatio;

			if (b2Joint_IsValid(jointID))
			{
				static_cast<Joint2D*>(b2Joint_GetUserData(jointID))->WakeBodies();
				b2PrismaticJoint_SetSpringDampingRatio(jointID, springDampingRatio);
			}
			else
			{
				PhysicsManager::gamePhysics2D.CreateJoint(static_cast<Joint2D*>(b2Joint_GetUserData(jointID)));
			}
		}
	}

	float PrismaticJoint2DData::GetSpringDampingRatio()
	{
		if (b2Joint_IsValid(jointID))
		{
			return b2PrismaticJoint_GetSpringDampingRatio(jointID);
		}
		else
		{
			return dampingRatio;
		}
	}

	void PrismaticJoint2DData::SetEnableMotor(bool b_setEnableMotor)
	{
		b_enableMotor = b_setEnableMotor;

		if (b2Joint_IsValid(jointID))
		{
			static_cast<Joint2D*>(b2Joint_GetUserData(jointID))->WakeBodies();
			b2PrismaticJoint_EnableSpring(jointID, b_enableMotor);
		}
		else
		{
			PhysicsManager::gamePhysics2D.CreateJoint(static_cast<Joint2D*>(b2Joint_GetUserData(jointID)));
		}
	}

	void PrismaticJoint2DData::SetMotorSpeed(float setMotorSpeed)
	{
		if (setMotorSpeed >= 0)
		{
			motorSpeed = setMotorSpeed;

			if (b2Joint_IsValid(jointID))
			{
				static_cast<Joint2D*>(b2Joint_GetUserData(jointID))->WakeBodies();
				b2PrismaticJoint_SetMotorSpeed(jointID, motorSpeed);
			}
			else
			{
				PhysicsManager::gamePhysics2D.CreateJoint(static_cast<Joint2D*>(b2Joint_GetUserData(jointID)));
			}
		}
	}

	float PrismaticJoint2DData::GetMotorSpeed()
	{
		if (b2Joint_IsValid(jointID))
		{
			return b2PrismaticJoint_GetMotorSpeed(jointID);
		}
		else
		{
			return motorSpeed;
		}
	}

	void PrismaticJoint2DData::SetMaxMotorForce(float setMaxMotorForce)
	{
		if (setMaxMotorForce >= 0)
		{
			maxMotorForce = setMaxMotorForce;

			if (b2Joint_IsValid(jointID))
			{
				static_cast<Joint2D*>(b2Joint_GetUserData(jointID))->WakeBodies();
				b2PrismaticJoint_SetMaxMotorForce(jointID, maxMotorForce);
			}
			else
			{
				PhysicsManager::gamePhysics2D.CreateJoint(static_cast<Joint2D*>(b2Joint_GetUserData(jointID)));
			}
		}
	}

	float PrismaticJoint2DData::GetMotorForce()
	{
		if (b2Joint_IsValid(jointID))
		{
			return b2PrismaticJoint_GetMotorForce(jointID);
		}
		else
		{
			return 0;
		}
	}

	void PrismaticJoint2DData::SetEnableLimit(bool b_setEnableLimit)
	{
		b_enableLimit = b_setEnableLimit;

		if (b2Joint_IsValid(jointID))
		{
			static_cast<Joint2D*>(b2Joint_GetUserData(jointID))->WakeBodies();
			b2PrismaticJoint_EnableLimit(jointID, b_enableLimit);
		}
		else
		{
			PhysicsManager::gamePhysics2D.CreateJoint(static_cast<Joint2D*>(b2Joint_GetUserData(jointID)));
		}
	}
}