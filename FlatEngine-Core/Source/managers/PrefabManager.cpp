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
				std::vector<long> grandchildIDs = child->GetChildren();
				
				bool b_IDOverride = true;

				json childJson = json::object({
					{ "parent", child->GetData(b_IDOverride) },
					{ "children", GetChildrenJson(grandchildIDs) }
				});

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

			bool b_IDOverride = true;

			json parentJson = json::object({
				{ "parent", gameObject->GetData(b_IDOverride) },
				{ "children", GetChildrenJson(childIDs) }
			});

			json prefabJson = json::object({
				{ "name", prefabName },
				{ "prefab", parentJson }				
			});

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

		long InstantiateChildPrefab(Prefab prefab, long parentID)
		{
			long nextHierarchyPosition = SceneManager::loadedScene.GetNextHierarchyPosition();
			GameObject* childPtr = SceneManager::loadedScene.CreateEmptyGameObject(parentID);
			childPtr->PutData(prefab.parent);					
			childPtr->hierarchyPosition = nextHierarchyPosition;		
			
			long ID = childPtr->GetID();	
			
			for (Prefab grandchild : prefab.children)
			{
				childPtr->AddChild(InstantiateChildPrefab(grandchild, ID));
			}

			return ID;
		}

		GameObject* Instantiate(std::string prefabName, Vector3 spawnLocation, long parentID, long ID)
		{			
			GameObject* prefabPtr = nullptr;

			if (prefabs.count(prefabName) > 0)
			{
				long nextHierarchyPosition = SceneManager::loadedScene.GetNextHierarchyPosition();
				Prefab prefab = prefabs.at(prefabName);				
				prefabPtr = SceneManager::loadedScene.CreateEmptyGameObject();
				prefabPtr->PutData(prefab.parent);	
				prefabPtr->hierarchyPosition = nextHierarchyPosition;
				prefabPtr->parentedHierarchyPosition = 0;
												
				for (Prefab child : prefab.children)
				{
					prefabPtr->AddChild(InstantiateChildPrefab(child, prefabPtr->GetID()));
				}
			}
			else 
			{
				Logger::log.Err("PrefabManager::Instantiate() - Prefab {} was not found.", prefabName);				
			}

			SceneManager::loadedScene.SortSceneObjects();

			return prefabPtr;
		}

		std::map<std::string, Prefab> GetPrefabs()
		{
			return prefabs;
		}
	}
}