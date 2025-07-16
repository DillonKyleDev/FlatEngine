#include "CPPScript.h"
#include "FlatEngine.h"

namespace FlatEngine
{
    GameObject* CPPScript::GetParent()
    {
        return GetObjectByID(m_parentID);
    }

    void CPPScript::SetParentID(long parentID)
    {
        m_parentID = parentID;
    }
}