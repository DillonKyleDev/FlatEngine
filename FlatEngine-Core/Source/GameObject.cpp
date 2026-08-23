#include "GameObject.h"
#include "components/Animation.h"
#include "components/Audio.h"
#include "components/Body2D.h"
#include "components/Button.h"
#include "components/Camera.h"
#include "components/Canvas.h"
#include "components/CharacterController.h"
#include "components/Component.h"
#include "components/Light.h"
#include "components/Mesh.h"
#include "components/Script.h"
#include "components/Sprite.h"
#include "components/Text.h"
#include "components/TileMap.h"
#include "components/Transform.h"
#include "managers/SceneManager.h"
#include "physics/Joint2D.h"
#include "tools/JsonHelper.h"
#include <math_functions.h>


namespace FlatEngine
{
	namespace SceneManager { class Scene; }
	
	GameObject::GameObject(long newParentID, long myID)
	{
		m_ID = myID != -1 ? myID : SceneManager::loadedScene.GetNextGameObjectID();
		m_parentID = newParentID;
		m_b_isPrefab = false;
		m_b_isPrefabChild = false;
		m_prefabName = "";
		m_tagList = TagList(m_ID);
		m_name = "GameObject";	
		m_b_isActive = true;
		hierarchyPosition = -1;
		parentedHierarchyPosition = -1;
		b_collapsed = false;
	}

	template<> Body2D*      		GameObject::Get<Body2D>()      		   { return SceneManager::loadedScene.Get<Body2D>(m_ID); }	
	template<> Button*     			GameObject::Get<Button>() 	 		   { return SceneManager::loadedScene.Get<Button>(m_ID); }
	template<> Transform*    		GameObject::Get<Transform>() 	 	   { return SceneManager::loadedScene.Get<Transform>(m_ID); }

	json GameObject::GetData(bool b_IDOverride)
	{
		json componentsArray = json::array();
		for (int i = 1; i < FL::ComponentType_Size; i++)
		{
			FL::Component* component = GetComponent((FL::ComponentType)i);
			if (component != nullptr)
			{               
				componentsArray.push_back(component->GetData(b_IDOverride));
			}
		}
		
		json childrenArray = json::array();
		if (!b_IDOverride)
		{
			for (int c = 0; c < m_childrenIDs.size(); c++)
			{
				childrenArray.push_back(m_childrenIDs[c]);
			}	
		}
		
		std::string objectName = GetName();
		Vector3 spawnLocation = GetPrefabSpawnLocation();
		if (Get<Transform>())
		{
			spawnLocation = Get<Transform>()->GetPosition();
		}
		
		json gameObjectJson = json::object({
			{ "b_isPrefab", IsPrefab() },
			{ "prefabName", GetPrefabName() },
			{ "spawnLocationX", spawnLocation.x },
			{ "spawnLocationY", spawnLocation.y },
			{ "spawnLocationZ", spawnLocation.z },
			{ "name", objectName },
			{ "id", b_IDOverride ? -1 : GetID() },
			{ "b_isActive", IsActive() },
			{ "b_collapsed", b_collapsed },
			{ "hierarchyPosition", hierarchyPosition },
			{ "parentedHierarchyPosition", parentedHierarchyPosition },
			{ "parent", b_IDOverride ? -1 : GetParentID() },
			{ "children", childrenArray },
			{ "components", componentsArray },
			{ "tags", m_tagList.GetData() }		
		});

		return gameObjectJson;
	}

	void GameObject::PutData(json objectJson)
	{
		if (objectJson.empty())
			return;
		
		m_name = JsonHelper::CheckJsonString(objectJson, "name", "Name");
		SetActive(JsonHelper::CheckJsonBool(objectJson, "b_isActive", m_name));
		SetIsPrefab(JsonHelper::CheckJsonBool(objectJson, "b_isPrefab", m_name));
		b_collapsed = JsonHelper::CheckJsonBool(objectJson, "b_collapsed", m_name);
		// if (JsonHelper::CheckJsonLong(objectJson, "id", m_name) != -1) SetID(JsonHelper::CheckJsonLong(objectJson, "id", m_name));
		// if (JsonHelper::CheckJsonLong(objectJson, "parent", m_name) != -1) SetParentID(JsonHelper::CheckJsonLong(objectJson, "parent", m_name));	
		if (JsonHelper::CheckJsonLong(objectJson, "hierarchyPosition", m_name) != -1) hierarchyPosition = JsonHelper::CheckJsonLong(objectJson, "hierarchyPosition", m_name);
		if (JsonHelper::CheckJsonLong(objectJson, "parentedHierarchyPosition", m_name) != -1) parentedHierarchyPosition = JsonHelper::CheckJsonLong(objectJson, "parentedHierarchyPosition", m_name);
		SetPrefabName(JsonHelper::CheckJsonString(objectJson, "prefabName", m_name));
		Vector3 spawnLocation = Vector3(JsonHelper::CheckJsonFloat(objectJson, "spawnLocationX", m_name), JsonHelper::CheckJsonFloat(objectJson, "spawnLocationY", m_name), JsonHelper::CheckJsonFloat(objectJson, "spawnLocationZ", m_name));
		m_tagList = TagList(GetID());				

		if (JsonHelper::JsonContains(objectJson, "tags", m_name))
			m_tagList.PutData(objectJson.at("tags"), m_name);	
		
		if (objectJson.contains("components"))
		{
			for (int j = 0; j < objectJson.at("components").size(); j++)
			{
				json componentJson = objectJson.at("components").at(j);								 
				ComponentType type = GetTypeFromString<ComponentType>(ComponentTypeFromString, JsonHelper::CheckJsonString(componentJson, "componentType", m_name));
				Component* component = AddComponent(type, componentJson);   						
			}
		}

		if (JsonHelper::JsonContains(objectJson, "children", m_name))
		{
			for (int c = 0; c < objectJson["children"].size(); c++)
			{
				m_childrenIDs.push_back(objectJson["children"][c]);
				m_sortedChildrenIDs.push_back(objectJson["children"][c]);
			}
		}              	
	}


	void GameObject::SetIsPrefab(bool b_isPrefab)
	{
		m_b_isPrefab = b_isPrefab;
	}

	bool GameObject::IsPrefab()
	{
		return m_b_isPrefab;
	}
	
	void GameObject::SetIsPrefabChild(bool b_isPrefabChild)
	{
		m_b_isPrefabChild = b_isPrefabChild;
	}

	bool GameObject::IsPrefabChild()
	{
		bool b_parentIsPrefab = false;

		if (GetParentID() != -1)
		{
			b_parentIsPrefab = GetParent()->m_b_isPrefab || GetParent()->IsPrefabChild();
		}
		if (b_parentIsPrefab && !m_b_isPrefabChild)
			Logger::log.Warn("GameObject \"{}\" was not saved because it is the child of a Prefab and will not be loaded at next Scene load.\n    (Unparent it or save the Prefab itself to save the GameObject.)", m_name);

		return m_b_isPrefabChild || b_parentIsPrefab;		
	}

	void GameObject::SetPrefabName(std::string prefabName)
	{
		m_prefabName = prefabName;
	}

	std::string GameObject::GetPrefabName()
	{
		return m_prefabName;
	}

	void GameObject::SetPrefabSpawnLocation(Vector3 spawnLocation)
	{
		m_prefabSpawnLocation = spawnLocation;
	}

	Vector3 GameObject::GetPrefabSpawnLocation()
	{
		return m_prefabSpawnLocation;
	}

	void GameObject::SetID(long newID)
	{
		m_ID = newID;
	}

	long GameObject::GetID()
	{
		return m_ID;
	}

	void GameObject::SetName(std::string name)
	{
		m_name = name;
	}

	std::string GameObject::GetName()
	{
		return m_name;
	}

	TagList& GameObject::GetTagList()
	{
		return m_tagList;
	}

	void GameObject::SetTagList(TagList tagList)
	{
		m_tagList = tagList;
	}

	bool GameObject::HasTag(std::string tagName)
	{
		return m_tagList.HasTag(tagName);
	}

	void GameObject::SetTag(std::string tagName, bool b_hasTag)
	{
		m_tagList.SetTag(tagName, b_hasTag);
	}

	void GameObject::SetCollides(std::string tagName, bool b_hasTag)
	{
		m_tagList.SetCollides(tagName, b_hasTag);
	}
	
	template<> Animation*           GameObject::Get<Animation>()  		   { return SceneManager::loadedScene.Get<Animation>(m_ID); }
	template<> Audio*      			GameObject::Get<Audio>()      		   { return SceneManager::loadedScene.Get<Audio>(m_ID); }
	template<> Body*      			GameObject::Get<Body>()      		   { return SceneManager::loadedScene.Get<Body>(m_ID); }	
	template<> Camera*     			GameObject::Get<Camera>() 			   { return SceneManager::loadedScene.Get<Camera>(m_ID); }
	template<> Canvas*     			GameObject::Get<Canvas>() 	 		   { return SceneManager::loadedScene.Get<Canvas>(m_ID); }
	template<> CharacterController* GameObject::Get<CharacterController>() { return SceneManager::loadedScene.Get<CharacterController>(m_ID); }	
	template<> Light*      			GameObject::Get<Light>()      		   { return SceneManager::loadedScene.Get<Light>(m_ID); }
	template<> Mesh*       			GameObject::Get<Mesh>() 			   { return SceneManager::loadedScene.Get<Mesh>(m_ID); }
	template<> Script*     			GameObject::Get<Script>()    		   { return SceneManager::loadedScene.Get<Script>(m_ID); }
	template<> Sprite*     			GameObject::Get<Sprite>()    		   { return SceneManager::loadedScene.Get<Sprite>(m_ID); }
	template<> Text*       			GameObject::Get<Text>() 			   { return SceneManager::loadedScene.Get<Text>(m_ID); }
	template<> TileMap*    			GameObject::Get<TileMap>() 	 		   { return SceneManager::loadedScene.Get<TileMap>(m_ID); }
	// Body2D, Button and Transform above
	
	template<> Animation*           GameObject::Add<Animation>(json componentJson)  		   { Animation* component = SceneManager::loadedScene.Add<Animation>(m_ID); component->PutData(componentJson, m_name); return component; }
	template<> Audio*      			GameObject::Add<Audio>(json componentJson)      		   { Audio* component = SceneManager::loadedScene.Add<Audio>(m_ID);         component->PutData(componentJson, m_name); return component; }
	template<> Body*      			GameObject::Add<Body>(json componentJson)      		   	   { Body* component = SceneManager::loadedScene.Add<Body>(m_ID);           component->PutData(componentJson, m_name); return component; }
	template<> Body2D*      		GameObject::Add<Body2D>(json componentJson)      		   { Body2D* component = SceneManager::loadedScene.Add<Body2D>(m_ID);       component->PutData(componentJson, m_name); return component; }
	template<> Button*     			GameObject::Add<Button>(json componentJson) 	 		   { Button* component = SceneManager::loadedScene.Add<Button>(m_ID);       component->PutData(componentJson, m_name); return component; }
	template<> Camera*     			GameObject::Add<Camera>(json componentJson) 			   { Camera* component = SceneManager::loadedScene.Add<Camera>(m_ID);       component->PutData(componentJson, m_name); return component; }
	template<> Canvas*     			GameObject::Add<Canvas>(json componentJson) 	 		   { Canvas* component = SceneManager::loadedScene.Add<Canvas>(m_ID);       component->PutData(componentJson, m_name); return component; }
	template<> CharacterController* GameObject::Add<CharacterController>(json componentJson)   { CharacterController* component = SceneManager::loadedScene.Add<CharacterController>(m_ID); component->PutData(componentJson, m_name); return component; }	
	template<> Light*      			GameObject::Add<Light>(json componentJson)      		   { Light* component = SceneManager::loadedScene.Add<Light>(m_ID);         component->PutData(componentJson, m_name); return component; }
	template<> Mesh*       			GameObject::Add<Mesh>(json componentJson) 			   	   { Mesh* component = SceneManager::loadedScene.Add<Mesh>(m_ID);           component->PutData(componentJson, m_name); return component; }
	template<> Script*    			GameObject::Add<Script>(json componentJson) 	 		   { Script* component = SceneManager::loadedScene.Add<Script>(m_ID);       component->PutData(componentJson, m_name); return component; }
	template<> Sprite*     			GameObject::Add<Sprite>(json componentJson)    		       { Sprite* component = SceneManager::loadedScene.Add<Sprite>(m_ID);       component->PutData(componentJson, m_name); return component; }
	template<> Text*       			GameObject::Add<Text>(json componentJson) 			       { Text* component = SceneManager::loadedScene.Add<Text>(m_ID);           component->PutData(componentJson, m_name); return component; }
	template<> TileMap*    			GameObject::Add<TileMap>(json componentJson) 	 		   { TileMap* component = SceneManager::loadedScene.Add<TileMap>(m_ID);     component->PutData(componentJson, m_name); return component; }
	template<> Transform*           GameObject::Add<Transform>(json componentJson)		       { Transform* component = SceneManager::loadedScene.Add<Transform>(m_ID); component->PutData(componentJson, m_name); return component; }

	Component* GameObject::AddComponent(ComponentType type, json componentJson)
	{
		switch (type)
		{
			case ComponentType_Animation:  			return Add<Animation>(componentJson);
			case ComponentType_Audio:      			return Add<Audio>(componentJson);
			case ComponentType_Body:       			return Add<Body>(componentJson);
			case ComponentType_Body2D:       		return Add<Body2D>(componentJson);
			case ComponentType_Button:     			return Add<Button>(componentJson);
			case ComponentType_Camera:     			return Add<Camera>(componentJson);
			case ComponentType_Canvas:     			return Add<Canvas>(componentJson);
			case ComponentType_CharacterController: return Add<CharacterController>(componentJson);			
			case ComponentType_Light:      			return Add<Light>(componentJson);
			case ComponentType_Mesh:       			return Add<Mesh>(componentJson);
			case ComponentType_Script:     			return Add<Script>(componentJson);
			case ComponentType_Sprite:     			return Add<Sprite>(componentJson);
			case ComponentType_Text:       			return Add<Text>(componentJson);
			case ComponentType_TileMap:    			return Add<TileMap>(componentJson);
			case ComponentType_Transform:  			return Add<Transform>(componentJson);
			default:                       			return nullptr;
		}
	}

	template<> void GameObject::Remove<Animation>(long ownerID)  		   { return SceneManager::loadedScene.Remove<Animation>(ownerID); }
	template<> void GameObject::Remove<Audio>(long ownerID)      		   { return SceneManager::loadedScene.Remove<Audio>(ownerID); }
	template<> void GameObject::Remove<Body>(long ownerID)      		   { return SceneManager::loadedScene.Remove<Body>(ownerID); }
	template<> void GameObject::Remove<Body2D>(long ownerID)      		   { return SceneManager::loadedScene.Remove<Body2D>(ownerID); }
	template<> void GameObject::Remove<Button>(long ownerID) 	 		   { return SceneManager::loadedScene.Remove<Button>(ownerID); }
	template<> void GameObject::Remove<Camera>(long ownerID) 			   { return SceneManager::loadedScene.Remove<Camera>(ownerID); }
	template<> void GameObject::Remove<Canvas>(long ownerID) 	 		   { return SceneManager::loadedScene.Remove<Canvas>(ownerID); }
	template<> void GameObject::Remove<CharacterController>(long ownerID)  { return SceneManager::loadedScene.Remove<CharacterController>(ownerID); }	
	template<> void GameObject::Remove<Light>(long ownerID)      		   { return SceneManager::loadedScene.Remove<Light>(ownerID); }
	template<> void GameObject::Remove<Mesh>(long ownerID) 			       { return SceneManager::loadedScene.Remove<Mesh>(ownerID); }
	template<> void GameObject::Remove<Script>(long ownerID)    		   { return SceneManager::loadedScene.Remove<Script>(ownerID); }
	template<> void GameObject::Remove<Sprite>(long ownerID)    		   { return SceneManager::loadedScene.Remove<Sprite>(ownerID); }
	template<> void GameObject::Remove<Text>(long ownerID) 			       { return SceneManager::loadedScene.Remove<Text>(ownerID); }
	template<> void GameObject::Remove<TileMap>(long ownerID) 	 		   { return SceneManager::loadedScene.Remove<TileMap>(ownerID); }
	template<> void GameObject::Remove<Transform>(long ownerID) 	 	   { return SceneManager::loadedScene.Remove<Transform>(ownerID); }

	void GameObject::RemoveComponent(Component* component)
	{		
		SceneManager::loadedScene.RemoveComponent(component);			
	}

	Component* GameObject::GetComponent(ComponentType type)
	{
		switch (type)
		{
			case ComponentType_Animation:  			return Get<Animation>();
			case ComponentType_Audio:      			return Get<Audio>();
			case ComponentType_Body:       			return Get<Body>();
			case ComponentType_Body2D:       		return Get<Body2D>();
			case ComponentType_Button:     			return Get<Button>();
			case ComponentType_Camera:     			return Get<Camera>();
			case ComponentType_Canvas:     			return Get<Canvas>();
			case ComponentType_CharacterController: return Get<CharacterController>();			
			case ComponentType_Light:      			return Get<Light>();
			case ComponentType_Mesh:       			return Get<Mesh>();
			case ComponentType_Script:     			return Get<Script>();
			case ComponentType_Sprite:     			return Get<Sprite>();
			case ComponentType_Text:       			return Get<Text>();
			case ComponentType_TileMap:    			return Get<TileMap>();
			case ComponentType_Transform:  			return Get<Transform>();
			default:                       			return nullptr;
		}
	}

	// This GameObject either has a Canvas Component or is a child of one that does.
	bool GameObject::IsCanvasChild()
	{
		return GetParent() != nullptr && (GetParent()->Get<Canvas>() != nullptr || GetParent()->IsCanvasChild());
	}

	Canvas* GameObject::GetFirstCanvas()
	{
		Canvas* canvas = Get<Canvas>();

		if (canvas == nullptr && GetParent() != nullptr)
		{
			canvas = GetParent()->GetFirstCanvas();
		}

		return canvas;
	}

	void GameObject::SetParentID(long newParentID)
	{
		m_parentID = newParentID;
	}

	long GameObject::GetParentID()
	{
		return m_parentID;
	}

	bool ChildOrderCompare(long childAID, long childBID)
	{
		bool b_aFirst = true;

		GameObject* objectA = SceneManager::loadedScene.GetObjectByID(childAID);
		GameObject* objectB = SceneManager::loadedScene.GetObjectByID(childBID);

		if (objectA != nullptr && objectB != nullptr)
		{
			b_aFirst = objectA->parentedHierarchyPosition < objectB->parentedHierarchyPosition;
		}

		return b_aFirst;
	}

	void GameObject::SortChildren()
	{
		std::sort(m_sortedChildrenIDs.begin(), m_sortedChildrenIDs.end(), ChildOrderCompare);
	}

	bool GameObject::HasChild(GameObject* child)
	{		
		for (long ID : m_childrenIDs)
		{
			if (ID == child->GetID())
			{
				return true;
			}
		}

		return false;
	}

	void GameObject::ReduceParentHierarchyPositions()
	{
		if (m_sortedChildrenIDs.size())
		{
			GameObject* lastChild = SceneManager::loadedScene.GetObjectByID(m_sortedChildrenIDs.back());
			
			if (lastChild != nullptr && lastChild->parentedHierarchyPosition > m_sortedChildrenIDs.size() + 10)
			{
				int newParentHierarchyPos = 0;
				for (long ID : m_sortedChildrenIDs)
				{
					GameObject* child = SceneManager::loadedScene.GetObjectByID(ID);	
					
					if (child != nullptr)
					{
						child->parentedHierarchyPosition = newParentHierarchyPos;
						newParentHierarchyPos++;
					}
				}
			}
		}
	}

	void GameObject::AddChild(GameObject* child)
	{
		if (child == nullptr || HasChild(child))
			return;

		m_childrenIDs.push_back(child->GetID());

		if (child->parentedHierarchyPosition == -1)
		{
			if (m_sortedChildrenIDs.size() && SceneManager::loadedScene.GetObjectByID(m_sortedChildrenIDs.back()) != nullptr)
				child->parentedHierarchyPosition = SceneManager::loadedScene.GetObjectByID(m_sortedChildrenIDs.back())->parentedHierarchyPosition;
			
			child->parentedHierarchyPosition++;
			m_sortedChildrenIDs.push_back(child->GetID());
			
			return;
		}

		bool b_inserted = false;
		for (std::vector<long>::iterator iter = m_sortedChildrenIDs.begin(); iter != m_sortedChildrenIDs.end(); iter++)
		{								
			GameObject* existingChild = SceneManager::loadedScene.GetObjectByID(*iter);
						
			if (existingChild != nullptr)
			{
				if (b_inserted)
					existingChild->parentedHierarchyPosition++;	

				if (existingChild->parentedHierarchyPosition >= child->parentedHierarchyPosition && !b_inserted)
				{				
					iter = m_sortedChildrenIDs.insert(iter, child->GetID());
					b_inserted = true;	

					if (child->parentedHierarchyPosition != existingChild->parentedHierarchyPosition)
					 	return; // only keep going if we needed to increment the remaining positions. ie inserted == existing
				}												
			}
		}			

		if (!b_inserted)
			m_sortedChildrenIDs.insert(m_sortedChildrenIDs.begin(), child->GetID());
	}

	void GameObject::RemoveChild(long childID)
	{
		for (auto iter = m_childrenIDs.begin(); iter != m_childrenIDs.end(); iter++)
		{
			if ((*iter) == childID)
			{
				m_childrenIDs.erase(iter);
				break;
			}
		}
		for (auto iter = m_sortedChildrenIDs.begin(); iter != m_sortedChildrenIDs.end(); iter++)
		{
			if ((*iter) == childID)
			{
				m_sortedChildrenIDs.erase(iter);
				return;
			}
		}
	}

	GameObject *GameObject::GetFirstChild()
	{
		return SceneManager::loadedScene.GetObjectByID(m_childrenIDs[0]);
	}

	GameObject *GameObject::FindChildByName(std::string name)
	{
		for (int i = 0; i < m_childrenIDs.size(); i++)
		{
			if (SceneManager::loadedScene.GetObjectByID(m_childrenIDs[i]) != nullptr && SceneManager::loadedScene.GetObjectByID(m_childrenIDs[i])->GetName() == name)
			{
				return SceneManager::loadedScene.GetObjectByID(m_childrenIDs[i]);
			}
		}

		return nullptr;
	}

	std::vector<long> GameObject::GetChildren()
	{
		return m_sortedChildrenIDs;
	}

	bool GameObject::HasChildren()
	{
		return m_childrenIDs.size() > 0;
	}

	void GameObject::SetActive(bool b_active)
	{
		m_b_isActive = b_active;

		for (long child : GetChildren())
		{
			if (SceneManager::loadedScene.GetObjectByID(child) != nullptr)
			{
				SceneManager::loadedScene.GetObjectByID(child)->SetActive(b_active);
			}
		}
	}

	bool GameObject::IsActive()
	{
		return m_b_isActive;
	}

	GameObject *GameObject::GetParent()
	{
		return SceneManager::loadedScene.GetObjectByID(m_parentID);
	}
}