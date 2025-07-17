#pragma once
#include "FlatEngine.h"
#include "CPPScript.h"
#include "RayCast.h"

namespace FlatEngine
{
	class Player : public CPPScript
	{
	public:
		Player(); 
		~Player(); 

		void Awake();
		void Start();
		void Update();
	};
}