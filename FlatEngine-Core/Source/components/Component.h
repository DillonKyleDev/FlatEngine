#pragma once

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
		virtual std::string GetData() { return "{}"; };
		virtual void PutData() {};

		void SetType(ComponentType type);
		ComponentType GetType();
		std::string GetTypeString();
		void SetID(long ID);
		long GetID();
		void SetParentObjectID(long ID);
		long GetParentObjectID();
		GameObject* GetParentObject();
		bool IsCollapsed();
		void SetCollapsed(bool collapsed);
		void SetActive(bool _active);
		bool IsActive();

	private:
		ComponentType m_type = ComponentType_None;
		long m_ID;
		long m_parentObjectID;
		bool m_b_isCollapsed;
		bool m_b_isActive;
	};
}