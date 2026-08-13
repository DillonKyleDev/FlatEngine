#include "components/Script.h"
#include "tools/JsonHelper.h"
#include "tools/Logger.h"
#include <string>


namespace FlatEngine
{
	Script::Script(long ownerID)
	{
		SetType(ComponentType_Script);
		SetOwnerID(ownerID);
	}

	Script::~Script()
	{
		// TODO: Remove Lua script from F_Lua state	
	}

	json Script::GetData(bool b_IDOverride)
	{
		json componentJson = Component::GetData(b_IDOverride);

		json scripts = json::array();

		for (ScriptData scriptData : m_scripts)
		{			
			json parameters = json::array();

			for (std::pair<std::string, LuaManager::LuaParameter> paramPair : scriptData.scriptParamContainer.parameters)
			{				
				parameters.push_back(paramPair.second.GetData());
			}

			json scriptDataJson = { 
				{ "name", scriptData.name },
				{ "parameters", parameters }
			};			

			scripts.push_back(scriptDataJson);
		}

		componentJson.push_back({ "scripts", scripts });

		return componentJson;
	}

	void Script::PutData(json componentJson, std::string objectName)
	{		
		if (componentJson.empty())		
			return;	
		
        Component::PutData(componentJson, objectName);
		
		if (!componentJson.contains("scripts"))
			return;

		for (auto scriptData : componentJson.at("scripts"))
		{			
			ScriptData script = ScriptData();
			script.name = JsonHelper::CheckJsonString(scriptData, "name", objectName);
			json scriptParamsJson = JsonHelper::JsonContains(scriptData, "parameters", objectName) ? scriptData.at("parameters") : json::array();

			for (int i = 0; i < scriptParamsJson.size(); i++)
			{
				try
				{
					json param = scriptParamsJson.at(i);
					LuaManager::LuaParameter parameter;
					parameter.PutData(param, objectName);
					script.scriptParamContainer.Add(parameter);
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