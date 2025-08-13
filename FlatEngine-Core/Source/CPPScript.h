#pragma once
#include "GameObject.h"
#include "Script.h"

#include <string>


namespace FlatEngine
{
	class CPPScript
	{
	public:
		CPPScript() {};
		~CPPScript() {};

		void SetName(std::string name);
		GameObject* GetParent();
		long GetParentID();
		void SetParentID(long parentID);
		Script::S_ScriptParam GetScriptParam(std::string paramName);

		virtual void Awake() {};
		virtual void Start() {};
		virtual void Update() {};

	private:
		std::string m_name;
		long m_parentID = -1;
	};
}
