#pragma once
#include <string>
#include "GameLoop.h"
#include <string>


namespace FlatEngine
{
	enum DirectoryType {
		RuntimeDir,
		EditorDir,
		NoDir
	};

	class GameLoop;

	class Application
	{
	public:
		Application()
		{
			m_b_hasQuit = false;
			m_b_windowResized = false;
			m_windowWidth = 1920;
			m_windowHeight = 1080;
			m_directoryType = NoDir;
		}
		~Application() {};

		virtual void Init() {};
		virtual void Run() {};
		virtual void RunOnceAfterInitialization() {};
		void SetWindowDimensions(int width, int height) { m_windowWidth = width; m_windowHeight = height; };
		int WindowWidth() { return m_windowWidth; };
		int WindowHeight() { return m_windowHeight; };
		void WindowResized() { m_b_windowResized = true; };
		void BeginRender(); // Defined in Application.cpp
		void EndRender();   // Defined in Application.cpp
		DirectoryType GetDirectoryType() { return m_directoryType; };
		void SetDirectoryType(DirectoryType dirType) { m_directoryType = dirType; };
		virtual GameLoop* GetGameLoop() { return nullptr; };
		virtual bool GameLoopStarted() { return false; };
		virtual bool GameLoopPaused() { return false; };
		virtual void StartGameLoop() {};
		virtual void UpdateGameLoop() {};
		virtual void PauseGameLoop() {};
		virtual void StopGameLoop() {};
		virtual void PauseGame() {};

		void Quit() { m_b_hasQuit = true; };
		bool& HasQuit() { return m_b_hasQuit; };
		virtual void OnLoadScene(std::string sceneName) {};
		
	private:
		bool m_b_hasQuit;
		bool m_b_windowResized;
		int m_windowWidth;
		int m_windowHeight;
		DirectoryType m_directoryType;
	};
}