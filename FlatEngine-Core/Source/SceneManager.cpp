#include "SceneManager.h"
#include "FlatEngine.h"
#include "Project.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <SDL.h>


namespace FlatEngine
{
	SceneManager::SceneManager()
	{
		m_loadedScene = Scene();
		m_loadedScenePath = "";
		m_animatorPreviewObjects = std::vector<GameObject*>();
	}

	SceneManager::~SceneManager()
	{
	}

	void SceneManager::SetLoadedScene(Scene scene)
	{
		m_loadedScene = scene;
		m_loadedScenePath = scene.GetPath();
	}

	Scene *SceneManager::CreateAndLoadNewScene()
	{
		m_loadedScene = Scene();
		return &m_loadedScene;
	}

	void SceneManager::SaveScene(Scene *scene, std::string filePath)
	{		
		if (scene != nullptr)
		{
			scene->SetPath(filePath);
			std::ofstream file_obj;
			std::ifstream ifstream(filePath);

			file_obj.open(filePath, std::ofstream::out | std::ofstream::trunc);
			file_obj.close();

			file_obj.open(filePath, std::ios::app);
			json sceneObjectsJsonArray;

			std::map<long, GameObject>& sceneObjects = scene->GetSceneObjects();
			if (sceneObjects.size() > 0)
			{
				for (std::map<long, GameObject>::iterator iter = sceneObjects.begin(); iter != sceneObjects.end(); iter++)
				{
					if (!iter->second.IsPrefabChild())
					{
						sceneObjectsJsonArray.push_back(CreateJsonFromObject(iter->second));
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

	void SceneManager::SaveTempScene()
	{
		std::string startedPersistantScenePath = GetLoadedProject().GetPersistantGameObjectsScenePath();
		SaveScene(GetLoadedScene(), "..\\engine\\tempFiles\\" + GetLoadedScene()->GetName() + ".scn");
		if (startedPersistantScenePath != "" && GetLoadedProject().GetPersistantGameObjectScene() != nullptr)
		{
			F_LoadedProject.SavePersistantScene("..\\engine\\tempFiles\\" + GetLoadedProject().GetPersistantGameObjectScene()->GetName() + ".scn");
		}
	}

	void SceneManager::SaveCurrentScene()
	{
		if (m_loadedScenePath != "")
		{
			SaveScene(&m_loadedScene, m_loadedScenePath);
		}
	}

	bool SceneManager::LoadScene(std::string loadFrom, std::string pointTo)
	{
		std::string pointToPath = "";
		if (pointTo != "")
		{
			pointToPath = pointTo;
		}
		else
		{
			pointToPath = loadFrom;
		}
		bool b_success = true;
			
		m_loadedScene.UnloadECSManager();
		m_loadedScene.UnloadSceneObjects();

		std::ofstream file_obj;
		std::ifstream ifstream(loadFrom);

		file_obj.open(loadFrom, std::ios::in);
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
				m_loadedScene = Scene();
				m_loadedScenePath = pointToPath;
				m_loadedScene.SetPath(pointToPath);
				m_loadedScene.SetName(GetFilenameFromPath(pointToPath, false));

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

								if (CheckJsonBool(objectJson, "_isPrefab", "GameObject"))
								{
									prefabsJson.push_back(objectJson);
								}
								else
								{
									CreateObjectFromJson(objectJson, &m_loadedScene);
								}
							}
							catch (const json::out_of_range& e)
							{
								LogError(e.what());
							}
						}

						// Create prefabs after regular objects so that prefab children don't steal "unused" GameObject IDs from regular objects and then get overwritten by those objects
						for (json objectJson : prefabsJson)
						{
							try
							{
								CreateObjectFromJson(objectJson, &m_loadedScene);
							}
							catch (const json::out_of_range& e)
							{
								LogError(e.what());
							}
						}

						// Just in case any parent objects had not been created at the time of children being created on scene load,
						// loop through objects with parents and add them as children to their parent objects
						for (std::pair<long, GameObject> sceneObject : GetLoadedScene()->GetSceneObjects())
						{
							long myID = sceneObject.first;
							long parentID = sceneObject.second.GetParentID();

							if (parentID != -1)
							{
								if (GetObjectByID(parentID) != nullptr)
								{
									GetObjectByID(parentID)->AddChild(myID);
								}
							}
						}

						m_loadedScene.SortSceneObjects();
						m_loadedScene.CreateJoints();

						F_Application->OnLoadScene(pointToPath);
					}
				}
				catch (const json::out_of_range& e)
				{
					LogError(e.what());
				}
			}
			catch (json::exception err)
			{
				LogError(err.what());
			}
		}
		else
		{
			LogError("Failed to load scene: " + pointToPath);
			b_success = false;
		}

		return b_success;
	}

	Scene* SceneManager::GetLoadedScene()
	{
		return &m_loadedScene;
	}

	void SceneManager::SetLoadedScenePath(std::string filePath)
	{
		m_loadedScenePath = filePath;
	}
	
	std::string SceneManager::GetLoadedScenePath()
	{
		return m_loadedScenePath;
	}
	
	void SceneManager::SaveAnimationPreviewObjects()
	{
		if (m_loadedScene.GetAnimatorPreviewObjects().size() > 0)
		{
			m_animatorPreviewObjects = m_loadedScene.GetAnimatorPreviewObjects();
		}
	}
	
	void SceneManager::LoadAnimationPreviewObjects()
	{
		m_loadedScene.SetAnimatorPreviewObjects(m_animatorPreviewObjects);
	}
}