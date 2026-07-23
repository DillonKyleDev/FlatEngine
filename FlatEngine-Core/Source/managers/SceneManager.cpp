#include "FlatEngine.h"
#include "managers/AudioManager.h"
#include "managers/SceneManager.h"
#include "managers/ProjectManager.h"
#include "render/VulkanManager.h"
#include "tools/FileHelper.h"
#include "tools/JsonHelper.h"
#include "tools/Logger.h"

#include <fstream>


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
				scene->path = filePath;
				std::ofstream file_obj;
				std::ifstream ifstream(filePath);

				file_obj.open(filePath, std::ofstream::out | std::ofstream::trunc);
				file_obj.close();

				file_obj.open(filePath, std::ios::app);
				json sceneObjectsJsonArray;

				std::vector<GameObject>& sceneObjects = scene->GetSceneObjects();
				if (sceneObjects.size() > 0)
				{
					for (GameObject sceneObject : sceneObjects)
					{
						if (!sceneObject.IsPrefabChild())
						{
							sceneObjectsJsonArray.push_back(JsonHelper::CreateJsonFromObject(&sceneObject));
						}
					}
				}
				else
				{
					sceneObjectsJsonArray.push_back("NULL");
				}

				json newFileObject = json::object({ {"Scene GameObjects", sceneObjectsJsonArray } });
				file_obj << newFileObject.dump(4).c_str() << std::endl;
				file_obj.close();
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
			AudioManager::soundController.StopMusic();
			VulkanManager::vulkan.ClearGroupedByMaterialMeshes();

			std::string pointToPath = "";
			if (pointTo != "")
			{
				pointToPath = pointTo;
			}
			else
			{
				pointToPath = actualPath;
			}
			
			bool b_success = true;
				
			loadedScene.Unload();

			std::ofstream file_obj;
			std::ifstream ifstream(actualPath);

			file_obj.open(actualPath, std::ios::in);
			std::string fileContent = "";

			if (file_obj.good())
			{
				std::string line;
				while (!ifstream.eof()) 
				{
					std::getline(ifstream, line);
					if (line != "")
					{
						fileContent.append(line + "\n");
					}
				}
			}

			file_obj.close();
			if (file_obj.good() && fileContent != "")
			{
				try
				{
					loadedScene = Scene();
					loadedScenePath = pointToPath;
					loadedScene.path = pointToPath;
					loadedScene.name = FileHelper::GetFilenameFromPath(pointToPath, false);

					json fileContentJson = json::parse(fileContent);
					std::vector<json> prefabsJson = std::vector<json>();

					try
					{
						if (fileContentJson.contains("Scene GameObjects") && fileContentJson.at("Scene GameObjects").at(0) != "NULL")
						{
							auto sceneObjectsjson = fileContentJson.at("Scene GameObjects");
							
							for (int i = 0; i < sceneObjectsjson.size(); i++)
							{												
								try
								{
									json objectJson = fileContentJson.at("Scene GameObjects").at(i);

									if (JsonHelper::CheckJsonBool(objectJson, "_isPrefab", "GameObject"))
									{
										prefabsJson.push_back(objectJson);
									}
									else
									{
										JsonHelper::CreateObjectFromJson(objectJson, &loadedScene, nullptr);						
									}
								}
								catch (const json::out_of_range& e)
								{
									Logger::log.Err("{}", e.what());
								}
							}

							// Create prefabs after regular objects so that prefab children don't steal "unused" GameObject IDs from regular objects and then get overwritten by those objects
							for (json objectJson : prefabsJson)
							{
								try
								{
									JsonHelper::CreateObjectFromJson(objectJson, &loadedScene, nullptr);
								}
								catch (const json::out_of_range& e)
								{
									Logger::log.Err("{}", e.what());
								}
							}

							// Just in case any parent objects had not been created at the time of children being created on scene load,
							// loop through objects with parents and add them as children to their parent objects
							for (GameObject& sceneObject : loadedScene.GetSceneObjects())
							{
								long myID = sceneObject.GetID();
								long parentID = sceneObject.GetParentID();

								if (parentID != -1)
								{
									if (loadedScene.GetObjectByID(parentID) != nullptr)
									{
										loadedScene.GetObjectByID(parentID)->AddChild(myID);
									}
								}
							}

							loadedScene.SortSceneObjects();
							loadedScene.CreateJoints();

							F_Application->OnLoadScene(pointToPath);
						}
					}
					catch (const json::out_of_range& e)
					{
						Logger::log.Err("{}", e.what());
					}
				}
				catch (json::exception err)
				{
					Logger::log.Err("{}", err.what());
				}
			}
			else
			{
				Logger::log.Err("Failed to load scene: {}", pointToPath);
				b_success = false;
			}

			return b_success;
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