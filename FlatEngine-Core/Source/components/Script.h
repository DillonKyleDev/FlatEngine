#pragma once
#include "components/Component.h"
#include "managers/LuaManager.h"

#include <memory>
#include <string>


namespace FlatEngine
{
	class CPPScript;

	struct ScriptData {			
        std::string name = "";      
		LuaManager::LuaParameterContainer scriptParamContainer;
		std::shared_ptr<CPPScript> cppScript;

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
		Script(long ownerID = -1);	
		~Script();	
		json GetData(bool b_IDOverride = false);
		void PutData(json componentJson, std::string objectName);
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
