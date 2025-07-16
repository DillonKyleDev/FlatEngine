#pragma once
#include "Component.h"
#include "Animation.h"
#include "CPPScript.h"

#include <string>


namespace FlatEngine
{
	class Script : public Component
	{
	public:
		Script(long myID = -1, long parentID = -1);
		~Script();
		std::string GetData();

		void SetAttachedScript(std::string script);
		std::string GetAttachedScript();
		std::map<std::string, Animation::S_EventFunctionParam> &GetScriptParams();
		void SetScriptParams(std::map<std::string, Animation::S_EventFunctionParam> scriptParams);
		Animation::S_EventFunctionParam GetParam(std::string paramName);
		void AddScriptParam(std::string paramName, Animation::S_EventFunctionParam parameter);
		void DeleteScriptParam(std::string paramName);
		std::shared_ptr<CPPScript> GetCPPScript();
		void RunAwakeAndStart();

	private:
		std::string m_attachedScript;
		std::map<std::string, Animation::S_EventFunctionParam> m_scriptParams;
		std::shared_ptr<CPPScript> m_CPPScript;
	};
}
