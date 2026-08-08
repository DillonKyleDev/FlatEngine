#include "FlatEngine.h"
#include "managers/AudioManager.h"
#include "managers/SceneManager.h"
#include "managers/ProjectManager.h"
#include "render/SceneView.h"
#include "render/VulkanManager.h"
#include "tools/FileHelper.h"
#include "tools/JsonHelper.h"
#include "tools/Logger.h"


namespace FlatEngine
{
	namespace SceneManager
	{
		Scene loadedScene = Scene();
		Scene loadedPersistantScene = Scene();
		std::string loadedScenePath = "";
		std::vector<GameObject*> animatorPreviewObjects = std::vector<GameObject*>();

		void SetLoadedScene(Scene scene)
		{
			loadedScene = scene;
			loadedScenePath = scene.path;
		}

		Scene *CreateAndLoadNewScene()
		{
			loadedScene = Scene();
			return &loadedScene;
		}

		void SaveScene(Scene *scene, std::string filePath)
		{		
			if (scene != nullptr)
			{
				json sceneObjectsJsonArray;
				std::vector<GameObject>& sceneObjects = scene->GetSceneObjects();
				if (sceneObjects.size() > 0)
				{
					for (GameObject& sceneObject : sceneObjects)
					{
						if (!sceneObject.IsPrefabChild())
						{
							sceneObjectsJsonArray.push_back(sceneObject.GetData());
						}
					}
				}

				json sceneJson = json::object({ {"Scene GameObjects", sceneObjectsJsonArray } });
				JsonHelper::WriteJsonToFile(sceneJson, filePath);
			}
		}

		void SaveTempScene(std::string sceneName)
		{
			if (sceneName == "")
				sceneName = loadedScene.name;
			
			SaveScene(&loadedScene, "../engine/tempFiles/" + sceneName + ".scn");
		}

		void SaveCurrentScene()
		{
			SaveScene(&loadedScene, loadedScene.path);
		}

		void CreateSceneBackup()
		{
			std::string copySceneTo = "../engine/tempFiles/" + loadedScene.name + "_backup_copy.scn";						
			FileHelper::CopyFileFL(loadedScene.path, copySceneTo);			
		}

		// Waits until EndImGuiRender() has been called in Application.cpp to load the next scene
		void QueueLoadScene(std::string scenePath)
		{
			F_b_loadNewScene = true;
			F_sceneToBeLoaded = scenePath;
		}

		// actualPath is the actual scene path we want to load from, pointTo is the scene path that will be considered the currently loaded scene path. Useful for loading temporary scene copies and not saving the temp scene file location as last scene loaded
		bool LoadScene(std::string actualPath, std::string pointTo)
		{
			if (!FileHelper::DoesFileExist(actualPath))
			{
				Logger::log.Err("Failed to load scene. Scene does not exist.");
				Logger::log.Err("Path: {}", actualPath);
				return false;
			}
			
			SceneManager::loadedScene.RemovePrimaryCamera();
			loadedScene.Unload();
			AudioManager::soundController.StopMusic();
			VulkanManager::vulkan.ClearGroupedByMaterialMeshes();
			SceneView::cameraSceneRenderObjects.Clear();
			PhysicsManager::physics2D.Shutdown();
			PhysicsManager::physics2D.Init();

			std::string pointToPath = pointTo != "" ? pointTo : actualPath;
			json fileContentJson = JsonHelper::LoadFileData(actualPath);

			loadedScene = Scene();
			loadedScenePath = pointToPath;
			loadedScene.path = pointToPath;
			loadedScene.name = FileHelper::GetFilenameFromPath(pointToPath, false);
			std::vector<json> prefabsJson = std::vector<json>();

			if (fileContentJson.contains("Scene GameObjects") && fileContentJson.at("Scene GameObjects").size())
			{
				auto sceneObjectsjson = fileContentJson.at("Scene GameObjects");
				
				for (auto objectJson : sceneObjectsjson)
				{																	
					if (JsonHelper::CheckJsonBool(objectJson, "b_isPrefab", "GameObject"))
					{
						prefabsJson.push_back(objectJson);
					}
					else
					{
						GameObject loadedObject = GameObject(JsonHelper::CheckJsonLong(objectJson, "parent", "GameObject"), JsonHelper::CheckJsonLong(objectJson, "id", "GameObject"));						
						GameObject* objectPtr = loadedScene.AddSceneObject(loadedObject);							
						objectPtr->PutData(objectJson);
					}
				}

				// Create prefabs after regular objects so that prefab children don't steal "unused" GameObject IDs from regular objects and then get overwritten by those objects
				for (json objectJson : prefabsJson)
				{
					GameObject loadedObject = GameObject(JsonHelper::CheckJsonLong(objectJson, "parent", "GameObject"), JsonHelper::CheckJsonLong(objectJson, "id", "GameObject"));						
					GameObject* objectPtr = loadedScene.AddSceneObject(loadedObject);							
					objectPtr->PutData(objectJson);														
				}

				loadedScene.SortSceneObjects();
				loadedScene.CreateJoints();
				F_Application->OnLoadScene(pointToPath);
			}

			return true;
		}

		Scene* GetLoadedScene()
		{
			return &loadedScene;
		}

		void SetLoadedScenePath(std::string filePath)
		{
			loadedScenePath = filePath;
		}
		
		std::string GetLoadedScenePath()
		{
			return loadedScenePath;
		}
		
		// void SaveAnimationPreviewObjects()
		// {
		// 	if (loadedScene.GetAnimatorPreviewObjects().size() > 0)
		// 	{
		// 		m_animatorPreviewObjects = loadedScene.GetAnimatorPreviewObjects();
		// 	}
		// }
		
		// void LoadAnimationPreviewObjects()
		// {
		// 	loadedScene.SetAnimatorPreviewObjects(m_animatorPreviewObjects);
		// }
		
		std::string CreateNewSceneFile(std::string filename, std::string path)
		{
			Scene newScene = Scene();
			newScene.name = filename;
			std::string filePath = "";

			if (path == "")
			{
				filePath = "../projects/" + FileHelper::GetFilenameFromPath(FL::ProjectManager::loadedProject.path) + "/scenes/" + filename + ".scn";
			}
			else
			{
				filePath = path + "/" + filename + ".scn";
			}

			newScene.path = filePath;

			SceneManager::SaveScene(&newScene, filePath);

			return filePath;
		}
	}
}