#include "FlatEngine.h"
#include "GameObject.h"
#include "GuiCore.h"
#include "managers/Assets.h"
#include "managers/AudioManager.h"
#include "managers/Controls.h"
#include "managers/LuaManager.h"
#include "managers/PhysicsManager.h"
#include "managers/PrefabManager.h"
#include "managers/ProjectManager.h"
#include "managers/SceneManager.h"
#include "managers/Settings.h"
#include "render/RenderWindow.h"
#include "render/VulkanManager.h"
#include "tools/FileHelper.h"
#include "tools/Logger.h"
#include "tools/Vector3.h"
#include "tools/Vector4.h"

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
	std::shared_ptr<Application> F_Application = std::make_shared<Application>();	
	
	std::string ROOT_DIR = "";

	bool F_b_closeProgram = false;
	bool F_b_closeProgramQueued = false;
	bool F_b_ProjectManagerelected = false;	
	bool F_b_loadNewScene = false;
	std::string F_sceneToBeLoaded = "";

	// Collision Detection
	// std::vector<std::pair<Collider*, Collider*>> F_ColliderPairs = std::vector<std::pair<Collider*, Collider*>>();

	bool Init(int windowWidth, int windowHeight)
	{
		bool b_success = true;
		ROOT_DIR = FileHelper::GetCurrentDir();

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

						PhysicsManager::physics.Init();
						Logger::log.Trace("Physics initialized...\n");

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
		PhysicsManager::physics.Shutdown();
		GuiCore::QuitImGui();

		// Clean up old gamepads
		for (SDL_Joystick* gamepad : Controls::gamepads)
		{
			SDL_JoystickClose(gamepad);
			gamepad = nullptr;
		}

		Assets::assetManager.FreeFonts();

		//Quit SDL subsystems
		Mix_Quit();
		TTF_Quit();
		IMG_Quit();
		SDL_Quit();
	
		F_b_closeProgram = true;
	}

	void HandleEngineEvents(SDL_Event event)
	{
		// Keyboard Keys Down
		if (event.type == SDL_KEYDOWN)
		{
			// Scene View keybinds
			if (true)
			{				
				switch (event.key.keysym.sym)
				{
				case SDLK_DELETE:
					// SceneManager::loadedScene.DeleteGameObject(ProjectManager::loadedProject.focusedGameObjectID);							
					break;
				case SDLK_r:					
					VulkanManager::vulkan.ReloadShaders();					
					break;
				case SDLK_HOME:
					SceneManager::LoadScene(SceneManager::GetLoadedScenePath());
					Logger::log.Info("Scene Reloaded");
					break;				
				case SDLK_SPACE:
					PauseGameLoop();
					break;	

				default:
					break;
				}
			}
		}
	}

	void HandleEvents(bool& quit)
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
			ImGui_ImplSDL2_ProcessEvent(&event);

			if (event.type == SDL_QUIT)
			{
				quit = true;
			}
			if (event.type == SDL_WINDOWEVENT)
			{	
				if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED || event.window.event == SDL_WINDOWEVENT_RESIZED)
				{
					F_Application->SetWindowResized(true);					
				}
				else if (event.window.event == SDL_WINDOWEVENT_CLOSE)
				{
					if (event.window.windowID == SDL_GetWindowID(RenderWindow::window.GetWindow()))
					{
						quit = true;
					}
				}
			}

			HandleEngineEvents(event);

			if (GameLoopStarted())
			{
				for (Controls::MappingContext& context : Controls::mappingContexts)
				{
					HandleContextEvents(context, event, firedKeys);
				}
			}
			else
			{
				Controls::MappingContext* context = Controls::GetMappingContext("EngineContext");

				if (context != nullptr)
				{
					HandleContextEvents(*context, event, firedKeys);
				}
			}
		}
	}

	GameObject* CreateAssetUsingFilePath(std::string filePath, Vector3 position)
	{
		std::string extension = std::filesystem::path(filePath).extension().string();

		if (extension == ".png" || extension == ".jpg" || extension == ".tif" || extension == ".webp" || extension == ".jxl")
		{
			GameObject* newObject = SceneManager::loadedScene.CreateGameObject();			 
			newObject->SetName(FileHelper::GetFilenameFromPath(filePath) + "(" + std::to_string(newObject->GetID()) + ")");
			newObject->Get<Transform>()->SetPosition(Vector3(position.x, position.y, 0));
			newObject->Add<Sprite>()->SetTexture(filePath);
			return newObject;
		}
		else if (extension == ".prf")
		{
			return PrefabManager::Instantiate(FileHelper::GetFilenameFromPath(filePath), position, &SceneManager::loadedScene);
		}
		else
		{
			return nullptr;
		}
		//else if (extension == ".scn")
		//{

		//}
		//// Mapping Context file
		//else if (extension == ".mpc")
		//{

		//}
		//// Animation file
		//else if (extension == ".anm")
		//{

		//}
		//// Lua file
		//else if (extension == ".scp")
		//{

		//}
	}

	// Game Loop
	void StartGameLoop()
	{
		F_Application->StartGameLoop();
	}

	void GameLoopUpdate()
	{
		F_Application->UpdateGameLoop();
	}

	void PauseGameLoop()
	{
		F_Application->PauseGameLoop();
	}

	void PauseGame()
	{
		F_Application->PauseGame();
	}

	void StopGameLoop()
	{
		F_Application->StopGameLoop();
	}

	float GetElapsedGameTimeInSec()
	{
		return F_Application->GetGameLoop()->TimeElapsedInSec();
	}

	long GetElapsedGameTimeInMs()
	{
		return F_Application->GetGameLoop()->TimeElapsedInMs();
	}

	bool GameLoopStarted()
	{
		return F_Application->GetGameLoop()->IsStarted();
	}

	bool GameLoopPaused()
	{
		return F_Application->GetGameLoop()->IsPaused();
	}

	long GetFramesCounted()
	{
		return F_Application->GetGameLoop()->GetFramesCounted();
	}

	float GetDeltaTime()
	{
		return F_Application->GetGameLoop()->GetDeltaTime();
	}

	// For Mouse button collisions - Vector4 objectA(top, right, bottom, left), Vector4 objectB(top, right, bottom, left)
	bool AreCollidingViewport(Vector4 ObjectA, Vector4 ObjectB)
	{
		float A_TopEdge = ObjectA.z;
		float A_RightEdge = ObjectA.y;
		float A_BottomEdge = ObjectA.x;
		float A_LeftEdge = ObjectA.w;

		float B_TopEdge = ObjectB.x;
		float B_RightEdge = ObjectB.y;
		float B_BottomEdge = ObjectB.z;
		float B_LeftEdge = ObjectB.w;

		return (A_LeftEdge < B_RightEdge && A_RightEdge > B_LeftEdge && A_TopEdge > B_BottomEdge && A_BottomEdge < B_TopEdge);
	}
}

// ImGui cheat sheet
// Border around object
//auto wPos = ImGui::GetWindowPos();
//auto wSize = ImGui::GetWindowSize();  // This is the size of the current box, perfect for getting the exact dimensions for a border
//ImGui::GetWindowDrawList()->AddRect({ wPos.x + 2, wPos.y + 2 }, { wPos.x + wSize.x - 2, wPos.y + wSize.y - 2 }, GetColor32("componentBorder"), 2);

