#pragma once
#include "FlatEngine.h"
#include "CPPScript.h"

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

	private:
		int m_initialXPos = 0;
	};
}