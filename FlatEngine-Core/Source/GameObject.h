#pragma once
#include "components/Component.h"
#include "TagList.h"
#include "tools/JsonHelper.h"
#include "tools/Vector3.h"

#include <string>
#include <vector>


namespace FlatEngine
{
	class Audio;	
	class Button;
	class Canvas;
	class Script;
	class Animation;
	class Sprite;
	class Camera;
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
	class Mesh;
	class Light;

	class GameObject
	{
	public:
		
		GameObject(long parentID, long myID);		

		void SetIsPrefab(bool b_isPrefab);
		bool IsPrefab();
		void SetIsPrefabChild(bool b_isPrefabChild);
		bool IsPrefabChild();
		void SetPrefabName(std::string newPrefabName);
		std::string GetPrefabName();
		void SetPrefabSpawnLocation(Vector3 newSpawnLocation);
		Vector3 GetPrefabSpawnLocation();
		void SetID(long ID);
		long GetID();
		void SetName(std::string name);
		std::string GetName();
		TagList &GetTagList();
		void SetTagList(TagList tagList);
		bool HasTag(std::string tagName);
		void SetTag(std::string tagName, bool b_hasTag);
		void SetCollides(std::string tagName, bool b_hasTag);

		// Components
		template<class T>
		T* Add(long componentID = -1, json componentJson = json::object());
		template<class T>
		T* Get();
		template<class T>
		void Remove(long ownerID);

		// Body* AddBody(PhysicsManager::BodyProps bodyProps, long componentID);
		Component* AddComponent(ComponentType type, long componentID = -1, json componentJson = json::object());
		void RemoveComponent(Component* component);		
		Component* GetComponent(ComponentType type);
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
		void SetHierarchyPosition(long position);
		long GetHierarchyPosition();
		
	private:
		std::string m_name;
		bool m_b_isPrefab;
		bool m_b_isPrefabsChild;
		std::string m_prefabName;
		Vector3 m_prefabSpawnLocation;
		TagList m_tagList;
		long m_ID;
		long m_parentID;
		bool m_b_isActive;
		std::vector<Component*> m_components;
		std::vector<long> m_childrenIDs;			
		long m_hierarchyPosition;
	};
}