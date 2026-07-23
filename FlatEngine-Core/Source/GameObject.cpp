#include "GameObject.h"
#include "components/Animation.h"
#include "components/Audio.h"
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


namespace FlatEngine
{
	namespace SceneManager { class Scene; }
	GameObject::GameObject(long newParentID, long myID)
	{
		if (myID == -1)
		{
			m_ID = SceneManager::loadedScene.GetNextGameObjectID();
		}
		else
		{
			m_ID = myID;
		}
		m_b_isPrefab = false;
		m_prefabName = "";
		m_b_isPrefabsChild = false;
		m_prefabSpawnLocation = Vector3();
		m_tagList = TagList(m_ID);
		m_parentID = newParentID;
		m_name = "GameObject(" + std::to_string(m_ID) + ")";		
		m_b_isActive = true;
		m_childrenIDs = std::vector<long>();
		m_hierarchyPosition = 0;
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
		m_b_isPrefabsChild = b_isPrefabChild;
	}

	bool GameObject::IsPrefabChild()
	{
		return m_b_isPrefabsChild;
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
	template<> Button*     			GameObject::Get<Button>() 	 		   { return SceneManager::loadedScene.Get<Button>(m_ID); }
	template<> Camera*     			GameObject::Get<Camera>() 			   { return SceneManager::loadedScene.Get<Camera>(m_ID); }
	template<> Canvas*     			GameObject::Get<Canvas>() 	 		   { return SceneManager::loadedScene.Get<Canvas>(m_ID); }
	template<> CharacterController* GameObject::Get<CharacterController>() { return SceneManager::loadedScene.Get<CharacterController>(m_ID); }
	template<> JointMaker* 			GameObject::Get<JointMaker>()		   { return SceneManager::loadedScene.Get<JointMaker>(m_ID); }
	template<> Light*      			GameObject::Get<Light>()      		   { return SceneManager::loadedScene.Get<Light>(m_ID); }
	template<> Mesh*       			GameObject::Get<Mesh>() 			   { return SceneManager::loadedScene.Get<Mesh>(m_ID); }
	template<> Script*     			GameObject::Get<Script>()    		   { return SceneManager::loadedScene.Get<Script>(m_ID); }
	template<> Sprite*     			GameObject::Get<Sprite>()    		   { return SceneManager::loadedScene.Get<Sprite>(m_ID); }
	template<> Text*       			GameObject::Get<Text>() 			   { return SceneManager::loadedScene.Get<Text>(m_ID); }
	template<> TileMap*    			GameObject::Get<TileMap>() 	 		   { return SceneManager::loadedScene.Get<TileMap>(m_ID); }
	template<> Transform*    		GameObject::Get<Transform>() 	 	   { return SceneManager::loadedScene.Get<Transform>(m_ID); }
	
	template<> Animation*           GameObject::Add<Animation>(long componentID, json componentJson)  		   { Animation* component = SceneManager::loadedScene.Add<Animation>(m_ID, componentID); component->PutData(componentJson); return component; }
	template<> Audio*      			GameObject::Add<Audio>(long componentID, json componentJson)      		   { Audio* component = SceneManager::loadedScene.Add<Audio>(m_ID, componentID); component->PutData(componentJson); return component; }
	template<> Body*      			GameObject::Add<Body>(long componentID, json componentJson)      		   { Body* component = SceneManager::loadedScene.Add<Body>(m_ID, componentID); component->PutData(componentJson); return component; }
	template<> Button*     			GameObject::Add<Button>(long componentID, json componentJson) 	 		   { Button* component = SceneManager::loadedScene.Add<Button>(m_ID, componentID); component->PutData(componentJson); return component; }
	template<> Camera*     			GameObject::Add<Camera>(long componentID, json componentJson) 			   { Camera* component = SceneManager::loadedScene.Add<Camera>(m_ID, componentID); component->PutData(componentJson); return component; }
	template<> Canvas*     			GameObject::Add<Canvas>(long componentID, json componentJson) 	 		   { Canvas* component = SceneManager::loadedScene.Add<Canvas>(m_ID, componentID); component->PutData(componentJson); return component; }
	template<> CharacterController* GameObject::Add<CharacterController>(long componentID, json componentJson) { CharacterController* component = SceneManager::loadedScene.Add<CharacterController>(m_ID, componentID); component->PutData(componentJson); return component; }
	template<> JointMaker* 			GameObject::Add<JointMaker>(long componentID, json componentJson)		   { JointMaker* component = SceneManager::loadedScene.Add<JointMaker>(m_ID, componentID); component->PutData(componentJson); return component; }
	template<> Light*      			GameObject::Add<Light>(long componentID, json componentJson)      		   { Light* component = SceneManager::loadedScene.Add<Light>(m_ID, componentID); component->PutData(componentJson); return component; }
	template<> Mesh*       			GameObject::Add<Mesh>(long componentID, json componentJson) 			   { Mesh* component = SceneManager::loadedScene.Add<Mesh>(m_ID, componentID); component->PutData(componentJson); return component; }
	template<> Script*    			GameObject::Add<Script>(long componentID, json componentJson) 	 		   { Script* component = SceneManager::loadedScene.Add<Script>(m_ID, componentID); component->PutData(componentJson); return component; }
	template<> Sprite*     			GameObject::Add<Sprite>(long componentID, json componentJson)    		   { Sprite* component = SceneManager::loadedScene.Add<Sprite>(m_ID, componentID); component->PutData(componentJson); return component; }
	template<> Text*       			GameObject::Add<Text>(long componentID, json componentJson) 			   { Text* component = SceneManager::loadedScene.Add<Text>(m_ID, componentID); component->PutData(componentJson); return component; }
	template<> TileMap*    			GameObject::Add<TileMap>(long componentID, json componentJson) 	 		   { TileMap* component = SceneManager::loadedScene.Add<TileMap>(m_ID, componentID); component->PutData(componentJson); return component; }
	template<> Transform*           GameObject::Add<Transform>(long componentID, json componentJson)		   { Transform* component = SceneManager::loadedScene.Add<Transform>(m_ID, componentID); component->PutData(componentJson); return component; }

	Component* GameObject::AddComponent(ComponentType type, long componentID, json componentJson)
	{
		switch (type)
		{
			case ComponentType_Animation:  			return Add<Animation>(componentID, componentJson);
			case ComponentType_Audio:      			return Add<Audio>(componentID, componentJson);
			case ComponentType_Body:       			return Add<Body>(componentID, componentJson);
			case ComponentType_Button:     			return Add<Button>(componentID, componentJson);
			case ComponentType_Camera:     			return Add<Camera>(componentID, componentJson);
			case ComponentType_Canvas:     			return Add<Canvas>(componentID, componentJson);
			case ComponentType_CharacterController: return Add<CharacterController>(componentID, componentJson);
			case ComponentType_JointMaker: 			return Add<JointMaker>(componentID, componentJson);
			case ComponentType_Light:      			return Add<Light>(componentID, componentJson);
			case ComponentType_Mesh:       			return Add<Mesh>(componentID, componentJson);
			case ComponentType_Script:     			return Add<Script>(componentID, componentJson);
			case ComponentType_Sprite:     			return Add<Sprite>(componentID, componentJson);
			case ComponentType_Text:       			return Add<Text>(componentID, componentJson);
			case ComponentType_TileMap:    			return Add<TileMap>(componentID, componentJson);
			case ComponentType_Transform:  			return Add<Transform>(componentID, componentJson);
			default:                       			return nullptr;
		}
	}

	template<> void GameObject::Remove<Animation>(long ownerID)  		   { return SceneManager::loadedScene.Remove<Animation>(ownerID); }
	template<> void GameObject::Remove<Audio>(long ownerID)      		   { return SceneManager::loadedScene.Remove<Audio>(ownerID); }
	template<> void GameObject::Remove<Body>(long ownerID)      		   { return SceneManager::loadedScene.Remove<Body>(ownerID); }
	template<> void GameObject::Remove<Button>(long ownerID) 	 		   { return SceneManager::loadedScene.Remove<Button>(ownerID); }
	template<> void GameObject::Remove<Camera>(long ownerID) 			   { return SceneManager::loadedScene.Remove<Camera>(ownerID); }
	template<> void GameObject::Remove<Canvas>(long ownerID) 	 		   { return SceneManager::loadedScene.Remove<Canvas>(ownerID); }
	template<> void GameObject::Remove<CharacterController>(long ownerID)  { return SceneManager::loadedScene.Remove<CharacterController>(ownerID); }
	template<> void GameObject::Remove<JointMaker>(long ownerID)		   { return SceneManager::loadedScene.Remove<JointMaker>(ownerID); }
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
			case ComponentType_Button:     			return Get<Button>();
			case ComponentType_Camera:     			return Get<Camera>();
			case ComponentType_Canvas:     			return Get<Canvas>();
			case ComponentType_CharacterController: return Get<CharacterController>();
			case ComponentType_JointMaker: 			return Get<JointMaker>();
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

	void GameObject::SetParentID(long newParentID)
	{
		m_parentID = newParentID;
	}

	long GameObject::GetParentID()
	{
		return m_parentID;
	}

	void GameObject::AddChild(long childID)
	{
		if (childID != -1)
		{
			bool b_contains = false;

			for (long ID : m_childrenIDs)
			{
				if (ID == childID)
				{
					b_contains = true;
				}
			}
			if (!b_contains)
			{
				m_childrenIDs.push_back(childID);
			}
		}
	}

	void GameObject::RemoveChild(long childID)
	{
		for (int i = 0; i < m_childrenIDs.size(); i++)
		{
			if (m_childrenIDs[i] == childID)
			{
				m_childrenIDs.erase(m_childrenIDs.begin() + i);
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
		return m_childrenIDs;
	}

	bool GameObject::HasChildren()
	{
		return m_childrenIDs.size() > 0;
	}

	void GameObject::SetActive(bool b_active)
	{
		m_b_isActive = b_active;

		//for (long child : GetChildren())
		//{
		//	if (SceneManager::loadedScene.GetObjectByID(child) != nullptr)
		//	{
		//		SceneManager::loadedScene.GetObjectByID(child)->SetActive(b_active);
		//	}
		//}
	}

	bool GameObject::IsActive()
	{
		return m_b_isActive;
	}

	GameObject *GameObject::GetParent()
	{
		return SceneManager::loadedScene.GetObjectByID(m_parentID);
	}

	void GameObject::SetHierarchyPosition(long position)
	{
		m_hierarchyPosition = position;
	}

	long GameObject::GetHierarchyPosition()
	{
		return m_hierarchyPosition;
	}
}