#pragma once
#include "tools/JsonHelper.h"

#include <string>
#include <unordered_map>
#include <vector>


namespace FlatEngine
{
	enum ComponentType {
		ComponentType_None,
		ComponentType_Transform,
		ComponentType_Animation,
		ComponentType_Audio,
		ComponentType_Body,
		ComponentType_Body2D,
		ComponentType_Button,
		ComponentType_Camera,
		ComponentType_Canvas,
		ComponentType_CharacterController,		
		ComponentType_Light,
		ComponentType_Mesh,
		ComponentType_Script,
		ComponentType_Sprite,
		ComponentType_Text,
		ComponentType_TileMap,
		ComponentType_Size,
	};
    const std::vector<std::string> ComponentTypeStrings =
    {
		"None",
		"Transform",
		"Animation",
		"Audio",
		"Body",
		"Body2D",
		"Button",
		"Camera",
		"Canvas",
		"CharacterController",		
		"Light",
		"Mesh",
		"Script",
		"Sprite",
		"Text",
		"TileMap"
	};
	const std::unordered_map<std::string, ComponentType> ComponentTypeFromString = {
		{ "None",                ComponentType_None },
		{ "Transform",           ComponentType_Transform },
		{ "Animation",           ComponentType_Animation },
		{ "Audio",               ComponentType_Audio },
		{ "Body",                ComponentType_Body },
		{ "Body2D",              ComponentType_Body2D },
		{ "Button",              ComponentType_Button },
		{ "Camera",              ComponentType_Camera },
		{ "Canvas",              ComponentType_Canvas },
		{ "CharacterController", ComponentType_CharacterController },
		{ "Light",               ComponentType_Light },
		{ "Mesh",                ComponentType_Mesh },
		{ "Script",              ComponentType_Script },
		{ "Sprite",              ComponentType_Sprite },
		{ "Text",                ComponentType_Text },
		{ "TileMap",             ComponentType_TileMap },
		{ "Size",                ComponentType_Size }
	};

	class GameObject;

	class Component
	{
	public:
		Component();		
		virtual json GetData(bool b_IDOverride = false) 
		{ 
			json componentJson = {
				{ "componentType", GetTypeString() },    				
				{ "b_isCollapsed", m_b_isCollapsed },
				{ "b_isActive", m_b_isActive }
			};

			return componentJson;
		};
		virtual void PutData(json componentJson, std::string objectName) 
		{
			m_b_isActive = JsonHelper::CheckJsonBool(componentJson, "b_isActive", objectName);
			m_b_isCollapsed = JsonHelper::CheckJsonBool(componentJson, "b_isCollapsed", objectName);			
		};

		void SetType(ComponentType type);
		ComponentType GetType();
		std::string GetTypeString();
		void SetOwnerID(long ID);
		long GetOwnerID();
		GameObject* GetOwningObject();
		bool IsCollapsed();
		void SetCollapsed(bool collapsed);
		void SetActive(bool b_active);
		bool IsActive();

	private:
		ComponentType m_type = ComponentType_None;		
		long m_ownerID;
		bool m_b_isCollapsed;
		bool m_b_isActive;
	};
}