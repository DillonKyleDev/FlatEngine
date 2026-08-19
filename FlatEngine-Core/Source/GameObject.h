#pragma once
#include "components/Component.h"
#include "TagList.h"
#include "tools/Vector3.h"

#include <string>
#include <vector>


namespace FlatEngine
{
	class GameObject
	{
	public:
		
		GameObject(long parentID = -1, long myID = -1);		
		json GetData(bool b_IDOverride = false);
		void PutData(json objectJson);

		void SetActive(bool b_active);
		bool IsActive();
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
		T* Add(json componentJson = json::object());
		template<class T>
		T* Get();
		template<class T>
		void Remove(long ownerID);

		Component* AddComponent(ComponentType type, json componentJson = json::object());
		void RemoveComponent(Component* component);		
		Component* GetComponent(ComponentType type);		
		bool IsCanvasGameObject();

		void SetParentID(long parentID);
		long GetParentID();
		GameObject *GetParent();
		void AddChild(GameObject* child);
		void RemoveChild(long childID);
		GameObject *GetFirstChild();
		GameObject *FindChildByName(std::string name);
		std::vector<long> GetChildren();
		bool HasChildren();
		bool HasChild(GameObject* child);
		void SortChildren();
		void ReduceParentHierarchyPositions();

		long hierarchyPosition;
		long parentedHierarchyPosition;
		bool b_collapsed;

	private:
		std::string m_name;
		bool m_b_isPrefab;
		bool m_b_isPrefabChild;
		std::string m_prefabName;
		Vector3 m_prefabSpawnLocation;
		TagList m_tagList;
		long m_ID;
		long m_parentID;
		bool m_b_isActive;		
		std::vector<long> m_childrenIDs;					
		std::vector<long> m_sortedChildrenIDs;					
	};
}