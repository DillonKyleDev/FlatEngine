#include "components/Script.h"
#include "managers/Assets.h"
#include "managers/Controls.h"
#include "managers/PrefabManager.h"
#include "managers/ProjectManager.h"
#include "managers/SceneManager.h"
#include "render/VulkanManager.h"
#include "tools/FileHelper.h"
#include "tools/JsonHelper.h"
#include "tools/Logger.h"
#include "tools/Time.h"


namespace FL = FlatEngine;


namespace FlatEngine
{
	namespace ProjectManager
	{
		const int PERSISTENT_SCRIPT_ID = -1000;
		bool b_projectSelected = false;
		Project loadedProject = Project();		
		std::map<std::string, FL::Text> projectNameTexts = std::map<std::string, FL::Text>();

		Project::Project()
		{
			path = "";
			loadedScenePath = "";
			loadedAnimationPath = "";
			currentFileDirectory = "../";
			sceneToLoadAtRuntime = "";
			b_autoSave = true;
			m_musicVolume = 10;
			m_effectsVolume = 10;
			lastFocusedID = -1;
		}

		json Project::GetData()
		{
			UpdateSavedTime();
			tm timeSaved = GetSavedTime();

			json projectJson = json::object({		
				{ "path", path },		
				{ "yearsSinceSave", timeSaved.tm_year },
				{ "monthsSinceSave", timeSaved.tm_mon },
				{ "daysSinceSave", timeSaved.tm_mday },
				{ "hoursSinceSave", timeSaved.tm_hour },
				{ "minutesSinceSave", timeSaved.tm_min },
				{ "secondsSinceSave", timeSaved.tm_sec },
				{ "loadedScenePath", loadedScenePath},
				{ "loadedAnimationPath", loadedAnimationPath },
				{ "sceneToLoadAtRuntime", sceneToLoadAtRuntime },
				{ "buildPath", buildPath },
				{ "b_autoSave", b_autoSave },
				{ "musicVolume", GetMusicVolume() },
				{ "effectsVolume", GetEffectsVolume() },
				{ "currentFileDirectory", currentFileDirectory },				
				{ "b_persistentScriptActive", persistentScript.IsActive() },
				{ "persistentScript", persistentScript.GetData() }
			});

			json focusedIDs = json::array();
			for (long ID : focusedGameObjectIDs)
			{
				focusedIDs.push_back(ID);
			}
			projectJson.emplace("focusedIDs", focusedIDs);

			return projectJson;
		}

		void Project::PutData(json projectJson)
		{
			if (JsonHelper::JsonContains(projectJson, "Project Properties", "Project Properties"))
			{								
				try
				{
					json projectData = projectJson["Project Properties"];
					std::string name = FileHelper::GetFilenameFromPath(path);
					path = JsonHelper::CheckJsonString(projectData, "path", name);
					loadedScenePath = JsonHelper::CheckJsonString(projectData, "loadedScenePath", name);
					buildPath = JsonHelper::CheckJsonString(projectData, "buildPath", name);
					loadedAnimationPath = JsonHelper::CheckJsonString(projectData, "loadedAnimationPath", name);						
					sceneToLoadAtRuntime = JsonHelper::CheckJsonString(projectData, "sceneToLoadAtRuntime", name);						
					b_autoSave = JsonHelper::CheckJsonBool(projectData, "b_autoSave", name);
					SetMusicVolume(JsonHelper::CheckJsonInt(projectData, "musicVolume", name));
					SetEffectsVolume(JsonHelper::CheckJsonInt(projectData, "effectsVolume", name));
					currentFileDirectory = JsonHelper::CheckJsonString(projectData, "currentFileDirectory", name);						
					persistentScript.PutData(JsonHelper::JsonContains(projectData, "persistentScript", name) ? projectData["persistentScript"] : json::object(), "Persistent Script");						
					persistentScript.SetOwnerID(PERSISTENT_SCRIPT_ID);
					persistentScript.SetActive(JsonHelper::CheckJsonBool(projectData, "b_persistentScriptActive", name));

					if (JsonHelper::JsonContains(projectData, "focusedIDs", "Project Properties"))
					{
						json focusedIDsJson = projectData.at("focusedIDs");

						for (int i = 0; i < focusedIDsJson.size(); i++)
							AddFocusedObjectID(focusedIDsJson[i]);							
					}
				}
				catch (const json::out_of_range& e)
				{
					Logger::log.Err("{}", e.what());
				}
			}
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
			m_timeSinceSave = Time::GetTMStructFromTimeStamp(currentTime);
		}

		tm Project::GetSavedTime()
		{
			return m_timeSinceSave;
		}

		void Project::AddFocusedObjectID(long ID)
		{
			focusedGameObjectIDs.push_back(ID);
			lastFocusedID = ID;
		}

		void Project::RemoveFocusedObjectID(long ID)
		{
			for (std::vector<long>::iterator iter = focusedGameObjectIDs.begin(); iter != focusedGameObjectIDs.end(); iter++)
			{
				if (*iter == ID)
				{
					focusedGameObjectIDs.erase(iter);
					
					if (lastFocusedID == ID && focusedGameObjectIDs.size())
						lastFocusedID = focusedGameObjectIDs.back();
					
					return;
				}
			}
		}

		bool Project::IsIDFocused(long ID)
		{
			for (std::vector<long>::iterator iter = focusedGameObjectIDs.begin(); iter != focusedGameObjectIDs.end(); iter++)
			{
				if (*iter == ID)
				{
					return true;
				}
			}

			return false;
		}

		void Project::RefocusID(long withID)
		{
			for (std::vector<long>::iterator iter = focusedGameObjectIDs.begin(); iter != focusedGameObjectIDs.end(); iter++)
			{
				if (*iter == lastFocusedID)
				{
					*iter = withID;
					lastFocusedID = withID;
					return;					
				}
			}

			// otherwise...
			AddFocusedObjectID(withID);			
		}


		// Project Manager
		void LoadProject(std::string path)
		{			 
			if (!FileHelper::DoesFileExist(path))
			{
				Logger::log.Err("ProjectManager::LoadProject() : Path {} does not exist.", path);
				return;
			}

			Scene newScene = Scene();
			SceneManager::SetLoadedScene(std::move(newScene));

			json projectJson = JsonHelper::LoadFileData(path);
			loadedProject = Project();
			loadedProject.PutData(projectJson);
	
			Assets::assetManager.CollectDirectories();		
			Assets::assetManager.UpdateProjectDirs(loadedProject.path);	
			FL::VulkanManager::vulkan.InitializeMaterials();			
			PrefabManager::InitializePrefabs();	
			LuaManager::RetrieveLuaScriptPaths();					
			Controls::InitializeMappingContexts();		

			if (loadedProject.loadedScenePath != "")
			{
				SceneManager::LoadScene(loadedProject.loadedScenePath);
			}
			else
			{
				Logger::log.Info("ProjectManager::LoadProject() : No project scene to load for Project: {}.", loadedProject.path);
			}
		}

		void CreateNewProject(std::string projectName)
		{
			SaveProject(&loadedProject, loadedProject.path);

			if (projectName != "")
			{
				Project newProject = Project();
				std::string directoryPath = "../ProjectManager/" + projectName;
				std::string projectFilePath = directoryPath + "/" + projectName + ".prj";
				std::string persistentScriptPath = directoryPath + "/persistent_scripts_" + projectName + ".scn";

				CreateProjectDirectory(directoryPath);	

				SaveProject(&newProject, projectFilePath);
				LoadProject(projectFilePath);
			}
		}

		void CreateProjectDirectory(std::string path)
		{
			std::filesystem::create_directory(path);
			std::filesystem::create_directory(path + "/animations");
			std::filesystem::create_directory(path + "/audio");
			std::filesystem::create_directory(path + "/images");
			std::filesystem::create_directory(path + "/images/tileTextures");
			std::filesystem::create_directory(path + "/mappingContexts");
			std::filesystem::create_directory(path + "/prefabs");
			std::filesystem::create_directory(path + "/scenes");
			std::filesystem::create_directory(path + "/scripts");
			std::filesystem::create_directory(path + "/scripts/cpp");
			std::filesystem::create_directory(path + "/scripts/lua");
			std::filesystem::create_directory(path + "/tileSets");
			std::filesystem::create_directory(path + "/materials");
			std::filesystem::create_directory(path + "/models");
			std::filesystem::create_directory(path + "/shaders");
			std::filesystem::create_directory(path + "/compiledShaders");
			// TODO: Copy shader compilation files into shaders directory
		}

		void SaveProject(Project* project, std::string path)
		{		
			if (!FileHelper::DoesFileExist(path))
			{
				Logger::log.Err("ProjectManager::SaveProject() : Path {} does not exist.", path);
				return;
			}

			loadedProject.path = path;
			json projectJson = json::object({ {"Project Properties", loadedProject.GetData() } });
			JsonHelper::WriteJsonToFile(projectJson, path);
		}

		void SaveCurrentProject()
		{
			SaveProject(&loadedProject, loadedProject.path);
		}

		void BuildProject()
		{
			if (loadedProject.buildPath != "")
			{
				std::string rootPath = Assets::assetManager.GetRootPath();

				try
				{				
					std::filesystem::create_directories(loadedProject.buildPath);
					std::filesystem::copy(rootPath + "/Build/windows-x86_64/Release/FlatEngine-Core", loadedProject.buildPath + "/Core", std::filesystem::copy_options::overwrite_existing | std::filesystem::copy_options::recursive);
				}
				catch (std::exception& e)
				{
					Logger::log.Err("Failed to copy FlatEngine-Core : ");
					Logger::log.Err("{}", e.what());
				}
				try
				{
					std::filesystem::copy(rootPath + "/Build/windows-x86_64/Release/FlatEngine-Runtime", loadedProject.buildPath + "/Runtime", std::filesystem::copy_options::overwrite_existing | std::filesystem::copy_options::recursive);
				}
				catch (std::exception& e)
				{
					Logger::log.Err("Failed to copy FlatEngine-Runtime : ");
					Logger::log.Err("{}", e.what());
				}
				try
				{
					std::string existingProjDir = rootPath + "/ProjectManager/" + FileHelper::GetFilenameFromPath(loadedProject.path);
					std::string buildProjDir = loadedProject.buildPath + "/ProjectManager/" + FileHelper::GetFilenameFromPath(loadedProject.path);
					std::filesystem::create_directories(buildProjDir);
					std::filesystem::copy(existingProjDir, buildProjDir, std::filesystem::copy_options::overwrite_existing | std::filesystem::copy_options::recursive);
				}
				catch (std::exception& e)
				{
					Logger::log.Err("Failed to copy assets : ");
					Logger::log.Err("{}", e.what());
				}
				try
				{
					std::filesystem::copy(rootPath + "/engine", loadedProject.buildPath + "/engine", std::filesystem::copy_options::overwrite_existing | std::filesystem::copy_options::recursive);
				}
				catch (std::exception& e)
				{
					Logger::log.Err("Failed to copy engine dependencies: ");
					Logger::log.Err("{}", e.what());
				}
				try
				{
					std::filesystem::copy(rootPath + "/Build/Intermediates", loadedProject.buildPath + "/Intermediates", std::filesystem::copy_options::overwrite_existing | std::filesystem::copy_options::recursive);
				}
				catch (std::exception& e)
				{
					Logger::log.Err("Failed to copy intermediates: ");
					Logger::log.Err("{}", e.what());
				}

				Logger::log.Info("Project built to: {}", loadedProject.buildPath);
			}
		}

		void SetProjectLoadedScenePath(std::string scenePath)
		{
			loadedProject.loadedScenePath = scenePath;
		}

		std::vector<std::string> RetrieveProjectPaths()
		{
			std::vector<std::string> projectPaths = std::vector<std::string>();

			for (std::string projectPath : FL::FileHelper::FindAllFilesWithExtension("../projects", ".prj"))
			{
				projectPaths.push_back(projectPath);
			}

			return projectPaths;
		}

		void RecreateProjectNameTexts()
		{
			std::vector<std::string> projectPaths = RetrieveProjectPaths();
			projectNameTexts.clear();

			for (int i = 0; i < projectPaths.size(); i++)
			{
				std::string projectName = FL::FileHelper::GetFilenameFromPath(projectPaths[i], true);
				FL::Text projectNameText = FL::Text();
				std::pair<std::string, FL::Text> textPair = { projectName, projectNameText };
				projectNameTexts.emplace(textPair);
				projectNameTexts.at(projectName).SetFontPath("../engine/fonts/Karla-Regular.ttf");
				projectNameTexts.at(projectName).SetFontSize(24);
				projectNameTexts.at(projectName).SetText(projectName);
			}
		}

		bool ProjectTimeCustomComp(std::string path1, std::string path2)
		{
			tm timeStruct1 = FL::Time::GetProjectTimeStruct(path1);
			tm timeStruct2 = FL::Time::GetProjectTimeStruct(path2);
			time_t timeStamp1 = FL::Time::CreateTimeStamp(timeStruct1);
			time_t timeStamp2 = FL::Time::CreateTimeStamp(timeStruct2);

			return FL::Time::CompareTimeStamps(timeStamp1, timeStamp2);		
		}
	}
}