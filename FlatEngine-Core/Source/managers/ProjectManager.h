#pragma once
#include "components/Script.h"
#include "components/Text.h"
#include "tools/JsonHelper.h"

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
			json GetData();
			void PutData(json projectJson);

			void SetMusicVolume(int volume);
			int GetMusicVolume();
			void SetEffectsVolume(int volume);
			int GetEffectsVolume();
			void UpdateSavedTime();
			tm GetSavedTime();
			void AddFocusedObjectID(long ID);
			void RemoveFocusedObjectID(long ID);
			bool IsIDFocused(long ID);
			void RefocusID(long withID);

			std::string path;
			std::string buildPath;
			std::string loadedScenePath;
			std::string sceneToLoadAtRuntime;
			std::string loadedAnimationPath;
			std::string currentFileDirectory;		
			long lastFocusedID;
			std::vector<long> focusedGameObjectIDs;
			bool b_autoSave;
			Script persistentScript;

		private:			
			int m_musicVolume;
			int m_effectsVolume;
			tm m_timeSinceSave;
		};
		
		extern bool b_projectSelected;
		extern Project loadedProject;		
		extern std::map<std::string, Text> projectNameTexts;
		extern const int PERSISTENT_SCRIPT_ID;
		
		extern void RecreateProjectNameTexts();
		extern std::vector<std::string> RetrieveProjectPaths();
		extern bool ProjectTimeCustomComp(std::string path1, std::string path2);
		extern void CreateNewProject(std::string projectName);
		extern void CreateProjectDirectory(std::string path);
		extern void LoadProject(std::string path);
		extern void SaveProject(Project* project, std::string path);
		extern void SaveCurrentProject();			
		extern void BuildProject();
		extern void SetProjectLoadedScenePath(std::string scenePath);
	}
}