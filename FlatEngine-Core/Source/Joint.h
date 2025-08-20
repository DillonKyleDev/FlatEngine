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

		struct JointProps {};

		struct BaseProps : public JointProps{
			long bodyAID = -1;
			long bodyBID = -1;
			JointType jointType = JT_None;
			bool b_collideConnected = false;
			Vector2 anchorA = Vector2(0, 0);
			Vector2 anchorB = Vector2(0, 0);
		};

		Joint(BaseProps baseProps = BaseProps());
		~Joint();
		json GetBasePropsData();
		void SetJointID(long jointID);
		long GetJointID();
		virtual json GetJointData() { return json::object(); };
		//virtual JointProps& GetJointProps() { return; };
		BaseProps GetBaseProps();
		void SetBodyAID(long bodyAID);
		void SetBodyBID(long bodyBID);
		JointType GetJointType();
		std::string GetJointString();

		void SetB2JointID(b2JointId jointID);
		b2JointId GetB2JointID();
		Body* GetBodyA();
		Body* GetBodyB();		
		bool HasValidBodies();
		bool CollideConnected();
		Vector2 GetAnchorA();
		Vector2 GetAnchorB();
		void SetAnchorA(Vector2 anchorA);
		void SetAnchorB(Vector2 anchorB);
		void CreateJoint();
		void CreateJoint(Body* bodyA, Body* bodyB);
		void RecreateJoint();
		void DestroyJoint();		

		Vector2 GetConstraintForce();
		float GetConstraintTorque();

	private:
		b2JointId m_b2JointID;
		long m_jointID;
		std::string m_jointString;
		BaseProps m_baseProps;
	};
}
