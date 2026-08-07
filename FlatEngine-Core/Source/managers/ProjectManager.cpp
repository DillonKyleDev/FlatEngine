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

#include <fstream>

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
			persistentScriptPath = "";
			sceneToLoadAtRuntime = "";
			persistentScript = Script();
			focusedGameObjectID = -1;
			b_autoSave = true;
			m_musicVolume = 10;
			m_effectsVolume = 10;
		}

		void Project::SavePersistentScript(std::string path)
		{
			std::string scriptPath = persistentScriptPath;
			if (path != "")
			{
				scriptPath = path;
			}
			std::ofstream file_obj;
			std::ifstream ifstream(scriptPath);
			file_obj.open(scriptPath, std::ofstream::out | std::ofstream::trunc);
			file_obj.close();
			file_obj.open(scriptPath, std::ios::app);
			json newFileObject = json::object({ {"Persistent Script", persistentScript.GetData() } });
			file_obj << newFileObject.dump(4).c_str() << std::endl;
			file_obj.close();
		}

		void Project::LoadPersistentScript(std::string path)
		{
			std::string scriptPath = path != "" ? path : persistentScriptPath;

			if (scriptPath != "")
			{
				std::ofstream file_obj;
				std::ifstream ifstream(scriptPath);
				file_obj.open(scriptPath, std::ios::in);
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
					json fileContentJson = json::parse(fileContent);

					if (fileContentJson.contains("Persistent Script"))
					{		
						persistentScript.PutData(fileContentJson["Persistent Script"], "Persistant Script");
					}
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

		const int Project::GetEffectsVolume()
		{
			return m_effectsVolume;
		}

		void Project::UpdateSavedTime()
		{
			time_t currentTime;
			time(&currentTime);
			m_timeSinceSave = Time::GetTMStructFromTimeStamp(currentTime);
		}

		const tm Project::GetSavedTime()
		{
			return m_timeSinceSave;
		}

		void LoadProject(std::string path)
		{			 
			Scene newScene = Scene();
			SceneManager::SetLoadedScene(std::move(newScene));

			Project newProject = Project();
			newProject.path = path;
			
			std::ofstream fileObject;
			std::ifstream ifstream(path);

			fileObject.open(path, std::ios::in);		
			std::string fileContent = "";
			
			if (fileObject.good())
			{
				std::string line;
				while (!ifstream.eof()) 
				{
					std::getline(ifstream, line);
					fileContent.append(line + "\n");
				}
			}
			
			fileObject.close();

			if (fileObject.good())
			{		
				json projectJson = json::parse(fileContent);

				if (JsonHelper::JsonContains(projectJson, "Project Properties", "Project Properties"))
				{								
					try
					{
						json projectData = projectJson["Project Properties"];
						std::string name = FileHelper::GetFilenameFromPath(path);

						newProject.path = JsonHelper::CheckJsonString(projectData, "path", name);
						newProject.loadedScenePath = JsonHelper::CheckJsonString(projectData, "loadedScenePath", name);
						newProject.buildPath = JsonHelper::CheckJsonString(projectData, "buildPath", name);
						newProject.loadedAnimationPath = JsonHelper::CheckJsonString(projectData, "loadedAnimationPath", name);
						newProject.focusedGameObjectID = JsonHelper::CheckJsonLong(projectData, "focusedGameObjectID", name);
						newProject.sceneToLoadAtRuntime = JsonHelper::CheckJsonString(projectData, "sceneToLoadAtRuntime", name);						
						newProject.b_autoSave = JsonHelper::CheckJsonBool(projectData, "b_autoSave", name);
						newProject.SetMusicVolume(JsonHelper::CheckJsonInt(projectData, "musicVolume", name));
						newProject.SetEffectsVolume(JsonHelper::CheckJsonInt(projectData, "effectsVolume", name));
						newProject.currentFileDirectory = JsonHelper::CheckJsonString(projectData, "currentFileDirectory", name);						
						newProject.persistentScript.PutData(JsonHelper::JsonContains(projectData, "persistentScript", name) ? projectData["persistentScript"] : json::object(), "Persistent Script");						
						newProject.persistentScript.SetOwnerID(PERSISTENT_SCRIPT_ID);
						newProject.persistentScript.SetActive(JsonHelper::CheckJsonBool(projectData, "b_persistentScriptActive", name));
					}
					catch (const json::out_of_range& e)
					{
						Logger::log.Err("{}", e.what());
					}
				}
			}
			
			loadedProject = newProject;
			Assets::assetManager.CollectDirectories();		
			Assets::assetManager.UpdateProjectDirs(loadedProject.path);	
			FL::VulkanManager::vulkan.InitializeMaterials();
			// InitializeTileSets();		
			PrefabManager::InitializePrefabs();	
			LuaManager::RetrieveLuaScriptPaths();		
			// RetrieveCPPScriptNames();
			Controls::InitializeMappingContexts();		

			if (loadedProject.loadedScenePath != "")
			{
				SceneManager::LoadScene(loadedProject.loadedScenePath);
			}
			else
			{
				Logger::log.Info("No project scene to load.");
			}

		}

		void CreateNewProject(std::string projectName)
		{
			SaveProject(loadedProject, loadedProject.path);

			if (projectName != "")
			{
				Project newProject = Project();
				std::string directoryPath = "../ProjectManager/" + projectName;
				std::string projectFilePath = directoryPath + "/" + projectName + ".prj";
				std::string persistentScriptPath = directoryPath + "/persistent_scripts_" + projectName + ".scn";

				CreateProjectDirectory(directoryPath);

				newProject.persistentScriptPath = persistentScriptPath;				

				SaveProject(newProject, projectFilePath);
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

		void SaveProject(Project& project, std::string path)
		{		
			std::ofstream file_obj;
			std::ifstream ifstream(path);

			// Delete old file contents
			file_obj.open(path, std::ofstream::out | std::ofstream::trunc);
			file_obj.close();
			file_obj.open(path, std::ios::app);			
			
			project.UpdateSavedTime();
			tm timeSaved = project.GetSavedTime();

			json properties = json::object({
				{ "path", path },
				{ "loadedScenePath", project.loadedScenePath},
				{ "loadedAnimationPath", project.loadedAnimationPath },
				{ "sceneToLoadAtRuntime", project.sceneToLoadAtRuntime },
				{ "buildPath", project.buildPath },
				{ "b_autoSave", project.b_autoSave },
				{ "musicVolume", project.GetMusicVolume() },
				{ "effectsVolume", project.GetEffectsVolume() },
				{ "currentFileDirectory", loadedProject.currentFileDirectory },
				{ "focusedGameObjectID", loadedProject.focusedGameObjectID },
				{ "yearsSinceSave", timeSaved.tm_year },
				{ "monthsSinceSave", timeSaved.tm_mon },
				{ "daysSinceSave", timeSaved.tm_mday },
				{ "hoursSinceSave", timeSaved.tm_hour },
				{ "minutesSinceSave", timeSaved.tm_min },
				{ "secondsSinceSave", timeSaved.tm_sec },
				{ "persistentScript", project.persistentScript.GetData() },
				{ "b_persistentScriptActive", project.persistentScript.IsActive() }
			});

			json newFileObject = json::object({ {"Project Properties", properties } });
			file_obj << newFileObject.dump(4).c_str() << std::endl;
			file_obj.close();
		}

		void SaveCurrentProject()
		{
			SaveProject(loadedProject, loadedProject.path);
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