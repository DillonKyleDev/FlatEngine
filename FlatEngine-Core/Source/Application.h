#pragma once
#include "GameLoop.h"

#include <string>


namespace FlatEngine
{
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
		}
		~Application() {};

		virtual void Init() {};
		virtual void Run() {};
		virtual void RunOnceAfterInitialization() {};
		void SetWindowDimensions(int width, int height) { m_windowWidth = width; m_windowHeight = height; };
		int WindowWidth() { return m_windowWidth; };
		int WindowHeight() { return m_windowHeight; };
		void SetWindowResized(bool b_resized) { m_b_windowResized = b_resized; };		
		void BeginRender(); // Defined in Application.cpp
		void EndRender();   // Defined in Application.cpp
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
	};
}