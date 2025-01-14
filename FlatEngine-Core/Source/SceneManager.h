#pragma once
#include "Scene.h"

#include <string>
#include <vector>
#include "json.hpp"

using json = nlohmann::json;
using namespace nlohmann::literals;


namespace FlatEngine
{
	class GameObject;

	class SceneManager
	{
	public:
		SceneManager();
		~SceneManager();

		void SetLoadedScene(Scene scene);
		Scene* CreateNewScene();
		void SaveScene(Scene* scene, std::string filePath);
		void SaveCurrentScene();
		bool LoadScene(std::string filePath, std::string pointTo = "");
		Scene* GetLoadedScene();
		void SetLoadedScenePath(std::string filePath);
		std::string GetLoadedScenePath();
		void SaveAnimationPreviewObjects();
		void LoadAnimationPreviewObjects();

	private:
		Scene m_loadedScene;
		std::string m_loadedScenePath;
		std::vector<GameObject*> m_animatorPreviewObjects;
	};
}

