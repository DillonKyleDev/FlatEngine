#pragma once
#include "FlatEngine.h"
#include "CPPScript.h"
#include "MappingContext.h"


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
		MappingContext* m_context;
		Body* m_body;
	};
}