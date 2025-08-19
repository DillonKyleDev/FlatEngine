#pragma once
#include "box2d.h"
#include "Vector2.h"

#include <string>
#include "json.hpp"

using json = nlohmann::json;
using namespace nlohmann::literals;


namespace FlatEngine
{
	class Body;

	class Joint
	{
		friend class DistanceJoint;
		friend class RevoluteJoint;
		friend class PrismaticJoint;
		friend class MouseJoint;
		friend class WeldJoint;
		friend class MotorJoint;
		friend class WheelJoint;

	public:
		enum JointType {
			JT_None,
			JT_Distance,
			JT_Revolute,
			JT_Prismatic,
			JT_Mouse,
			JT_Weld,
			JT_Motor,
			JT_Wheel
		};

		struct JointProps {
			long bodyAID = -1;
			long bodyBID = -1;
			JointType jointType = JT_None;
			bool b_collideConnected = true;
			Vector2 anchorA = Vector2(0, 0);
			Vector2 anchorB = Vector2(0, 0);
			b2BodyId b2BodyAID = b2_nullBodyId;
			b2BodyId b2BodyBID = b2_nullBodyId;
		};
		
		Joint(JointProps* jointProps);
		~Joint();
		virtual json GetJointData() { return json::object(); };
		virtual void SetBodyA(Body* bodyA);
		virtual void SetBodyB(Body* bodyB);
		JointType GetJointType();
		std::string GetJointString();

		void SetJointID(b2JointId jointID);
		b2JointId GetJointID();
		Body* GetBodyA();
		Body* GetBodyB();		
		bool HasValidBodies();
		bool CollideConnected();
		Vector2 GetAnchorA();
		Vector2 GetAnchorB();
		void SetAnchorA(Vector2 anchorA);
		void SetAnchorB(Vector2 anchorB);
		virtual JointProps* GetJointProps() { return nullptr; };
		void CreateJoint();
		void CreateJoint(Body* bodyA, Body* bodyB);
		void RecreateJoint();
		void DestroyJoint();		

		Vector2 GetConstraintForce();
		float GetConstraintTorque();

	private:
		b2JointId m_jointID;
		JointType m_jointType;
		std::string m_jointString;
		long m_bodyAID;
		long m_bodyBID;
		Body* m_bodyA;
		Body* m_bodyB;
		bool m_b_collideConnected;
		Vector2 m_anchorA;
		Vector2 m_anchorB;
		b2BodyId m_b2BodyAID;
		b2BodyId m_b2BodyBID;
	};
}
