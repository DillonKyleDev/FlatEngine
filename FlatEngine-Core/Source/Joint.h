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
			Body* bodyPtr = nullptr;
			JointType jointType = JT_None;
			bool b_collideConnected = true;
			b2BodyId bodyIDA = b2_nullBodyId;
			b2BodyId bodyIDB = b2_nullBodyId;
			Vector2 anchorA = Vector2(0, 0);
			Vector2 anchorB = Vector2(0, 0);
		};

		Joint();
		~Joint();
		json GetJointData();
		JointType GetJointType();
		std::string GetJointString();

		void SetJointID(b2JointId jointID);
		b2JointId GetJointID();
		Body* GetBodyA();
		Body* GetBodyB();		
		void SetAnchorA(Vector2 anchorA);
		void SetAnchorB(Vector2 anchorB);
		virtual JointProps* GetJointProps() { return nullptr; };
		void CreateJoint(Body* bodyA, Body* bodyB);
		void RecreateJoint();
		void DestroyJoint();		

		Vector2 GetConstraintForce();
		float GetConstraintTorque();

	private:
		b2JointId m_jointID;
		JointType m_jointType;
		Body* m_bodyA;
		Body* m_bodyB;
	};
}
