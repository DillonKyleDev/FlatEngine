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

		void SetLength(float setLength)
		{
			if (setLength > 0)
			{
				length = setLength;

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

		void SetLengthRange(float setMinLength, float setMaxLength)
		{
			if (setMinLength >= 0 && setMaxLength >= setMinLength)
			{
				minLength = setMinLength;
				maxLength = setMaxLength;

				if (b2Joint_IsValid(jointID))
				{
					b2DistanceJoint_SetLengthRange(jointID, minLength, maxLength);				
				}
			}
		}

		void SetEnableSpring(bool b_setEnableSpring)
		{
			b_enableSpring = b_setEnableSpring;

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

		void SetEnableMotor(bool b_setEnableMotor)
		{
			b_enableMotor = b_setEnableMotor;

			if (b2Joint_IsValid(jointID))
			{
				b2DistanceJoint_EnableSpring(jointID, b_enableMotor);
			}
		}

		void SetMotorSpeed(float setMotorSpeed)
		{
			if (setMotorSpeed >= 0)
			{
				motorSpeed = setMotorSpeed;

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

		void SetMaxMotorForce(float setMaxMotorForce)
		{
			if (setMaxMotorForce >= 0)
			{
				maxMotorForce = setMaxMotorForce;

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

		void SetEnableLimit(bool b_setEnableLimit)
		{
			b_enableLimit = b_setEnableLimit;

			if (b2Joint_IsValid(jointID))
			{
				b2DistanceJoint_EnableLimit(jointID, b_enableLimit);
			}
		}
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
		void SetTranslationRange(float setLowerTranslation, float setUpperTranslation)
		{
			if (true) // some constraint on translation range todo)
			{
				lowerTranslation = setLowerTranslation;
				upperTranslation = setUpperTranslation;

				if (b2Joint_IsValid(jointID))
				{
					b2PrismaticJoint_SetLimits(jointID, lowerTranslation, upperTranslation);
				}
				else
				{
					PhysicsManager::gamePhysics2D.CreateJoint(static_cast<Joint2D*>(b2Joint_GetUserData(jointID)));
				}
			}
		}

		void SetEnableSpring(bool b_setEnableSpring)
		{
			b_enableSpring = b_setEnableSpring;

			if (b2Joint_IsValid(jointID))
			{
				b2PrismaticJoint_EnableSpring(jointID, b_enableSpring);
			}
			else
			{
				PhysicsManager::gamePhysics2D.CreateJoint(static_cast<Joint2D*>(b2Joint_GetUserData(jointID)));
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
					PhysicsManager::gamePhysics2D.CreateJoint(static_cast<Joint2D*>(b2Joint_GetUserData(jointID)));
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
					PhysicsManager::gamePhysics2D.CreateJoint(static_cast<Joint2D*>(b2Joint_GetUserData(jointID)));
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

		void SetEnableMotor(bool b_setEnableMotor)
		{
			b_enableMotor = b_setEnableMotor;

			if (b2Joint_IsValid(jointID))
			{
				b2PrismaticJoint_EnableSpring(jointID, b_enableMotor);
			}
			else
			{
				PhysicsManager::gamePhysics2D.CreateJoint(static_cast<Joint2D*>(b2Joint_GetUserData(jointID)));
			}
		}

		void SetMotorSpeed(float setMotorSpeed)
		{
			if (setMotorSpeed >= 0)
			{
				motorSpeed = setMotorSpeed;

				if (b2Joint_IsValid(jointID))
				{
					b2PrismaticJoint_SetMotorSpeed(jointID, motorSpeed);
				}
				else
				{
					PhysicsManager::gamePhysics2D.CreateJoint(static_cast<Joint2D*>(b2Joint_GetUserData(jointID)));
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

		void SetMaxMotorForce(float setMaxMotorForce)
		{
			if (setMaxMotorForce >= 0)
			{
				maxMotorForce = setMaxMotorForce;

				if (b2Joint_IsValid(jointID))
				{
					b2PrismaticJoint_SetMaxMotorForce(jointID, maxMotorForce);
				}
				else
				{
					PhysicsManager::gamePhysics2D.CreateJoint(static_cast<Joint2D*>(b2Joint_GetUserData(jointID)));
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

		void SetEnableLimit(bool b_setEnableLimit)
		{
			b_enableLimit = b_setEnableLimit;

			if (b2Joint_IsValid(jointID))
			{
				b2PrismaticJoint_EnableLimit(jointID, b_enableLimit);
			}
			else
			{
				PhysicsManager::gamePhysics2D.CreateJoint(static_cast<Joint2D*>(b2Joint_GetUserData(jointID)));
			}
		}
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
		json GetData();
		void PutData(json jointData, std::string name);
		const long GetID();
		void SetOwnerID(long ownerID);
		long GetOwnerID();
		void SetBodyAID(long bodyAID);
		void SetBodyBID(long bodyBID);
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
		long bodyAID = -1;
		long bodyBID = -1;
		JointType2D jointType = JointType2D_None;
		bool b_collideConnected = false;
		Vector2 anchorA = Vector2();
		Vector2 anchorB = Vector2();
		bool b_isCollapsed;

	private:
		long m_ID;
		b2JointId m_jointID;
		long m_ownerID;		
	};
}
