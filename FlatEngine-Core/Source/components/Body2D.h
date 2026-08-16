#pragma once
#include "GameObject.h"
#include "Types.h"
#include "components/Component.h"
#include "physics/Joint2D.h"
#include "physics/PhysicsManager.h"
#include "physics/Shape2D.h"
#include "tools/Vector2.h"

#include <id.h>
#include <list>
#include <unordered_map>
#include <vector>


namespace FlatEngine
{
	class Joint2D;

	class Body2D : public Component
	{
		friend GameObject;
		friend PhysicsManager::Physics2D;

	public:
		Body2D(long ownerID = -1);
		json GetData(bool b_IDOverride = false);
		void PutData(json jsonData, std::string objectName);
		void SetActive(bool b_isActive);
		void Cleanup();

		Vector2 ConvertWorldToLocalPoint(Vector2 worldPoint);
		Vector2 ConvertLocalToWorldPoint(Vector2 localPoint);
		Vector2 ConvertWorldToLocalVector(Vector2 worldVector);
		Vector2 ConvertLocalToWorldVector(Vector2 localVector);

		// Contacts
		void SetOnBeginContact(void (*beginContactCallback)(b2Manifold manifold, b2ShapeId myID, b2ShapeId collidedWithID));
		void OnBeginContact(b2Manifold manifold, b2ShapeId myID, b2ShapeId collidedWithID);
		void SetOnEndContact(void (*endContactCallback)(b2ShapeId myID, b2ShapeId collidedWithID));
		void OnEndContact(b2ShapeId myID, b2ShapeId collidedWithID);
		// Sensors
		void SetOnSensorBeginTouch(void (*beginSensorTouchCallback)(b2ShapeId myID, b2ShapeId touchedID));
		void OnSensorBeginTouch(b2ShapeId myID, b2ShapeId touchedID);
		void SetOnSensorEndTouch(void (*endSensorTouchCallback)(b2ShapeId myID, b2ShapeId touchedID));
		void OnSensorEndTouch(b2ShapeId myID, b2ShapeId touchedID);
	
		void AddShape(ShapeType2D type, json jsonData = json::object(), std::string name = "");
		const b2BodyId GetBodyID();
		std::vector<Shape2D*> GetShapes();		
		void RemoveShape(b2ShapeId shapeID);
		void RemoveChain(b2ChainId chainID);
		void AddJoint(JointType2D type, json jsonData = json::object(), std::string name = "");
		void AddConnectedJoint(Joint2D* joint);
		void RemoveConnectedJoint(Joint2D* joint);
		std::unordered_map<long, Joint2D*> GetConnectedJoints();
        std::vector<Joint2D*> GetJoints();        
        void RemoveJoint(long jointID);
		void UpdateRenderShapes();

		void SetBodyID(b2BodyId bodyID);
		void SetBodyType(b2BodyType type);
		void SetPosition(Vector2 position);
		Vector2 GetPosition();
		void SetRotation(float rotation);
		void SetRotation(b2Rot rotation);
		float GetRotation();
		b2Rot GetB2Rotation();
		void SetLockedRotation(bool b_lockedRotation);
		void SetLockedXAxis(bool b_lockedXAxis);
		void SetLockedYAxis(bool b_lockedYAxis);
		void SetGravityScale(float gravityScale);
		void SetLinearDamping(float linearDamping);	
		void SetAngularDamping(float angularDamping);
		void ApplyForce(Vector2 force, Vector2 worldPoint);
		void ApplyLinearInpulse(Vector2 impulse, Vector2 worldPoint);
		void ApplyForceToCenter(Vector2 force);
		void ApplyLinearImpulseToCenter(Vector2 impulse);
		void ApplyTorque(float torque);
		void ApplyAngularImpulse(float impulse);
		Vector2 GetLinearVelocity();
		float GetAngularVelocity();

		b2BodyType type = b2_dynamicBody;
		bool b_lockedRotation = false;
		bool b_lockedXAxis = false;
		bool b_lockedYAxis = false;
		float gravityScale = 1.0f;
		float linearDamping = 0.0f;
		float angularDamping = 0.0f;

		void RecreateJoints();
		
	private:
		b2BodyId m_bodyID = b2_nullBodyId;
		FL::Vector2 position = FL::Vector2();	
		b2Rot rotation = b2MakeRot(0);

		std::list<Shape2D> m_boxes;
		std::list<Shape2D> m_circles;
		std::list<Shape2D> m_capsules;
		std::list<Shape2D> m_polygons;
		std::list<Shape2D> m_chains;

		std::unordered_map<long, Joint2D*> m_jointsConnected;

		std::list<Joint2D> m_distanceJoints;
        std::list<Joint2D> m_prismaticJoints;
        std::list<Joint2D> m_revoluteJoints;
        std::list<Joint2D> m_mouseJoints;
        std::list<Joint2D> m_wheelJoints;
        std::list<Joint2D> m_motorJoints;
        std::list<Joint2D> m_weldJoints;  

		UMapVector<Joint2D> m_joints;

		// Contacts
		void (*m_beginContactCallback)(b2Manifold, b2ShapeId, b2ShapeId);
		bool m_b_beginContactCallbackSet;
		void (*m_endContactCallback)(b2ShapeId, b2ShapeId);
		bool m_b_endContactCallbackSet;
		// Sensors
		void (*m_beginSensorTouchCallback)(b2ShapeId, b2ShapeId);
		bool m_b_beginSensorTouchCallbackSet;
		void (*m_endSensorTouchCallback)(b2ShapeId, b2ShapeId);
		bool m_b_endSensorTouchCallbackSet;
	};
}
