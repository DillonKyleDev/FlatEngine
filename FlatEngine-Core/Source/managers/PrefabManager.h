#pragma once
#include "managers/LuaManager.h"
#include "tools/JsonHelper.h"
#include "tools/Vector2.h"
#include "tools/Vector3.h"

#include <string>
#include <map>


namespace FlatEngine 
{
	class GameObject;
	class Scene;

	namespace PrefabManager
	{
		struct Prefab {
			json prefab;
			std::vector<Prefab> children;
		};

		extern std::map<std::string, Prefab> prefabs;

		extern void CreatePrefabFromJson(json objectJson, Prefab &prefab);
		extern void CreatePrefab(std::string path, GameObject gameObject);		
		extern void AddPrefab(std::string path);
		extern void InitializePrefabs();
		extern GameObject* InstantiateSelfAndChildren(long parentID, long myID, Prefab prefab, Scene* scene, Vector3 spawnLocation = Vector3(0, 0, 0));
		extern GameObject* Instantiate(std::string prefabName, Vector3 spawnLocation, Scene* scene, long parentID = -1, long ID = -1);
		extern std::map<std::string, Prefab> GetPrefabs();
	}
}