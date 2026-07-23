#include "components/Script.h"
#include "tools/JsonHelper.h"
#include "tools/Logger.h"
#include <string>


namespace FlatEngine
{
	Script::Script(long myID, long parentObjectID)
	{
		SetType(ComponentType_Script);
		SetID(myID);
		SetParentObjectID(parentObjectID);
	}

	Script::~Script()
	{
		// TODO: Remove Lua script from F_Lua state	
	}

	std::string Script::GetData()
	{
		json jsonData = {
			{ "type", "Script" },
			{ "id", GetID() },
			{ "_isCollapsed", IsCollapsed() },
			{ "_isActive", IsActive() }			
		};

		json scripts = json::array();

		for (ScriptData scriptData : m_scripts)
		{			
			json parameters = json::array();

			for (std::pair<std::string, LuaManager::LuaParameter> paramPair : scriptData.GetScriptParameters())
			{				
				parameters.push_back(paramPair.second.GetData());
			}

			json scriptDataJson = { 
				{ "name", scriptData.name },
				{ "parameters", parameters }
			};			

			scripts.push_back(scriptDataJson);
		}

		jsonData.push_back({ "scripts", scripts });

		std::string data = jsonData.dump();
		// Return dumped json object with required data for saving
		return data;
	}

	void Script::PutData(json componentJson)
	{
		std::string objectName = "Script GameObject";
		std::string type = JsonHelper::CheckJsonString(componentJson, "type", objectName);
		long componentID = JsonHelper::CheckJsonLong(componentJson, "id", objectName);
		bool b_isCollapsed = JsonHelper::CheckJsonBool(componentJson, "_isCollapsed", objectName);
		bool b_isActive = JsonHelper::CheckJsonBool(componentJson, "_isActive", objectName);
		SetActive(b_isActive);
		SetCollapsed(b_isCollapsed);
		
		if (!componentJson.contains("scripts"))
			return;

		for (auto scriptData : componentJson.at("scripts"))
		{			
			ScriptData script = ScriptData();
			script.name = JsonHelper::CheckJsonString(scriptData, "name", objectName);;
			json scriptParamsJson = JsonHelper::JsonContains(scriptData, "parameters", objectName) ? scriptData.at("parameters") : json::array();

			for (int i = 0; i < scriptParamsJson.size(); i++)
			{
				try
				{
					json param = scriptParamsJson.at(i);
					LuaManager::LuaParameter parameter;
					parameter.PutData(param, objectName);
					script.AddScriptParam(parameter);
				}
				catch (const json::out_of_range& e)
				{
					Logger::log.Err("{}", e.what());
				}
			}

			m_scripts.push_back(script);
		}
    }

	std::vector<ScriptData>& Script::GetScripts()
	{
		return m_scripts;
	}

	ScriptData* Script::FindScript(std::string name)
	{
		for (ScriptData& scriptData : m_scripts)
		{
			if (scriptData.name == name)
			{
				return &scriptData;
			}
		}

		return nullptr;
	}

	void Script::AddScript(ScriptData scriptData)
	{
		m_scripts.push_back(scriptData);
	}

	void Script::RemoveScript(ScriptData* scriptData)
	{
		for (std::vector<ScriptData>::iterator iter = m_scripts.begin(); iter != m_scripts.end(); iter++)
		{
			if (scriptData->name == iter->name)
			{
				m_scripts.erase(iter);
				break;
			}			
		}
	}
}