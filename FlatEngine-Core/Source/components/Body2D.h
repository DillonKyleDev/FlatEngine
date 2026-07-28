#pragma once
#include "components/Component.h"
#include "managers/PhysicsManager.h"
#include "shapes/Box.h"
#include "shapes/Capsule.h"
#include "shapes/Circle.h"
#include "shapes/Chain.h"
#include "shapes/Polygon.h"
#include "shapes/Shape.h"
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
		Body2D(long myID = -1, long parentObjectID = -1);
		json GetData(bool b_IDOverride = false);
		void PutData(json componentJson, std::string objectName);
		void SetActive(bool b_isActive);

		Vector2 ConvertWorldToLocalPoint(Vector2 worldPoint);
		Vector2 ConvertLocalToWorldPoint(Vector2 localPoint);
		Vector2 ConvertWorldToLocalVector(Vector2 worldVector);
		Vector2 ConvertLocalToWorldVector(Vector2 localVector);
		
		static Body2D* GetBodyFromShapeID(b2ShapeId shapeID);

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
	
		PhysicsManager::BodyProps GetLiveProps();
		void SetBodyID(b2BodyId bodyID);
		const b2BodyId GetBodyID();
		std::vector<Shape*> GetShapes();		
		void RemoveShape(b2ShapeId shapeID);
		void RemoveChain(b2ChainId chainID);
		std::list<Box>& GetBoxes();
		std::list<Circle>& GetCircles();
		std::list<Capsule>& GetCapsules();
		std::list<Polygon>& GetPolygons();
		std::list<Chain>& GetChains();
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
		void CreateBody();
		void RecreateBody();
		void RecreateLiveBody();
		void RecreateShapes();
		void ApplyForce(Vector2 force, Vector2 worldPoint);
		void ApplyLinearInpulse(Vector2 impulse, Vector2 worldPoint);
		void ApplyForceToCenter(Vector2 force);
		void ApplyLinearImpulseToCenter(Vector2 impulse);
		void ApplyTorque(float torque);
		void ApplyAngularImpulse(float impulse);
		Vector2 GetLinearVelocity();
		float GetAngularVelocity();

		void AddBox(Shape::ShapeProps shapeProps = Shape::ShapeProps());
		void AddCircle(Shape::ShapeProps shapeProps = Shape::ShapeProps());
		void AddCapsule(Shape::ShapeProps shapeProps = Shape::ShapeProps());
		void AddPolygon(Shape::ShapeProps shapeProps = Shape::ShapeProps());
		void AddChain(Shape::ShapeProps shapeProps = Shape::ShapeProps());

		void AddJoint(Joint* joint);

		void Cleanup();

		PhysicsManager::BodyProps bodyProps;

	private:
		b2BodyId m_bodyID = b2_nullBodyId;
		std::list<Box> m_boxes;
		std::list<Circle> m_circles;
		std::list<Capsule> m_capsules;
		std::list<Polygon> m_polygons;
		std::list<Chain> m_chains;
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
