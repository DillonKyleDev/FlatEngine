#include "PrismaticJoint.h"
#include "Body.h"


namespace FlatEngine
{
	PrismaticJoint::PrismaticJoint(PrismaticJointProps jointProps) : Joint(&jointProps)
	{
		m_jointProps = jointProps;	
		m_jointString = "Prismatic Joint";
		m_jointType = JT_Prismatic;
	}

	PrismaticJoint::~PrismaticJoint()
	{
	}

	json PrismaticJoint::GetJointData()
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
			{ "targetTranslation", m_jointProps.targetTranslation },
			{ "lowerTranslation", m_jointProps.lowerTranslation },
			{ "upperTranslation", m_jointProps.upperTranslation },
			{ "maxMotorForce", m_jointProps.maxMotorForce },
			{ "motorSpeed", m_jointProps.motorSpeed },
			{ "referenceAngle", m_jointProps.referenceAngle },
			{ "localAxisAX", m_jointProps.localAxisA.x },
			{ "localAxisAY", m_jointProps.localAxisA.y }			
		};

		return jsonData;
	}

	Joint::JointProps* PrismaticJoint::GetJointProps()
	{
		return &m_jointProps;
	}

	void PrismaticJoint::SetBodyA(Body* bodyA)
	{
		Joint::SetBodyA(bodyA);
		m_jointProps.bodyAID = bodyA->GetParentID();		
	}

	void PrismaticJoint::SetBodyB(Body* bodyB)
	{
		Joint::SetBodyB(bodyB);
		m_jointProps.bodyBID = bodyB->GetParentID();		
	}
}