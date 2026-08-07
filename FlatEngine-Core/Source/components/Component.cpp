#include "components/Component.h"
#include "managers/SceneManager.h"


namespace FlatEngine
{
	Component::Component()
	{
		m_type = ComponentType_None;
		m_ownerID = -1;
		m_b_isCollapsed = false;
		m_b_isActive = true;		
	}

	void Component::SetOwnerID(long ownerID)
	{
		m_ownerID = ownerID;
	}

	long Component::GetOwnerID()
	{
		return m_ownerID;
	}

	GameObject* Component::GetOwningObject()
	{
		return SceneManager::loadedScene.GetObjectByID(m_ownerID);
	}

	void Component::SetType(ComponentType componentType)
	{
		m_type = componentType;
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