#include "Scene.h"
#include "FlatEngine.h"
#include "Application.h"
#include "GameObject.h"
#include "Transform.h"
#include "Sprite.h"
#include "Camera.h"
#include "Script.h"
#include "Button.h"
#include "Canvas.h"
#include "Animation.h"
#include "Audio.h"
#include "Text.h"
#include "GameLoop.h"
#include "ECSManager.h"
#include "TileMap.h"

namespace FL = FlatEngine;


namespace FlatEngine
{
	Scene::Scene()
	{
		m_name = "New Scene";
		m_path = "";
		m_sceneObjects = std::map<long, GameObject>();		
		m_animatorPreviewObjects = std::vector<GameObject*>();
		m_ECSManager = ECSManager();		
		m_nextGameObjectID = 0;
		m_nextComponentID = 0;
		m_freedComponentIDs = std::vector<long>();
		m_freedGameObjectIDs = std::vector<long>();
		m_b_persistantScene = false;
	}

	Scene::~Scene()
	{
	}

	bool Scene::SortHierarchyObjects(GameObject* gameObjectA, GameObject* gameObjectB)
	{
		return gameObjectA->GetHierarchyPosition() < gameObjectB->GetHierarchyPosition();
	}

    void Scene::SetName(std::string newName)
    {
		m_name = newName;
    }

	std::string Scene::GetName()
	{
		return m_name;
	}

	void Scene::SetPath(std::string scenePath)
	{
		m_path = scenePath;
	}

	std::string Scene::GetPath()
	{
		return m_path;
	}

	void Scene::UnloadECSManager()
	{
		m_ECSManager.Cleanup();
	}

	void Scene::UnloadSceneObjects()
	{
		m_sceneObjects.clear();
	}

	GameObject* Scene::AddSceneObject(GameObject sceneObject)
	{
		long id = sceneObject.GetID();
		sceneObject.SetHierarchyPosition((int)m_sceneObjects.size());
		m_sceneObjects.emplace(id, sceneObject);
		KeepNextGameObjectIDUpToDate(id);
		SortSceneObjects();

		return &m_sceneObjects.at(id);
	}

	void Scene::KeepNextGameObjectIDUpToDate(long ID)
	{
		if (ID >= m_nextGameObjectID)
		{
			m_nextGameObjectID = ID + 1;
		}
	}

	std::map<long, GameObject> &Scene::GetSceneObjects()
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
		if (m_sceneObjects.count(ID) > 0)
		{
			return &m_sceneObjects.at(ID);
		}
		else 
		{
			return nullptr;
		}
	}

	GameObject* Scene::GetObjectByName(std::string name)
	{
		for (std::map<long, GameObject>::iterator iter = m_sceneObjects.begin(); iter != m_sceneObjects.end(); iter++)
		{
			if (name == iter->second.GetName())
			{
				return &iter->second;
			}
		
		}
		for (GameObject *animPreviewObject : m_animatorPreviewObjects)
		{
			if (animPreviewObject != nullptr && name == animPreviewObject->GetName())
			{
				return animPreviewObject;
			}
		}
		return nullptr;
	}

	GameObject* Scene::GetObjectByTag(std::string tag)
	{
		for (std::map<long, GameObject>::iterator iter = m_sceneObjects.begin(); iter != m_sceneObjects.end(); iter++)
		{
			if (iter->second.GetTagList().HasTag(tag))
			{
				return &iter->second;
			}
		}
		return nullptr;
	}

	GameObject* Scene::CreateGameObject(long parentID, long myID)
	{
		if (m_sceneObjects.count(myID))
		{
			LogError("GameObject not created, ID taken already: " + std::to_string(myID));
			return nullptr;
		}

		GameObject newObject = GameObject(parentID, myID);
		newObject.SetPersistant(m_b_persistantScene);
		newObject.AddTransform();

		if (parentID != -1 && m_sceneObjects.count(parentID))
		{
			m_sceneObjects.at(parentID).AddChild(newObject.GetID());
		}

		return AddSceneObject(newObject);
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

	// Recursive
	void Scene::DeleteChildrenAndSelf(GameObject *objectToDelete)
	{
		if (objectToDelete != nullptr)
		{
			long ID = objectToDelete->GetID();

			if (F_primaryCamera != nullptr && F_primaryCamera->GetParentID() == objectToDelete->GetID())
			{
				F_primaryCamera->SetPrimaryCamera(false);
				F_primaryCamera = nullptr;
			}

			if (objectToDelete->GetAnimation() != nullptr)
			{
				objectToDelete->GetAnimation()->StopAll();
			}

			for (Component* component : objectToDelete->GetComponents())
			{
				RemoveComponent(component);
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

			m_sceneObjects.erase(ID);
			m_freedGameObjectIDs.push_back(ID);
		}
	}

	void Scene::IncrementGameObjectID()
	{
		m_nextGameObjectID += 1;
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
			IncrementGameObjectID();
		}

		return ID;
	}

	void  Scene::IncrementComponentID()
	{
		m_nextComponentID += 1;
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
			IncrementComponentID();
		}

		return ID;
	}

	void Scene::SetPersistantScene(bool b_persistant)
	{
		m_b_persistantScene = b_persistant;
	}

	bool Scene::IsPersistantScene()
	{
		return m_b_persistantScene;
	}

	void Scene::OnPrefabInstantiated()
	{		

	}

	void Scene::SortSceneObjects()
	{
		m_sortedHierarchyObjects.clear();

		for (std::map<long, GameObject>::iterator iter = m_sceneObjects.begin(); iter != m_sceneObjects.end(); iter++)
		{
			m_sortedHierarchyObjects.push_back(&(iter->second));
		}

		std::sort(m_sortedHierarchyObjects.begin(), m_sortedHierarchyObjects.end(), SortHierarchyObjects);
	}

	std::vector<GameObject*> Scene::GetSortedHierarchyObjects()
	{
		return m_sortedHierarchyObjects;
	}

	void Scene::CreateJoints()
	{
		for (std::map<long, JointMaker>::iterator iter = GetJointMakers().begin(); iter != GetJointMakers().end(); iter++)
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

	Transform* Scene::AddTransform(Transform transform, long ownerID)
	{
		KeepNextComponentIDUpToDate(transform.GetID());
		return m_ECSManager.AddTransform(transform, ownerID);
	}

	Sprite* Scene::AddSprite(Sprite sprite, long ownerID)
	{
		KeepNextComponentIDUpToDate(sprite.GetID());
		return m_ECSManager.AddSprite(sprite, ownerID);
	}

	Camera* Scene::AddCamera(Camera camera, long ownerID)
	{
		KeepNextComponentIDUpToDate(camera.GetID());
		return m_ECSManager.AddCamera(camera, ownerID);
	}

	Script* Scene::AddScript(Script script, long ownerID)
	{
		KeepNextComponentIDUpToDate(script.GetID());
		return m_ECSManager.AddScript(script, ownerID);
	}

	Canvas* Scene::AddCanvas(Canvas canvas, long ownerID)
	{
		KeepNextComponentIDUpToDate(canvas.GetID());
		return m_ECSManager.AddCanvas(canvas, ownerID);
	}

	Audio* Scene::AddAudio(Audio audio, long ownerID)
	{
		KeepNextComponentIDUpToDate(audio.GetID());
		return m_ECSManager.AddAudio(audio, ownerID);
	}

	Text* Scene::AddText(Text text, long ownerID)
	{
		KeepNextComponentIDUpToDate(text.GetID());
		return m_ECSManager.AddText(text, ownerID);
	}

	Body* Scene::AddBody(Body body, long ownerID)
	{
		KeepNextComponentIDUpToDate(body.GetID());
		return m_ECSManager.AddBody(body, ownerID);
	}

	Animation* Scene::AddAnimation(Animation animation, long ownerID)
	{
		KeepNextComponentIDUpToDate(animation.GetID());
		return m_ECSManager.AddAnimation(animation, ownerID);
	}

	Button* Scene::AddButton(Button button, long ownerID)
	{
		KeepNextComponentIDUpToDate(button.GetID());
		return m_ECSManager.AddButton(button, ownerID);
	}

	JointMaker* Scene::AddJointMaker(JointMaker jointMaker, long ownerID)
	{
		KeepNextComponentIDUpToDate(jointMaker.GetID());
		return m_ECSManager.AddJointMaker(jointMaker, ownerID);
	}

	CharacterController* Scene::AddCharacterController(CharacterController characterController, long ownerID)
	{
		KeepNextComponentIDUpToDate(characterController.GetID());
		return m_ECSManager.AddCharacterController(characterController, ownerID);
	}

	TileMap* Scene::AddTileMap(TileMap tileMap, long ownerID)
	{
		KeepNextComponentIDUpToDate(tileMap.GetID());
		return m_ECSManager.AddTileMap(tileMap, ownerID);
	}

	void Scene::RemoveComponent(Component* component, long ownerID)
	{
		if (component != nullptr)
		{
			long ID = component->GetID();
			if (m_ECSManager.RemoveComponent(component, ownerID))
			{
				m_freedComponentIDs.push_back(ID);
			}
		}
	}

	Transform* Scene::GetTransformByOwner(long ownerID)
	{
		return m_ECSManager.GetTransformByOwner(ownerID);
	}

	Sprite* Scene::GetSpriteByOwner(long ownerID)
	{
		return m_ECSManager.GetSpriteByOwner(ownerID);
	}

	Camera* Scene::GetCameraByOwner(long ownerID)
	{
		return m_ECSManager.GetCameraByOwner(ownerID);
	}

	std::vector<Script*> Scene::GetScriptsByOwner(long ownerID)
	{
		return m_ECSManager.GetScriptsByOwner(ownerID);
	}

	Canvas* Scene::GetCanvasByOwner(long ownerID)
	{
		return m_ECSManager.GetCanvasByOwner(ownerID);
	}

	Audio* Scene::GetAudioByOwner(long ownerID)
	{
		return m_ECSManager.GetAudioByOwner(ownerID);
	}

	Text* Scene::GetTextByOwner(long ownerID)
	{
		return m_ECSManager.GetTextByOwner(ownerID);
	}

	Animation* Scene::GetAnimationByOwner(long ownerID)
	{
		return m_ECSManager.GetAnimationByOwner(ownerID);
	}

	Button* Scene::GetButtonByOwner(long ownerID)
	{
		return m_ECSManager.GetButtonByOwner(ownerID);
	}

	Body* Scene::GetBodyByOwner(long ownerID)
	{
		return m_ECSManager.GetBodyByOwner(ownerID);
	}

	JointMaker* Scene::GetJointMakerByOwner(long ownerID)
	{
		return m_ECSManager.GetJointMakerByOwner(ownerID);
	}

	CharacterController* Scene::GetCharacterControllerByOwner(long ownerID)
	{
		return m_ECSManager.GetCharacterControllerByOwner(ownerID);
	}

	TileMap* Scene::GetTileMapByOwner(long ownerID)
	{
		return m_ECSManager.GetTileMapByOwner(ownerID);
	}

	std::map<long, Transform> &Scene::GetTransforms()
	{
		return m_ECSManager.GetTransforms();
	}
	std::map<long, Sprite>& Scene::GetSprites()
	{
		return m_ECSManager.GetSprites();
	}
	std::map<long, Camera>& Scene::GetCameras()
	{
		return m_ECSManager.GetCameras();
	}
	std::map<long, std::map<long, Script>>& Scene::GetScripts()
	{
		return m_ECSManager.GetScripts();
	}
	std::map<long, std::vector<std::string>> &Scene::GetLuaScriptsByOwner()
	{
		return m_ECSManager.GetLuaScriptsByOwner();
	}
	std::map<long, Button>& Scene::GetButtons()
	{
		return m_ECSManager.GetButtons();
	}
	std::map<long, Canvas>& Scene::GetCanvases()
	{
		return m_ECSManager.GetCanvases();
	}
	std::map<long, Animation>& Scene::GetAnimations()
	{
		return m_ECSManager.GetAnimations();
	}
	std::map<long, Audio>& Scene::GetAudios()
	{
		return m_ECSManager.GetAudios();
	}
	std::map<long, Text>& Scene::GetTexts()
	{
		return m_ECSManager.GetTexts();
	}
	std::map<long, Body>& Scene::GetBodies()
	{
		return m_ECSManager.GetBodies();
	}
	std::map<long, JointMaker>& Scene::GetJointMakers()
	{
		return m_ECSManager.GetJointMakers();
	}
	std::map<long, CharacterController>& Scene::GetCharacterControllers()
	{
		return m_ECSManager.GetCharacterControllers();
	}
	std::map<long, TileMap>& Scene::GetTileMaps()
	{
		return m_ECSManager.GetTileMaps();
	}
}