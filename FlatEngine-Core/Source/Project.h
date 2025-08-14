#pragma once
#include "Scene.h"	
#include "Vector2.h"

#include <string>


namespace FlatEngine
{
	class Project
	{
	public:
		Project();
		~Project();

		std::string GetPath();
		void SetPath(std::string path);
		void SetLoadedScenePath(std::string path);
		std::string GetLoadedScenePath();
		void SetRuntimeScene(std::string scene);
		std::string GetRuntimeScene();
		void SetBuildPath(std::string path);
		std::string GetBuildPath();
		void SetCurrentFileDirectory(std::string dirName);
		std::string GetCurrentFileDirectory();
		void SetPersistantGameObjectsScenePath(std::string path);
		Scene* GetPersistantGameObjectScene();
		std::string GetPersistantGameObjectsScenePath();
		void CreateFreshPersistantScene();
		void SavePersistantScene(std::string filePath);
		void LoadPersistantScene(std::string actualPath = "");
		void SaveScene();
		std::map<long, GameObject>& GetPersistantObjects();
		void SetFocusedGameObjectID(long ID);
		long GetFocusedGameObjectID();
		void SetLoadedPreviewAnimationPath(std::string path);
		std::string GetLoadedPreviewAnimationPath();
		void SetSceneViewScrolling(Vector2 scrolling);
		Vector2 GetSceneViewScrolling();
		void SetSceneViewGridStep(Vector2 gridStep);
		Vector2 GetSceneViewGridStep();
		bool AutoSaveOn();
		void SetAutoSave(bool b_autoSave);
		void SetResolution(Vector2 resolution);
		Vector2 GetResolution();
		void SetFullscreen(bool b_newFullscreen);
		bool IsFullscreen();
		void SetVsyncEnabled(bool b_vsync);
		bool IsVsyncEnabled();
		void SetMusicVolume(int volume);
		int GetMusicVolume();
		void SetEffectsVolume(int volume);
		int GetEffectsVolume();
		void UpdateSavedTime();
		tm GetSavedTime();

	private:
		std::string m_path;
		std::string m_buildPath;
		std::string m_loadedScenePath;
		std::string m_sceneToLoadAtRuntime;
		std::string m_loadedAnimationPath;
		std::string m_currentFileDirectory;
		std::string m_persistantGameObjectsScenePath;
		Scene m_persistantGameObjectsScene;
		Vector2 m_sceneViewScrolling;
		Vector2 m_sceneViewGridStep;
		long m_focusedGameObjectID;
		bool m_b_autoSave;
		Vector2 m_resolution;
		bool m_b_fullscreen;
		bool m_b_vsyncEnabled;
		int m_musicVolume;
		int m_effectsVolume;
		tm m_timeSinceSave;
	};
}