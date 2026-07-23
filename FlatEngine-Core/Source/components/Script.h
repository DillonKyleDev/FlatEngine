#pragma once
#include "components/Component.h"
#include "managers/LuaManager.h"
// #include "scripting/CPPScript.h"
#include "tools/JsonHelper.h"
#include "tools/Logger.h"

#include <memory>
#include <map>
#include <string>


namespace FlatEngine
{
	class CPPScript;

	struct ScriptData {			
        std::string name = "";  
		long parentID = -1;      
		std::map<std::string, LuaManager::LuaParameter> scriptParams;
		std::shared_ptr<CPPScript> cppScript;
		// For inspector gui rendering
		std::string tempParameterName = "";
		int tempParamType = 0;

		void SetAttachedScript(std::string script)
		{
			name = script;

			if (name.find("C++") != std::string::npos)
			{
				// cppScript = InstantiatecppScript(name);
				// cppScript->SetParentID(GetParentObjectID());
			}
			else
			{
				cppScript = nullptr;
			}
		}

		std::string GetAttachedScript()
		{
			return name;
		}

		LuaManager::LuaParameter GetScriptParameter(std::string paramName)
		{
			if (scriptParams.count(paramName))
			{
				return scriptParams.at(paramName);
			}
			return LuaManager::LuaParameter();
		}

		std::map<std::string, LuaManager::LuaParameter> &GetScriptParameters()
		{
			return scriptParams;
		}

		void SetScriptParams(std::map<std::string, LuaManager::LuaParameter> scriptParams)
		{
			scriptParams = scriptParams;
		}


		void AddScriptParam(LuaManager::LuaParameter parameter)
		{
			if (scriptParams.count(parameter.name) == 0)
			{
				scriptParams.emplace(parameter.name, parameter);
			}
			else
			{
				Logger::log.Err("Script parameter already exists with that name.");
			}
		}

		void DeleteScriptParam(std::string paramName)
		{
			if (scriptParams.count(paramName) == 0)
			{
				scriptParams.erase(paramName);
			}
			else
			{
				Logger::log.Err("No Script parameter with that name was found.");
			}
		}

		std::shared_ptr<CPPScript> GetCPPScript()
		{
			return cppScript;
		}

		// void RunAwakeAndStart()
		// {
		// 	if (cppScript != nullptr)
		// 	{
		// 		// cppScript->Awake();
		// 		// cppScript->Start();
		// 	}
		// 	else
		// 	{
		// 		LuaManager::RunLuaFuncOnSingleScript(this, "Awake");
		// 		LuaManager::RunLuaFuncOnSingleScript(this, "Start");
		// 	}
		// }
	};

	class Script : public Component
	{
	public:		
		Script(long myID = -1, long parentObjectID = -1);	
		~Script();	
		std::string GetData();
		void PutData(json componentJson);
		std::vector<ScriptData>& GetScripts();
		ScriptData* FindScript(std::string name);
		void AddScript(ScriptData scriptData = ScriptData());
		void RemoveScript(ScriptData* scriptData);
		// void SetAttachedScript(std::string script);
		// std::string GetAttachedScript();
		// LuaManager::LuaParameter GetScriptParameter(std::string paramName);
		// std::map<std::string, LuaManager::LuaParameter> &GetScriptParameters();
		// void SetScriptParams(std::map<std::string, LuaManager::LuaParameter> scriptParams);		
		// void AddScriptParam(LuaManager::LuaParameter parameter);
		// void DeleteScriptParam(std::string paramName);
		// std::shared_ptr<CPPScript> GetcppScript();
		// void RunAwakeAndStart();

	private:
		std::vector<ScriptData> m_scripts;
	};
}
