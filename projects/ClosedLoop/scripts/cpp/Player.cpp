#include "Player.h"
#include "GameObject.h"
#include "Transform.h"
#include "Vector2.h"
#include "Body.h"
#include "Animation.h"
#include "FlatEngine.h"


namespace FlatEngine
{
	void SensorCallbackFunction(b2ShapeId myID, b2ShapeId collidedWithID)
	{
		GameObject* self = Body::GetBodyFromShapeID(myID)->GetParent();
		GameObject* collidedWith = Body::GetBodyFromShapeID(collidedWithID)->GetParent();
		LogString("Sensor began touching from C++");
	}

	void AnimationEventFuncLog(GameObject* callingObject, Animation::S_EventFunctionParam params)
	{
		//LogString("Event function called on Animation! By " + callingObject->GetName());
		LogString(params.e_string);
		//LogInt(params.e_int);
		//LogLong(params.e_long);
		//LogFloat(params.e_float);
		//LogVector2(params.e_Vector2);

		if (params.e_boolean)
		{
			//LogString();
		}
	}

	void OnLeftClick(GameObject* callingObject)
	{
		LogString(callingObject->GetName() + " button was clicked.");
	}

	Player::Player()
	{
		AddCPPAnimationEventFunction("EventFuncLog", AnimationEventFuncLog);
	}

	Player::~Player()
	{

	}

	void Player::Awake()
	{
		if (GetParent()->GetButton() != nullptr)
		{
			GetParent()->GetButton()->SetOnLeftClickCallback(OnLeftClick);
		}
	}

	void Player::Start()
	{		
		//GetParent()->GetAnimation()->Play("explode");
		//std::string name = GetScriptParam("nameString").e_string;
		//LogString(name);

	}

	void Player::Update()
    {

	}
}