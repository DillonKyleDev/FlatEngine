#pragma once
#include "scripting/CPPScript.h"

#include <memory>
#include <string>


namespace FlatEngine
{
    class CPPScript;
    
    extern std::vector<std::string> cppScriptNames;
	extern bool b_shouldReloadProjectFiles;	

    extern void RetrieveCPPScriptNames();
	extern void CreateNewCPPScript(std::string fileName, std::string path = "");
	extern std::shared_ptr<CPPScript> InstantiateCPPScript(std::string scriptName);
}