#include "DistanceJoint.h"
#include "Body.h"
#include "FlatEngine.h"


namespace FlatEngine
{
	DistanceJoint::DistanceJoint(DistanceJointProps jointProps) : Joint(&jointProps)
	{
		m_jointProps = jointProps;
		m_jointString = "Distance Joint";
		m_jointProps.jointType = JT_Distance;
		m_jointType = JT_Distance;
		m_jointID = b2_nullJointId;	

		if (jointProps.bodyAID != -1 && GetObjectByID(jointProps.bodyAID) != nullptr && GetObjectByID(jointProps.bodyAID)->GetBody() != nullptr)
		{
			SetBodyA(GetObjectByID(jointProps.bodyAID)->GetBody());
		}
		if (jointProps.bodyBID != -1 && GetObjectByID(jointProps.bodyBID) != nullptr && GetObjectByID(jointProps.bodyBID)->GetBody() != nullptr)
		{
			SetBodyB(GetObjectByID(jointProps.bodyBID)->GetBody());
		}
	}

	DistanceJoint::~DistanceJoint()
	{
	}

	json DistanceJoint::GetJointData()
	{	
		json jsonData = {
			// Base Joint Props
			{ "jointType", (int)m_jointType },
			{ "bodyAID", m_bodyAID },
			{ "bodyBID", m_bodyBID },
			{ "_collideConnected", m_b_collideConnected },
			{ "anchorAX", m_anchorA.x },
			{ "anchorAY", m_anchorA.y },
			{ "anchorBX", m_anchorB.x },
			{ "anchorBY", m_anchorB.y },
			// Distance Joint Props
			{ "dampingRatio", m_jointProps.dampingRatio },
			{ "_enableLimit", m_jointProps.b_enableLimit },
			{ "_enableMotor", m_jointProps.b_enableMotor },
			{ "_enableSpring", m_jointProps.b_enableSpring },
			{ "hertz", m_jointProps.hertz },
			{ "minLength", m_jointProps.minLength },
			{ "maxLength", m_jointProps.maxLength },
			{ "length", m_jointProps.length },
			{ "maxMotorForce", m_jointProps.maxMotorForce },
			{ "motorSpeed", m_jointProps.motorSpeed }
		};

		return jsonData;
	}

	Joint::JointProps* DistanceJoint::GetJointProps()
	{
		return &m_jointProps;
	}

	void DistanceJoint::SetBodyA(Body* bodyA)
	{
		Joint::SetBodyA(bodyA);
		m_jointProps.bodyAID = bodyA->GetParentID();
	}

	void DistanceJoint::SetBodyB(Body* bodyB)
	{
		Joint::SetBodyB(bodyB);
		m_jointProps.bodyBID = bodyB->GetParentID();
	}

	void DistanceJoint::SetJointProps(DistanceJointProps jointProps)
	{
		m_jointProps = jointProps;
	}
}