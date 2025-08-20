#pragma once
#include "Component.h"
#include "TagList.h"
#include "Vector2.h"
#include "Physics.h"

#include <string>
#include <vector>


namespace FlatEngine
{
	class Button;
	class Canvas;
	class Script;
	class Animation;
	class Sprite;
	class Camera;
	class Audio;
	class Transform;
	class Text;	
	class CharacterController;
	class CompositeCollider;
	class TileMap;
	class Body;
	class Box;
	class Circle;
	class Capsule;
	class Polygon;	
	class Chain;
	class JointMaker;

	class GameObject
	{
	public:
		
		GameObject(long parentID = -1, long myID = -1);
		~GameObject();

		void SetIsPrefab(bool b_isPrefab);
		bool IsPrefab();
		void SetIsPrefabChild(bool b_isPrefabChild);
		bool IsPrefabChild();
		void SetPrefabName(std::string newPrefabName);
		std::string GetPrefabName();
		void SetPrefabSpawnLocation(Vector2 newSpawnLocation);
		Vector2 GetPrefabSpawnLocation();
		void SetID(long ID);
		long GetID();
		void SetName(std::string name);
		std::string GetName();
		TagList &GetTagList();
		void SetTagList(TagList tagList);
		bool HasTag(std::string tagName);
		void SetTag(std::string tagName, bool b_hasTag);
		void SetCollides(std::string tagName, bool b_hasTag);
		bool HasScript(std::string scriptName);
		Script* GetScript(std::string scriptName);


		// Components
		void RemoveComponent(Component* component);
		void DeleteComponents();
		Transform* AddTransform(long ID = -1, bool b_active = true, bool b_collapsed = false);
		Sprite* AddSprite(long ID = -1, bool b_active = true, bool b_collapsed = false);
		Camera* AddCamera(long ID = -1, bool b_active = true, bool b_collapsed = false);
		Script* AddScript(long ID = -1, bool b_active = true, bool b_collapsed = false);
		Button* AddButton(long ID = -1, bool b_active = true, bool b_collapsed = false);
		Canvas* AddCanvas(long ID = -1, bool b_active = true, bool b_collapsed = false);
		Animation* AddAnimation(long ID = -1, bool b_active = true, bool b_collapsed = false);
		Audio* AddAudio(long ID = -1, bool b_active = true, bool b_collapsed = false);
		Text* AddText(long ID = -1, bool b_active = true, bool b_collapsed = false);
		Body* AddBody(Physics::BodyProps bodyProps = Physics::BodyProps(), long ID = -1, bool b_active = true, bool b_collapsed = false);
		JointMaker* AddJointMaker(long ID = -1, bool b_active = true, bool b_collapsed = false);
		CharacterController* AddCharacterController(long ID = -1, bool b_active = true, bool b_collapsed = false);
		TileMap* AddTileMap(long ID = -1, bool b_active = true, bool b_collapsed = false);

		Sprite* AddSpriteLua() { return AddSprite(-1, true, false); };
		Camera* AddCameraLua() { return AddCamera(-1, true, false); };
		Script* AddScriptLua() { return AddScript(-1, true, false); };
		Button* AddButtonLua() { return AddButton(-1, true, false); };
		Canvas* AddCanvasLua() { return AddCanvas(-1, true, false); };
		Animation* AddAnimationLua() { return AddAnimation(-1, true, false); };
		Audio* AddAudioLua() { return AddAudio(-1, true, false); };
		Text* AddTextLua() { return AddText(-1, true, false); };
		CharacterController* AddCharacterControllerLua() { return AddCharacterController(-1, true, false); };
		TileMap* AddTileMapLua() { return AddTileMap(-1, true, false); };

		Component* GetComponent(ComponentTypes type);
		bool HasComponent(ComponentTypes type);
		bool HasComponent(std::string type);
		bool HasComponentLua(std::string);
		Transform* GetTransform();
		Sprite* GetSprite();
		Camera* GetCamera();
		Animation* GetAnimation();
		Audio* GetAudio();
		Button* GetButton();
		Canvas* GetCanvas();
		std::vector<Script*> GetScripts();
		Text* GetText();
		CharacterController* GetCharacterController();
		Body* GetBody();
		JointMaker* GetJointMaker();
		TileMap* GetTileMap();
		std::vector<Component*> GetComponents();		

		void SetParentID(long parentID);
		long GetParentID();
		GameObject *GetParent();
		void AddChild(long childID);
		void RemoveChild(long childID);
		GameObject *GetFirstChild();
		GameObject *FindChildByName(std::string name);
		std::vector<long> GetChildren();
		bool HasChildren();
		void SetActive(bool b_active);
		bool IsActive();
		void SetPersistant(bool b_persistant);
		bool IsPersistant();
		void SetHierarchyPosition(long position);
		long GetHierarchyPosition();

	private:
		std::string m_name;
		bool m_b_isPrefab;
		bool m_b_isPrefabsChild;
		std::string m_prefabName;
		Vector2 m_prefabSpawnLocation;
		TagList m_tagList;
		long m_ID;
		long m_parentID;
		bool m_b_isActive;
		std::vector<Component*> m_components;
		std::vector<long> m_childrenIDs;
		bool m_b_persistant;
		long m_hierarchyPosition;
	};
}