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

	public:
		Body(long myID = -1, long parentID = -1);
		~Body();
		
		Physics::BodyProps GetLiveProps();
		b2BodyId GetBodyID();
		std::vector<b2ShapeId> GetShapeIDs();
		void AddShapeID(b2ShapeId shapeID);
		void SetOwner(GameObject* ownerPtr);
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

		Physics::BodyProps GetBodyProps();

	private:
		b2BodyId m_bodyID;
		std::vector<b2ShapeId> m_shapeIDs;
		Physics::BodyProps m_bodyProps;		
	};
}
