#include "MotorJoint.h"
#include "Body.h"



namespace FlatEngine
{
	MotorJoint::MotorJoint(MotorJointProps jointProps) : Joint(&jointProps)
	{
		m_jointProps = jointProps;
		m_jointString = "Motor Joint";
		m_jointProps.jointType = JT_Motor;
		m_jointType = JT_Motor;
	}

	MotorJoint::~MotorJoint()
	{
	}

	json MotorJoint::GetJointData()
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
			{ "angleBetween", m_jointProps.angleBetween },
			{ "angularDampingRatio", m_jointProps.angularDampingRatio },
			{ "angularHertz", m_jointProps.angularHertz },
			{ "angularVelocity", m_jointProps.angularVelocity },
			{ "linearDampingRatio", m_jointProps.linearDampingRatio },
			{ "linearHertz", m_jointProps.linearHertz },
			{ "linearVelocityX", m_jointProps.linearVelocity.x },
			{ "linearVelocityY", m_jointProps.linearVelocity.y },
			{ "maxSpringForce", m_jointProps.maxSpringForce },
			{ "maxSpringTorque", m_jointProps.maxSpringTorque },
			{ "maxVelocityForce", m_jointProps.maxVelocityForce },
			{ "relativeTransformPosX", m_jointProps.relativeTransformPos.x },
			{ "relativeTransformPosY", m_jointProps.relativeTransformPos.y }
		};

		return jsonData;
	}

	Joint::JointProps* MotorJoint::GetJointProps()
	{
		return &m_jointProps;
	}

	void MotorJoint::SetBodyA(Body* bodyA)
	{
		Joint::SetBodyA(bodyA);
		m_jointProps.bodyAID = bodyA->GetParentID();
	}

	void MotorJoint::SetBodyB(Body* bodyB)
	{
		Joint::SetBodyB(bodyB);
		m_jointProps.bodyBID = bodyB->GetParentID();
	}
}