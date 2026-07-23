#pragma once
#include "components/Component.h"
#include "shapes/Capsule.h"
#include "tools/JsonHelper.h"
#include "tools/Vector2.h"

#include <string>


namespace FlatEngine 
{
	class Body;	

	class CharacterController : public Component
	{
	public:
		CharacterController(long myID, long parentObjectID);
		std::string GetData();
		void PutData(json componentJson);

		Capsule& GetCapsule();
		void Move(Vector2 direction);
		void MoveToward(Vector2 position);
		void SetMaxAcceleration(float speed);
		float GetMaxAcceleration();
		void SetMaxSpeed(float newMaxSpeed);
		float GetMaxSpeed();
		void SetAirControl(float newAirControl);
		float GetAirControl();
		void SetMoving(bool b_isMoving);
		bool IsMoving();

	private:
		Capsule m_capsule;
		float m_maxAcceleration;
		float m_maxSpeed;
		float m_airControl;
		float m_speedCorrection;
		bool m_b_isMoving;
	};
}