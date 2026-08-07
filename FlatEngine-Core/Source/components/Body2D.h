#pragma once
#include "components/Component.h"
#include "physics/PhysicsManager.h"
#include "physics/Shape.h"
#include "render/SceneView.h"
#include "tools/Vector2.h"

#include <list>
#include <vector>


namespace FlatEngine
{
	class Joint;
	class DistanceJoint;

	class Body2D : public Component
	{
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
	
		void UpdateRenderShapes();
		void RecreateShapes();

		void SetBodyID(b2BodyId bodyID);
		void AddShape(ShapeType type);
		const b2BodyId GetBodyID();
		std::vector<Shape*> GetShapes();		
		void RemoveShape(b2ShapeId shapeID);
		void RemoveChain(b2ChainId chainID);
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

		void AddJoint(Joint* joint);

		b2BodyType type = b2_dynamicBody;
		FL::Vector2 position = FL::Vector2();	
		b2Rot rotation = b2MakeRot(0);
		bool b_lockedRotation = false;
		bool b_lockedXAxis = false;
		bool b_lockedYAxis = false;
		float gravityScale = 1.0f;
		float linearDamping = 0.0f;
		float angularDamping = 0.0f;

		std::vector<SceneView::SceneRenderObject> renderShapes;
		std::list<Shape> boxes;
		std::list<Shape> circles;
		std::list<Shape> capsules;
		std::list<Shape> polygons;
		std::list<Shape> chains;

	private:
		b2BodyId m_bodyID = b2_nullBodyId;
		std::list<DistanceJoint*> m_distanceJoints;
		

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
