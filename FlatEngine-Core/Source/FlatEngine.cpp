#include "Application.h"
#include "FlatEngine.h"
#include "GuiCore.h"
#include "managers/Assets.h"
#include "managers/AudioManager.h"
#include "managers/Controls.h"
#include "managers/LuaManager.h"
#include "managers/Settings.h"
#include "physics/PhysicsManager.h"
#include "render/RenderWindow.h"
#include "render/VulkanManager.h"
#include "tools/FileHelper.h"
#include "tools/Logger.h"

#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <memory>
#include <string>
#include <vector>


/*
######################################
######							######
######         Engine           ######
######                          ######
######################################
*/


namespace FlatEngine
{	
	std::shared_ptr<Application> application;	
	std::string rootDir = "";
	bool b_closeProgramQueued = false;
	bool b_loadNewScene = false;
	std::string sceneToBeLoaded = "";
	std::vector<SDL_Event> events;
	Vector2 mouseDelta;
	Vector2 mousePos;
	Vector2 lastMousePos;


	bool Init(int windowWidth, int windowHeight)
	{
		bool b_success = true;
		rootDir = FileHelper::GetCurrentDir();

		SDL_SetHint(SDL_HINT_JOYSTICK_THREAD, "1");
		if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER) < 0)
		{
			Logger::log.Critical("SDL could not initialize! SDL_Error: {}\n", SDL_GetError());
			b_success = false;
		}
		else
		{
			LuaManager::InitLua();
			Assets::assetManager.FindRootDir();
			Assets::assetManager.CollectDirectories();
			Assets::assetManager.CollectColors();						            // Collect global colors from Colors.lua

			Logger::log.Trace("SDL initialized... - Video - Audio - Joystick -\n"); // First log only after colors are collected for debug colors.
			Logger::log.Trace("Lua initialized...\n");

			if (!VulkanManager::vulkan.Init(windowWidth, windowHeight))
			{
				Logger::log.Critical("Vulkan initialization failed...\n");
				b_success = false;
			}
			else
			{
				Logger::log.Trace("Vulkan initialized...\n");

				if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0"))
				{
					Logger::log.Warn("Linear texture filtering not enabled!\n");
				}

				int imgFlags = IMG_INIT_PNG | IMG_INIT_JPG | IMG_INIT_TIF | IMG_INIT_WEBP | IMG_INIT_JXL;
				if (!(IMG_Init(imgFlags) && imgFlags))
				{
					Logger::log.Critical("SDL_image could not initialize! SDL_image Error: {}\n", IMG_GetError());
					b_success = false;
				}
				else
				{
					Logger::log.Trace("SDL_image initialized...\n");
					if (TTF_Init() == -1)
					{
						Logger::log.Err("SDL_ttf could not initialize! SDL_ttf Error: {}\n", TTF_GetError());
						b_success = false;
					}
					else
					{
						Logger::log.Trace("TTF_Fonts initialized...\n");
						if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
						{							
							Logger::log.Err("SDL_mixer could not initialize! SDL_mixer Error: {}\n", Mix_GetError());
							b_success = false;
						}
						else
						{
							Mix_AllocateChannels(AudioManager::totalAvailableChannels);
							Logger::log.Trace("SDL_mixer initialized...\n");
						}					

						Assets::assetManager.CollectTags();
						Assets::assetManager.CollectTextures();
						Assets::assetManager.LoadFonts();
						GuiCore::SetImGuiVars();

						Settings::settings.LoadSettings("../engine/engine_settings.json");

						Logger::log.Trace("Engine assets initialized...\n");
						Logger::log.Info("=================================================");
						Logger::log.Info("======= Engine Initialization Complete... =======");
						Logger::log.Info("=================================================");
					}
				}
			}
		}

		return b_success;
	}

	void Cleanup()
	{
		PhysicsManager::physics2D.Shutdown();
		GuiCore::QuitImGui();

		for (SDL_Joystick* gamepad : Controls::gamepads)
		{
			SDL_JoystickClose(gamepad);
			gamepad = nullptr;
		}

		Assets::assetManager.FreeFonts();

		Mix_Quit();
		TTF_Quit();
		IMG_Quit();
		SDL_Quit();
	}

	Vector2 GetMousePosWindow()
	{
		int globalX, globalY;
		SDL_GetGlobalMouseState(&globalX, &globalY);
		int windowX, windowY;
		SDL_GetWindowPosition(RenderWindow::window.GetWindow(), &windowX, &windowY);
		int windowWidth, windowHeight;
		SDL_GetWindowSize(RenderWindow::window.GetWindow(), &windowWidth, &windowHeight);

		return Vector2(globalX - windowX, globalY - windowY);		
	}

	void CalculateMouseDelta()
	{
		mousePos = GetMousePosWindow();	
		mouseDelta = Vector2(mousePos.x - lastMousePos.x, mousePos.y - lastMousePos.y);	
		ImGui::GetIO().MouseDelta = ImVec2(mouseDelta.x, mouseDelta.y);

		if (ImGui::IsMouseDragging(ImGuiMouseButton_Left) || ImGui::IsMouseDragging(ImGuiMouseButton_Right))
		{
			int windowWidth, windowHeight;
			SDL_GetWindowSize(RenderWindow::window.GetWindow(), &windowWidth, &windowHeight);

			if (mousePos.x > windowWidth - 2)
			{
				SDL_WarpMouseInWindow(RenderWindow::window.GetWindow(), 1, mousePos.y);
				lastMousePos = Vector2(0, mousePos.y);
				mouseDelta.x = 1;
				return;
			}
			else if (mousePos.x < 1)
			{
				SDL_WarpMouseInWindow(RenderWindow::window.GetWindow(), windowWidth - 2, mousePos.y);
				lastMousePos = Vector2(windowWidth, mousePos.y);
				mouseDelta.x = -1;
				return;
			}
			if (mousePos.y > windowHeight - 1)
			{
				SDL_WarpMouseInWindow(RenderWindow::window.GetWindow(), mousePos.x, 1);
				lastMousePos = Vector2(mousePos.x, 0);
				mouseDelta.y = 1;
				return;
			}
			else if (mousePos.y < 1)
			{
				SDL_WarpMouseInWindow(RenderWindow::window.GetWindow(), mousePos.x, windowHeight - 1);
				lastMousePos = Vector2(mousePos.x, windowHeight);
				mouseDelta.y = 1;
				return;
			}			
		}

		lastMousePos = mousePos;
	}

	void HandleEvents()
	{
		// Unfire all keybinds that were fired in the last frame then clear the saved keys
		static std::vector<std::string> firedKeys = std::vector<std::string>();
		static std::vector<std::string> firedLastFrameKeys = std::vector<std::string>();
		firedLastFrameKeys = firedKeys;

		for (std::string keybind : firedKeys)
		{
			for (Controls::MappingContext& context : Controls::mappingContexts)
			{
				context.UnFireEvent(keybind);
			}
		}
		firedKeys.clear();

		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			events.push_back(event);
			
			ImGui_ImplSDL2_ProcessEvent(&event);

			if (event.type == SDL_QUIT)
			{
				b_closeProgramQueued = true;
			}
			if (event.type == SDL_WINDOWEVENT)
			{	
				if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED || event.window.event == SDL_WINDOWEVENT_RESIZED)
				{
					application->SetWindowResized(true);					
				}
				else if (event.window.event == SDL_WINDOWEVENT_CLOSE)
				{
					if (event.window.windowID == SDL_GetWindowID(RenderWindow::window.GetWindow()))
					{
						b_closeProgramQueued = true;
					}
				}
			}

			// Key press events (should roll this into EngineContext.mpc below)
			if (event.type == SDL_KEYDOWN)
			{		
				switch (event.key.keysym.sym)
				{
				case SDLK_DELETE:											
					break;
				case SDLK_r:										
					break;
				case SDLK_HOME:					
					break;				
				case SDLK_SPACE:
					FL::application->gameloop->TogglePauseGameLoop();
					break;	
				default:
					break;
				}
			}

			Controls::MappingContext* context = Controls::GetMappingContext("EngineContext");
			if (context != nullptr)
			{
				HandleContextEvents(*context, event, firedKeys);
			}				
		}

		CalculateMouseDelta();
	}
}