#pragma once
#include "FlatEngine.h"
#include "CPPScript.h"

namespace FlatEngine
{
	class EnemyAI : public CPPScript
	{
	public:
		EnemyAI(); 
		~EnemyAI(); 

		void Awake();
		void Start();
		void Update();
	};
}