#include "components/Animation.h"
#include "components/Component.h"
#include "components/Script.h"
#include "components/Transform.h"
#include "managers/SceneManager.h"
#include "render/VulkanManager.h"
#include "tools/Logger.h"


namespace FlatEngine
{
	Scene::Scene()
	{
		name = "New Scene";
		path = "";				 
		m_sceneObjects = std::vector<GameObject>();		
		m_IDToIndex = std::unordered_map<long, size_t>();
		m_animatorPreviewObjects = std::vector<GameObject*>();			
		m_nextGameObjectID = 0;
		m_nextComponentID = 0;
		m_freedComponentIDs = std::vector<long>();
		m_freedGameObjectIDs = std::vector<long>();
		m_primaryCamera = nullptr;

		m_Transforms = std::map<long, Transform>();
		m_Sprites = std::map<long, Sprite>();
		m_Cameras = std::map<long, Camera>();
		m_Scripts = std::map<long, Script>();
		m_LuaScriptsByOwner = std::map<long, std::vector<std::string>>();
		m_Buttons = std::map<long, Button>();
		m_Canvases = std::map<long, Canvas>();
		m_Animations = std::map<long, Animation>();
		m_Audios = std::map<long, Audio>();
		m_Texts = std::map<long, Text>();		
		m_Bodies = std::map<long, Body>();
		m_CharacterControllers = std::map<long, CharacterController>();		
		m_TileMaps = std::map<long, TileMap>();
		m_Meshes = std::map<long, Mesh>();
		m_MeshesByMaterial = std::map<std::string, std::vector<Mesh>>();
	}
	
	bool Scene::SortHierarchyObjects(GameObject* gameObjectA, GameObject* gameObjectB)
	{
		return gameObjectA->GetHierarchyPosition() < gameObjectB->GetHierarchyPosition();
	}

	void Scene::Unload()
	{
		m_sceneObjects.clear();

		m_Transforms.clear();
		m_Sprites.clear();
		m_Cameras.clear();
		m_Scripts.clear();
		m_LuaScriptsByOwner.clear();
		m_Buttons.clear();
		m_Canvases.clear();
		m_Animations.clear();
		m_Audios.clear();
		m_Texts.clear();
		m_CharacterControllers.clear();		
		m_TileMaps.clear();

		for (std::map<long, Body>::iterator iterator = m_Bodies.begin(); iterator != m_Bodies.end(); iterator++)
		{
			iterator->second.Cleanup();
		}
		m_Bodies.clear();

		for (std::map<long, JointMaker>::iterator iterator = m_JointMakers.begin(); iterator != m_JointMakers.end(); iterator++)
		{
			iterator->second.Cleanup();
		}
		m_JointMakers.clear();
	}

	GameObject* Scene::AddSceneObject(GameObject sceneObject)
	{
		long ID = sceneObject.GetID();

		if (ID == -1)
			ID = GetNextGameObjectID();
		
		if (m_IDToIndex.count(ID))
		{
			Logger::log.Err("GameObject not created, ID taken already: {}", std::to_string(ID));
			return nullptr;
		}

		sceneObject.SetID(ID);

		long parentID = sceneObject.GetParentID();

		// For objects created after initial Scene load
		if (parentID != -1 && GetObjectByID(parentID) != nullptr)
		{
			GetObjectByID(parentID)->AddChild(ID);
		}	

		sceneObject.SetHierarchyPosition((int)m_sceneObjects.size());		    

		m_IDToIndex[ID] = m_sceneObjects.size();
		m_sceneObjects.push_back(std::move(sceneObject));

		KeepNextGameObjectIDUpToDate(ID);
		SortSceneObjects();

		return &m_sceneObjects.back();
	}

	void Scene::KeepNextGameObjectIDUpToDate(long ID)
	{
		if (ID >= m_nextGameObjectID)
		{
			m_nextGameObjectID = ID + 1;
		}
	}

	std::vector<GameObject> &Scene::GetSceneObjects()
	{
		return m_sceneObjects;
	}

	void Scene::SetAnimatorPreviewObjects(std::vector<GameObject*> previewObjects)
	{
		m_animatorPreviewObjects = previewObjects;
	}

	std::vector<GameObject*> Scene::GetAnimatorPreviewObjects()
	{
		return m_animatorPreviewObjects;
	}

	GameObject* Scene::GetObjectByID(long ID)
	{
		auto iter = m_IDToIndex.find(ID);

		if (iter == m_IDToIndex.end())
		{
			return nullptr;
		}

		return &m_sceneObjects[iter->second];
	}

	GameObject* Scene::GetObjectByName(std::string name)
	{
		for (GameObject& gameObject : m_sceneObjects)
		{
			if (name == gameObject.GetName())
			{
				return &gameObject;
			}
		
		}
		return nullptr;
	}

	GameObject* Scene::GetObjectByTag(std::string tag)
	{
		for (GameObject& gameObject : m_sceneObjects)
		{
			if (gameObject.GetTagList().HasTag(tag))
			{
				return &gameObject;
			}
		}
		return nullptr;
	}

	GameObject* Scene::CreateGameObject(long parentID, long myID)
	{
		GameObject* newObjectPtr = CreateEmptyGameObject(parentID, myID);
		newObjectPtr->Add<Transform>(myID);

		return newObjectPtr;
	}

	GameObject* Scene::CreateEmptyGameObject(long parentID, long myID)
	{
		if (myID == -1)
			myID = GetNextGameObjectID();
		
		if (m_IDToIndex.count(myID))
		{
			Logger::log.Err("GameObject not created, ID taken already: {}", std::to_string(myID));
			return nullptr;
		}

		GameObject newObject = GameObject(parentID, myID);
		long ID = newObject.GetID();

		if (parentID != -1 && GetObjectByID(parentID) != nullptr)
		{
			GetObjectByID(parentID)->AddChild(ID);
		}	

		return AddSceneObject(newObject);
	}

	void Scene::DeleteGameObject(long sceneObjectID)
	{
		GameObject *objectToDelete = GetObjectByID(sceneObjectID);	

		if (objectToDelete != nullptr)
		{
			DeleteChildrenAndSelf(objectToDelete);
			SortSceneObjects();
		}
	}

	void Scene::DeleteGameObject(GameObject *objectToDelete)
	{
		if (objectToDelete != nullptr)
		{
			DeleteChildrenAndSelf(objectToDelete);
		}
	}

	void Scene::RemoveSceneObject(long ID)
	{
		auto it = m_IDToIndex.find(ID);
		if (it == m_IDToIndex.end()) return;

		size_t index     = it->second;
		size_t lastIndex = m_sceneObjects.size() - 1;

		if (index != lastIndex)
		{
			// Swap target with last element
			std::swap(m_sceneObjects[index], m_sceneObjects[lastIndex]);
			// Update the moved element's index entry
			long movedID = m_sceneObjects[index].GetID();
			m_IDToIndex[movedID] = index;
		}

		// Remove the last element (our target is now there)
		m_sceneObjects.pop_back();
		m_IDToIndex.erase(it);
	}

	// Recursive
	void Scene::DeleteChildrenAndSelf(GameObject *objectToDelete)
	{
		if (objectToDelete != nullptr)
		{
			long ID = objectToDelete->GetID();

			if (m_primaryCamera != nullptr && m_primaryCamera->GetParentObjectID() == objectToDelete->GetID())
			{
				m_primaryCamera->SetPrimaryCamera(false);
				m_primaryCamera = nullptr;
			}

			if (objectToDelete->Get<Animation>() != nullptr)
			{
				objectToDelete->Get<Animation>()->StopAll();
			}

			for (int i = 1; i < FL::ComponentType_Size; i++)
			{
				FL::Component* component = objectToDelete->GetComponent((FL::ComponentType)i);
				if (component != nullptr)
				{
					RemoveComponent(component);
				}
			}

			long parentID = objectToDelete->GetParentID();
			if (parentID != -1 && GetObjectByID(parentID) != nullptr)
			{
				GetObjectByID(parentID)->RemoveChild(ID);
			}

			if (objectToDelete->HasChildren())
			{
				std::vector<long> childrenIDs = objectToDelete->GetChildren();

				for (int i = 0; i < childrenIDs.size(); i++)
				{
					GameObject* child = GetObjectByID(childrenIDs[i]);
					if (child != nullptr)
					{
						DeleteChildrenAndSelf(child);
					}
				}
			}

			RemoveSceneObject(ID);
			m_freedGameObjectIDs.push_back(ID);
		}
	}

	void Scene::SetNextGameObjectID(long nextID)
	{
		m_nextGameObjectID = nextID;
	}

	long Scene::GetNextGameObjectID()
	{
		long ID;

		if (m_freedGameObjectIDs.size() > 0)
		{
			ID = m_freedGameObjectIDs.back();
			m_freedGameObjectIDs.pop_back();
		}
		else
		{
			ID = m_nextGameObjectID;
			m_nextGameObjectID += 1;
		}

		return ID;
	}

	void Scene::SetNextComponentID(long nextID)
	{
		m_nextComponentID = nextID;
	}

	long  Scene::GetNextComponentID()
	{
		long ID;

		if (m_freedComponentIDs.size() > 0)
		{
			ID = m_freedComponentIDs.back();
			m_freedComponentIDs.pop_back();
		}
		else
		{
			ID = m_nextComponentID;
			m_nextComponentID += 1;
		}

		return ID;
	}

	void Scene::OnPrefabInstantiated()
	{		
	}

	void Scene::SortSceneObjects()
	{
		m_sortedHierarchyObjects.clear();

		for (GameObject& gameObject : m_sceneObjects)
		{
			m_sortedHierarchyObjects.push_back(&(gameObject));
		}

		std::sort(m_sortedHierarchyObjects.begin(), m_sortedHierarchyObjects.end(), SortHierarchyObjects);
	}

	std::vector<GameObject*> Scene::GetSortedHierarchyObjects()
	{
		return m_sortedHierarchyObjects;
	}

	void Scene::CreateJoints()
	{
		for (std::map<long, JointMaker>::iterator iter = m_JointMakers.begin(); iter != m_JointMakers.end(); iter++)
		{
			for (Joint* joint : iter->second.GetJoints())
			{
				if (joint->HasValidBodies())
				{
					joint->CreateJoint();
				}
			}
		}
	}

	void Scene::KeepNextComponentIDUpToDate(long ID)
	{
		if (ID >= m_nextComponentID)
		{
			m_nextComponentID = ID + 1;
		}
	}

	template<> std::map<long, Animation>&           Scene::GetContainer<Animation>()  		   { return m_Animations; }
	template<> std::map<long, Audio>&      			Scene::GetContainer<Audio>()      		   { return m_Audios; }
	template<> std::map<long, Body>&       			Scene::GetContainer<Body>()       		   { return m_Bodies; }
	template<> std::map<long, Button>&     			Scene::GetContainer<Button>() 	 		   { return m_Buttons; }
	template<> std::map<long, Camera>&     			Scene::GetContainer<Camera>() 			   { return m_Cameras; }
	template<> std::map<long, Canvas>&     			Scene::GetContainer<Canvas>() 	 		   { return m_Canvases; }
	template<> std::map<long, CharacterController>& Scene::GetContainer<CharacterController>() { return m_CharacterControllers; }
	template<> std::map<long, JointMaker>& 			Scene::GetContainer<JointMaker>()		   { return m_JointMakers; }
	template<> std::map<long, Light>&      			Scene::GetContainer<Light>()      		   { return m_Lights; }
	template<> std::map<long, Mesh>&       			Scene::GetContainer<Mesh>() 			   { return m_Meshes; }
	template<> std::map<long, Script>&     			Scene::GetContainer<Script>()    		   { return m_Scripts; }
	template<> std::map<long, Sprite>&     			Scene::GetContainer<Sprite>()    		   { return m_Sprites; }
	template<> std::map<long, Text>&       			Scene::GetContainer<Text>() 			   { return m_Texts; }
	template<> std::map<long, TileMap>&    			Scene::GetContainer<TileMap>() 	 		   { return m_TileMaps; }
	template<> std::map<long, Transform>&  			Scene::GetContainer<Transform>()  		   { return m_Transforms; }

	std::map<long, std::vector<std::string>> &Scene::GetLuaScriptsByOwner()
	{
		return m_LuaScriptsByOwner;
	}
	std::map<std::string, std::vector<Mesh>>& Scene::GetMeshesByMaterial()
	{
		return m_MeshesByMaterial;
	}

	Component* Scene::AddComponent(ComponentType type, long ownerID, long componentID)
	{
		switch (type)
		{
			case ComponentType_Animation:  			return Add<Animation>(ownerID, componentID);
			case ComponentType_Audio:      			return Add<Audio>(ownerID, componentID);
			case ComponentType_Body:       			return Add<Body>(ownerID, componentID);
			case ComponentType_Button:     			return Add<Button>(ownerID, componentID);
			case ComponentType_Camera:     			return Add<Camera>(ownerID, componentID);
			case ComponentType_Canvas:     			return Add<Canvas>(ownerID, componentID);
			case ComponentType_CharacterController: return Add<CharacterController>(ownerID, componentID);
			case ComponentType_JointMaker: 			return Add<JointMaker>(ownerID, componentID);
			case ComponentType_Light:      			return Add<Light>(ownerID, componentID);
			case ComponentType_Mesh:       			return Add<Mesh>(ownerID, componentID);
			case ComponentType_Script:     			return Add<Script>(ownerID, componentID);
			case ComponentType_Sprite:     			return Add<Sprite>(ownerID, componentID);
			case ComponentType_Text:       			return Add<Text>(ownerID, componentID);
			case ComponentType_TileMap:    			return Add<TileMap>(ownerID, componentID);
			case ComponentType_Transform:  			return Add<Transform>(ownerID, componentID);
			default:                       			return nullptr;
		}
	}

	template<> void Scene::Remove<Camera>(long ownerID)
	{
		if (m_Cameras.count(ownerID))
		{
			if (m_Cameras.at(ownerID).IsPrimary())
			{
				RemovePrimaryCamera();
			}
			m_Cameras.erase(ownerID);			
		}
	}
	template<> void Scene::Remove<Body>(long ownerID)
	{
		if (m_Bodies.count(ownerID))
		{
			m_Bodies.at(ownerID).Cleanup();
			m_Bodies.erase(ownerID);			
		}
	}
	template<> void Scene::Remove<Mesh>(long ownerID)
	{
		if (m_Meshes.count(ownerID))
		{
			std::string materialName = m_Meshes.at(ownerID).GetMaterialName();
			VulkanManager::vulkan.RemoveSceneViewMaterialMesh(materialName, m_Meshes.at(ownerID).GetID(), &m_Meshes.at(ownerID));
			VulkanManager::vulkan.RemoveGameViewMaterialMesh(materialName, m_Meshes.at(ownerID).GetID(), &m_Meshes.at(ownerID));
			m_Meshes.at(ownerID).Cleanup();
			m_Meshes.erase(ownerID);
		}
	}

	void Scene::RemoveComponent(Component* component)
	{	
		if (component == nullptr)
			return;

		long ownerID = component->GetParentObjectID();
		switch (component->GetType())
		{
			case ComponentType_Animation:  			Remove<Animation>(ownerID); break;
			case ComponentType_Audio:      			Remove<Audio>(ownerID); break;
			case ComponentType_Body:       			Remove<Body>(ownerID); break;
			case ComponentType_Button:     			Remove<Button>(ownerID); break;
			case ComponentType_Camera:     			Remove<Camera>(ownerID); break;
			case ComponentType_Canvas:     			Remove<Canvas>(ownerID); break;
			case ComponentType_CharacterController: Remove<CharacterController>(ownerID); break;
			case ComponentType_JointMaker: 			Remove<JointMaker>(ownerID); break;
			case ComponentType_Light:      			Remove<Light>(ownerID); break;
			case ComponentType_Mesh:       			Remove<Mesh>(ownerID); break;
			case ComponentType_Script:     			Remove<Script>(ownerID); break;
			case ComponentType_Sprite:     			Remove<Sprite>(ownerID); break;
			case ComponentType_Text:       			Remove<Text>(ownerID); break;
			case ComponentType_TileMap:    			Remove<TileMap>(ownerID); break;
			case ComponentType_Transform:  			Remove<Transform>(ownerID); break;
			default:                       			return;
		}
	}

	Camera* Scene::GetPrimaryCamera()
	{
		return m_primaryCamera;
	}

	void Scene::SetPrimaryCamera(Camera* camera)
	{
		if (camera != nullptr)
		{
			if (m_primaryCamera != nullptr)
			{
				m_primaryCamera->SetPrimaryCamera(false);
			}
			m_primaryCamera = camera;
		}
		else
		{
			Logger::log.Warn("Failed to set primary Camera.");
		}
	}

	void Scene::RemovePrimaryCamera()
	{
		if (m_primaryCamera != nullptr)
		{
			m_primaryCamera->SetPrimaryCamera(false);
			m_primaryCamera = nullptr;
		}
	}
}