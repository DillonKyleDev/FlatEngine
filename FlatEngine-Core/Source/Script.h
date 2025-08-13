#pragma once
#include "Component.h"
#include "Animation.h"

#include <string>
#include <memory>
#include <map>


namespace FlatEngine
{

	class CPPScript;

	class Script : public Component
	{
	public:

		struct S_ScriptParam {
			std::string type = "empty"; // Can be "string", "int", "float", "double", "long", "bool", "Vector2"
			int e_int = 0;
			long e_long = 0;
			float e_float = 0.0f;
			double e_double = 0;
			bool e_boolean = false;
			Vector2 e_Vector2 = Vector2(0, 0);
			std::string e_string = "";
			void SetType(std::string newType) { type = newType; };
			void SetString(std::string newString) { e_string = newString; };
			void SetInt(int newInt) { e_int = newInt; };
			void SetLong(long newLong) { e_long = newLong; };
			void SetFloat(float newFloat) { e_float = newFloat; };
			void SetDouble(double newDouble) { e_double = newDouble; };
			void SetBool(bool newBoolean) { e_boolean = newBoolean; };
			void SetVector2(Vector2 newVector2) { e_Vector2 = newVector2; };
			std::string GetType() { return type; };
			std::string GetString() { return e_string; };
			int GetInt() { return e_int; };
			long GetLong() { return e_long; };
			float GetFloat() { return e_float; };
			double GetDouble() { return e_double; };
			bool GetBool() { return e_boolean; };
			Vector2 GetVector2() { return e_Vector2; };
		};

		static S_ScriptParam GetScriptParam(std::string paramName, long ID, std::string scriptName);

		Script(long myID = -1, long parentID = -1);
		~Script();
		std::string GetData();

		void SetAttachedScript(std::string script);
		std::string GetAttachedScript();
		std::map<std::string, Script::S_ScriptParam> &GetScriptParams();
		void SetScriptParams(std::map<std::string, Script::S_ScriptParam> scriptParams);
		Script::S_ScriptParam GetParam(std::string paramName);
		void AddScriptParam(std::string paramName, Script::S_ScriptParam parameter);
		void DeleteScriptParam(std::string paramName);
		std::shared_ptr<CPPScript> GetCPPScript();
		void RunAwakeAndStart();

	private:
		std::string m_attachedScript;
		std::map<std::string, Script::S_ScriptParam> m_scriptParams;
		std::shared_ptr<CPPScript> m_CPPScript;
	};
}
