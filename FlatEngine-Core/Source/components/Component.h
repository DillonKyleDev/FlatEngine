#pragma once
#include "tools/JsonHelper.h"

#include <string>
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
		ComponentType_JointMaker,
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
		"JointMaker",
		"Light",
		"Mesh",
		"Script",
		"Sprite",
		"Text",
		"TileMap"
	};

	class GameObject;

	class Component
	{
	public:
		Component();		
		virtual json GetData(bool b_IDOverride = false) { return "{}"; };
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