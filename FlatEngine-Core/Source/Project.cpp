#include "Project.h"
#include "FlatEngine.h"
#include "WindowManager.h"

#include "SDL_mixer.h"
#include "json.hpp"
#include <SDL.h>
#include <fstream>


using json = nlohmann::json;
using namespace nlohmann::literals;


namespace FlatEngine
{
	Project::Project()
	{
		m_path = "";
		m_loadedScenePath = "";
		m_loadedAnimationPath = "";
		m_currentFileDirectory = "";
		m_persistantGameObjectsScenePath = "";
		m_persistantGameObjectsScene = Scene();
		m_sceneViewScrolling = Vector2(0, 0);
		m_focusedGameObjectID = -1;
		m_b_autoSave = true;
		m_resolution = Vector2(1920, 1080);
		m_b_vsyncEnabled = true;
		m_b_fullscreen = false;
		m_musicVolume = 10;
		m_effectsVolume = 10;
	}

	Project::~Project()
	{
	}

	void Project::SetPath(std::string projectPath)
	{
		m_path = projectPath;
	}

	std::string Project::GetPath()
	{
		return m_path;
	}

	void Project::SetLoadedScenePath(std::string path)
	{
		m_loadedScenePath = path;
	}

	std::string Project::GetLoadedScenePath()
	{
		return m_loadedScenePath;
	}

	void Project::SetBuildPath(std::string path)
	{
		m_buildPath = path;
	}

	std::string Project::GetBuildPath()
	{
		return m_buildPath;
	}

	void Project::SetCurrentFileDirectory(std::string dirName)
	{
		m_currentFileDirectory = dirName;
	}

	std::string Project::GetCurrentFileDirectory()
	{
		return m_currentFileDirectory;
	}

	Scene* Project::GetPersistantGameObjectScene()
	{
		return &m_persistantGameObjectsScene;
	}

	void Project::SetPersistantGameObjectsScenePath(std::string path)
	{
		m_persistantGameObjectsScenePath = path;
	}

	std::string Project::GetPersistantGameObjectsScenePath()
	{
		return m_persistantGameObjectsScenePath;
	}

	void Project::CreateFreshPersistantScene()
	{
		m_persistantGameObjectsScene.UnloadSceneObjects();
		m_persistantGameObjectsScene.UnloadECSManager();
		m_persistantGameObjectsScene = Scene();
		// Set persistant gameobjects starting ID's at arbitrarily high values so they will never collide with the regular scene object ID's
		m_persistantGameObjectsScene.SetNextComponentID(10000000);
		m_persistantGameObjectsScene.SetNextGameObjectID(10000000);
	}

	void Project::SavePersistantScene(std::string filePath)
	{
		std::ofstream file_obj;
		std::ifstream ifstream(filePath);

		file_obj.open(filePath, std::ofstream::out | std::ofstream::trunc);
		file_obj.close();

		file_obj.open(filePath, std::ios::app);
		json sceneObjectsJsonArray;

		std::map<long, GameObject>& sceneObjects = m_persistantGameObjectsScene.GetSceneObjects();
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

		json newFileObject = json::object({ {"Persistant GameObjects", sceneObjectsJsonArray } });
		file_obj << newFileObject.dump(4).c_str() << std::endl;
		file_obj.close();
	}

	void Project::LoadPersistantScene(std::string actualPath)
	{
		if (F_LoadedProject.GetPersistantGameObjectsScenePath() != "")
		{
			std::string pathToLoad = actualPath;
			if (pathToLoad == "")
			{
				pathToLoad = m_persistantGameObjectsScenePath;
			}

			std::ofstream file_obj;
			std::ifstream ifstream(pathToLoad);

			file_obj.open(pathToLoad, std::ios::in);
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
				m_persistantGameObjectsScene.SetName(GetFilenameFromPath(m_persistantGameObjectsScenePath, false));
				m_persistantGameObjectsScene.SetPersistantScene(true);

				json fileContentJson = json::parse(fileContent);

				if (fileContentJson.contains("Persistant GameObjects") && fileContentJson["Persistant GameObjects"][0] != "NULL")
				{
					auto firstObjectName = fileContentJson["Persistant GameObjects"];
					std::vector<json> prefabsJson = std::vector<json>();

					// Loop through the saved GameObjects in the JSON file
					for (int i = 0; i < fileContentJson["Persistant GameObjects"].size(); i++)
					{
						if (CheckJsonBool(fileContentJson["Persistant GameObjects"][i], "_isPrefab", "GameObject"))
						{
							prefabsJson.push_back(fileContentJson["Persistant GameObjects"][i]);
						}
						else
						{						
							// Add created GameObject to our new Scene
							bool b_persistant = true;
							GameObject* loadedObject = CreateObjectFromJson(fileContentJson["Persistant GameObjects"][i], &m_persistantGameObjectsScene);
							// Check for primary camera
							if (loadedObject != nullptr && loadedObject->HasComponent("Camera") && loadedObject->GetCamera()->IsPrimary())
							{
								SetPrimaryCamera(loadedObject->GetCamera());
							}
						}
					}

					// Create prefabs after regular objects so that prefab children don't steal "unused" GameObject IDs from regular objects and then get overwritten by those objects
					for (json objectJson : prefabsJson)
					{
						try
						{
							CreateObjectFromJson(objectJson, &m_persistantGameObjectsScene);
						}
						catch (const json::out_of_range& e)
						{
							LogError(e.what());
						}
					}

					// Just in case any parent objects had not been created at the time of children being created on scene load,
					// loop through objects with parents and add them as children to their parent objects
					for (std::pair<long, GameObject> sceneObject : m_persistantGameObjectsScene.GetSceneObjects())
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
				}
			}
			else
			{
				LogError("Failed to load scene. Scene file empty: " + m_persistantGameObjectsScenePath);
			}
		}
	}

	void Project::SaveScene()
	{
		std::ofstream file_obj;
		std::ifstream ifstream(m_persistantGameObjectsScenePath);

		file_obj.open(m_persistantGameObjectsScenePath, std::ofstream::out | std::ofstream::trunc);
		file_obj.close();

		file_obj.open(m_persistantGameObjectsScenePath, std::ios::app);
		json persistantObjectsJsonArray;

		std::map<long, GameObject>& persistantObjects = m_persistantGameObjectsScene.GetSceneObjects();
		if (persistantObjects.size() > 0)
		{
			for (std::map<long, GameObject>::iterator iter = persistantObjects.begin(); iter != persistantObjects.end(); iter++)
			{
				persistantObjectsJsonArray.push_back(CreateJsonFromObject(iter->second));
			}
		}
		else
		{
			persistantObjectsJsonArray.push_back("NULL");
		}

		json newFileObject = json::object({ { "Persistant GameObjects", persistantObjectsJsonArray } });
		file_obj << newFileObject.dump(4).c_str() << std::endl;
		file_obj.close();
	}

	std::map<long, GameObject>& Project::GetPersistantObjects()
	{
		return m_persistantGameObjectsScene.GetSceneObjects();
	}

	void Project::SetFocusedGameObjectID(long ID)
	{
		m_focusedGameObjectID = ID;
	}

	long Project::GetFocusedGameObjectID()
	{
		return m_focusedGameObjectID;
	}

	void Project::SetLoadedPreviewAnimationPath(std::string path)
	{
		m_loadedAnimationPath = path;
	}

	std::string Project::GetLoadedPreviewAnimationPath()
	{
		return m_loadedAnimationPath;
	}

	std::string Project::GetRuntimeScene()
	{
		return m_sceneToLoadAtRuntime;
	}

	void Project::SetRuntimeScene(std::string scene)
	{
		m_sceneToLoadAtRuntime = scene;
	}

	void Project::SetSceneViewScrolling(Vector2 scrolling)
	{
		m_sceneViewScrolling = scrolling;
	}

	Vector2 Project::GetSceneViewScrolling()
	{
		return m_sceneViewScrolling;
	}

	void Project::SetSceneViewGridStep(Vector2 gridStep)
	{
		m_sceneViewGridStep = gridStep;
	}

	Vector2 Project::GetSceneViewGridStep()
	{
		return m_sceneViewGridStep;
	}

	bool Project::AutoSaveOn()
	{
		return m_b_autoSave;
	}

	void Project::SetAutoSave(bool b_autoSave)
	{
		m_b_autoSave = b_autoSave;
	}

	void Project::SetResolution(Vector2 newResolution)
	{
		m_resolution = newResolution;
		//Window::SetScreenDimensions(resolution.x, resolution.y);
		//SDL_bool setIntegerScale = SDL_bool(true);
		//SDL_RenderSetIntegerScale(Window::renderer, setIntegerScale);
		//SDL_RenderSetLogicalSize(Window::renderer, resolution.x, resolution.y);
	}

	Vector2 Project::GetResolution()
	{
		return m_resolution;
	}

	void Project::SetFullscreen(bool b_fullscreen)
	{
		m_b_fullscreen = b_fullscreen;
		F_Window->SetFullscreen(m_b_fullscreen);
	}

	bool Project::IsFullscreen()
	{
		return m_b_fullscreen;
	}

	void Project::SetVsyncEnabled(bool b_vsync)
	{
		m_b_vsyncEnabled = b_vsync;
		int interval = 0;

		if (m_b_vsyncEnabled)
		{
			interval = 1;
		}

		// vsync disabled -- 1 to activate
		SDL_RenderSetVSync(F_Window->GetRenderer(), interval);
	}

	bool Project::IsVsyncEnabled()
	{
		return m_b_vsyncEnabled;
	}

	void Project::SetMusicVolume(int volume)
	{
		if (volume >= 0 && volume <= MIX_MAX_VOLUME)
		{
			m_musicVolume = volume;
		}
	}

	int Project::GetMusicVolume()
	{
		return m_musicVolume;
	}

	void Project::SetEffectsVolume(int volume)
	{
		if (volume >= 0 && volume <= MIX_MAX_VOLUME)
		{
			m_effectsVolume = volume;
		}
	}

	int Project::GetEffectsVolume()
	{
		return m_effectsVolume;
	}

	void Project::UpdateSavedTime()
	{
		time_t currentTime;
		time(&currentTime);
		m_timeSinceSave = GetTMStructFromTimeStamp(currentTime);
	}

	tm Project::GetSavedTime()
	{
		return m_timeSinceSave;
	}
}