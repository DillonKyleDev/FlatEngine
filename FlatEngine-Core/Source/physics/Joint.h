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
	
	enum JointType {
		JointType_None,
		JointType_Distance,
		JointType_Revolute,
		JointType_Prismatic,
		JointType_Mouse,
		JointType_Weld,
		JointType_Motor,
		JointType_Wheel,
		JointType_Size
	};
	const std::vector<std::string> JointTypeStrings {
		"None",
		"Distance",
		"Revolute",
		"Prismatic",
		"Mouse",
		"Weld",
		"Motor",
		"Wheel"
	};
	const std::unordered_map<std::string, JointType> JointTypeFromString = {
		{ "Distance",  JointType_Distance },
		{ "Revolute",  JointType_Revolute },
		{ "Prismatic", JointType_Prismatic },
		{ "Weld",      JointType_Weld },
		{ "Wheel",     JointType_Wheel },
		{ "Motor",     JointType_Motor },
		{ "Mouse",     JointType_Mouse },
		{ "Size",     JointType_Size }
	};

	struct DistanceJointData {
		b2JointId jointID;
		float dampingRatio = 0.5f;
		float hertz = 2.0f;
		bool b_enableLimit = false;
		bool b_enableMotor = true;
		bool b_enableSpring = false;			
		float minLength = 3.0f;
		float maxLength = 5.0f;
		float length = 4.0f;
		float maxMotorForce = 2.0f;
		float motorSpeed = 10.0f;
		
		json GetData()
		{
			json jsonData = {			
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
			
			return jsonData;
		}
		void PutData(json jsonData, std::string name)
		{
			b_enableLimit = JsonHelper::CheckJsonBool(jsonData, "b_enableLimit", name);
			b_enableMotor = JsonHelper::CheckJsonBool(jsonData, "b_enableMotor", name);
			b_enableSpring = JsonHelper::CheckJsonBool(jsonData, "b_enableSpring", name);
			dampingRatio = JsonHelper::CheckJsonFloat(jsonData, "dampingRatio", name);
			hertz = JsonHelper::CheckJsonFloat(jsonData, "hertz", name);
			minLength = JsonHelper::CheckJsonFloat(jsonData, "minLength", name);
			maxLength = JsonHelper::CheckJsonFloat(jsonData, "maxLength", name);		
			length = JsonHelper::CheckJsonFloat(jsonData, "length", name);
			maxMotorForce = JsonHelper::CheckJsonFloat(jsonData, "maxMotorForce", name);
			motorSpeed = JsonHelper::CheckJsonFloat(jsonData, "motorSpeed", name);
		}

		void SetLength(float length)
		{
			if (length > 0)
			{
				length = length;

				if (b2Joint_IsValid(jointID))
				{
					b2DistanceJoint_SetLength(jointID, length);
				}
			}
		}

		float GetLength()
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

		void SetLengthRange(float minLength, float maxLength)
		{
			if (minLength >= 0 && maxLength >= minLength)
			{
				minLength = minLength;
				maxLength = maxLength;

				if (b2Joint_IsValid(jointID))
				{
					b2DistanceJoint_SetLengthRange(jointID, minLength, maxLength);				
				}
			}
		}

		void SetEnableSpring(bool b_enableSpring)
		{
			b_enableSpring = b_enableSpring;

			if (b2Joint_IsValid(jointID))
			{
				b2DistanceJoint_EnableSpring(jointID, b_enableSpring);
			}
		}

		void SetSpringHertz(float springHertz)
		{
			if (springHertz >= 0)
			{
				hertz = springHertz;

				if (b2Joint_IsValid(jointID))
				{
					b2DistanceJoint_SetSpringHertz(jointID, springHertz);
				}
			}
		}

		float GetSpringHertz()
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

		void SetSpringDampingRatio(float springDampingRatio)
		{
			if (springDampingRatio >= 0)
			{
				dampingRatio = springDampingRatio;

				if (b2Joint_IsValid(jointID))
				{				
					b2DistanceJoint_SetSpringDampingRatio(jointID, springDampingRatio);
				}
			}
		}

		float GetSpringDampingRatio()
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

		void SetEnableMotor(bool b_enableMotor)
		{
			b_enableMotor = b_enableMotor;

			if (b2Joint_IsValid(jointID))
			{
				b2DistanceJoint_EnableSpring(jointID, b_enableMotor);
			}
		}

		void SetMotorSpeed(float motorSpeed)
		{
			if (motorSpeed >= 0)
			{
				motorSpeed = motorSpeed;

				if (b2Joint_IsValid(jointID))
				{
					b2DistanceJoint_SetMotorSpeed(jointID, motorSpeed);
				}
			}
		}

		float GetMotorSpeed()
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

		void SetMaxMotorForce(float maxMotorForce)
		{
			if (maxMotorForce >= 0)
			{
				maxMotorForce = maxMotorForce;

				if (b2Joint_IsValid(jointID))
				{
					b2DistanceJoint_SetMaxMotorForce(jointID, maxMotorForce);
				}
			}
		}

		float GetMotorForce()
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

		void SetEnableLimit(bool b_enableLimit)
		{
			b_enableLimit = b_enableLimit;

			if (b2Joint_IsValid(jointID))
			{
				b2DistanceJoint_EnableLimit(jointID, b_enableLimit);
			}
		}
	};
	struct MotorJointData {
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
			json jsonData = {
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

			return jsonData;
		}
		void PutData(json jsonData, std::string name)
		{
			angleBetween = JsonHelper::CheckJsonBool(jsonData, "angleBetween", name);
			angularDampingRatio = JsonHelper::CheckJsonBool(jsonData, "angularDampingRatio", name);
			angularHertz = JsonHelper::CheckJsonBool(jsonData, "angularHertz", name);
			angularVelocity = JsonHelper::CheckJsonFloat(jsonData, "angularVelocity", name);
			linearDampingRatio = JsonHelper::CheckJsonFloat(jsonData, "linearDampingRatio", name);
			linearHertz = JsonHelper::CheckJsonFloat(jsonData, "linearHertz", name);
			//linearVelocity = JsonHelper::CheckJsonFloat(jsonData, "linearVelocity", name);
			maxSpringForce = JsonHelper::CheckJsonFloat(jsonData, "maxSpringForce", name);
			maxVelocityForce = JsonHelper::CheckJsonFloat(jsonData, "maxVelocityForce", name);
			maxVelocityTorque = JsonHelper::CheckJsonFloat(jsonData, "maxVelocityTorque", name);
			relativeTransformPos = Vector2(JsonHelper::CheckJsonFloat(jsonData, "relativeTransformPosX", name), JsonHelper::CheckJsonFloat(jsonData, "relativeTransformPosY", name));	
		}
	};
	struct MouseJointData {
		b2JointId jointID;
		float dampingRatio = 0.5f;            
		float hertz = 2.0f;
		float maxForce = 2000.0f;

		json GetData()
		{
			json jsonData = {
				{ "dampingRatio", dampingRatio },
				{ "hertz", hertz },
				{ "maxForce", maxForce }
			};

			return jsonData;
		}
		void PutData(json jsonData, std::string name)
		{
			dampingRatio = JsonHelper::CheckJsonFloat(jsonData, "dampingRatio", name);
			hertz = JsonHelper::CheckJsonFloat(jsonData, "hertz", name);
			maxForce = JsonHelper::CheckJsonFloat(jsonData, "maxForce", name);
		}
	};
	struct PrismaticJointData {
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
			json jsonData = {
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

			return jsonData;
		}
		void PutData(json jsonData, std::string name)
		{
			b_enableLimit = JsonHelper::CheckJsonBool(jsonData, "b_enableLimit", name);
			b_enableMotor = JsonHelper::CheckJsonBool(jsonData, "b_enableMotor", name);
			b_enableSpring = JsonHelper::CheckJsonBool(jsonData, "b_enableSpring", name);
			dampingRatio = JsonHelper::CheckJsonFloat(jsonData, "dampingRatio", name);
			hertz = JsonHelper::CheckJsonFloat(jsonData, "hertz", name);
			localAxisA = Vector2(JsonHelper::CheckJsonFloat(jsonData, "localAxisAX", name), JsonHelper::CheckJsonFloat(jsonData, "localAxisAY", name));
			lowerTranslation = JsonHelper::CheckJsonFloat(jsonData, "lowerTranslation", name);
			upperTranslation = JsonHelper::CheckJsonFloat(jsonData, "upperTranslation", name);
			targetTranslation = JsonHelper::CheckJsonFloat(jsonData, "targetTranslation", name);
			maxMotorForce = JsonHelper::CheckJsonFloat(jsonData, "maxMotorForce", name);
			motorSpeed = JsonHelper::CheckJsonFloat(jsonData, "motorSpeed", name);
		}

		void SetReferenceAngle(float length)
		{
			
		}

		float GetReferenceAngle()
		{
			return 0;
		}

		void SetTargetTranslation(float targetTranslation)
		{

		}

		float GetTargetTranslation()
		{
			return 0;
		}

		// Translation upper and lower tied to b_enableLimit
		void SetTranslationRange(float lowerTranslation, float upperTranslation)
		{
			if (true) // some constraint on translation range todo)
			{
				lowerTranslation = lowerTranslation;
				upperTranslation = upperTranslation;

				if (b2Joint_IsValid(jointID))
				{
					b2PrismaticJoint_SetLimits(jointID, lowerTranslation, upperTranslation);
				}
				else
				{
					PhysicsManager::physics2D.CreateJoint(static_cast<Joint*>(b2Joint_GetUserData(jointID)));
				}
			}
		}

		void SetEnableSpring(bool b_enableSpring)
		{
			b_enableSpring = b_enableSpring;

			if (b2Joint_IsValid(jointID))
			{
				b2PrismaticJoint_EnableSpring(jointID, b_enableSpring);
			}
			else
			{
				PhysicsManager::physics2D.CreateJoint(static_cast<Joint*>(b2Joint_GetUserData(jointID)));
			}
		}

		void SetSpringHertz(float springHertz)
		{
			if (springHertz >= 0)
			{
				hertz = springHertz;

				if (b2Joint_IsValid(jointID))
				{
					b2PrismaticJoint_SetSpringHertz(jointID, springHertz);
				}
				else
				{
					PhysicsManager::physics2D.CreateJoint(static_cast<Joint*>(b2Joint_GetUserData(jointID)));
				}
			}
		}

		float GetSpringHertz()
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

		void SetSpringDampingRatio(float springDampingRatio)
		{
			if (springDampingRatio >= 0)
			{
				dampingRatio = springDampingRatio;

				if (b2Joint_IsValid(jointID))
				{
					b2PrismaticJoint_SetSpringDampingRatio(jointID, springDampingRatio);
				}
				else
				{
					PhysicsManager::physics2D.CreateJoint(static_cast<Joint*>(b2Joint_GetUserData(jointID)));
				}
			}
		}

		float GetSpringDampingRatio()
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

		void SetEnableMotor(bool b_enableMotor)
		{
			b_enableMotor = b_enableMotor;

			if (b2Joint_IsValid(jointID))
			{
				b2PrismaticJoint_EnableSpring(jointID, b_enableMotor);
			}
			else
			{
				PhysicsManager::physics2D.CreateJoint(static_cast<Joint*>(b2Joint_GetUserData(jointID)));
			}
		}

		void SetMotorSpeed(float motorSpeed)
		{
			if (motorSpeed >= 0)
			{
				motorSpeed = motorSpeed;

				if (b2Joint_IsValid(jointID))
				{
					b2PrismaticJoint_SetMotorSpeed(jointID, motorSpeed);
				}
				else
				{
					PhysicsManager::physics2D.CreateJoint(static_cast<Joint*>(b2Joint_GetUserData(jointID)));
				}
			}
		}

		float GetMotorSpeed()
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

		void SetMaxMotorForce(float maxMotorForce)
		{
			if (maxMotorForce >= 0)
			{
				maxMotorForce = maxMotorForce;

				if (b2Joint_IsValid(jointID))
				{
					b2PrismaticJoint_SetMaxMotorForce(jointID, maxMotorForce);
				}
				else
				{
					PhysicsManager::physics2D.CreateJoint(static_cast<Joint*>(b2Joint_GetUserData(jointID)));
				}
			}
		}

		float GetMotorForce()
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

		void SetEnableLimit(bool b_enableLimit)
		{
			b_enableLimit = b_enableLimit;

			if (b2Joint_IsValid(jointID))
			{
				b2PrismaticJoint_EnableLimit(jointID, b_enableLimit);
			}
			else
			{
				PhysicsManager::physics2D.CreateJoint(static_cast<Joint*>(b2Joint_GetUserData(jointID)));
			}
		}
	};
	struct RevoluteJointData {
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
			json jsonData = {
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

			return jsonData;
		}
		void PutData(json jsonData, std::string name)
		{
			b_enableLimit = JsonHelper::CheckJsonBool(jsonData, "b_enableLimit", name);
			b_enableMotor = JsonHelper::CheckJsonBool(jsonData, "b_enableMotor", name);
			b_enableSpring = JsonHelper::CheckJsonBool(jsonData, "b_enableSpring", name);
			dampingRatio = JsonHelper::CheckJsonFloat(jsonData, "dampingRatio", name);
			hertz = JsonHelper::CheckJsonFloat(jsonData, "hertz", name);
			drawSize = JsonHelper::CheckJsonFloat(jsonData, "drawSize", name);
			lowerAngle = JsonHelper::CheckJsonFloat(jsonData, "lowerAngle", name);
			upperAngle = JsonHelper::CheckJsonFloat(jsonData, "upperAngle", name);
			maxMotorTorque = JsonHelper::CheckJsonFloat(jsonData, "maxMotorForce", name);
			motorSpeed = JsonHelper::CheckJsonFloat(jsonData, "motorSpeed", name);
			referenceAngle = JsonHelper::CheckJsonFloat(jsonData, "referenceAngle", name);
			targetAngle = JsonHelper::CheckJsonFloat(jsonData, "targetAngle", name);	
		}
	};
	struct WeldJointData {
		b2JointId jointID;
		float angularDampingRatio = 0.5f;
		float linearDampingRatio = 0.5f;
		float angularHertz = 2.0f;
		float linearHertz = 2.0f;
		float referenceAngle = 0.0f;

		json GetData()
		{
			json jsonData = {
				{ "angularDampingRatio", angularDampingRatio },
				{ "angularHertz", angularHertz },
				{ "linearDampingRatio", linearDampingRatio },
				{ "linearHertz", linearHertz },
				{ "referenceAngle", referenceAngle }
			};

			return jsonData;
		}
		void PutData(json jsonData, std::string name)
		{
			angularDampingRatio = JsonHelper::CheckJsonBool(jsonData, "angularDampingRatio", name);
			angularHertz = JsonHelper::CheckJsonBool(jsonData, "angularHertz", name);
			linearDampingRatio = JsonHelper::CheckJsonBool(jsonData, "linearDampingRatio", name);
			linearHertz = JsonHelper::CheckJsonFloat(jsonData, "linearHertz", name);
			referenceAngle = JsonHelper::CheckJsonFloat(jsonData, "referenceAngle", name);
		}
	};
	struct WheelJointData {
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
			json jsonData = {
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

			return jsonData;
		}
		void PutData(json jsonData, std::string name)
		{
			b_enableLimit = JsonHelper::CheckJsonBool(jsonData, "b_enableLimit", name);
			b_enableMotor = JsonHelper::CheckJsonBool(jsonData, "b_enableMotor", name);
			b_enableSpring = JsonHelper::CheckJsonBool(jsonData, "b_enableSpring", name);
			dampingRatio = JsonHelper::CheckJsonFloat(jsonData, "dampingRatio", name);
			hertz = JsonHelper::CheckJsonFloat(jsonData, "hertz", name);
			localAxisA = Vector2(JsonHelper::CheckJsonFloat(jsonData, "localAxisAX", name), JsonHelper::CheckJsonFloat(jsonData, "localAxisAY", name));
			lowerTranslation = JsonHelper::CheckJsonFloat(jsonData, "lowerTranslation", name);
			upperTranslation = JsonHelper::CheckJsonFloat(jsonData, "upperTranslation", name);
			maxMotorTorque = JsonHelper::CheckJsonFloat(jsonData, "maxMotorForce", name);
			motorSpeed = JsonHelper::CheckJsonFloat(jsonData, "motorSpeed", name);
		}
	};

	class Joint
	{
		friend PhysicsManager::Physics2D;

	public:
		Joint(long ownerID, long myID, JointType type = JointType_None);
		json GetData();
		void PutData(json jsonData, std::string name);
		const long GetID();
		void SetOwnerID(long ownerID);
		long GetOwnerID();
		void SetBodyAID(long bodyAID);
		void SetBodyBID(long bodyBID);
		JointType GetJointType();
		void SetJointID(b2JointId jointID);
		b2JointId GetJointID();
		Body2D* GetBodyA();
		Body2D* GetBodyB();		
		bool HasValidBodies();
		bool CollideConnected();
		Vector2 GetAnchorA();
		Vector2 GetAnchorB();
		void SetAnchorA(Vector2 anchorA);
		void SetAnchorB(Vector2 anchorB);
		Vector2 GetConstraintForce();
		float GetConstraintTorque();

		JointType type;
		std::vector<SceneView::SceneRenderObject> renderShapes;
		std::variant<DistanceJointData, MotorJointData, MouseJointData, PrismaticJointData, RevoluteJointData, WeldJointData, WheelJointData> jointData;
		long bodyAID = -1;
		long bodyBID = -1;
		JointType jointType = JointType_None;
		bool b_collideConnected = false;
		Vector2 anchorA = Vector2();
		Vector2 anchorB = Vector2();

	private:
		long m_ID;
		b2JointId m_jointID;
		long m_ownerID;
	};
}
