#pragma once
#include <string>


namespace FlatEngine
{
	enum ComponentTypes {
		T_Null,
		T_Transform,
		T_Sprite,
		T_Camera,
		T_Script,
		T_Button,
		T_Canvas,
		T_Animation,
		T_Audio,
		T_Text,
		T_CharacterController,
		T_TileMap,
		T_Body,
		T_JointMaker
	};

	class GameObject;

	class Component
	{
	public:
		Component();
		~Component();
		virtual std::string GetData() { return "{}"; };

		void SetType(ComponentTypes type);
		ComponentTypes GetType();
		std::string GetTypeString();
		void SetID(long ID);
		long GetID();
		void SetParentID(long ID);
		long GetParentID();
		GameObject* GetParent();
		bool IsCollapsed();
		void SetCollapsed(bool collapsed);
		void SetActive(bool _active);
		bool IsActive();

	private:
		ComponentTypes m_type = T_Null;
		long m_ID;
		long m_parentID;
		bool m_b_isCollapsed;
		bool m_b_isActive;
	};
}