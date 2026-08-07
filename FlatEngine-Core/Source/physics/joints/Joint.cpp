#include "components/Body2D.h"
#include "managers/SceneManager.h"
#include "physics/joints/Joint.h"
#include "physics/PhysicsManager.h"


namespace FlatEngine
{
	Joint::Joint(BaseProps baseProps)
	{
		m_b2JointID = b2_nullJointId;
		m_jointID = 0;
		m_jointString = "";
		m_baseProps = baseProps;
		CreateJoint();
	}

	json Joint::GetBasePropsData()
	{
		json jsonData = {
			{ "jointType", (int)m_baseProps.jointType },
			{ "bodyAID", m_baseProps.bodyAID },
			{ "bodyBID", m_baseProps.bodyBID },
			{ "b_collideConnected", m_baseProps.b_collideConnected },
			{ "anchorAX", m_baseProps.anchorA.x },
			{ "anchorAY", m_baseProps.anchorA.y },
			{ "anchorBX", m_baseProps.anchorB.x },
			{ "anchorBY", m_baseProps.anchorB.y }
		};

		return jsonData;
	}


	void Joint::SetJointID(long jointID)
	{
		m_jointID = jointID;
	}

	long Joint::GetJointID()
	{
		return m_jointID;
	}

	void Joint::SetB2JointID(b2JointId jointID)
	{
		m_b2JointID = jointID;
	}

	b2JointId Joint::GetB2JointID()
	{
		return m_b2JointID;
	}

	Joint::BaseProps Joint::GetBaseProps()
	{
		return m_baseProps;
	}

	void Joint::SetBodyAID(long bodyAID)
	{ 
		if (bodyAID == m_baseProps.bodyBID)
		{
			m_baseProps.bodyBID = -1;
		}
		
		m_baseProps.bodyAID = bodyAID;
		RecreateJoint();
	}

	void Joint::SetBodyBID(long bodyBID)
	{			
		if (bodyBID == m_baseProps.bodyAID)
		{
			m_baseProps.bodyAID = -1;
		}

		m_baseProps.bodyBID = bodyBID;
		RecreateJoint();
	}

	Joint::JointType Joint::GetJointType()
	{
		return m_baseProps.jointType;
	}

	std::string Joint::GetJointString()
	{
		return m_jointString;
	}

	Body2D* Joint::GetBodyA()
	{
		if (SceneManager::loadedScene.GetObjectByID(m_baseProps.bodyAID) != nullptr)
		{
			return SceneManager::loadedScene.GetObjectByID(m_baseProps.bodyAID)->Get<Body2D>();
		}
		else
		{
			return nullptr;
		}
	}

	Body2D* Joint::GetBodyB()
	{
		if (SceneManager::loadedScene.GetObjectByID(m_baseProps.bodyBID) != nullptr)
		{
			return SceneManager::loadedScene.GetObjectByID(m_baseProps.bodyBID)->Get<Body2D>();
		}
		else
		{
			return nullptr;
		}
	}

	bool Joint::HasValidBodies()
	{
		return (m_baseProps.bodyAID != -1 && m_baseProps.bodyBID != -1);
	}

	void Joint::SetAnchorA(Vector2 anchorA)
	{
		m_baseProps.anchorA = anchorA;
		RecreateJoint();
	}

	void Joint::SetAnchorB(Vector2 anchorB)
	{
		m_baseProps.anchorB = anchorB;
		RecreateJoint();
	}

	bool Joint::CollideConnected()
	{
		return m_baseProps.b_collideConnected;
		RecreateJoint();
	}

	Vector2 Joint::GetAnchorA()
	{
		return m_baseProps.anchorA;
	}

	Vector2 Joint::GetAnchorB()
	{
		return m_baseProps.anchorB;
	}

	void Joint::CreateJoint()
	{
		DestroyJoint();

		GameObject* objectA = SceneManager::loadedScene.GetObjectByID(m_baseProps.bodyAID);
		GameObject* objectB = SceneManager::loadedScene.GetObjectByID(m_baseProps.bodyBID);

		if (objectA != nullptr && objectB != nullptr && objectA->Get<Body2D>() && objectB->Get<Body2D>())
		{
			PhysicsManager::physics2D.CreateJoint(SceneManager::loadedScene.GetObjectByID(m_baseProps.bodyAID)->Get<Body2D>(), SceneManager::loadedScene.GetObjectByID(m_baseProps.bodyBID)->Get<Body2D>(), this);
		}
	}

	void Joint::CreateJoint(Body2D* bodyA, Body2D* bodyB)
	{
		if (bodyA != nullptr && bodyB != nullptr)
		{
			PhysicsManager::physics2D.CreateJoint(bodyA, bodyB, this);
		}
	}

	void Joint::RecreateJoint()
	{
		DestroyJoint();
		CreateJoint();
	}

	void Joint::DestroyJoint()
	{
		if (b2Joint_IsValid(m_b2JointID))
		{
			b2DestroyJoint(m_b2JointID);
			m_b2JointID = b2_nullJointId;
		}
	}

	Vector2 Joint::GetConstraintForce()
	{
		return b2Joint_GetConstraintForce(m_b2JointID);
	}

	float Joint::GetConstraintTorque()
	{
		return b2Joint_GetConstraintTorque(m_b2JointID);
	}
}