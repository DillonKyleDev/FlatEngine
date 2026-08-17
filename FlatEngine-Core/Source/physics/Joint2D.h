#pragma once
#include "physics/PhysicsManager.h"
#include "render/SceneView.h"
#include "tools/JsonHelper.h"
#include "tools/Vector2.h"

#include <id.h>
#include <string>
#include <variant>


namespace FlatEngine
{
	class Body2D;
	
	enum JointType2D {
		JointType2D_None,
		JointType2D_Distance,
		JointType2D_Revolute,
		JointType2D_Prismatic,
		JointType2D_Mouse,
		JointType2D_Weld,
		JointType2D_Motor,
		JointType2D_Wheel,
		JointType2D_Size
	};
	const std::vector<std::string> JointType2DStrings {
		"None",
		"Distance2D",
		"Revolute2D",
		"Prismatic2D",
		"Mouse2D",
		"Weld2D",
		"Motor2D",
		"Wheel2D"
	};
	const std::unordered_map<std::string, JointType2D> JointType2DFromString = {
		{ "Distance2D",  JointType2D_Distance },
		{ "Revolute2D",  JointType2D_Revolute },
		{ "Prismatic2D", JointType2D_Prismatic },
		{ "Weld2D",      JointType2D_Weld },
		{ "Wheel2D",     JointType2D_Wheel },
		{ "Motor2D",     JointType2D_Motor },
		{ "Mouse2D",     JointType2D_Mouse },
		{ "Size",        JointType2D_Size }
	};

	struct DistanceJoint2DData {
		b2JointId jointID;
		float dampingRatio = 0.5f;
		float hertz = 2.0f;
		bool b_enableLimit = false;
		bool b_enableMotor = true;
		bool b_enableSpring = false;			
		float minLength = 0.1f;
		float maxLength = 100.0f;
		float length = 4.0f;
		float maxMotorForce = 2.0f;
		float motorSpeed = 10.0f;
		
		json GetData()
		{
			json jointData = {			
				{ "dampingRatio", dampingRatio },
				{ "b_enableLimit", b_enableLimit },
				{ "b_enableMotor", b_enableMotor },
				{ "b_enableSpring", b_enableSpring },
				{ "hertz", hertz },
				{ "minLength", minLength },
				{ "maxLength", maxLength },
				{ "length", length },
				{ "maxMotorForce", maxMotorForce },
				{ "motorSpeed", motorSpeed }
			};
			
			return jointData;
		}
		void PutData(json jointData, std::string name)
		{
			if (jointData.empty())
				return;

			b_enableLimit = JsonHelper::CheckJsonBool(jointData, "b_enableLimit", name);
			b_enableMotor = JsonHelper::CheckJsonBool(jointData, "b_enableMotor", name);
			b_enableSpring = JsonHelper::CheckJsonBool(jointData, "b_enableSpring", name);
			dampingRatio = JsonHelper::CheckJsonFloat(jointData, "dampingRatio", name);
			hertz = JsonHelper::CheckJsonFloat(jointData, "hertz", name);
			minLength = JsonHelper::CheckJsonFloat(jointData, "minLength", name);
			maxLength = JsonHelper::CheckJsonFloat(jointData, "maxLength", name);		
			length = JsonHelper::CheckJsonFloat(jointData, "length", name);
			maxMotorForce = JsonHelper::CheckJsonFloat(jointData, "maxMotorForce", name);
			motorSpeed = JsonHelper::CheckJsonFloat(jointData, "motorSpeed", name);
		}

		void SetLength(float setLength);
		float GetLength();
		void SetLengthRange(float setMinLength, float setMaxLength);
		void SetEnableSpring(bool b_setEnableSpring);
		void SetSpringHertz(float springHertz);
		float GetSpringHertz();
		void SetSpringDampingRatio(float springDampingRatio);
		float GetSpringDampingRatio();
		void SetEnableMotor(bool b_setEnableMotor);
		void SetMotorSpeed(float setMotorSpeed);
		float GetMotorSpeed();
		void SetMaxMotorForce(float setMaxMotorForce);
		float GetMotorForce();
		void SetEnableLimit(bool b_setEnableLimit);
	};
	struct MotorJoint2DData {
		b2JointId jointID;
		float angularDampingRatio = 0.0f;
		float linearHertz = 2.0f;
		float angularHertz = 2.0f;
		float angularVelocity = 10.0f;
		float linearDampingRatio = 0.5f;			
		Vector2 linearVelocity = Vector2(0.0f, 0.0f);
		float maxSpringForce = 20.0f;
		float maxSpringTorque = 2000.0f;
		float maxVelocityForce = 100.0f;
		float maxVelocityTorque = 2000.0f;
		Vector2 relativeTransformPos = Vector2(2.0f, 2.0f);
		float angleBetween = 0.0f;

		json GetData()
		{
			json jointData = {
				{ "angleBetween", angleBetween },
				{ "angularDampingRatio", angularDampingRatio },
				{ "angularHertz", angularHertz },
				{ "angularVelocity", angularVelocity },
				{ "linearDampingRatio", linearDampingRatio },
				{ "linearHertz", linearHertz },
				{ "linearVelocityX", linearVelocity.x },
				{ "linearVelocityY", linearVelocity.y },
				{ "maxSpringForce", maxSpringForce },
				{ "maxSpringTorque", maxSpringTorque },
				{ "maxVelocityForce", maxVelocityForce },
				{ "relativeTransformPosX", relativeTransformPos.x },
				{ "relativeTransformPosY", relativeTransformPos.y }
			};

			return jointData;
		}
		void PutData(json jointData, std::string name)
		{
			if (jointData.empty())
				return;

			angleBetween = JsonHelper::CheckJsonFloat(jointData, "angleBetween", name);
			angularDampingRatio = JsonHelper::CheckJsonFloat(jointData, "angularDampingRatio", name);
			angularHertz = JsonHelper::CheckJsonFloat(jointData, "angularHertz", name);
			angularVelocity = JsonHelper::CheckJsonFloat(jointData, "angularVelocity", name);
			linearDampingRatio = JsonHelper::CheckJsonFloat(jointData, "linearDampingRatio", name);
			linearHertz = JsonHelper::CheckJsonFloat(jointData, "linearHertz", name);			
			maxSpringForce = JsonHelper::CheckJsonFloat(jointData, "maxSpringForce", name);
			maxVelocityForce = JsonHelper::CheckJsonFloat(jointData, "maxVelocityForce", name);
			maxVelocityTorque = JsonHelper::CheckJsonFloat(jointData, "maxVelocityTorque", name);
			relativeTransformPos = Vector2(JsonHelper::CheckJsonFloat(jointData, "relativeTransformPosX", name), JsonHelper::CheckJsonFloat(jointData, "relativeTransformPosY", name));	
		}
	};
	struct MouseJoint2DData {
		b2JointId jointID;
		float dampingRatio = 0.5f;            
		float hertz = 2.0f;
		float maxForce = 2000.0f;

		json GetData()
		{
			json jointData = {
				{ "dampingRatio", dampingRatio },
				{ "hertz", hertz },
				{ "maxForce", maxForce }
			};

			return jointData;
		}
		void PutData(json jointData, std::string name)
		{
			if (jointData.empty())
				return;

			dampingRatio = JsonHelper::CheckJsonFloat(jointData, "dampingRatio", name);
			hertz = JsonHelper::CheckJsonFloat(jointData, "hertz", name);
			maxForce = JsonHelper::CheckJsonFloat(jointData, "maxForce", name);
		}
	};
	struct PrismaticJoint2DData {
		b2JointId jointID;
		Vector2 localAxisA = Vector2(0, 0);
		float referenceAngle = 0.0f;
		float lowerTranslation = 1.0f;
		float upperTranslation = 2.0f;
		float maxMotorForce = 2000.0f;
		float motorSpeed = 10.0f;
		float dampingRatio = 0.5f;
		bool b_enableLimit = false;
		bool b_enableMotor = true;
		bool b_enableSpring = true;			
		float targetTranslation = 0.0f;
		float hertz = 2.0f;

		json GetData()
		{
			json jointData = {
				{ "dampingRatio", dampingRatio },
				{ "b_enableLimit", b_enableLimit },
				{ "b_enableMotor", b_enableMotor },
				{ "b_enableSpring", b_enableSpring },
				{ "hertz", hertz },
				{ "targetTranslation", targetTranslation },
				{ "lowerTranslation", lowerTranslation },
				{ "upperTranslation", upperTranslation },
				{ "maxMotorForce", maxMotorForce },
				{ "motorSpeed", motorSpeed },
				{ "referenceAngle", referenceAngle },
				{ "localAxisAX", localAxisA.x },
				{ "localAxisAY", localAxisA.y }			
			};

			return jointData;
		}
		void PutData(json jointData, std::string name)
		{
			if (jointData.empty())
				return;

			b_enableLimit = JsonHelper::CheckJsonBool(jointData, "b_enableLimit", name);
			b_enableMotor = JsonHelper::CheckJsonBool(jointData, "b_enableMotor", name);
			b_enableSpring = JsonHelper::CheckJsonBool(jointData, "b_enableSpring", name);
			dampingRatio = JsonHelper::CheckJsonFloat(jointData, "dampingRatio", name);
			hertz = JsonHelper::CheckJsonFloat(jointData, "hertz", name);
			localAxisA = Vector2(JsonHelper::CheckJsonFloat(jointData, "localAxisAX", name), JsonHelper::CheckJsonFloat(jointData, "localAxisAY", name));
			lowerTranslation = JsonHelper::CheckJsonFloat(jointData, "lowerTranslation", name);
			upperTranslation = JsonHelper::CheckJsonFloat(jointData, "upperTranslation", name);
			targetTranslation = JsonHelper::CheckJsonFloat(jointData, "targetTranslation", name);
			maxMotorForce = JsonHelper::CheckJsonFloat(jointData, "maxMotorForce", name);
			motorSpeed = JsonHelper::CheckJsonFloat(jointData, "motorSpeed", name);
		}

		void SetReferenceAngle(float length);
		float GetReferenceAngle();
		void SetTargetTranslation(float targetTranslation);
		float GetTargetTranslation();		
		void SetTranslationRange(float setLowerTranslation, float setUpperTranslation);
		void SetEnableSpring(bool b_setEnableSpring);
		void SetSpringHertz(float springHertz);
		float GetSpringHertz();
		void SetSpringDampingRatio(float springDampingRatio);
		float GetSpringDampingRatio();
		void SetEnableMotor(bool b_setEnableMotor);
		void SetMotorSpeed(float setMotorSpeed);
		float GetMotorSpeed();
		void SetMaxMotorForce(float setMaxMotorForce);
		float GetMotorForce();
		void SetEnableLimit(bool b_setEnableLimit);
	};
	struct RevoluteJoint2DData {
		b2JointId jointID;
		float dampingRatio = 0.5f;
		float drawSize = 3.0f;
		bool b_enableLimit = false;
		bool b_enableMotor = true;
		bool b_enableSpring = true;
		float hertz = 2.0f;
		float lowerAngle = -1.0f;
		float upperAngle = 1.0f;
		float maxMotorTorque = 2000.0f;
		float motorSpeed = 100.0f;
		float referenceAngle = 0.0f;
		float targetAngle = 0.0f;

		json GetData()
		{
			json jointData = {
				{ "dampingRatio", dampingRatio },
				{ "b_enableLimit", b_enableLimit },
				{ "b_enableMotor", b_enableMotor },
				{ "b_enableSpring", b_enableSpring },
				{ "hertz", hertz },
				{ "drawSize", drawSize },
				{ "lowerAngle", lowerAngle },
				{ "upperAngle", upperAngle },
				{ "maxMotorTorque", maxMotorTorque },
				{ "motorSpeed", motorSpeed },
				{ "referenceAngle", referenceAngle },
				{ "targetAngle", targetAngle }
			};

			return jointData;
		}
		void PutData(json jointData, std::string name)
		{
			if (jointData.empty())
				return;

			b_enableLimit = JsonHelper::CheckJsonBool(jointData, "b_enableLimit", name);
			b_enableMotor = JsonHelper::CheckJsonBool(jointData, "b_enableMotor", name);
			b_enableSpring = JsonHelper::CheckJsonBool(jointData, "b_enableSpring", name);
			dampingRatio = JsonHelper::CheckJsonFloat(jointData, "dampingRatio", name);
			hertz = JsonHelper::CheckJsonFloat(jointData, "hertz", name);
			drawSize = JsonHelper::CheckJsonFloat(jointData, "drawSize", name);
			lowerAngle = JsonHelper::CheckJsonFloat(jointData, "lowerAngle", name);
			upperAngle = JsonHelper::CheckJsonFloat(jointData, "upperAngle", name);
			maxMotorTorque = JsonHelper::CheckJsonFloat(jointData, "maxMotorForce", name);
			motorSpeed = JsonHelper::CheckJsonFloat(jointData, "motorSpeed", name);
			referenceAngle = JsonHelper::CheckJsonFloat(jointData, "referenceAngle", name);
			targetAngle = JsonHelper::CheckJsonFloat(jointData, "targetAngle", name);	
		}
	};
	struct WeldJoint2DData {
		b2JointId jointID;
		float angularDampingRatio = 0.5f;
		float linearDampingRatio = 0.5f;
		float angularHertz = 2.0f;
		float linearHertz = 2.0f;
		float referenceAngle = 0.0f;

		json GetData()
		{
			json jointData = {
				{ "angularDampingRatio", angularDampingRatio },
				{ "angularHertz", angularHertz },
				{ "linearDampingRatio", linearDampingRatio },
				{ "linearHertz", linearHertz },
				{ "referenceAngle", referenceAngle }
			};

			return jointData;
		}
		void PutData(json jointData, std::string name)
		{
			if (jointData.empty())
				return;

			angularDampingRatio = JsonHelper::CheckJsonFloat(jointData, "angularDampingRatio", name);
			angularHertz = JsonHelper::CheckJsonFloat(jointData, "angularHertz", name);
			linearDampingRatio = JsonHelper::CheckJsonFloat(jointData, "linearDampingRatio", name);
			linearHertz = JsonHelper::CheckJsonFloat(jointData, "linearHertz", name);
			referenceAngle = JsonHelper::CheckJsonFloat(jointData, "referenceAngle", name);
		}
	};
	struct WheelJoint2DData {
		b2JointId jointID;
		float dampingRatio = 0.5f;
		float hertz = 2.0f;
		bool b_enableLimit = false;
		bool b_enableMotor = true;
		bool b_enableSpring = true;			
		Vector2 localAxisA = Vector2(0, 0);
		float lowerTranslation = 1.0f;
		float upperTranslation = 2.0f;
		float maxMotorTorque = 2000.0f;
		float motorSpeed = 10.0f;

		json GetData()
		{
			json jointData = {
				{ "dampingRatio", dampingRatio },
				{ "b_enableLimit", b_enableLimit },
				{ "b_enableMotor", b_enableMotor },
				{ "b_enableSpring", b_enableSpring },
				{ "hertz", hertz },
				{ "localAxisAX", localAxisA.x },
				{ "localAxisAY", localAxisA.y },
				{ "lowerTranslation", lowerTranslation },
				{ "upperTranslation", upperTranslation },
				{ "maxMotorTorque", maxMotorTorque },
				{ "motorSpeed", motorSpeed }			
			};

			return jointData;
		}
		void PutData(json jointData, std::string name)
		{
			if (jointData.empty())
				return;

			b_enableLimit = JsonHelper::CheckJsonBool(jointData, "b_enableLimit", name);
			b_enableMotor = JsonHelper::CheckJsonBool(jointData, "b_enableMotor", name);
			b_enableSpring = JsonHelper::CheckJsonBool(jointData, "b_enableSpring", name);
			dampingRatio = JsonHelper::CheckJsonFloat(jointData, "dampingRatio", name);
			hertz = JsonHelper::CheckJsonFloat(jointData, "hertz", name);
			localAxisA = Vector2(JsonHelper::CheckJsonFloat(jointData, "localAxisAX", name), JsonHelper::CheckJsonFloat(jointData, "localAxisAY", name));
			lowerTranslation = JsonHelper::CheckJsonFloat(jointData, "lowerTranslation", name);
			upperTranslation = JsonHelper::CheckJsonFloat(jointData, "upperTranslation", name);
			maxMotorTorque = JsonHelper::CheckJsonFloat(jointData, "maxMotorForce", name);
			motorSpeed = JsonHelper::CheckJsonFloat(jointData, "motorSpeed", name);
		}
	};

	class Joint2D
	{
		friend PhysicsManager::Physics2D;

	public:
		Joint2D(long ownerID, long myID, JointType2D type = JointType2D_None);
		// Joint2D(Joint2D&& toMove) noexcept = default;
		json GetData();
		void PutData(json jointData, std::string name);
		long GetID();
		void Cleanup();
		void SetOwnerID(long ownerID);
		long GetOwnerID();
		void SetBodyAID(long bodyAID);
		void SetBodyBID(long bodyBID);
		void WakeBodies();
		JointType2D GetType();
		void SetJointID(b2JointId jointID);
		b2JointId GetJointID();
		Body2D* GetBodyA();
		Body2D* GetBodyB();		
		bool HasValidBodies();
		bool DoesCollideConnected();
		void SetCollideConnected(bool b_collideConnected);
		Vector2 GetAnchorA();
		Vector2 GetAnchorB();
		void SetAnchorA(Vector2 anchorA);
		void SetAnchorB(Vector2 anchorB);
		Vector2 GetConstraintForce();
		float GetConstraintTorque();

		JointType2D type;
		std::vector<SceneView::SceneRenderObject> renderShapes;
		std::variant<DistanceJoint2DData, MotorJoint2DData, MouseJoint2DData, PrismaticJoint2DData, RevoluteJoint2DData, WeldJoint2DData, WheelJoint2DData> jointData;
		long bodyAID;
		long bodyBID;		
		bool b_collideConnected;
		Vector2 anchorA;
		Vector2 anchorB;
		bool b_isCollapsed;

	private:
		long m_ID;
		b2JointId m_jointID;
		long m_ownerID;		
	};
}
