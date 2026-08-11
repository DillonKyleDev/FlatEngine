#include "Types.h"
#include "components/Animation.h"
#include "components/Body.h"
#include "components/Component.h"
#include "components/JointMaker.h"
#include "components/Script.h"
#include "components/Transform.h"
#include "managers/SceneManager.h"
#include "physics/PhysicsManager.h"
#include "render/VulkanManager.h"
#include "tools/Logger.h"


namespace FlatEngine
{
	Scene::Scene()
	{
		name = "New Scene";
		path = "";				 		
		m_nextGameObjectID = 0;
		m_primaryCameraID = -1;
	}
	
	bool Scene::SortHierarchyObjects(GameObject* gameObjectA, GameObject* gameObjectB)
	{
		return gameObjectA->GetHierarchyPosition() < gameObjectB->GetHierarchyPosition();
	}

	void Scene::Unload()
	{
		m_sceneObjects.Clear();

		m_Transforms.Clear();
		m_Sprites.Clear();
		m_Cameras.Clear();
		m_Scripts.Clear();		
		m_Buttons.Clear();
		m_Canvases.Clear();
		m_Animations.Clear();
		m_Audios.Clear();
		m_Texts.Clear();
		m_CharacterControllers.Clear();		
		m_TileMaps.Clear();

		for (Body2D& body : m_Bodies2D.GetAll())
		{
			body.Cleanup();
		}
		m_Bodies2D.Clear();		

		for (JointMaker& jointMaker : m_JointMakers.GetAll())
		{
			jointMaker.Cleanup();
		}
		m_JointMakers.Clear();
	}

	GameObject* Scene::AddSceneObject(GameObject sceneObject)
	{
		long ID = sceneObject.GetID();

		if (ID == -1)
			ID = GetNextGameObjectID();

		sceneObject.SetID(ID);

		long parentID = sceneObject.GetParentID();

		// For objects created after initial Scene load
		if (parentID != -1 && GetObjectByID(parentID) != nullptr)
		{
			GetObjectByID(parentID)->AddChild(ID);
		}	

		sceneObject.SetHierarchyPosition((int)m_sceneObjects.GetAll().size());		    

		KeepNextGameObjectIDUpToDate(ID);
		SortSceneObjects();

		return m_sceneObjects.Add(ID, sceneObject);
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
		return m_sceneObjects.GetAll();
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
		return m_sceneObjects.Get(ID);
	}

	GameObject* Scene::GetObjectByName(std::string name)
	{
		for (GameObject& gameObject : m_sceneObjects.GetAll())
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
		for (GameObject& gameObject : m_sceneObjects.GetAll())
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
		m_sceneObjects.Remove(ID);
	}

	// Recursive
	void Scene::DeleteChildrenAndSelf(GameObject *objectToDelete)
	{
		if (objectToDelete != nullptr)
		{
			long ID = objectToDelete->GetID();

			if (SceneManager::loadedScene.Get<Camera>(m_primaryCameraID) && m_primaryCameraID == objectToDelete->GetID())
			{
				SceneManager::loadedScene.Get<Camera>(m_primaryCameraID)->SetPrimaryCamera(false);
				m_primaryCameraID = -1;
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

	void Scene::OnPrefabInstantiated()
	{		
	}

	void Scene::SortSceneObjects()
	{
		m_sortedHierarchyObjects.clear();

		for (GameObject& gameObject : m_sceneObjects.GetAll())
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
		for (JointMaker& jointMaker : m_JointMakers.GetAll())
		{
			for (Joint* joint : jointMaker.GetJoints())
			{
				if (joint->HasValidBodies())
				{
					PhysicsManager::physics2D.CreateJoint(joint);
				}
			}
		}
	}

	template<> UMapVector<Animation>&           Scene::GetContainer<Animation>()  		   { return m_Animations; }
	template<> UMapVector<Audio>&      			Scene::GetContainer<Audio>()      		   { return m_Audios; }
	template<> UMapVector<Body>&       			Scene::GetContainer<Body>()       		   { return m_Bodies; }
	template<> UMapVector<Body2D>&       		Scene::GetContainer<Body2D>()       	   { return m_Bodies2D; }
	template<> UMapVector<Button>&     			Scene::GetContainer<Button>() 	 		   { return m_Buttons; }
	template<> UMapVector<Camera>&     			Scene::GetContainer<Camera>() 			   { return m_Cameras; }
	template<> UMapVector<Canvas>&     			Scene::GetContainer<Canvas>() 	 		   { return m_Canvases; }
	template<> UMapVector<CharacterController>& Scene::GetContainer<CharacterController>() { return m_CharacterControllers; }
	template<> UMapVector<JointMaker>& 			Scene::GetContainer<JointMaker>()		   { return m_JointMakers; }
	template<> UMapVector<Light>&      			Scene::GetContainer<Light>()      		   { return m_Lights; }
	template<> UMapVector<Mesh>&       			Scene::GetContainer<Mesh>() 			   { return m_Meshes; }
	template<> UMapVector<Script>&     			Scene::GetContainer<Script>()    		   { return m_Scripts; }
	template<> UMapVector<Sprite>&     			Scene::GetContainer<Sprite>()    		   { return m_Sprites; }
	template<> UMapVector<Text>&       			Scene::GetContainer<Text>() 			   { return m_Texts; }
	template<> UMapVector<TileMap>&    			Scene::GetContainer<TileMap>() 	 		   { return m_TileMaps; }
	template<> UMapVector<Transform>&  			Scene::GetContainer<Transform>()  		   { return m_Transforms; }

	std::map<std::string, std::vector<Mesh>>& Scene::GetMeshesByMaterial()
	{
		return m_MeshesByMaterial;
	}

	Component* Scene::AddComponent(ComponentType type, long ownerID)
	{
		switch (type)
		{
			case ComponentType_Animation:  			return Add<Animation>(ownerID);
			case ComponentType_Audio:      			return Add<Audio>(ownerID);
			case ComponentType_Body:       			return Add<Body>(ownerID);
			case ComponentType_Body2D:       		return Add<Body2D>(ownerID);
			case ComponentType_Button:     			return Add<Button>(ownerID);
			case ComponentType_Camera:     			return Add<Camera>(ownerID);
			case ComponentType_Canvas:     			return Add<Canvas>(ownerID);
			case ComponentType_CharacterController: return Add<CharacterController>(ownerID);
			case ComponentType_JointMaker: 			return Add<JointMaker>(ownerID);
			case ComponentType_Light:      			return Add<Light>(ownerID);
			case ComponentType_Mesh:       			return Add<Mesh>(ownerID);
			case ComponentType_Script:     			return Add<Script>(ownerID);
			case ComponentType_Sprite:     			return Add<Sprite>(ownerID);
			case ComponentType_Text:       			return Add<Text>(ownerID);
			case ComponentType_TileMap:    			return Add<TileMap>(ownerID);
			case ComponentType_Transform:  			return Add<Transform>(ownerID);
			default:                       			return nullptr;
		}
	}

	template<> void Scene::Remove<Camera>(long ownerID)
	{
		if (m_Cameras.Get(ownerID))
		{
			if (m_Cameras.Get(ownerID)->IsPrimary())
				RemovePrimaryCamera();
			
			m_Cameras.Remove(ownerID);	
			SceneView::cameraSceneRenderObjects.Remove(ownerID);		
		}
	}
	template<> void Scene::Remove<Body2D>(long ownerID)
	{
		if (m_Bodies2D.Get(ownerID))
		{
			m_Bodies2D.Get(ownerID)->Cleanup();
			m_Bodies2D.Remove(ownerID);			
		}
	}
	template<> void Scene::Remove<Mesh>(long ownerID)
	{
		if (m_Meshes.Get(ownerID))
		{
			std::string materialName = m_Meshes.Get(ownerID)->GetMaterialName();
			VulkanManager::vulkan.RemoveSceneViewMaterialMesh(materialName, ownerID);
			VulkanManager::vulkan.RemoveGameViewMaterialMesh(materialName, ownerID);
			m_Meshes.Get(ownerID)->Cleanup();
			m_Meshes.Remove(ownerID);
		}
	}

	void Scene::RemoveComponent(Component* component)
	{	
		if (component == nullptr)
			return;

		long ownerID = component->GetOwnerID();
		switch (component->GetType())
		{
			case ComponentType_Animation:  			Remove<Animation>(ownerID); break;
			case ComponentType_Audio:      			Remove<Audio>(ownerID); break;
			case ComponentType_Body:       			Remove<Body>(ownerID); break;
			case ComponentType_Body2D:       		Remove<Body2D>(ownerID); break;
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
		return SceneManager::loadedScene.Get<Camera>(m_primaryCameraID);
	}

	void Scene::SetPrimaryCamera(long cameraID)
	{
		if (SceneManager::loadedScene.Get<Camera>(cameraID))
		{
			if (SceneManager::loadedScene.Get<Camera>(m_primaryCameraID) != nullptr && cameraID != m_primaryCameraID)
			{
				SceneManager::loadedScene.Get<Camera>(m_primaryCameraID)->SetPrimaryCamera(false);
			}
			m_primaryCameraID = cameraID;
		}
		else
		{
			Logger::log.Warn("Failed to set primary Camera.");
		}
	}

	void Scene::RemovePrimaryCamera()
	{
		if (SceneManager::loadedScene.Get<Camera>(m_primaryCameraID))
		{
			SceneManager::loadedScene.Get<Camera>(m_primaryCameraID)->SetPrimaryCamera(false);
			m_primaryCameraID = -1;
		}
	}
}