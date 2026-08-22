#pragma once
#include "GameObject.h"
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
			json parent;
			std::vector<Prefab> children;
		};

		extern std::map<std::string, Prefab> prefabs;
		
		extern void CreatePrefab(std::string path, GameObject* gameObject);		
		extern void LoadPrefab(std::string path);
		extern void InitializePrefabs();
		extern GameObject* Instantiate(std::string prefabName, Vector3 spawnLocation = Vector3(), long parentID = -1, long ID = -1);
		extern std::map<std::string, Prefab> GetPrefabs();
		extern void UnpackPrefab(GameObject* gameObject);
	}
}