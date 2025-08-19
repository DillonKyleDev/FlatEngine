#pragma once
#include "Scene.h"

#include <string>
#include <vector>
#include "json.hpp"

#define JSON_NOEXCEPTION
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
		Scene* CreateAndLoadNewScene();
		void SaveScene(Scene* scene, std::string filePath);
		void SaveTempScene();
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

