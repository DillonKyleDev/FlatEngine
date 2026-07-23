#pragma once
#include "components/Script.h"
#include "components/Text.h"

#include <map>
#include <string>


namespace FlatEngine
{
	namespace ProjectManager
	{
		class Project
		{
		public:
			Project();
	
			void SavePersistantScript(std::string path = ""); // path for temp files when starting scene
			void LoadPersistantScript(std::string path = "");			
			Script& GetPersistantScript();
			void SetPersistantScript(Script persistantScript);
			void SetMusicVolume(int volume);
			int GetMusicVolume();
			void SetEffectsVolume(int volume);
			const int GetEffectsVolume();
			void UpdateSavedTime();
			const tm GetSavedTime();

			std::string path;
			std::string persistantScriptPath;
			std::string buildPath;
			std::string loadedScenePath;
			std::string sceneToLoadAtRuntime;
			std::string loadedAnimationPath;
			std::string currentFileDirectory;		
			long focusedGameObjectID;
			bool b_autoSave;
			Script persistantScript;

		private:
			int m_musicVolume;
			int m_effectsVolume;
			tm m_timeSinceSave;
		};
		
		extern bool b_projectSelected;
		extern Project loadedProject;		
		extern std::map<std::string, Text> projectNameTexts;
		
		extern void RecreateProjectNameTexts();
		extern std::vector<std::string> RetrieveProjectPaths();
		extern bool ProjectTimeCustomComp(std::string path1, std::string path2);
		extern void CreateNewProject(std::string projectName);
		extern void CreateProjectDirectory(std::string path);
		extern void LoadProject(std::string path);
		extern void SaveProject(Project& project, std::string path);
		extern void SaveCurrentProject();			
		extern void BuildProject();
		extern void SetProjectLoadedScenePath(std::string scenePath);
		extern long CreatePersistantGameObject(long parentID = -1, long myID = -1);		
	}
}