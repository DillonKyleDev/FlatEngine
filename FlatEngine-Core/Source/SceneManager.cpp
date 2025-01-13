#include "SceneManager.h"
#include "FlatEngine.h"

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

	Scene *SceneManager::CreateNewScene()
	{
		m_loadedScene = Scene();
		return &m_loadedScene;
	}

	void SceneManager::SaveScene(Scene *scene, std::string filePath)
	{		
		scene->SetPath(filePath);
		std::ofstream file_obj;
		std::ifstream ifstream(filePath);

		file_obj.open(filePath, std::ofstream::out | std::ofstream::trunc);
		file_obj.close();

		file_obj.open(filePath, std::ios::app);
		json sceneObjectsJsonArray;

		std::map<long, GameObject> &sceneObjects = scene->GetSceneObjects();
		if (sceneObjects.size() > 0)
		{
			for (std::map<long, GameObject>::iterator iter = sceneObjects.begin(); iter != sceneObjects.end(); iter++)
			{				
				sceneObjectsJsonArray.push_back(CreateJsonFromObject(iter->second));				
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

	void SceneManager::SaveCurrentScene()
	{
		if (m_loadedScenePath != "")
		{
			SaveScene(&m_loadedScene, m_loadedScenePath);
		}
		else
		{

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

		m_loadedScene.UnloadSceneObjects();
		m_loadedScene.UnloadECSManager();

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
			m_loadedScene = Scene();
			m_loadedScenePath = pointToPath;
			m_loadedScene.SetPath(pointToPath);
			m_loadedScene.SetName(GetFilenameFromPath(pointToPath, false));

			json fileContentJson = json::parse(fileContent);

			if (fileContentJson.contains("Scene GameObjects") && fileContentJson["Scene GameObjects"][0] != "NULL")
			{
				auto firstObjectName = fileContentJson["Scene GameObjects"];

				// Loop through the saved GameObjects in the JSON file
				for (int i = 0; i < fileContentJson["Scene GameObjects"].size(); i++)
				{
					// Add created GameObject to our freshScene
					GameObject *loadedObject = CreateObjectFromJson(fileContentJson["Scene GameObjects"][i], &m_loadedScene);
					// Check for primary camera
					if (loadedObject != nullptr && loadedObject->HasComponent("Camera") && loadedObject->GetCamera()->IsPrimary())
					{
						SetPrimaryCamera(loadedObject->GetCamera());
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

				F_Application->OnLoadScene(pointToPath);
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