#pragma once
#include "Component.h"
#include "FlatEngine.h"
#include "Physics.h"
#include "Vector2.h"

#include <vector>


namespace FlatEngine
{
	class Body : public Component
	{
		friend class BoxBody;
		friend class CircleBody;
		friend class CapsuleBody;
		friend class PolygonBody;
		friend class ChainBody;

	public:
		Body(long myID = -1, long parentID = -1);
		~Body();
		
		void SetOnBeginContact(void (*beginContactCallback)(b2Manifold manifold, b2ShapeId myID, b2ShapeId collidedWithID));
		void OnBeginContact(b2Manifold manifold, b2ShapeId myID, b2ShapeId collidedWithID);
		static Body* GetBodyFromShapeID(b2ShapeId shapeID);
		void SetOnEndContact(void (*endContactCallback)(b2ShapeId myID, b2ShapeId collidedWithID));
		void OnEndContact(b2ShapeId myID, b2ShapeId collidedWithID);

		void SetBodyProps(Physics::BodyProps bodyProps);
		Physics::BodyProps& GetBodyProps();
		Physics::BodyProps GetLiveProps();
		void SetBodyID(b2BodyId bodyID);
		b2BodyId GetBodyID();
		std::vector<b2ShapeId> GetShapeIDs();
		void SetChainID(b2ChainId chainID);
		void AddShapeID(b2ShapeId shapeID);		
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
		void SetRestitution(float restitution);
		void SetDensity(float mass);
		void SetFriction(float friction);
		void CreateBody();
		void RecreateBody();
		void RecreateLiveBody();
		void ApplyForce(Vector2 force, Vector2 worldPoint);
		void ApplyLinearInpulse(Vector2 impulse, Vector2 worldPoint);
		void ApplyForceToCenter(Vector2 force);
		void ApplyLinearImpulseToCenter(Vector2 impulse);
		void ApplyTorque(float torque);
		void ApplyAngularImpulse(float impulse);
		Vector2 GetLinearVelocity();
		float GetAngularVelocity();

		// for PolygonBody and ChainBody
		virtual void SetPoints(std::vector<Vector2> points) {};
		virtual void UpdatePoints() {};

		void CleanupIDs();

	private:
		b2BodyId m_bodyID = b2_nullBodyId;
		std::vector<b2ShapeId> m_shapeIDs;
		b2ChainId m_chainID = b2_nullChainId;
		Physics::BodyProps m_bodyProps;		
		void (*m_beginContactCallback)(b2Manifold, b2ShapeId, b2ShapeId);
		bool m_b_beginContactCallbackSet;
		void (*m_endContactCallback)(b2ShapeId, b2ShapeId);
		bool m_b_endContactCallbackSet;
	};
}
