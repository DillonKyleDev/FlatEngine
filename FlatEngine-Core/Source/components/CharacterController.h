#pragma once
#include "components/Component.h"
#include "physics/Shape2D.h"
#include "tools/JsonHelper.h"
#include "tools/Vector2.h"


namespace FlatEngine 
{
	class Body;	

	class CharacterController : public Component
	{
	public:
		CharacterController(long ownerID = -1);
		json GetData(bool b_IDOverride = false);
		void PutData(json componentJson, std::string objectName);

		Shape2D& GetCapsule();
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
		// Shape m_capsule;
		float m_maxAcceleration;
		float m_maxSpeed;
		float m_airControl;
		float m_speedCorrection;
		bool m_b_isMoving;
	};
}