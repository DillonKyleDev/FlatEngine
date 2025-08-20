#include "Component.h"
#include "FlatEngine.h"


namespace FlatEngine
{
	Component::Component()
	{
		m_type = T_Null;
		m_ID = -1;
		m_parentID = -1;
		m_b_isCollapsed = false;
		m_b_isActive = true;
	}

	Component::~Component()
	{
	}

	void Component::SetParentID(long newParentID)
	{
		m_parentID = newParentID;
	}

	long Component::GetParentID()
	{
		return m_parentID;
	}

	GameObject* Component::GetParent()
	{
		return GetObjectByID(m_parentID);
	}

	void Component::SetType(ComponentTypes componentType)
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

	ComponentTypes Component::GetType()
	{
		return m_type;
	}

	bool Component::IsCollapsed()
	{
		return m_b_isCollapsed;
	}

	std::string Component::GetTypeString()
	{
		switch (m_type)
		{
		case T_Transform:
			return "Transform";
			break;

		case T_Sprite:
			return "Sprite";
			break;

		case T_Camera:
			return "Camera";
			break;

		case T_Script:
			return "Script";
			break;

		case T_Button:
			return "Button";
			break;

		case T_Canvas:
			return "Canvas";
			break;

		case T_Animation:
			return "Animation";
			break;

		case T_Audio:
			return "Audio";
			break;

		case T_Text:
			return "Text";
			break;

		case T_CharacterController:
			return "CharacterController";
			break;

		case T_TileMap:
			return "TileMap";
			break;

		case T_Body:
			return "Body";
			break;

		case T_JointMaker:
			return "JointMaker";
			break;

		default:
			return "Null";
		}
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