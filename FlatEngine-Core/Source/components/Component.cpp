#include "components/Component.h"
#include "managers/SceneManager.h"


namespace FlatEngine
{
	Component::Component()
	{
		m_type = ComponentType_None;
		m_ID = -1;
		m_parentObjectID = -1;
		m_b_isCollapsed = false;
		m_b_isActive = true;		
	}

	void Component::SetParentObjectID(long newParentObjectID)
	{
		m_parentObjectID = newParentObjectID;
	}

	long Component::GetParentObjectID()
	{
		return m_parentObjectID;
	}

	GameObject* Component::GetParentObject()
	{
		return SceneManager::loadedScene.GetObjectByID(m_parentObjectID);
	}

	void Component::SetType(ComponentType componentType)
	{
		m_type = componentType;
	}

	void Component::SetID(long newID)
	{
		m_ID = newID;
	}

	long Component::GetID()
	{
		return m_ID;
	}

	ComponentType Component::GetType()
	{
		return m_type;
	}

	bool Component::IsCollapsed()
	{
		return m_b_isCollapsed;
	}

	std::string Component::GetTypeString()
	{
		return ComponentTypeStrings[m_type];
	}

	void Component::SetCollapsed(bool b_isCollapsed)
	{
		m_b_isCollapsed = b_isCollapsed;
	}

	void Component::SetActive(bool b_isActive)
	{
		m_b_isActive = b_isActive;
	}

	bool Component::IsActive()
	{
		return m_b_isActive;
	}
}