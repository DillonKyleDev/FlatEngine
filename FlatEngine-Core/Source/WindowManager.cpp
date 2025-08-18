#include "WindowManager.h"

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"
#include <string>


namespace FlatEngine
{
	WindowManager::WindowManager()
	{
		m_window = nullptr;
		m_renderer = nullptr;
		m_screenSurface = nullptr;		
		m_title = "";
		m_windowWidth = 900;
		m_windowHeight = 600;
		m_b_isFullscreen = false;
	}

	WindowManager::~WindowManager()
	{
		Close();
	}

	SDL_Window* WindowManager::GetWindow()
	{
		return m_window;
	}

	SDL_Renderer* WindowManager::GetRenderer()
	{
		return m_renderer;
	}

	void WindowManager::SetFullscreen(bool b_isFullscreen)
	{
		m_b_isFullscreen = b_isFullscreen;

		if (m_b_isFullscreen)
		{
			SDL_SetWindowFullscreen(m_window, SDL_WINDOW_FULLSCREEN_DESKTOP);
		}
		else
		{
			SDL_SetWindowFullscreen(m_window, 0);
		}
		//SDL_ShowCursor(true);
	}

	void WindowManager::SetTitle(std::string title)
	{
		m_title = title;
		RecreateWindow();
	}

	bool WindowManager::CreateNewWindow(std::string title, int width, int height)
	{
		m_title = title;
		m_windowWidth = width;
		m_windowHeight = height;
		return RecreateWindow();
	}

	bool WindowManager::RecreateWindow()
	{
		bool b_success = true;

		if (m_renderer != NULL)
		{
			SDL_DestroyRenderer(m_renderer);
		}
		if (m_window != NULL)
		{
			SDL_DestroyWindow(m_window);
		}

		// Create window with SDL_Renderer graphics context
		SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
		m_window = SDL_CreateWindow("FlatEngine", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, m_windowWidth, m_windowHeight, window_flags);
		// Vsync on
		m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
		// Vsync off
		//m_renderer = SDL_CreateRenderer(m_window, -1, 0);
		SDL_RenderSetLogicalSize(m_renderer, m_windowWidth, m_windowHeight);
		SDL_RenderSetIntegerScale(m_renderer, SDL_bool(true));
		if (m_window == NULL)
		{
			printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
			b_success = false;
		}
		else
		{
			//Create vsynced renderer for window
			//m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
			if (m_renderer == NULL)
			{
				printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
				b_success = false;
			}
		}

		if (b_success)
		{
			//Get window surface down here so we don't have to delete it if the SDL_image init fails anyway
			m_screenSurface = SDL_GetWindowSurface(m_window);
		}
		
		if (m_b_isFullscreen)
		{
			SDL_SetWindowFullscreen(m_window, SDL_WINDOW_FULLSCREEN_DESKTOP);
		}
		else
		{
			SDL_SetWindowFullscreen(m_window, 0);
		}

		return b_success;
	}

	bool WindowManager::Init(std::string title, int width, int height)
	{
		return CreateNewWindow(title, width, height);
	}

	void WindowManager::Close()
	{
		//Destroy window
		SDL_DestroyRenderer(m_renderer);
		SDL_DestroyWindow(m_window);
		m_renderer = NULL;
		m_window = NULL;
	}

	bool WindowManager::SetScreenDimensions(int width, int height)
	{
		m_windowWidth = width;
		m_windowHeight = height;
		return RecreateWindow();
	}

	void WindowManager::ResizeWindow(int width, int height)
	{
		m_windowWidth = width;
		m_windowHeight = height;
		SDL_SetWindowSize(m_window, m_windowWidth, m_windowHeight);
		SDL_SetWindowPosition(m_window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
	}

	void WindowManager::Render()
	{
		//Clear the screen
		SDL_SetRenderDrawColor(m_renderer, 0xFF, 0xFF, 0xFF, 0xFF);
		SDL_RenderClear(m_renderer);

		//Update the screen
		SDL_RenderPresent(m_renderer);
	}
}