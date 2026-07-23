#include "managers/SceneManager.h"
#include "scripting/CPPScript.h"


namespace FlatEngine
{
    void CPPScript::SetName(std::string name)
    {
        m_name = name;
    }

    GameObject* CPPScript::GetParent()
    {
        return SceneManager::loadedScene.GetObjectByID(m_parentID);
    }

    long CPPScript::GetParentID()
    {
        return m_parentID;
    }

    void CPPScript::SetParentID(long parentID)
    {
        m_parentID = parentID;
    }

    LuaManager::LuaParameter CPPScript::GetScriptParam(std::string paramName)
    {
        // return Script::GetScriptParam(paramName, m_parentID, m_name);
        return LuaManager::LuaParameter();
    }
}