#pragma once
#include "GameLoop.h"

#include <memory>
#include <string>


namespace FlatEngine
{
	class GameLoop;

	class Application
	{
	public:
		Application()
		{
			m_b_windowResized = false;
			m_windowWidth = 1920;
			m_windowHeight = 1006;
		}

		virtual void Init() {};
		virtual void Run() {};
		virtual void Cleanup() {};
		void SetWindowDimensions(int width, int height) { m_windowWidth = width; m_windowHeight = height; };
		int GetWindowWidth() { return m_windowWidth; };
		int GetWindowHeight() { return m_windowHeight; };
		void SetWindowResized(bool b_resized) { m_b_windowResized = b_resized; };		
		virtual void BeginRender(); // Defined in Application.cpp
		virtual void EndRender();   // Defined in Application.cpp
		virtual void OnLoadScene(std::string sceneName) {};
		
		std::shared_ptr<FL::GameLoop> gameloop;
		
	private:
		bool m_b_windowResized;
		int m_windowWidth;
		int m_windowHeight;
	};
}