// Hookup file for CPP scripts...
// DO NOT CHANGE ANYTHING IN THIS FILE.
// New C++ script classes are programmatically added to the end of this file in the InstantiateCPPScript method
#include "FlatEngine.h"
#include "CPPScriptHeaders.h"
#include "CPPScript.h"

#include <memory>


std::shared_ptr<FlatEngine::CPPScript> FlatEngine::InstantiateCPPScript(std::string scriptName)
{
	std::shared_ptr<CPPScript> newScript = nullptr;

	if (scriptName == "EnemyAI (C++)")
	{
		newScript = std::make_shared<FlatEngine::EnemyAI>();
		newScript->SetName("EnemyAI (C++)");
	}
	return newScript; 
 }