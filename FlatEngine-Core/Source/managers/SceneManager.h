#pragma once
#include "managers/Scene.h"

#include <string>
#include <vector>


namespace FlatEngine
{
	class GameObject;

	namespace SceneManager
	{
		extern Scene loadedScene;		
		extern std::vector<GameObject*> animatorPreviewObjects;

		extern void SetLoadedScene(Scene scene);
		extern Scene* CreateAndLoadNewScene();
		extern void CreateSceneBackup();
		extern void SaveScene(Scene* scene, std::string filePath);
		extern void SaveTempScene(std::string sceneName = "");
		extern void SaveCurrentScene();
		extern void QueueLoadScene(std::string scenePath);
		extern bool LoadScene(std::string actualPath, std::string pointTo = "");
		extern void SetLoadedScenePath(std::string filePath);
		extern std::string GetLoadedScenePath();
		extern std::string CreateNewSceneFile(std::string fileName, std::string path = "");
	}
}

