#include "components/Button.h"
#include "components/Transform.h"
#include "GameObject.h"
#include "managers/Assets.h"
#include "managers/PrefabManager.h"
#include "managers/SceneManager.h"
#include "tools/FileHelper.h"
#include "tools/JsonHelper.h"


namespace FlatEngine
{
	namespace PrefabManager
	{
		std::map<std::string, Prefab> prefabs = std::map<std::string, Prefab>();


		json GetChildrenJson(std::vector<long> childIDs)
		{
			json childrenJson = json::array();
			
			for (int i = 0; i < childIDs.size(); i++)
			{
				GameObject* child = SceneManager::loadedScene.GetObjectByID(childIDs[i]);

				if (child == nullptr)
					continue;

				std::vector<long> grandchildIDs = child->GetChildren();
				long hierarchyPos = child->hierarchyPosition;
				long parentedHierarchyPos = child->parentedHierarchyPosition;
				child->hierarchyPosition = -1;
				child->parentedHierarchyPosition = -1;

				bool b_IDOverride = true;

				json childJson = json::object({
					{ "parent", child->GetData(b_IDOverride) },
					{ "children", GetChildrenJson(grandchildIDs) }
				});

				child->hierarchyPosition = hierarchyPos;
				child->parentedHierarchyPosition = parentedHierarchyPos;

				childrenJson.push_back(childJson);
			}

			return childrenJson;
		}

		void CreatePrefab(std::string path, GameObject* gameObject)
		{
			std::string prefabName = FileHelper::GetFilenameFromPath(path);
			std::vector<long> childIDs = gameObject->GetChildren();
			gameObject->SetName(prefabName);
			gameObject->SetPrefabName(prefabName);
			gameObject->SetIsPrefab(true);
			long hierarchyPos = gameObject->hierarchyPosition;
			long parentedHierarchyPos = gameObject->parentedHierarchyPosition;
			gameObject->hierarchyPosition = -1;
			gameObject->parentedHierarchyPosition = -1;

			bool b_IDOverride = true;

			json parentJson = json::object({
				{ "parent", gameObject->GetData(b_IDOverride) },
				{ "children", GetChildrenJson(childIDs) }
			});

			json prefabJson = json::object({
				{ "name", prefabName },
				{ "prefab", parentJson }				
			});

			gameObject->hierarchyPosition = hierarchyPos;
			gameObject->parentedHierarchyPosition = parentedHierarchyPos;

			JsonHelper::WriteJsonToFile(prefabJson, path);

			LoadPrefab(path);
		}

		Prefab LoadChildJson(json childJson, std::string name)
		{
			Prefab prefab;
							
			if (JsonHelper::JsonContains(childJson, "parent", name))
			{
				prefab.parent = childJson.at("parent");
				
				for (auto grandChildJson : childJson.at("children"))				
				{					
					prefab.children.push_back(LoadChildJson(grandChildJson, name));		
				}
			}

			return prefab;
		}

		void LoadPrefab(std::string path)
		{
			Prefab prefab;			
			json prefabJson = JsonHelper::LoadFileData(path);
			std::string name = JsonHelper::CheckJsonString(prefabJson, "name", "Prefab Name");
			json parentPrefabJson = prefabJson["prefab"];

			if (JsonHelper::JsonContains(parentPrefabJson, "parent", name))
			{
				prefab.parent = parentPrefabJson.at("parent");				
				
				for (auto childJson : parentPrefabJson.at("children"))				
				{
					prefab.children.push_back(LoadChildJson(childJson, name));
				}
			}

			if (prefabs.count(name))
			{
				prefabs.at(name) = prefab;
			}
			else
			{
				prefabs.emplace(name, prefab);
			}			
		}

		void InitializePrefabs()
		{
			prefabs.clear();
			std::vector<std::string> prefabPaths = FileHelper::FindAllFilesWithExtension(Assets::assetManager.GetDir("projectDir"), ".prf");

			for (std::string path : prefabPaths)
			{
				LoadPrefab(path);
			}
		}

		GameObject* InstantiateChildPrefab(Prefab prefab, long parentID)
		{
			GameObject* childPtr = SceneManager::loadedScene.AddSceneObject(prefab.parent, parentID);								
			childPtr->SetIsPrefabChild(true);
			long ID = childPtr->GetID();

			for (Prefab grandchild : prefab.children)
			{
				GameObject* grandChildObject = InstantiateChildPrefab(grandchild, ID);
				SceneManager::loadedScene.GetObjectByID(ID)->AddChild(grandChildObject);
			}

			return SceneManager::loadedScene.GetObjectByID(ID);
		}

		GameObject* Instantiate(std::string prefabName, Vector3 spawnLocation, long parentID, long ID)
		{			
			GameObject prefabObject = GameObject(parentID, ID);
			GameObject* prefabPtr = nullptr;

			if (prefabs.count(prefabName) > 0)
			{
				Prefab prefab = prefabs.at(prefabName);				
				prefabPtr = SceneManager::loadedScene.AddSceneObject(prefab.parent, parentID, ID);								
				ID = prefabPtr->GetID();

				for (Prefab child : prefab.children)
				{
					GameObject* childObject = InstantiateChildPrefab(child, ID);
					SceneManager::loadedScene.GetObjectByID(ID)->AddChild(childObject);
				}
			}
			else 
			{
				Logger::log.Err("PrefabManager::Instantiate() - Prefab {} was not found.", prefabName);				
			}

			prefabPtr = SceneManager::loadedScene.GetObjectByID(ID);

			if (prefabPtr != nullptr)
				prefabPtr->SortChildren();

			prefabPtr->Get<Transform>()->SetPosition(spawnLocation);
			
			return prefabPtr;
		}

		std::map<std::string, Prefab> GetPrefabs()
		{
			return prefabs;
		}

		void UnpackPrefab(GameObject* gameObject)
		{
			gameObject->SetIsPrefab(false);
			gameObject->SetIsPrefabChild(false);
			gameObject->SetPrefabName("");
			gameObject->SetPrefabSpawnLocation(FL::Vector3());

			for (long childID : gameObject->GetChildren())
			{
				GameObject* child = SceneManager::loadedScene.GetObjectByID(childID);

				if (child != nullptr)
				{
					UnpackPrefab(child);
				}
			}
		}
	}
}