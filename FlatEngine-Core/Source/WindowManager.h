#pragma once
#include <SDL.h>
#include <string>


namespace FlatEngine
{
	class WindowManager {
	public:
		WindowManager();
		~WindowManager();
		SDL_Window* GetWindow();
		SDL_Renderer* GetRenderer();
		bool RecreateWindow();
		bool Init(std::string title, int width, int height);
		void Close();
		bool SetScreenDimensions(int width, int height);
		void ResizeWindow(int width, int height);
		void SetTitle(std::string title);
		void Render();
		void SetFullscreen(bool b_isFullscreen);

	private:
		bool CreateNewWindow(std::string title, int width, int height);

		std::string m_title;
		int m_windowWidth;
		int m_windowHeight;
		bool m_b_isFullscreen;
		SDL_Window* m_window;
		SDL_Renderer* m_renderer;
		SDL_Surface* m_screenSurface;
	};
}