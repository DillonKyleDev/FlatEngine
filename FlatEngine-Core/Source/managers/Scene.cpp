#include "Types.h"
#include "components/Animation.h"
#include "components/Body.h"
#include "components/Component.h"
#include "components/Script.h"
#include "components/Transform.h"
#include "managers/SceneManager.h"
#include "physics/PhysicsManager.h"
#include "tools/Logger.h"


namespace FlatEngine
{	
	void CleanupBody(Body2D& body) { body.Cleanup(); }
	void CleanupMesh(Mesh& mesh) { mesh.Cleanup(); }
	void CleanupCamera(Camera& camera) 
	{
		if (camera.IsPrimary())
			SceneManager::loadedScene.RemovePrimaryCamera();
		
		SceneView::cameraSceneRenderObjects.Remove(camera.GetOwnerID());			
	}

	Scene::Scene()
	{
		name = "New Scene";
		path = "";				 		
		m_nextGameObjectID = 0;
		m_nextJoint2DID = 0;
		m_primaryCameraID = -1;

		m_Bodies2D = UMapVector<Body2D>(CleanupBody);
		m_Meshes = UMapVector<Mesh>(CleanupMesh);
		m_Cameras = UMapVector<Camera>(CleanupCamera);
	}

	void Scene::Unload()
	{
		m_sceneObjects.Clear();
		m_sortedHierarchyObjects.clear();
		m_Transforms.Clear();
		m_Animations.Clear();
		m_Audios.Clear();
		m_Bodies2D.Clear();
		m_Buttons.Clear();
		m_Cameras.Clear();
		m_Canvases.Clear();
		m_CharacterControllers.Clear();	
		m_Meshes.Clear();	
		m_Scripts.Clear();		
		m_Sprites.Clear();
		m_Texts.Clear();
		m_TileMaps.Clear();		
	}

	void Scene::ReduceHierarchyPositions()
	{
		if (m_sortedHierarchyObjects.size())
		{
			GameObject* lastObject = SceneManager::loadedScene.GetObjectByID(m_sortedHierarchyObjects.back());
			
			if (lastObject != nullptr && lastObject->hierarchyPosition > m_sortedHierarchyObjects.size() + 10)
			{
				int newHierarchyPos = 0;
				for (long ID : m_sortedHierarchyObjects)
				{
					GameObject* object = SceneManager::loadedScene.GetObjectByID(ID);	
					
					if (object != nullptr)
					{
						object->hierarchyPosition = newHierarchyPos;
						newHierarchyPos++;
					}
				}
			}
		}
	}

	void Scene::InsertSortedHierarchyObject(GameObject* object)
	{
		if (object == nullptr || object->GetParentID() != -1)
			return;

		if (object->hierarchyPosition == -1)
		{
			object->hierarchyPosition = (m_sortedHierarchyObjects.size() && GetObjectByID(m_sortedHierarchyObjects.back()) != nullptr) ? GetObjectByID(m_sortedHierarchyObjects.back())->hierarchyPosition + 1 : 0;
			m_sortedHierarchyObjects.push_back(object->GetID());
		}		
		else 
		{
			bool b_inserted = false;
			for (auto iter = m_sortedHierarchyObjects.begin(); iter != m_sortedHierarchyObjects.end(); iter++)
			{				
				GameObject* existingObject = GetObjectByID((*iter));
				if (existingObject != nullptr)
				{	
					if (b_inserted)
						existingObject->hierarchyPosition++;

					if (existingObject->hierarchyPosition >= object->hierarchyPosition && !b_inserted)
					{
						iter = m_sortedHierarchyObjects.insert(iter, object->GetID());
						b_inserted = true;

						if (object->hierarchyPosition != existingObject->hierarchyPosition)
							return;
					}
				}
			}

			if (!b_inserted)
				m_sortedHierarchyObjects.insert(m_sortedHierarchyObjects.begin(), object->GetID());
		}
	}

	void Scene::RemoveSortedHierarchyObject(GameObject* object)
	{
		for (auto iter = m_sortedHierarchyObjects.begin(); iter != m_sortedHierarchyObjects.end(); iter++)
		{
			if (object->GetID() == (*iter))
			{
				m_sortedHierarchyObjects.erase(iter);
				return;
			}
		}
	}

	// (internal) To add a new GameObject to Scene, use Scene::CreateGameObject();
	GameObject* Scene::AddSceneObject(json objectJson, long parentID, long ID)
	{
		GameObject loadedObject = GameObject(parentID != -1 || objectJson.empty() ? parentID : JsonHelper::CheckJsonLong(objectJson, "parent", "GameObject"), ID != -1 || objectJson.empty() ? ID : JsonHelper::CheckJsonLong(objectJson, "id", "GameObject"))	;

		ID = loadedObject.GetID();
		if (ID == -1)
		{
			ID = GetNextGameObjectID();
			loadedObject.SetID(ID);
		}
		GameObject* objectPtr = m_sceneObjects.Add(ID, loadedObject);		
		objectPtr->PutData(objectJson);

		InsertSortedHierarchyObject(objectPtr);

		// For objects created after initial Scene load
		parentID = loadedObject.GetParentID();
		GameObject* parent = GetObjectByID(parentID);
		if (objectJson.empty() && parent != nullptr)
		{
			parent->AddChild(objectPtr);
		}	

		KeepNextGameObjectIDUpToDate(ID);		

		return objectPtr;
	}

	GameObject* Scene::CreateGameObject(long parentID, long myID)
	{
		GameObject* newObjectPtr = AddSceneObject(json::object(), parentID, myID);
		newObjectPtr->Add<Transform>();
		return newObjectPtr;
	}

	void Scene::DeleteGameObject(long sceneObjectID)
	{
		GameObject *objectToDelete = GetObjectByID(sceneObjectID);	

		if (objectToDelete != nullptr)
		{
			DeleteChildrenAndSelf(objectToDelete);			
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
		RemoveSortedHierarchyObject(GetObjectByID(ID));
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

	// Gets the highest ID of any GameObject currently in the Scene
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

	// *NOT PERSISTENT* Be careful! If you add objects to the scene after getting this pointer, it WILL eventually become garbage because Scene::m_sceneObjects is a std::vector under the hood
	GameObject* Scene::GetObjectByID(long ID)
	{
		return m_sceneObjects.Get(ID);
	}

	// *NOT PERSISTENT* Be careful! If you add objects to the scene after getting this pointer, it WILL eventually become garbage because Scene::m_sceneObjects is a std::vector under the hood
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

	// *NOT PERSISTENT* Be careful! If you add objects to the scene after getting this pointer, it WILL eventually become garbage because Scene::m_sceneObjects is a std::vector under the hood
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

	void Scene::CollectUnusedGameObjectIDs()
	{
		for (long i = 0; i < m_nextGameObjectID; i++)
		{
			if (GetObjectByID(i) == nullptr)
			{
				m_freedGameObjectIDs.push_back(i);
			}
		}
	}

	long Scene::GetNextJoint2DID()
	{
		long ID;

		if (m_freedJoint2DIDs.size() > 0)
		{
			ID = m_freedJoint2DIDs.back();
			m_freedJoint2DIDs.pop_back();
		}
		else
		{
			ID = m_nextJoint2DID;
			m_nextJoint2DID += 1;
		}

		return ID;
	}

	void Scene::AddFreedJoint2DID(long freedID)
	{
		m_freedJoint2DIDs.push_back(freedID);
	}

	bool SortHierarchyObjects(long objectAID, long objectBID)
	{
		return SceneManager::loadedScene.GetObjectByID(objectAID)->hierarchyPosition < SceneManager::loadedScene.GetObjectByID(objectBID)->hierarchyPosition;
	}

	void Scene::SortSceneObjects()
	{
		std::sort(m_sortedHierarchyObjects.begin(), m_sortedHierarchyObjects.end(), SortHierarchyObjects);
	}

	std::vector<long> Scene::GetSortedHierarchyObjects()
	{
		return m_sortedHierarchyObjects;
	}

	long Scene::GetNextHierarchyPosition()
	{
		if (m_sortedHierarchyObjects.size())
			return GetObjectByID(m_sortedHierarchyObjects.back())->hierarchyPosition + 1;
		else
		 	return 0;
	}

	template<> UMapVector<Animation>&           Scene::GetContainer<Animation>()  		   { return m_Animations; }
	template<> UMapVector<Audio>&      			Scene::GetContainer<Audio>()      		   { return m_Audios; }
	template<> UMapVector<Body>&       			Scene::GetContainer<Body>()       		   { return m_Bodies; }
	template<> UMapVector<Body2D>&       		Scene::GetContainer<Body2D>()       	   { return m_Bodies2D; }
	template<> UMapVector<Button>&     			Scene::GetContainer<Button>() 	 		   { return m_Buttons; }
	template<> UMapVector<Camera>&     			Scene::GetContainer<Camera>() 			   { return m_Cameras; }
	template<> UMapVector<Canvas>&     			Scene::GetContainer<Canvas>() 	 		   { return m_Canvases; }
	template<> UMapVector<CharacterController>& Scene::GetContainer<CharacterController>() { return m_CharacterControllers; }	
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
		}
		m_primaryCameraID = -1;
	}
}