#include "Script.h"
#include "FlatEngine.h"

#include "json.hpp"

using json = nlohmann::json;
using namespace nlohmann::literals;


namespace FlatEngine
{
	Script::S_ScriptParam Script::GetScriptParam(std::string paramName, long ID, std::string scriptName)
	{
		GameObject* thisObject = GetObjectByID(ID);
		Script::S_ScriptParam parameter = Script::S_ScriptParam();

		if (thisObject != nullptr)
		{
			Script* script = thisObject->GetScript(scriptName);
			if (script != nullptr)
			{
				parameter = script->GetParam(paramName);

				if (parameter.type == "empty")
				{
					LogError("No parameter with the name \"" + paramName + "\" found in " + scriptName + " Script on the " + thisObject->GetName() + " GameObject");
				}
			}
			else
			{
				LogError(thisObject->GetName() + " does not contain the Script named " + scriptName);
			}
		}
		else
		{
			LogError("GameObject with that id not found.");
		}

		return parameter;
	}


	Script::Script(long myID, long parentID)
	{
		SetType(T_Script);
		SetID(myID);
		SetParentID(parentID);
		m_attachedScript = "";
		m_CPPScript = nullptr;
	}

	Script::~Script()
	{
	}

	std::string Script::GetData()
	{
		json jsonData = {
			{ "type", "Script" },
			{ "id", GetID() },
			{ "_isCollapsed", IsCollapsed() },
			{ "_isActive", IsActive() },
			{ "attachedScript", m_attachedScript },
		};

		json parameters = json::array();

		for (std::pair<std::string, Script::S_ScriptParam> paramPair : m_scriptParams)
		{
			std::string paramName = paramPair.first;
			Script::S_ScriptParam parameter = paramPair.second;

			parameters.push_back({
				{ "paramName", paramName },
				{ "type", parameter.type },
				{ "string", parameter.e_string },
				{ "int", parameter.e_int },
				{ "float", parameter.e_float },
				{ "double", parameter.e_double },
				{ "long", parameter.e_long },
				{ "bool", parameter.e_boolean },
				{ "vector2X", parameter.e_Vector2.x },
				{ "vector2Y", parameter.e_Vector2.y },
			});
		}
		jsonData.push_back({ "scriptParameters", parameters });

		std::string data = jsonData.dump();
		// Return dumped json object with required data for saving
		return data;
	}

	void Script::SetAttachedScript(std::string script)
	{
		m_attachedScript = script;

		if (m_attachedScript.find("C++") != std::string::npos)
		{
			m_CPPScript = InstantiateCPPScript(m_attachedScript);
			m_CPPScript->SetParentID(GetParentID());
		}
		else
		{
			m_CPPScript = nullptr;
		}
	}

	std::string Script::GetAttachedScript()
	{
		return m_attachedScript;
	}

	std::map<std::string, Script::S_ScriptParam> &Script::GetScriptParams()
	{
		return m_scriptParams;
	}

	void Script::SetScriptParams(std::map<std::string, Script::S_ScriptParam> scriptParams)
	{
		m_scriptParams = scriptParams;
	}

	Script::S_ScriptParam Script::GetParam(std::string paramName)
	{
		if (m_scriptParams.count(paramName))
		{
			return m_scriptParams.at(paramName);
		}

		return Script::S_ScriptParam();
	}

	void Script::AddScriptParam(std::string paramName, Script::S_ScriptParam parameter)
	{
		if (m_scriptParams.count(paramName) == 0)
		{
			m_scriptParams.emplace(paramName, parameter);
		}
		else
		{
			LogError("Script parameter already exists with that name.");
		}
	}

	void Script::DeleteScriptParam(std::string paramName)
	{
		if (m_scriptParams.count(paramName) == 0)
		{
			m_scriptParams.erase(paramName);
		}
		else
		{
			LogError("No Script parameter with that name was found.");
		}
	}

	std::shared_ptr<CPPScript> Script::GetCPPScript()
	{
		return m_CPPScript;
	}

	void Script::RunAwakeAndStart()
	{
		if (m_CPPScript != nullptr)
		{
			m_CPPScript->Awake();
			m_CPPScript->Start();
		}
		else
		{
			RunLuaFuncOnSingleScript(this, "Awake");
			RunLuaFuncOnSingleScript(this, "Start");
		}
	}
}