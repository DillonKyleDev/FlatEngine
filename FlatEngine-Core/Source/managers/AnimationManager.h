#pragma once
#include "components/Animation.h"
#include "managers/LuaManager.h"

#include <map>
#include <string>


namespace FlatEngine
{
    class GameObject;

    namespace AnimationManager
    {
        extern std::map<std::string, void (*)(GameObject*, LuaManager::LuaParameter)> cppAnimationEventFunctions;
        
        extern void CreateNewAnimationFile(std::string fileName, std::string path = "");
        extern void SaveAnimationFile(const AnimationData& propertiesObject, std::string path);
        extern AnimationData LoadAnimationFile(std::string path);
        extern void AddCPPAnimationEventFunction(std::string functionName, void (*eventFunction)(GameObject*, LuaManager::LuaParameter));        	
    }
}