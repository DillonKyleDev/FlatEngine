#include "FlatEngine.h"
#include "WindowManager.h"
#include "PrefabManager.h"
#include "Logger.h"
#include "SceneManager.h"
#include "Vector2.h"
#include "Vector4.h"
#include "Project.h"
#include "AssetManager.h"
#include "MappingContext.h"
#include "Scene.h"
#include "Animation.h"

#include <fstream>
#include <string>
#include <filesystem>
#include <cmath>
#include <shobjidl.h> 
#include "implot.h"
#include "imgui_internal.h"
#include <random>



/*
######################################
######							######
######         Engine           ######
######                          ######
######################################
*/


namespace FlatEngine
{	
	bool F_b_closeProgram = false;
	bool F_b_closeProgramQueued = false;

	std::shared_ptr<WindowManager> F_Window = std::make_shared<WindowManager>();

	std::string F_RuntimeDirectoriesLuaFilepath = "..\\engine\\scripts\\RuntimeDirectories.lua";
	std::string F_EditorDirectoriesLuaFilepath = "..\\engine\\scripts\\EditorDirectories.lua";
	std::shared_ptr<Application> F_Application = std::make_shared<Application>();
	AssetManager F_AssetManager = AssetManager();
	std::vector<std::string> F_selectedFiles = std::vector<std::string>();

	F_CURSOR_MODE F_CursorMode = F_CURSOR_MODE::TRANSLATE;
	bool F_b_sceneViewFocused = false;
	


	bool F_b_loadNewScene = false;
	std::string F_sceneToBeLoaded = "";


	std::vector<SDL_Joystick*> F_gamepads = std::vector<SDL_Joystick*>();	


	// For rendering sprites
	int F_maxSpriteLayers = 55;

	// sprite size * 0.1f brings sprite down from 1 sprite pixel per 1 grid block to 10 sprite pixels per 1 grid block
	// then to make an 8x8 sprite fit properly into a 10px by 10px grid block
	// take the ratio of 10px / 8px and multiply it by our now scaled down sprite size to get:  0.1 * (10 / 8) = 0.125 = the scale multiplier
	// Now our 8x8, 16x16, 32x32, etc, pixel art fits nicely inside the grid space blocks
	float F_pixelsPerGridSpace = 8.0f;
	float F_spriteScaleMultiplier = 0.1f * (10.0f / F_pixelsPerGridSpace);

	GameObject* F_PlayerObject = nullptr;
	Project F_LoadedProject = Project();

	// Managers
	Logger F_Logger = Logger();
	SceneManager F_SceneManager = SceneManager();	
	Sound F_SoundController = Sound();

	std::vector<MappingContext> F_MappingContexts = std::vector<MappingContext>();
	std::vector<std::string> F_KeyBindingsAvailable = std::vector<std::string>();
	std::string F_selectedMappingContextName = "";
	TTF_Font* F_fontCinzel;
	std::shared_ptr<PrefabManager> F_PrefabManager = std::make_shared<PrefabManager>();
	std::vector<TileSet> F_TileSets = std::vector<TileSet>();
	std::string F_selectedTileSetToEdit = "- none -";
	std::pair<std::string, int> F_tileSetAndIndexOnBrush = std::pair<std::string, int>();

	// Drag/Drop target IDs
	std::string F_fileExplorerTarget = "DND_FILE_PATH_OBJECT";
	std::string F_hierarchyTarget = "DND_HIERARCHY_OBJECT";

	// Animator
	std::shared_ptr<Animation::S_AnimationProperties> FocusedAnimation = std::make_shared<Animation::S_AnimationProperties>();
	std::shared_ptr<GameObject> objectForFocusedAnimation = nullptr;
	std::shared_ptr<Animation::S_Property> selectedKeyFrameToEdit = nullptr;

	// Camera
	Camera* F_primaryCamera = nullptr;

	// Collision Detection
	std::vector<std::pair<Collider*, Collider*>> F_ColliderPairs = std::vector<std::pair<Collider*, Collider*>>();


	bool LoadFonts()
	{
		bool b_success = true;
		F_fontCinzel = TTF_OpenFont(GetFilePath("cinzelBlack").c_str(), 40);
		if (F_fontCinzel == nullptr)
		{
			printf("Failed to load lazy font! SDL_ttf Error: %s\n", TTF_GetError());
			b_success = false;
		}

		return b_success;
	}

	void FreeFonts()
	{
		//Free global fonts
		TTF_CloseFont(F_fontCinzel);
		F_fontCinzel = nullptr;
	}

	// Get directory path using name given in Directories.lua
	std::string GetDir(std::string dirName)
	{
		return F_AssetManager.GetDir(dirName);
	}

	// Get file path using name given in Directories.lua
	std::string GetFilePath(std::string fileName)
	{
		return F_AssetManager.GetFilePath(fileName);
	}
	
	// Get entire std::shared_ptr<Texture> object using name given in Textures.lua file
	std::shared_ptr<Texture> GetTextureObject(std::string textureName)
	{
		return F_AssetManager.GetTextureObject(textureName);
	}

	// Get SDL_Texture* using name given in Textures.lua file
	SDL_Texture* GetTexture(std::string textureName)
	{
		return F_AssetManager.GetTexture(textureName);
	}

	// Get color using name given in Colors.lua file
	Vector4 GetColor(std::string colorName)
	{
		return F_AssetManager.GetColor(colorName);
	}

	// Get color using name given in Colors.lua file converted to Uint32 format
	Uint32 GetColor32(std::string colorName)
	{
		return F_AssetManager.GetColor32(colorName);
	}

	Vector2 AddImageToDrawList(SDL_Texture* texture, Vector2 positionInGrid, Vector2 relativeCenterPoint, float textureWidthPx, float textureHeightPx, Vector2 offsetPx, Vector2 scale, bool b_scalesWithZoom, float zoomMultiplier, ImDrawList* drawList, float rotation, ImU32 addColor, Vector2 uvStart, Vector2 uvEnd)
	{
		// Changing the scale here because sprites render too large
		Vector2 newScale = Vector2(scale.x * F_spriteScaleMultiplier, scale.y * F_spriteScaleMultiplier);

		float scalingXStart = relativeCenterPoint.x + (positionInGrid.x * zoomMultiplier) - (offsetPx.x * newScale.x * zoomMultiplier);
		float scalingYStart = relativeCenterPoint.y - (positionInGrid.y * zoomMultiplier) - (offsetPx.y * newScale.y * zoomMultiplier);
		float scalingXEnd = scalingXStart + (textureWidthPx * newScale.x * zoomMultiplier);
		float scalingYEnd = scalingYStart + (textureHeightPx * newScale.y * zoomMultiplier);

		float unscaledXStart = relativeCenterPoint.x + (positionInGrid.x * zoomMultiplier) - offsetPx.x * scale.x;
		float unscaledYStart = relativeCenterPoint.y + (-positionInGrid.y * zoomMultiplier) - offsetPx.y * scale.y;

		Vector2 renderStart;
		Vector2 renderEnd;

		if (b_scalesWithZoom)
		{
			renderStart = Vector2(scalingXStart, scalingYStart);
			renderEnd = Vector2(scalingXEnd, scalingYEnd);

			// FOR DEBUGGING - draw white box around where the texture should be
			//DrawRectangle(renderStart, renderEnd, Vector2(0,0), Vector2(ImGui::GetWindowWidth(), ImGui::GetWindowHeight()), F_whiteColor, 2, draw_list);
		}
		else
		{
			renderStart = Vector2(unscaledXStart, unscaledYStart);
			renderEnd = Vector2(renderStart.x + textureWidthPx * scale.x, renderStart.y + textureHeightPx * scale.y);
		}

		if (rotation != 0)
		{
			float cosA = cosf(rotation * 2.0f * (float)M_PI / 360.0f);
			float sinA = sinf(rotation * 2.0f * (float)M_PI / 360.0f);

			Vector2 topLeft = ImRotate(Vector2(-(renderEnd.x - renderStart.x) / 2, -(renderEnd.y - renderStart.y) / 2), cosA, sinA);
			Vector2 topRight = ImRotate(Vector2(+(renderEnd.x - renderStart.x) / 2, -(renderEnd.y - renderStart.y) / 2), cosA, sinA);
			Vector2 bottomRight = ImRotate(Vector2(+(renderEnd.x - renderStart.x) / 2, (renderEnd.y - renderStart.y) / 2), cosA, sinA);
			Vector2 bottomLeft = ImRotate(Vector2(-(renderEnd.x - renderStart.x) / 2, +(renderEnd.y - renderStart.y) / 2), cosA, sinA);

			Vector2 center = Vector2(renderStart.x + ((renderEnd.x - renderStart.x) / 2), renderStart.y + ((renderEnd.y - renderStart.y) / 2));
			Vector2 pos[4] =
			{
				Vector2(center.x + topLeft.x, center.y + topLeft.y),
				Vector2(center.x + topRight.x, center.y + topRight.y),
				Vector2(center.x + bottomRight.x, center.y + bottomRight.y),
				Vector2(center.x + bottomLeft.x, center.y + bottomLeft.y),
			};
			Vector2 uvs[4] =
			{
				Vector2(0.0f, 0.0f),
				Vector2(1.0f, 0.0f),
				Vector2(1.0f, 1.0f),
				Vector2(0.0f, 1.0f)
			};

			// Render sprite to viewport
			drawList->AddImageQuad(texture, pos[0], pos[1], pos[2], pos[3], uvs[0], uvs[1], uvs[2], uvs[3], addColor);
		}
		else
		{
			// Render sprite to viewport
			drawList->AddImage((void*)texture, renderStart, renderEnd, uvStart, uvEnd, addColor);
		}

		return renderStart;
	}


	void SetMusicVolume(int volume)
	{
		GetLoadedProject().SetMusicVolume(volume);
		for (std::pair<long, Audio> audio : GetLoadedScene()->GetAudios())
		{
			for (SoundData sound : audio.second.GetSounds())
			{
				sound.sound->setMusicVolume(volume);
			}
		}
	}

	void SetEffectsVolume(int volume)
	{
		GetLoadedProject().SetEffectsVolume(volume);
		for (std::pair<long, Audio> audio : GetLoadedScene()->GetAudios())
		{
			for (SoundData sound : audio.second.GetSounds())
			{
				sound.sound->setEffectVolume(volume);
			}
		}
	}


	bool Init(int windowWidth, int windowHeight, DirectoryType dirType)
	{
		//Initialization flag
		bool b_success = true;

		//Initialize SDL
		SDL_SetHint(SDL_HINT_JOYSTICK_THREAD, "1");
		if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER) < 0)
		{
			printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
			b_success = false;
		}
		else
		{
			printf("SDL initialized... - Video - Audio - Joystick -\n");

			//Set texture filtering to linear
			if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0"))
			{
				printf("Warning: Linear texture filtering not enabled!\n");
			}

			//Initialize Window
			if (F_Window->Init("FlatEngine", windowWidth, windowHeight))
			{
				printf("Window initialized...\n");

				//Initialize SDL_image for png loading
				int imgFlags = IMG_INIT_PNG;
				if (!(IMG_Init(imgFlags) && imgFlags))
				{
					printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
					b_success = false;
				}
				else
				{
					printf("SDL_image initialized...\n");
					//Initialize SDL_ttf for text rendering
					if (TTF_Init() == -1)
					{
						printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
						b_success = false;
					}
					else
					{
						printf("TTF_Fonts initialized...\n");
						//Initialize SDL_mixer
						if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
						{
							printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());						
							LogError("Audio Device Not Found - SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
							b_success = false;
						}
						else
						{
							Mix_AllocateChannels(100);							   // Sets number of individual audios that can play at once
							printf("SDL_mixer initialized...\n");
						}

						InitLua();
						printf("Lua initialized...\n");

						F_AssetManager.CollectDirectories(dirType);			   // Collect important directories and file paths from Directories.lua
						F_AssetManager.CollectColors();						   // Collect global colors from Colors.lua
						F_AssetManager.CollectTextures();				       // Collect and create Texture icons from Textures.lua
						SetupImGui();										   // Setup ImGui for use in the prompt for Directories.lua location
						SetImGuiColors();									   // Use the collected colors to style ImGui elements
						RetrieveLuaScriptPaths();
						InitializeTileSets();

						printf("Engine Assets initialized...\n");
						printf("System Ready...\n");

						LogSeparator();
						LogString("System ready. Begin logging...");
						LogSeparator();
					}
				}
			}
		}

		return b_success;
	}

	void SetupImGui()
	{
		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImPlot::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls for imgui ui nav
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();

		ImGuiStyle& style = ImGui::GetStyle();

		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		style.WindowPadding = { 5.0f, 5.0f };
		style.DockingSeparatorSize = 1;
		style.SeparatorTextAlign = Vector2(0.5f, 0.0f);
		style.SeparatorTextBorderSize = 1;

		ImGui_ImplSDL2_InitForSDLRenderer(F_Window->GetWindow(), F_Window->GetRenderer());
		ImGui_ImplSDLRenderer2_Init(F_Window->GetRenderer());
		SetImGuiColors();

		printf("ImGui initialized...\n");
	}

	void SetImGuiColors()
	{
		// Round about way of editing the active titlebgactive color since pushstylecolor doesn't seem to work for it.
		for (int i = 0; i < ImGuiCol_COUNT; i++)
		{
			const char* name = ImGui::GetStyleColorName(i);

			if (name == "SeparatorActive")
			{
				ImGuiStyle* ref = &ImGui::GetStyle();
				ref->Colors[i] = GetColor("buttonActive");
			}
			if (name == "SeparatorHovered")
			{
				ImGuiStyle* ref = &ImGui::GetStyle();
				ref->Colors[i] = GetColor("buttonHovered");
			}
			if (name == "ButtonHovered")
			{
				ImGuiStyle* ref = &ImGui::GetStyle();
				ref->Colors[i] = GetColor("buttonHovered");
			}
			if (name == "ButtonActive")
			{
				ImGuiStyle* ref = &ImGui::GetStyle();
				ref->Colors[i] = GetColor("buttonActive");
			}
			if (name == "Button")
			{
				ImGuiStyle* ref = &ImGui::GetStyle();
				ref->Colors[i] = GetColor("button");
			}
			if (name == "WindowBg")
			{
				ImGuiStyle* ref = &ImGui::GetStyle();
				ref->Colors[i] = GetColor("windowBg");
			}
			if (name == "TitleBg")
			{
				ImGuiStyle* ref = &ImGui::GetStyle();
				ref->Colors[i] = GetColor("viewportTitleBg");
			}
			else if (name == "TitleBgActive")
			{
				ImGuiStyle* ref = &ImGui::GetStyle();
				ref->Colors[i] = GetColor("viewportTitleBgActive");
			}
			else if (name == "TabUnfocusedActive")
			{
				ImGuiStyle* ref = &ImGui::GetStyle();
				ref->Colors[i] = GetColor("tabUnfocusedActive");
			}
			else if (name == "TabActive")
			{
				ImGuiStyle* ref = &ImGui::GetStyle();
				ref->Colors[i] = GetColor("tabActive");
			}
			else if (name == "ResizeGripHovered")
			{
				ImGuiStyle* ref = &ImGui::GetStyle();
				ref->Colors[i] = GetColor("resizeGripHovered");
			}
			else if (name == "ResizeGrip")
			{
				ImGuiStyle* ref = &ImGui::GetStyle();
				ref->Colors[i] = GetColor("resizeGrip");
			}
			else if (name == "ResizeGripActive")
			{
				ImGuiStyle* ref = &ImGui::GetStyle();
				ref->Colors[i] = GetColor("resizeGripActive");
			}
			else if (name == "DockingPreview")
			{
				ImGuiStyle* ref = &ImGui::GetStyle();
				ref->Colors[i] = GetColor("dockingPreview");
			}
			else if (name == "DockingEmptyBg")
			{
				ImGuiStyle* ref = &ImGui::GetStyle();
				ref->Colors[i] = GetColor("dockingPreviewEmpty");
			}
		}
	}

	void RestartImGui()
	{
		QuitImGui();
		SetupImGui();
	}

	void QuitImGui()
	{
		ImGui_ImplSDLRenderer2_Shutdown();
		ImGui_ImplSDL2_Shutdown();
		ImPlot::DestroyContext();
		ImGui::DestroyContext();
	}

	void CloseProgram()
	{
		QuitImGui();

		// Clean up old gamepads
		for (SDL_Joystick* gamepad : F_gamepads)
		{
			SDL_JoystickClose(gamepad);
			gamepad = nullptr;
		}

		FreeFonts();

		//Quit SDL subsystems
		Mix_Quit();
		TTF_Quit();
		IMG_Quit();
		SDL_Quit();
	
		F_b_closeProgram = true;
	}


	void ManageControllers()
	{
		static int controllersConnected = 0;
		if (SDL_NumJoysticks() != controllersConnected)
		{
			// Clean up old gamepads
			for (SDL_Joystick* gamepad : F_gamepads)
			{
				SDL_JoystickClose(gamepad);
				gamepad = nullptr;
			}

			controllersConnected = SDL_NumJoysticks();
			for (int i = 0; i < controllersConnected; i++)
			{
				SDL_Joystick* gamepad = SDL_JoystickOpen(i);
				if (gamepad == nullptr)
				{
					printf("Warning: Unable to open game controller! SDL Error: %s\n", SDL_GetError());
				}
				else
				{
					LogString("Gamepad connected...");
					F_gamepads.push_back(gamepad);
				}
			}
		}
	}


	Uint32 GetEngineTime()
	{
		return (int)SDL_GetTicks();
	}


	GameObject* GetPlayerObject()
	{
		return F_PlayerObject;
	}


	// Project Management
	void SetLoadedProject(Project &loadedProject)
	{
		F_LoadedProject = loadedProject;
	}

	Project& GetLoadedProject()
	{
		return F_LoadedProject;
	}

	void LoadGameProject(std::string path, json &projectJson)
	{
		Project newProject = Project();
		newProject.SetPath(path);
		
		std::ofstream fileObject;
		std::ifstream ifstream(path);

		// Open file in in mode
		fileObject.open(path, std::ios::in);		
		std::string fileContent = "";
		
		if (fileObject.good())
		{
			std::string line;
			while (!ifstream.eof()) 
			{
				std::getline(ifstream, line);
				fileContent.append(line + "\n");
			}
		}
		
		fileObject.close();

		if (fileObject.good())
		{		
			projectJson = json::parse(fileContent);

			if (projectJson["Project Properties"][0] != "nullptr")
			{				
				for (int i = 0; i < projectJson["Project Properties"].size(); i++)
				{					
					json projectData = projectJson["Project Properties"][i];
					std::string name = GetFilenameFromPath(path);

					newProject.SetPath(CheckJsonString(projectData, "path", name));
					newProject.SetLoadedScenePath(CheckJsonString(projectData, "loadedScenePath", name));
					newProject.SetBuildPath(CheckJsonString(projectData, "buildPath", name));
					newProject.SetLoadedPreviewAnimationPath(CheckJsonString(projectData, "loadedAnimationPath", name));
					newProject.SetFocusedGameObjectID(CheckJsonLong(projectData, "focusedGameObjectID", name));
					newProject.SetRuntimeScene(CheckJsonString(projectData, "sceneToLoadAtRuntime", name));		
					newProject.SetPersistantGameObjectsScenePath(CheckJsonString(projectData, "persistantGameObjectsScenePath", name));
					newProject.SetSceneViewScrolling(Vector2(CheckJsonFloat(projectData, "sceneViewScrollingX", name), CheckJsonFloat(projectData, "sceneViewScrollingY", name)));					
					newProject.SetSceneViewGridStep(Vector2(CheckJsonFloat(projectData, "sceneViewGridStepX", name), CheckJsonFloat(projectData, "sceneViewGridStepY", name)));
					newProject.SetAutoSave(CheckJsonBool(projectData, "_autoSave", name));
					newProject.SetResolution(Vector2(CheckJsonFloat(projectData, "resolutionWidth", name), CheckJsonFloat(projectData, "resolutionHeight", name)));
					newProject.SetFullscreen(CheckJsonBool(projectData, "_fullscreen", name));
					newProject.SetVsyncEnabled(CheckJsonBool(projectData, "_vsyncEnabled", name));
					newProject.SetMusicVolume(CheckJsonInt(projectData, "musicVolume", name));
					newProject.SetEffectsVolume(CheckJsonInt(projectData, "effectsVolume", name));
				}
			}
		}

		SetLoadedProject(newProject);

		if (F_LoadedProject.GetPersistantGameObjectsScenePath() != "")
		{
			F_LoadedProject.LoadPersistantScene();
		}

		if (F_LoadedProject.GetLoadedScenePath() != "")
		{
			LoadScene(F_LoadedProject.GetLoadedScenePath());
		}
		else
		{
			CreateNewScene();
		}
	}

	void BuildProject()
	{
		if (F_LoadedProject.GetBuildPath() != "")
		{
			try
			{
				std::filesystem::copy("..\\FlatEngine-Core", F_LoadedProject.GetBuildPath() + "\\Core", std::filesystem::copy_options::overwrite_existing | std::filesystem::copy_options::recursive);
			}
			catch (std::exception& e)
			{
				LogError("Failed to copy FlatEngine-Core : ");
				LogError(e.what());
			}
			try
			{
				std::filesystem::copy("..\\FlatEngine-Runtime", F_LoadedProject.GetBuildPath() + "\\Runtime", std::filesystem::copy_options::overwrite_existing | std::filesystem::copy_options::recursive);
			}
			catch (std::exception& e)
			{
				LogError("Failed to copy FlatEngine-Runtime : ");
				LogError(e.what());
			}
			try
			{
				std::filesystem::copy("..\\projects\\" + GetFilenameFromPath(F_LoadedProject.GetPath()), F_LoadedProject.GetBuildPath() + "\\projects\\" + GetFilenameFromPath(F_LoadedProject.GetPath()), std::filesystem::copy_options::overwrite_existing | std::filesystem::copy_options::recursive);
			}
			catch (std::exception& e)
			{
				LogError("Failed to copy assets : ");
				LogError(e.what());
			}
			try
			{
				std::filesystem::copy("..\\engine", F_LoadedProject.GetBuildPath() + "\\engine", std::filesystem::copy_options::overwrite_existing | std::filesystem::copy_options::recursive);
			}
			catch (std::exception& e)
			{
				LogError("Failed to copy engine dependencies: ");
				LogError(e.what());
			}
			try
			{
				std::filesystem::copy("..\\..\\..\\intermediates", F_LoadedProject.GetBuildPath() + "\\intermediates", std::filesystem::copy_options::overwrite_existing | std::filesystem::copy_options::recursive);
			}
			catch (std::exception& e)
			{
				LogError("Failed to copy intermediates: ");
				LogError(e.what());
			}
		}
	}

	void SetProjectLoadedScenePath(std::string scenePath)
	{
		F_LoadedProject.SetLoadedScenePath(scenePath);
	}

	std::map<long, GameObject>& GetPersistantObjects()
	{
		return GetLoadedProject().GetPersistantObjects();
	}

	// GameObject / Scene management
	void SaveScene(Scene* scene, std::string filepath)
	{
		F_SceneManager.SaveScene(scene, filepath);
	}

	void SaveCurrentScene()
	{
		F_SceneManager.SaveCurrentScene();
	}

	// Waits until EndImGuiRender() has been called in Application.cpp to load the next scene
	void QueueLoadScene(std::string scenePath)
	{
		F_b_loadNewScene = true;
		F_sceneToBeLoaded = scenePath;
	}

	// actualPath is the actual scene path we want to load from, pointTo is the scene path we say we're loading. Useful for loading temporary scene copies and not saving the temp scene file location as last scene loaded
	void LoadScene(std::string actualPath, std::string pointTo)
	{				
		if (DoesFileExist(actualPath))
		{
			F_SoundController.StopMusic();

			if (F_SceneManager.LoadScene(actualPath, pointTo))
			{
				if (F_SceneManager.GetLoadedScene() != nullptr)
				{
					F_SceneManager.SaveAnimationPreviewObjects();
				}

				F_SceneManager.LoadAnimationPreviewObjects();
				F_PlayerObject = GetObjectByTag("Player");
			}
		}
		else
		{
			LogError("Failed to load scene. Scene does not exist.");
			LogString("Path: " + actualPath);
		}
	}

	long GetNextComponentID()
	{
		long nextID = -1;
		Scene* loadedScene = GetLoadedScene();
		if (loadedScene != nullptr)
		{
			nextID = loadedScene->GetNextComponentID();
			GetLoadedScene()->IncrementComponentID();
		}

		return nextID;
	}

	long GetNextGameObjectID()
	{
		long nextID = -1;
		Scene* loadedScene = GetLoadedScene();
		if (loadedScene != nullptr)
		{
			nextID = loadedScene->GetNextGameObjectID();
		}

		return nextID;
	}

	Scene *GetLoadedScene()
	{
		return F_SceneManager.GetLoadedScene();
	}

	std::string CreateNewSceneFile(std::string filename, std::string path)
	{
		Scene* newScene = CreateNewScene();
		newScene->SetName(filename);
		std::string filePath = "";

		if (path == "")
		{
			filePath = GetDir("scenes") + "\\" + filename + ".scn";
		}
		else
		{
			filePath = path + "\\" + filename + ".scn";
		}

		newScene->SetPath(filePath);

		SaveScene(newScene, filePath);

		return filePath;
	}

	Scene *CreateNewScene()
	{
		return F_SceneManager.CreateNewScene();
	}

	std::string GetLoadedScenePath()
	{
		return F_SceneManager.GetLoadedScenePath();
	}

	std::map<long, GameObject> &GetSceneObjects()
	{
		return GetLoadedScene()->GetSceneObjects();
	}

	GameObject* CreatePersistantGameObject(long parentID, long myID)
	{
		if (myID == -1)
		{
			myID = GetLoadedProject().GetPersistantGameObjectScene()->GetNextGameObjectID();
		}
		return GetLoadedProject().GetPersistantGameObjectScene()->CreateGameObject(parentID, myID);
	}

	GameObject* CreateGameObject(long parentID, long myID, Scene* scene)
	{
		if (scene != nullptr)
		{
			return scene->CreateGameObject(parentID, myID);
		}
		else
		{
			return GetLoadedScene()->CreateGameObject(parentID, myID);
		}
	}

	void DeleteGameObject(long sceneObjectID)
	{
		if (GetLoadedScene()->GetObjectByID(sceneObjectID) != nullptr)
		{
			GetLoadedScene()->DeleteGameObject(sceneObjectID);
		}
		else if (GetLoadedProject().GetPersistantGameObjectScene()->GetObjectByID(sceneObjectID) != nullptr)
		{
			GetLoadedProject().GetPersistantGameObjectScene()->DeleteGameObject(sceneObjectID);
		}
	}

	Component* GetObjectComponent(long objectID, ComponentTypes type)
	{
		return GetLoadedScene()->GetObjectByID(objectID)->GetComponent(type);
	}

	GameObject* GetObjectByID(long objectID)
	{
		GameObject* sceneObject = GetLoadedScene()->GetObjectByID(objectID);
		GameObject* persistantObject = GetLoadedProject().GetPersistantGameObjectScene()->GetObjectByID(objectID);

		if (sceneObject != nullptr)
		{
			return sceneObject;
		}
		else
		{
			return persistantObject;
		}
	}

	GameObject* GetObjectByName(std::string name)
	{
		GameObject* sceneObject = GetLoadedScene()->GetObjectByName(name);
		GameObject* persistantObject = GetLoadedProject().GetPersistantGameObjectScene()->GetObjectByName(name);

		if (sceneObject != nullptr)
		{
			return sceneObject;
		}
		else
		{
			return persistantObject;
		}
	}

	GameObject* GetObjectByTag(std::string tag)
	{
		GameObject* sceneObject = GetLoadedScene()->GetObjectByTag(tag);
		GameObject* persistantObject = GetLoadedProject().GetPersistantGameObjectScene()->GetObjectByTag(tag);

		if (sceneObject != nullptr)
		{
			return sceneObject;
		}
		else
		{
			return persistantObject;
		}
	}

	void SetPrimaryCamera(Camera* primaryCamera)
	{
		if (F_primaryCamera != nullptr)
		{
			F_primaryCamera->SetPrimaryCamera(false);
		}
		F_primaryCamera = primaryCamera;
		F_primaryCamera->SetPrimaryCamera(true);
	}

	void RemovePrimaryCamera()
	{
		if (F_primaryCamera != nullptr)
		{
			F_primaryCamera->SetPrimaryCamera(false);
			F_primaryCamera = nullptr;
		}
	}

	Camera* GetPrimaryCamera()
	{
		return F_primaryCamera;
	}


	void CreateNewMappingContextFile(std::string fileName, std::string path)
	{
		std::string filePath = "";		
		MappingContext newContext = MappingContext();

		if (path == "")
		{
			filePath = GetDir("mappingContexts") + "\\" + fileName + ".mpc";
		}
		else
		{
			filePath = path + "\\" + fileName + ".mpc";
		}

		newContext.SetPath(filePath);
		newContext.SetName(fileName);
		SaveMappingContext(filePath, newContext);
		AddMappingContext(filePath);
	}

	GameObject* CreateAssetUsingFilePath(std::string filePath, Vector2 position)
	{
		std::string extension = std::filesystem::path(filePath).extension().string();

		if (extension == ".png")
		{
			GameObject* newObject = CreateGameObject();
			newObject->SetName(GetFilenameFromPath(filePath) + "(" + std::to_string(newObject->GetID()) + ")");
			newObject->GetTransform()->SetPosition(position);
			newObject->AddSprite()->SetTexture(filePath);
			return newObject;
		}
		else if (extension == ".prf")
		{
			return Instantiate(GetFilenameFromPath(filePath), position, GetLoadedScene());
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
		//// Project file
		//else if (extension == ".prj")
		//{
		//
		//}
	}


	// Mapping Context Management
	void SaveMappingContext(std::string path, MappingContext context)
	{
		std::ofstream fileObject;
		std::ifstream ifstream(path);

		// Delete old contents of the file
		fileObject.open(path, std::ofstream::out | std::ofstream::trunc);
		fileObject.close();

		// Opening file in append mode
		fileObject.open(path, std::ios::app);


		json mappings = json::array();
		std::string data = context.GetData();
		mappings.push_back(json::parse(data));


		json newFileObject = json::object({ {"Mapping Context", mappings } });
		fileObject << newFileObject.dump(4).c_str() << std::endl;
		fileObject.close();

		InitializeMappingContexts();
	}

	void AddMappingContext(std::string path)
	{
		MappingContext newContext = MappingContext();

		json contextData = LoadFileData(path);
		if (contextData != nullptr)
		{
			auto mappings = contextData["Mapping Context"][0];
			newContext.SetName(CheckJsonString(mappings, "name", "MappingContext"));
			newContext.SetPath(path);

			if (newContext.GetName() == "")
			{
				newContext.SetName(GetFilenameFromPath(path));
			}

			std::string errorMessage = "";
			for (std::string possibleBinding : F_KeyBindingsAvailable)
			{
				std::string inputAction = CheckJsonString(mappings, possibleBinding, newContext.GetName(), errorMessage);
				if (inputAction != "")
				{
					newContext.AddKeyBinding(possibleBinding, inputAction);
				}
			}

			F_MappingContexts.push_back(newContext);
		}
	}

	void InitializeMappingContexts()
	{
		F_MappingContexts.clear();

		// Get available input keycodes from MappingContext.h
		for (std::pair<long, std::string> inputKeycode : F_MappedKeyboardCodes)
		{
			F_KeyBindingsAvailable.push_back(inputKeycode.second);
		}
		for (std::pair<long, std::string> inputKeycode : F_MappedXInputButtonCodes)
		{
			F_KeyBindingsAvailable.push_back(inputKeycode.second);
		}
		for (std::pair<long, std::string> inputKeycode : F_MappedXInputDPadCodes)
		{
			F_KeyBindingsAvailable.push_back(inputKeycode.second);
		}
		for (std::pair<long, std::string> inputKeycode : F_MappedXInputAnalogCodes)
		{
			F_KeyBindingsAvailable.push_back(inputKeycode.second);
		}


		std::vector<std::string> mappingContextFiles = std::vector<std::string>();
		mappingContextFiles = FindAllFilesWithExtension(GetDir("projectDir"), ".mpc");

		for (std::string path : mappingContextFiles)
		{
			AddMappingContext(path);
		}
	}

	MappingContext* GetMappingContext(std::string contextName)
	{
		for (std::vector<MappingContext>::iterator iter = F_MappingContexts.begin(); iter != F_MappingContexts.end(); iter++)
		{
			if (iter->GetName() == contextName)
			{
				return &(*iter);
			}
		}

		return nullptr;
	}

	// Events
	void HandleEvents(bool& quit)
	{
		// Unfire all keybinds that were fired in the last frame then clear the saved keys
		static std::vector<std::string> firedKeys = std::vector<std::string>();
		for (std::string keybind : firedKeys)
		{
			for (MappingContext& context : F_MappingContexts)
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
				switch (event.window.event)
				{
					case SDL_WINDOWEVENT_CLOSE:
					{
						if (event.window.windowID == SDL_GetWindowID(F_Window->GetWindow()))
						{
							quit = true;
						}
						break;
					}
					case SDL_WINDOWEVENT_RESIZED:
					{
						F_Application->WindowResized();
						break;
					}				
					case SDL_WINDOWEVENT_MAXIMIZED:
					{
						F_Application->WindowResized();
						break;
					}
					default:
					{
						break;
					}
				}
			}

			HandleEngineEvents(event);

			if (GameLoopStarted())
			{
				for (MappingContext& context : F_MappingContexts)
				{
					HandleContextEvents(context, event, firedKeys);
				}
			}
		}
	}

	void HandleEngineEvents(SDL_Event event)
	{
		// Keyboard Keys Down
		if (event.type == SDL_KEYDOWN)
		{
			// Scene View keybinds
			if (F_b_sceneViewFocused)
			{
				switch (event.key.keysym.sym)
				{
				case SDLK_1:
					F_CursorMode = F_CURSOR_MODE::TRANSLATE;
					break;

				case SDLK_2:
					F_CursorMode = F_CURSOR_MODE::SCALE;
					break;

				case SDLK_3:
					F_CursorMode = F_CURSOR_MODE::ROTATE;
					break;

				case SDLK_4:
					F_CursorMode = F_CURSOR_MODE::TILE_BRUSH;
					break;

				case SDLK_5:
					F_CursorMode = F_CURSOR_MODE::TILE_ERASE;
					break;

				case SDLK_6:
					F_CursorMode = F_CURSOR_MODE::TILE_COLLIDER_DRAW;
					break;

				case SDLK_7:
					F_CursorMode = F_CURSOR_MODE::TILE_MULTISELECT;
					break;

				case SDLK_8:
					F_CursorMode = F_CURSOR_MODE::TILE_MOVE;
					break;
				}
			}
		}
	}

	void HandleContextEvents(MappingContext& context, SDL_Event event, std::vector<std::string> &firedKeys)
	{
		// Keyboard Keys Down
		if (event.type == SDL_KEYDOWN)
		{
			if (F_MappedKeyboardCodes.count(event.key.keysym.sym))
			{
				std::string key = F_MappedKeyboardCodes.at(event.key.keysym.sym);
				if (context.FireEvent(key, event))
				{
					firedKeys.push_back(key);
				}
			}
		}
		// Keyboard Keys Up
		else if (event.type == SDL_KEYUP)
		{
			// Clear Mapping Context Events of buttons that are released
			if (F_MappedKeyboardCodes.count(event.key.keysym.sym))
			{
				std::string key = F_MappedKeyboardCodes.at(event.key.keysym.sym);
				context.ClearInputActionEvent(key);
			}
		}
		// Axis (analog inputs)
		else if (event.type == SDL_JOYAXISMOTION)
		{
			// Axis (analogs)
			//if (event.jaxis.which == 0)
			//{			
			if (F_MappedXInputAnalogCodes.count(event.jaxis.axis))
			{
				std::string key = F_MappedXInputAnalogCodes.at(event.jaxis.axis);			
				if (event.jaxis.value > -F_JOYSTICK_DEAD_ZONE && event.jaxis.value < F_JOYSTICK_DEAD_ZONE)
				{
					event.jaxis.value = 0;
				}					
				context.FireEvent(key, event);
			}
		}
		// Buttons Down
		else if (event.type == SDL_JOYBUTTONDOWN)
		{
			if (F_MappedXInputButtonCodes.count(event.jbutton.button))
			{
				std::string key = F_MappedXInputButtonCodes.at(event.jbutton.button);
				if (context.FireEvent(key, event))
				{					
					firedKeys.push_back(key);
				}
			}
		}
		// Buttons Up
		else if (event.type == SDL_JOYBUTTONUP)
		{
			if (F_MappedXInputButtonCodes.count(event.jbutton.button))
			{
				std::string key = F_MappedXInputButtonCodes.at(event.jbutton.button);
				context.ClearInputActionEvent(key);
			}
		}
		// Hats
		else if (event.type == SDL_JOYHATMOTION)
		{
			if (F_MappedXInputDPadCodes.count(event.jhat.value))
			{
				std::string key = F_MappedXInputDPadCodes.at(event.jhat.value);
				LogString(key);
			}
		}
	}

	void RemapInputAction(std::string contextName, std::string inputAction, Uint32 timeoutTime)
	{
		for (MappingContext &context : F_MappingContexts)
		{
			if (context.GetName() == contextName)
			{
				context.SetRemapStartTime(GetEngineTime());
				context.SetWaitingForRemap(true);
				context.SetActionToRemap(inputAction);
				context.SetRemapTimeoutTime(timeoutTime);
			}
		}
	}

	// TileSet / TileMap Management
	void CreateNewTileSetFile(std::string fileName, std::string path)
	{
		std::string filePath = "";		
		TileSet tileSet = TileSet();

		if (path == "")
		{
			filePath = GetDir("tileSets") + "\\" + fileName + ".tls";
		}
		else
		{
			filePath = path + "\\" + fileName + ".tls";
		}

		tileSet.SetTileSetPath(filePath);
		tileSet.SetName(fileName);
		SaveTileSet(tileSet);
		AddTileSet(filePath);
	}

	void SaveTileSet(TileSet tileSet)
	{
		std::string texturePath = tileSet.GetTexturePath();
		std::string filepath = tileSet.GetTileSetPath();		

		std::ofstream fileObject;
		std::ifstream ifstream(filepath);

		// Delete old contents of the file
		fileObject.open(filepath, std::ofstream::out | std::ofstream::trunc);
		fileObject.close();

		// Opening file in append mode
		fileObject.open(filepath, std::ios::app);

		std::string data = tileSet.GetData();		
		json tileArray = json::array();

		for (int index : tileSet.GetTileSetIndices())
		{
			json jsonData = {
				{ "index", index }
			};
			
			std::string tileData = jsonData.dump();
			tileArray.push_back(json::parse(tileData));
		}
		
		json tileSetJson = json::object({
			{ "tileSetData", json::parse(data) },
			{ "tiles", tileArray }
		});

		fileObject << tileSetJson.dump(4).c_str() << std::endl;
		fileObject.close();
	}

	void AddTileSet(std::string path)
	{
		TileSet tileSet = TileSet();
		json tileSetJson = LoadFileData(path);

		if (tileSetJson != nullptr)
		{
			auto tileSetData = tileSetJson["tileSetData"];

			tileSet.SetName(tileSetData["name"]);
			tileSet.SetTileSetPath(tileSetData["tileSetPath"]);
			tileSet.SetTexturePath(tileSetData["texturePath"]);
			tileSet.SetTileWidth(tileSetData["tileWidth"]);
			tileSet.SetTileHeight(tileSetData["tileHeight"]);

			std::vector<int> indices;
			for (auto tile : tileSetJson["tiles"])
			{
				int index = CheckJsonInt(tile, "index", "TileSet tiles");
				if (index != -1)
				{
					indices.push_back(index);
				}
			}
			tileSet.SetTileSetIndices(indices);

			F_TileSets.push_back(tileSet);
		}
	}

	void InitializeTileSets()
	{
		F_TileSets.clear();

		std::vector<std::string> tileSetFiles = std::vector<std::string>();
		tileSetFiles = FindAllFilesWithExtension(GetDir("projectDir"), ".tls");

		for (std::string path : tileSetFiles)
		{			
			AddTileSet(path);
		}
	}

	TileSet* GetTileSet(std::string tileSetName) 
	{
		for (std::vector<TileSet>::iterator iter = F_TileSets.begin(); iter != F_TileSets.end(); iter++)
		{
			if (iter->GetName() == tileSetName)
			{
				return &(*iter);
			}			
		}

		return nullptr;
	}


	// Rendering
	void BeginImGuiRender()
	{	
		ImGui_ImplSDLRenderer2_NewFrame();
		ImGui_ImplSDL2_NewFrame();
		ImGui::NewFrame();
		ImGui::GetIO().MouseDelta;

		//Create dockable background space for all viewports
		ImGui::DockSpaceOverViewport();
	}
	
	void EndImGuiRender()
	{		
		Vector4 clearColor = Vector4(1.00f, 1.00f, 1.00f, 1.00f);
		ImGui::Render();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		SDL_RenderSetScale(F_Window->GetRenderer(), io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
		SDL_SetRenderDrawColor(F_Window->GetRenderer(), (Uint8)(clearColor.x * 255), (Uint8)(clearColor.y * 255), (Uint8)(clearColor.z * 255), (Uint8)(clearColor.w * 255));
		SDL_RenderClear(F_Window->GetRenderer());
		ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());
		
		SDL_RenderPresent(F_Window->GetRenderer());		
	}

	void SetNextViewportToFillWindow()
	{
		ImGuiIO io = ImGui::GetIO();
		ImGui::SetNextWindowSize(io.DisplaySize);
		ImGui::SetNextWindowPos({ 0,0 });
	}


	// Animations
	void CreateNewAnimationFile(std::string filename, std::string path)
	{		
		std::string filePath = "";

		if (path == "")
		{
			filePath = GetDir("animations") + filename + ".anm";
		}
		else
		{
			filePath = path + "\\" + filename + ".anm";
		}

		std::shared_ptr<Animation::S_AnimationProperties> propertiesObject = std::make_shared< Animation::S_AnimationProperties>();
		propertiesObject->animationName = filename;
		propertiesObject->animationPath = filePath;
		SaveAnimationFile(propertiesObject, filePath);
	}

	void SaveAnimationFile(std::shared_ptr<Animation::S_AnimationProperties> propertiesObject, std::string path)
	{		
		std::ofstream fileObject;
		std::ifstream ifstream(path);

		// Delete old contents of the file
		fileObject.open(path, std::ofstream::out | std::ofstream::trunc);
		fileObject.close();

		// Opening file in append mode
		fileObject.open(path, std::ios::app);


		json eventProps = json::array();
		for (std::shared_ptr<Animation::S_Event> eventProp : propertiesObject->eventProps)
		{
			json jsonData = {
				{ "functionName", eventProp->functionName },
				{ "time", eventProp->time },
			};

			json parameters = json::array();
			for (Animation::S_EventFunctionParam parameter : eventProp->parameters)
			{
				parameters.push_back({
					{ "type", parameter.type },
					{ "string", parameter.e_string },
					{ "int", parameter.e_int },
					{ "float", parameter.e_float },
					{ "double", parameter.e_double },
					{ "long", parameter.e_long },
					{ "bool", parameter.e_boolean },
					{ "vector2X", parameter.e_Vector2.x },
					{ "vector2Y", parameter.e_Vector2.y },
				});
			}
			jsonData.push_back({ "parameters", parameters });

			std::string data = jsonData.dump();
			eventProps.push_back(json::parse(data));
		}
				
		json transformProps = json::array();
		for (std::shared_ptr<Animation::S_Transform> transformProp : propertiesObject->transformProps)
		{		
			json jsonData = {
				{ "transformInterpType", transformProp->transformInterpType },
				{ "transformSpeed", transformProp->transformSpeed },
				{ "scaleInterpType", transformProp->scaleInterpType },
				{ "scaleSpeed", transformProp->scaleSpeed },
				{ "time", transformProp->time },
				{ "xPos", transformProp->xPos },
				{ "yPos", transformProp->yPos },
				{ "xScale", transformProp->xScale },
				{ "yScale", transformProp->yScale },
				{ "_posAnimated", transformProp->b_posAnimated },
				{ "_scaleAnimated", transformProp->b_scaleAnimated }
			};
			std::string data = jsonData.dump();
			transformProps.push_back(json::parse(data));
		}
	
		json spriteProps = json::array();
		for (std::shared_ptr<Animation::S_Sprite> spriteProp : propertiesObject->spriteProps)
		{
			json jsonData = {
				{ "interpType", spriteProp->interpType },
				{ "speed", spriteProp->speed },
				{ "time", spriteProp->time },
				{ "path", spriteProp->path },
				{ "xOffset", spriteProp->xOffset },
				{ "yOffset", spriteProp->yOffset },
				{ "tintColorX", spriteProp->tintColor.x },
				{ "tintColorY", spriteProp->tintColor.y },
				{ "tintColorZ", spriteProp->tintColor.z },
				{ "tintColorW", spriteProp->tintColor.w },
				{ "_instantTintChange", spriteProp->b_instantTintChange },
				{ "_pathAnimated", spriteProp->b_pathAnimated },
				{ "_offsetAnimated", spriteProp->b_offsetAnimated },
				{ "_scaleAnimated", spriteProp->b_scaleAnimated },
				{ "_tintColorAnimated", spriteProp->b_tintColorAnimated }
			};

			std::string data = jsonData.dump();
			spriteProps.push_back(json::parse(data));
		}
		
		json cameraProps = json::array();
		for (std::shared_ptr<Animation::S_Camera> cameraProp : propertiesObject->cameraProps)
		{
			json jsonData = {
				{ "time", cameraProp->time },
				{ "_isPrimaryCamera", cameraProp->b_isPrimaryCamera }
			};
			std::string data = jsonData.dump();
			cameraProps.push_back(json::parse(data));
		}
		
		json scriptProps = json::array();
		for (std::shared_ptr<Animation::S_Script> scriptProp : propertiesObject->scriptProps)
		{
			json jsonData = {
				{ "time", scriptProp->time },
				{ "path", scriptProp->path }
			};
			std::string data = jsonData.dump();
			scriptProps.push_back(json::parse(data));
		}
		
		json buttonProps = json::array();
		for (std::shared_ptr<Animation::S_Button> buttonProp : propertiesObject->buttonProps)
		{
			json jsonData = {
				{ "time", buttonProp->time },
				{ "_isActive", buttonProp->b_isActive }
			};
			std::string data = jsonData.dump();
			buttonProps.push_back(json::parse(data));
		}
		
		json canvasProps = json::array();
		for (std::shared_ptr<Animation::S_Canvas> canvasProp : propertiesObject->canvasProps)
		{
			json jsonData = {
				{ "time", canvasProp->time }
			};
			std::string data = jsonData.dump();
			canvasProps.push_back(json::parse(data));
		}
		
		json audioProps = json::array();
		for (std::shared_ptr<Animation::S_Audio> audioProp : propertiesObject->audioProps)
		{
			json jsonData = {
				{ "time", audioProp->time },
				{ "soundName", audioProp->soundName },
				{ "_stopAllOtherSounds", audioProp->b_stopAllOtherSounds }
			};
			std::string data = jsonData.dump();
			audioProps.push_back(json::parse(data));
		}
		
		json textProps = json::array();
		for (std::shared_ptr<Animation::S_Text> textProp : propertiesObject->textProps)
		{
			json jsonData = {
				{ "time", textProp->time },
				{ "fontPath", textProp->fontPath },
				{ "text", textProp->text },
				{ "xOffset", textProp->xOffset },
				{ "yOffset", textProp->yOffset },
				{ "tintColorX", textProp->tintColor.x },
				{ "tintColorY", textProp->tintColor.y },
				{ "tintColorZ", textProp->tintColor.z },
				{ "tintColorW", textProp->tintColor.w },
				{ "_instantTintChange", textProp->b_instantTintChange },
				{ "_fontPathAnimated", textProp->b_fontPathAnimated },
				{ "_textAnimated", textProp->b_textAnimated },
				{ "_tintColorAnimated", textProp->b_tintColorAnimated },
				{ "_offsetAnimated", textProp->b_offsetAnimated }
			};
			std::string data = jsonData.dump();
			textProps.push_back(json::parse(data));
		}
		
		json boxColliderProps = json::array();
		for (std::shared_ptr<Animation::S_BoxCollider> boxColliderProp : propertiesObject->boxColliderProps)
		{
			json jsonData = {
				{ "time", boxColliderProp->time },
				{ "_isActive", boxColliderProp->b_isActive }
			};
			std::string data = jsonData.dump();
			boxColliderProps.push_back(json::parse(data));
		}
		
		json circleColliderProps = json::array();
		for (std::shared_ptr<Animation::S_CircleCollider> circleColliderProp : propertiesObject->circleColliderProps)
		{
			json jsonData = {
				{ "time", circleColliderProp->time },
				{ "_isActive", circleColliderProp->b_isActive }
			};
			std::string data = jsonData.dump();
			circleColliderProps.push_back(json::parse(data));
		}
		
		json rigidBodyProps = json::array();
		for (std::shared_ptr<Animation::S_RigidBody> rigidBodyProp : propertiesObject->rigidBodyProps)
		{
			json jsonData = {
				{ "time", rigidBodyProp->time },
				{ "interpType", rigidBodyProp->interpType },
				{ "speed", rigidBodyProp->speed },
				{ "_isActive", rigidBodyProp->b_isActive },
				{ "gravityScale", rigidBodyProp->gravityScale },
			};
			std::string data = jsonData.dump();
			rigidBodyProps.push_back(json::parse(data));
		}
	
		json characterControllerProps = json::array();
		for (std::shared_ptr<Animation::S_CharacterController> characterControllerProp : propertiesObject->characterControllerProps)
		{
			json jsonData = {
				{ "time", characterControllerProp->time },
				{ "_isActive", characterControllerProp->b_isActive }
			};
			std::string data = jsonData.dump();
			characterControllerProps.push_back(json::parse(data));
		}

		json animProps = json::object({
			{ "event", eventProps },
			{ "transform", transformProps },
			{ "sprite", spriteProps },
			{ "camera", cameraProps },
			{ "script", scriptProps },
			{ "button", buttonProps },
			{ "canvas", canvasProps },
			{ "audio", audioProps },
			{ "text", textProps },
			{ "boxCollider", boxColliderProps },
			{ "circleCollider", circleColliderProps },
			{ "rigidBody", rigidBodyProps },
			{ "characterController", characterControllerProps }
		});

		json animationData = json::object({
			{ "name", propertiesObject->animationName },
			{ "length", propertiesObject->animationLength },
			{ "_loop", propertiesObject->b_loop },
			{ "animationProperties", animProps }
		});
		
		json newFileObject = json::object({ {"animation", animationData } });
		fileObject << newFileObject.dump(4).c_str() << std::endl;	
		fileObject.close();
	}

	std::shared_ptr<Animation::S_AnimationProperties> LoadAnimationFile(std::string path)
	{
		std::shared_ptr<Animation::S_AnimationProperties> animProps = std::make_shared<Animation::S_AnimationProperties>();
		std::shared_ptr<Animation::S_Event> eventProperties;
		std::shared_ptr<Animation::S_Transform> transformProperties;
		std::shared_ptr<Animation::S_Sprite > spriteProperties;
		animProps->animationPath = path;

		std::ofstream fileObject;
		std::ifstream ifstream(path);
		fileObject.open(path, std::ios::in);
		std::string fileContent = "";

		if (fileObject.good())
		{
			std::string line;
			while (!ifstream.eof()) 
			{
				std::getline(ifstream, line);
				fileContent.append(line + "\n");
			}
		}

		fileObject.close();

		if (fileObject.good())
		{
			json fileContentJson = json::parse(fileContent);

			if (fileContentJson.contains("animation"))
			{
				animProps->animationName = "New Animation";
				json animationJson = fileContentJson["animation"];
				std::string animName = CheckJsonString(animationJson, "name", "name");

				animProps->animationName = animName;
				animProps->animationLength = CheckJsonFloat(animationJson, "length", animName);
				animProps->b_loop = CheckJsonBool(animationJson, "_loop", animName);
				
				json eventProps = animationJson["animationProperties"]["event"];
				for (int i = 0; i < eventProps.size(); i++)
				{
					std::shared_ptr<Animation::S_Event> frame = std::make_shared<Animation::S_Event>();
					frame->name = "Event";
					frame->functionName = CheckJsonString(eventProps[i], "functionName", animName);
					frame->time = CheckJsonFloat(eventProps[i], "time", animName);

					for (int p = 0; p < eventProps[i]["parameters"].size(); p++)
					{
						json param = eventProps[i]["parameters"][p];						
						Animation::S_EventFunctionParam parameter;
						parameter.type = CheckJsonString(param, "type", animName);
						parameter.e_string = CheckJsonString(param, "string", animName);
						parameter.e_int = CheckJsonInt(param, "int", animName);
						parameter.e_float = CheckJsonFloat(param, "float", animName);
						parameter.e_long = CheckJsonLong(param, "long", animName);
						parameter.e_double = CheckJsonDouble(param, "double", animName);
						parameter.e_boolean = CheckJsonBool(param, "bool", animName);
						parameter.e_Vector2 = Vector2(CheckJsonFloat(param, "vector2X", animName), CheckJsonFloat(param, "vector2Y", animName));

						frame->parameters.push_back(parameter);
					}

					animProps->eventProps.push_back(frame);				
				}
				
				json transformProps = animationJson["animationProperties"]["transform"];
				for (int i = 0; i < transformProps.size(); i++)
				{
					std::shared_ptr<Animation::S_Transform> frame = std::make_shared<Animation::S_Transform>();
					frame->name = "Transform";
					frame->transformInterpType = (Animation::InterpType)CheckJsonInt(transformProps[i], "transformInterpType", animName);
					frame->scaleInterpType = (Animation::InterpType)CheckJsonInt(transformProps[i], "scaleInterpType", animName);
					frame->scaleSpeed = CheckJsonFloat(transformProps[i], "scaleSpeed", animName);
					frame->time = CheckJsonFloat(transformProps[i], "time", animName);
					frame->xPos = CheckJsonFloat(transformProps[i], "xPos", animName);
					frame->yPos = CheckJsonFloat(transformProps[i], "yPos", animName);
					frame->xScale = CheckJsonFloat(transformProps[i], "xScale", animName);
					frame->yScale = CheckJsonFloat(transformProps[i], "yScale", animName);
					frame->b_posAnimated = CheckJsonBool(transformProps[i], "_posAnimated", animName);
					frame->b_scaleAnimated = CheckJsonBool(transformProps[i], "_scaleAnimated", animName);
					animProps->transformProps.push_back(frame);
				}
				
				json spriteProps = animationJson["animationProperties"]["sprite"];
				for (int i = 0; i < spriteProps.size(); i++)
				{
					std::shared_ptr<Animation::S_Sprite> frame = std::make_shared<Animation::S_Sprite>();
					frame->name = "Sprite";
					frame->interpType = (Animation::InterpType)CheckJsonInt(spriteProps[i], "interpType", animName);
					frame->speed = CheckJsonFloat(spriteProps[i], "speed", animName);
					frame->time = CheckJsonFloat(spriteProps[i], "time", animName);
					frame->xOffset = CheckJsonFloat(spriteProps[i], "xOffset", animName);
					frame->yOffset = CheckJsonFloat(spriteProps[i], "yOffset", animName);
					frame->path = CheckJsonString(spriteProps[i], "path", animName);
					frame->b_instantTintChange = CheckJsonBool(spriteProps[i], "_instantTintChange", animName);
					frame->tintColor = Vector4(
						CheckJsonFloat(spriteProps[i], "tintColorX", animName),
						CheckJsonFloat(spriteProps[i], "tintColorY", animName),
						CheckJsonFloat(spriteProps[i], "tintColorZ", animName),
						CheckJsonFloat(spriteProps[i], "tintColorW", animName)
					);
					frame->b_pathAnimated = CheckJsonBool(spriteProps[i], "_pathAnimated", animName);
					frame->b_scaleAnimated = CheckJsonBool(spriteProps[i], "_scaleAnimated", animName);
					frame->b_offsetAnimated = CheckJsonBool(spriteProps[i], "_offsetAnimated", animName);
					frame->b_tintColorAnimated = CheckJsonBool(spriteProps[i], "_tintColorAnimated", animName);
					animProps->spriteProps.push_back(frame);			
				}
				
				json cameraProps = animationJson["animationProperties"]["camera"];
				for (int i = 0; i < cameraProps.size(); i++)
				{
					std::shared_ptr<Animation::S_Camera> frame = std::make_shared<Animation::S_Camera>();
					frame->name = "Camera";
					frame->time = CheckJsonFloat(cameraProps[i], "time", animName);
					frame->b_isPrimaryCamera = CheckJsonBool(cameraProps[i], "_isPrimaryCamera", animName);
					animProps->cameraProps.push_back(frame);
				}
				
				json scriptProps = animationJson["animationProperties"]["script"];
				for (int i = 0; i < scriptProps.size(); i++)
				{
					std::shared_ptr<Animation::S_Script> frame = std::make_shared<Animation::S_Script>();
					frame->name = "Script";
					frame->time = CheckJsonFloat(scriptProps[i], "time", animName);
					frame->path = CheckJsonString(scriptProps[i], "path", animName);
					animProps->scriptProps.push_back(frame);
				}
				
				json buttonProps = animationJson["animationProperties"]["button"];
				for (int i = 0; i < buttonProps.size(); i++)
				{
					std::shared_ptr<Animation::S_Button> frame = std::make_shared<Animation::S_Button>();
					frame->name = "Button";
					frame->time = CheckJsonFloat(buttonProps[i], "time", animName);
					frame->b_isActive = CheckJsonBool(buttonProps[i], "_isActive", animName);
					animProps->buttonProps.push_back(frame);
				}
				
				json canvasProps = animationJson["animationProperties"]["canvas"];
				for (int i = 0; i < canvasProps.size(); i++)
				{
					std::shared_ptr<Animation::S_Canvas> frame = std::make_shared<Animation::S_Canvas>();
					frame->name = "Canvas";
					frame->time = CheckJsonFloat(canvasProps[i], "time", animName);
					animProps->canvasProps.push_back(frame);
				}
				
				json audioProps = animationJson["animationProperties"]["audio"];
				for (int i = 0; i < audioProps.size(); i++)
				{
					std::shared_ptr<Animation::S_Audio> frame = std::make_shared<Animation::S_Audio>();
					frame->name = "Audio";
					frame->time = CheckJsonFloat(audioProps[i], "time", animName);
					frame->soundName = CheckJsonString(audioProps[i], "soundName", animName);
					frame->b_stopAllOtherSounds = CheckJsonBool(audioProps[i], "_stopAllOtherSounds", animName);
					animProps->audioProps.push_back(frame);
				}
				
				json textProps = animationJson["animationProperties"]["text"];
				for (int i = 0; i < textProps.size(); i++)
				{
					std::shared_ptr<Animation::S_Text> frame = std::make_shared<Animation::S_Text>();
					frame->name = "Text";
					frame->time = CheckJsonFloat(textProps[i], "time", animName);
					frame->fontPath = CheckJsonString(textProps[i], "fontPath", frame->name);
					frame->text = CheckJsonString(textProps[i], "text", frame->name);
					frame->tintColor = Vector4(
						CheckJsonFloat(textProps[i], "tintColorX", frame->name),
						CheckJsonFloat(textProps[i], "tintColorY", frame->name),
						CheckJsonFloat(textProps[i], "tintColorZ", frame->name),
						CheckJsonFloat(textProps[i], "tintColorW", frame->name)
					);
					frame->b_fontPathAnimated = CheckJsonBool(textProps[i], "_fontPathAnimated", animName);
					frame->b_textAnimated = CheckJsonBool(textProps[i], "_textAnimated", animName);
					frame->b_tintColorAnimated = CheckJsonBool(textProps[i], "_tintColorAnimated", animName);
					frame->b_offsetAnimated = CheckJsonBool(textProps[i], "_offsetAnimated", animName);
					animProps->textProps.push_back(frame);
				}
				
				json boxColliderProps = animationJson["animationProperties"]["boxCollider"];
				for (int i = 0; i < boxColliderProps.size(); i++)
				{
					std::shared_ptr<Animation::S_BoxCollider> frame = std::make_shared<Animation::S_BoxCollider>();
					frame->name = "BoxCollider";
					frame->time = CheckJsonFloat(boxColliderProps[i], "time", animName);
					frame->b_isActive = CheckJsonBool(boxColliderProps[i], "_isActive", animName);
					animProps->boxColliderProps.push_back(frame);
				}
				
				json circleColliderProps = animationJson["animationProperties"]["circleCollider"];
				for (int i = 0; i < circleColliderProps.size(); i++)
				{
					std::shared_ptr<Animation::S_CircleCollider> frame = std::make_shared<Animation::S_CircleCollider>();
					frame->name = "CircleCollider";
					frame->time = CheckJsonFloat(circleColliderProps[i], "time", animName);
					frame->b_isActive = CheckJsonBool(circleColliderProps[i], "_isActive", animName);
					animProps->circleColliderProps.push_back(frame);
				}
				
				json rigidBodyProps = animationJson["animationProperties"]["rigidBody"];
				for (int i = 0; i < rigidBodyProps.size(); i++)
				{
					std::shared_ptr<Animation::S_RigidBody> frame = std::make_shared<Animation::S_RigidBody>();
					frame->name = "RigidBody";
					frame->time = CheckJsonFloat(rigidBodyProps[i], "time", animName);
					frame->b_isActive = CheckJsonBool(rigidBodyProps[i], "_isActive", animName);
					frame->interpType = (Animation::InterpType)CheckJsonInt(rigidBodyProps[i], "interpType", animName);
					frame->speed = CheckJsonFloat(rigidBodyProps[i], "speed", animName);
					frame->gravityScale = CheckJsonFloat(rigidBodyProps[i], "gravityScale", animName);
					animProps->rigidBodyProps.push_back(frame);
				}
				
				json characterControllerProps = animationJson["animationProperties"]["characterController"];
				for (int i = 0; i < characterControllerProps.size(); i++)
				{
					std::shared_ptr<Animation::S_CharacterController> frame = std::make_shared<Animation::S_CharacterController>();
					frame->name = "CharacterController";
					frame->time = CheckJsonFloat(characterControllerProps[i], "time", animName);
					frame->b_isActive = CheckJsonBool(characterControllerProps[i], "_isActive", animName);
					animProps->characterControllerProps.push_back(frame);
				}
			}
		}

		return animProps;
	}


	// Prefabs
	void CreatePrefab(std::string path, GameObject &gameObject)
	{
		F_PrefabManager->CreatePrefab(path, gameObject);
	}

	void InitializePrefabs()
	{
		F_PrefabManager->InitializePrefabs();
	}

	GameObject *Instantiate(std::string prefabName, Vector2 position, Scene* scene, long parentID, long ID)
	{
		return F_PrefabManager->Instantiate(prefabName, position, scene, parentID, ID);
	}

	
	// Collision Detection
	void UpdateColliderPairs()
	{
		std::map<long, std::map<long, BoxCollider>>& sceneBoxColliders = GetLoadedScene()->GetBoxColliders();
		std::map<long, std::map<long, BoxCollider>> &persistantBoxColliders = GetLoadedProject().GetPersistantGameObjectScene()->GetBoxColliders();

		//if (GetLoadedScene() != nullptr)
		//{
		//	sceneBoxColliders = ;
		//}
		//if (GetLoadedProject().GetPersistantGameObjectScene() != nullptr)
		//{
		//	persistantBoxColliders = 
		//}

		// Remake colliderPairs
		F_ColliderPairs.clear();
		std::vector<BoxCollider*> colliders;

		// Collect BoxColliders into a simple to navigate vector
		for (std::map<long, std::map<long, BoxCollider>>::iterator colliderMap = sceneBoxColliders.begin(); colliderMap != sceneBoxColliders.end();)
		{
			for (std::map<long, BoxCollider>::iterator innerMap = colliderMap->second.begin(); innerMap != colliderMap->second.end();)
			{
				colliders.push_back(&innerMap->second);
				innerMap++;
			}
			colliderMap++;
		}
		for (std::map<long, std::map<long, BoxCollider>>::iterator colliderMap = persistantBoxColliders.begin(); colliderMap != persistantBoxColliders.end();)
		{
			for (std::map<long, BoxCollider>::iterator innerMap = colliderMap->second.begin(); innerMap != colliderMap->second.end();)
			{
				colliders.push_back(&innerMap->second);
				innerMap++;
			}
			colliderMap++;
		}

		int colliderCounter = 1;
		for (std::vector<BoxCollider*>::iterator collider1 = colliders.begin(); collider1 != colliders.end(); collider1++)
		{
			for (std::vector<BoxCollider*>::iterator collider2 = collider1 + colliderCounter; collider2 != colliders.end(); collider2++)
			{
				if ((*collider1)->GetParentID() != (*collider2)->GetParentID())
				{
					TagList coll1TagList = (*collider1)->GetParent()->GetTagList();
					TagList coll2TagList = (*collider2)->GetParent()->GetTagList();

					std::vector<std::string> coll1Ignored = coll1TagList.GetIgnoredTags();
					std::vector<std::string> coll2Ignored = coll2TagList.GetIgnoredTags();

					bool _ignorePair = false;

					for (std::string ignoredTag : coll1Ignored)
					{
						if (coll2TagList.HasTag(ignoredTag))
						{
							_ignorePair = true;
							break;
						}
					}
					if (!_ignorePair)
					{
						for (std::string ignoredTag : coll2Ignored)
						{
							if (coll1TagList.HasTag(ignoredTag))
							{
								_ignorePair = true;
								break;
							}
						}
					}
					if (!_ignorePair)
					{
						std::pair<Collider*, Collider*> newPair = { (*collider1), (*collider2) };
						F_ColliderPairs.push_back(newPair);
					}
				}
			}
		}
	}


	// Logging
	void LogError(std::string line, std::string from)
	{
		//ImGui::SetWindowFocus("Logger"); Find a way to set focus once but not have it completely disable all other GUI interaction
		F_Logger.LogString("ERROR : " + line, from);
	}

	void LogString(std::string line, std::string from)
	{
		F_Logger.LogString(line, from);
	}

	void LogFloat(float var, std::string line, std::string from)
	{
		F_Logger.LogFloat(var, line, from);
	}

	void LogDouble(double var, std::string line, std::string from)
	{
		F_Logger.LogDouble(var, line, from);
	}

	void LogLong(long var, std::string line, std::string from)
	{
		F_Logger.LogLong(var, line, from);
	}

	void LogInt(int var, std::string line, std::string from)
	{
		F_Logger.LogInt(var, line, from);
	}

	void LogVector2(Vector2 vector, std::string line, std::string from)
	{
		F_Logger.LogVector2(vector, line, from);
	}

	void LogSeparator()
	{
		F_Logger.LogSeparator();
	}

	void DrawRectangle(Vector2 startingPoint, Vector2 endPoint, Vector2 canvas_p0, Vector2 canvas_sz, Vector4 color, float thickness, ImDrawList* drawList)
	{
		if (startingPoint.x < canvas_p0.x)
		{
			startingPoint.x = canvas_p0.x;
		}
		if (endPoint.x > canvas_p0.x + canvas_sz.x)
		{
			endPoint.x = canvas_p0.x + canvas_sz.x;
		}
		if (startingPoint.y < canvas_p0.y)
		{
			startingPoint.y = canvas_p0.y;
		}
		if (endPoint.y > canvas_p0.y + canvas_sz.y)
		{
			endPoint.y = canvas_p0.y + canvas_sz.y;
		}

		F_Logger.DrawRectangle(startingPoint, endPoint, color, thickness, drawList);
	}

	void DebugRectangle(Vector2 startingPoint, Vector2 endPoint, Vector4 color, float thickness, ImDrawList* drawList)
	{
		F_Logger.DrawRectangle(startingPoint, endPoint, color, thickness, drawList);
	}

	void DrawLine(Vector2 startingPoint, Vector2 endingPoint, Vector4 color, float thickness, ImDrawList* drawList)
	{
		F_Logger.DrawLine(startingPoint, endingPoint, color, thickness, drawList);
	}

	void DrawRectangleFromLines(Vector2* corners, Vector4 color, float thickness, ImDrawList* drawList)
	{
		DrawLine(corners[0], corners[1], color, thickness, drawList);
		DrawLine(corners[1], corners[2], color, thickness, drawList);
		DrawLine(corners[2], corners[3], color, thickness, drawList);
		DrawLine(corners[3], corners[0], color, thickness, drawList);
	}

	void DrawCircle(Vector2 center, float radius, Vector4 color, ImDrawList* drawList, float thickness, int segments)
	{
		drawList->AddCircle(center, radius, ImGui::GetColorU32(color), segments, thickness);
	}

	void DrawPoint(Vector2 point, Vector4 color, ImDrawList* drawList)
	{
		F_Logger.DrawPoint(point, color, drawList);
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

	float GetEllapsedGameTimeInSec()
	{
		return F_Application->GetGameLoop()->TimeEllapsedInSec();
	}

	long GetEllapsedGameTimeInMs()
	{
		return F_Application->GetGameLoop()->TimeEllapsedInMs();
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


	// Helper
	// 
	std::string OpenSaveFileExplorer()
	{
		std::string sSelectedFile;
		std::string sFilePath;
		HRESULT hr = 0;
		wchar_t* pSaveFileName = nullptr;
		IShellItem* pShellItem = nullptr;
		wchar_t* ppszName = nullptr;

		//  CREATE FILE OBJECT INSTANCE
		HRESULT f_SysHr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
		if (FAILED(f_SysHr))
		{
			return "";
		}

		// CREATE FileSaveDialog OBJECT
		IFileSaveDialog* f_FileSystem = nullptr;
		hr = CoCreateInstance(CLSID_FileSaveDialog, nullptr, CLSCTX_INPROC_SERVER, IID_IFileSaveDialog, (void**)(&f_FileSystem));
		if (FAILED(f_SysHr)) 
		{
			CoUninitialize();
			return "";
		}

		//  SHOW OPEN FILE DIALOG WINDOW
		f_SysHr = f_FileSystem->Show(nullptr);
		if (FAILED(f_SysHr)) 
		{
			f_FileSystem->Release();
			CoUninitialize();
			return "";
		}

		//  RETRIEVE FILE NAME FROM THE SELECTED ITEM
		IShellItem* f_Files;
		f_SysHr = f_FileSystem->GetResult(&f_Files);
		if (FAILED(f_SysHr)) 
		{
			f_FileSystem->Release();
			CoUninitialize();
			return "";
		}

		//  STORE AND CONVERT THE FILE NAME
		PWSTR f_Path;
		f_SysHr = f_Files->GetDisplayName(SIGDN_FILESYSPATH, &f_Path);
		if (FAILED(f_SysHr)) 
		{
			f_Files->Release();
			f_FileSystem->Release();
			CoUninitialize();
			return "";
		}

		//  FORMAT AND STORE THE FILE PATH
		std::wstring path(f_Path);
		std::string c(path.begin(), path.end());
		sFilePath = c;

		//  FORMAT STRING FOR EXECUTABLE NAME
		const size_t slash = sFilePath.find_last_of("/\\");
		sSelectedFile = sFilePath.substr(slash + 1);

		//  SUCCESS, CLEAN UP
		CoTaskMemFree(f_Path);
		f_Files->Release();
		f_FileSystem->Release();
		CoUninitialize();

		return sFilePath;
	}

	std::string OpenLoadFileExplorer()
	{
		std::string sSelectedFile;
		std::string sFilePath;
		HRESULT hr = 0;
		wchar_t* pSaveFileName = nullptr;
		IShellItem* pShellItem = nullptr;
		wchar_t* ppszName = nullptr;

		//  CREATE FILE OBJECT INSTANCE
		HRESULT f_SysHr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
		if (FAILED(f_SysHr))
		{
			return "";
		}

		// CREATE FileOpenDialog OBJECT
		IFileOpenDialog* f_FileSystem;
		f_SysHr = CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_ALL, IID_IFileOpenDialog, reinterpret_cast<void**>(&f_FileSystem));
		if (FAILED(f_SysHr)) 
		{
			CoUninitialize();
			return "";
		}

		//  SHOW OPEN FILE DIALOG WINDOW
		f_SysHr = f_FileSystem->Show(nullptr);
		if (FAILED(f_SysHr)) 
		{
			f_FileSystem->Release();
			CoUninitialize();
			return "";
		}

		//  RETRIEVE FILE NAME FROM THE SELECTED ITEM
		IShellItem* f_Files;
		f_SysHr = f_FileSystem->GetResult(&f_Files);
		if (FAILED(f_SysHr)) 
		{
			f_FileSystem->Release();
			CoUninitialize();
			return "";
		}

		//  STORE AND CONVERT THE FILE NAME
		PWSTR f_Path;
		f_SysHr = f_Files->GetDisplayName(SIGDN_FILESYSPATH, &f_Path);
		if (FAILED(f_SysHr)) 
		{
			f_Files->Release();
			f_FileSystem->Release();
			CoUninitialize();
			return "";
		}

		//  FORMAT AND STORE THE FILE PATH
		std::wstring path(f_Path);
		std::string c(path.begin(), path.end());
		sFilePath = c;

		//  FORMAT STRING FOR EXECUTABLE NAME
		const size_t slash = sFilePath.find_last_of("/\\");
		sSelectedFile = sFilePath.substr(slash);
		//  SUCCESS, CLEAN UP

		std::string relativePath = MakePathRelative(sFilePath);

		CoTaskMemFree(f_Path);
		f_Files->Release();
		f_FileSystem->Release();
		CoUninitialize();

	
		return relativePath;
	}

	std::string GetFilenameFromPath(std::string path, bool b_keepExtension)
	{
		std::string finalName = "";

		if (path != "")
		{
			const size_t slash = path.find_last_of("/\\");
			std::string wholeFilename = path.substr(slash + 1);
			const size_t dot1 = wholeFilename.find_last_of(".");
			std::string extension1 = "";

			if (dot1 < 100)
			{
				extension1 = wholeFilename.substr(dot1);
			}

			if (!b_keepExtension)
			{
				finalName = wholeFilename.substr(0, wholeFilename.size() - extension1.size());
				// For scripting files because they have (.scp.lua) two extensions
				const size_t dot2 = finalName.find_last_of(".");
				if (dot2 < 100)
				{
					std::string extension2 = finalName.substr(dot2);
					finalName = finalName.substr(0, finalName.size() - extension2.size());
				}
			}
			else
			{
				finalName = wholeFilename;
			}
		}

		return finalName;
	}

	//  Removes absolute path from the beginning of the selected filepath up to just after "FlatEngine"
	std::string MakePathRelative(std::string filepath)
	{
		std::string relativePath = "";
		std::string root = GetDir("root");
		size_t rootDirIndex;

		if (filepath != "")
		{
			if (root != "")
			{
				rootDirIndex = filepath.find(root) + 10;
			}
			else
			{
				rootDirIndex = 0;
			}

			if (rootDirIndex < 1000 && rootDirIndex != 0)
			{
				relativePath = ".." + filepath.substr(rootDirIndex);
			}
			else
			{
				relativePath = filepath;
			}
		}

		return relativePath;
	}

	std::string GetCurrentDir()
	{
		TCHAR buffer[MAX_PATH] = { 0 };
		GetModuleFileName(nullptr, buffer, MAX_PATH);
		std::wstring::size_type pos = std::wstring(buffer).find_last_of(L"\\/");
		std::wstring ws = std::wstring(buffer).substr(0, pos);
		std::string dir(ws.begin(), ws.end());
		return dir;
	}

	json LoadFileData(std::string filepath)
	{		
		std::ofstream fileObject;
		std::ifstream ifstream(filepath);

		// Open file in in mode
		fileObject.open(filepath, std::ios::in);		
		std::string fileContent = "";

		if (fileObject.good())
		{
			std::string line;
			while (!ifstream.eof()) {
				std::getline(ifstream, line);
				fileContent.append(line + "\n");
			}
		}
		
		fileObject.close();

		if (fileObject.good() && fileContent != "\n")
		{			
			return json::parse(fileContent);
		}
		else
		{
			return nullptr;
		}
	}


	bool DoesFileExist(std::string filepath)
	{
		return (filepath != "" && std::filesystem::exists(filepath));
	}

	bool FilepathHasExtension(std::string filepath, std::string extension)
	{
		if (extension.substr(0, 1) == ".")
		{
			extension = extension.substr(1);
		}
		const size_t dot = filepath.find_last_of(".");
		std::string actualExtension = filepath.substr(dot + 1);

		return actualExtension == extension;
	}

	void DeleteFileUsingPath(std::string filepath)
	{
		if (filepath != "" && DoesFileExist(filepath))
		{
			std::filesystem::remove_all(filepath);
		}
	}

	std::string GetFilePathUsingFileName(std::string dirPath, std::string name)
	{
		std::string file = "";

		for (auto& p : std::filesystem::recursive_directory_iterator(dirPath))
		{
			if (p.path().stem().string() == name || p.path().string().find(name) != std::string::npos)
			{
				file = p.path().string();
			}
		}

		return file;
	}

	std::vector<std::string> FindAllFilesWithExtension(std::string dirPath, std::string extension)
	{
		std::vector<std::string> files;

		for (auto& p : std::filesystem::recursive_directory_iterator(dirPath))
		{
			if (p.path().extension() == extension || p.path().string().find(extension) != std::string::npos)
			{
				files.push_back(p.path().string());
			}
		}

		return files;
	}


	//Vector4 objectA(top, right, bottom, left), Vector4 objectB(top, right, bottom, left)
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

	Vector2 Lerp(Vector2 startPos, Vector2 endPos, float ease)
	{
		Vector2 difference = Vector2(endPos.x - startPos.x, endPos.y - startPos.y);
		Vector2 easedDiff = Vector2(difference.x * ease, difference.y * ease);
		return Vector2(startPos.x + easedDiff.x, startPos.y + easedDiff.y);
	}


	// Helper Functions
	int GetRandInt(int low, int high)
	{
		std::random_device dev;
		std::mt19937 rng(dev());
		std::uniform_int_distribution<std::mt19937::result_type> dist(low, high);

		return dist(rng);
	}
	float GetRandFloat(float low, float high)
	{
		if (low < high)
		{
			std::random_device dev;
			std::mt19937 rng(dev());
			int multiplier = 1;

			if (low < 0)
			{
				float chanceToBeNegative = (float)(low*(-1) / (low*(-1) + high)) * 100;			
				std::uniform_int_distribution<std::mt19937::result_type> negativeDist(0, 100);
				if (negativeDist(rng) <= chanceToBeNegative)
				{
					multiplier *= -1;
				}

				low = 0;
			}

			low *= 100;
			high *= 100;
			std::uniform_int_distribution<std::mt19937::result_type> dist((unsigned int)(low), (unsigned int)(high));

			return (float)(dist(rng) / 100.0f * multiplier);
		}
		else
		{
			LogError("RandomFloat() - low must be lower than high.");
			return 0;
		}
	}

	// Json Parsing
	json CreateJsonFromObject(GameObject currentObject)
	{		
		json componentsArray = json::array();
		std::vector<Component*> components = currentObject.GetComponents();

		for (int j = 0; j < components.size(); j++)
		{
			std::string typeString = components[j]->GetTypeString();

			if (typeString != "Null")
			{
				std::string data = components[j]->GetData();
				componentsArray.push_back(json::parse(data));
			}
		}
		
		json childrenArray = json::array();

		for (int c = 0; c < currentObject.GetChildren().size(); c++)
		{
			childrenArray.push_back(currentObject.GetChildren()[c]);
		}
		
		std::map <std::string, bool> tagList = currentObject.GetTagList().GetTagsMap();
		std::map <std::string, bool> ignoreTagList = currentObject.GetTagList().GetIgnoreTagsMap();

		json tagsObjectJson = json::object({
			{ "Player", tagList.at("Player") },
			{ "Enemy", tagList.at("Enemy") },
			{ "Npc", tagList.at("Npc") },
			{ "Terrain", tagList.at("Terrain") },
			{ "PlayerTrigger", tagList.at("PlayerTrigger") },
			{ "EnemyTrigger", tagList.at("EnemyTrigger") },
			{ "NpcTrigger", tagList.at("NpcTrigger") },
			{ "EnvironmentalTrigger", tagList.at("EnvironmentalTrigger") },
			{ "TerrainTrigger", tagList.at("TerrainTrigger") },
			{ "PlayerDamage", tagList.at("PlayerDamage") },
			{ "EnemyDamage", tagList.at("EnemyDamage") },
			{ "EnvironmentalDamage", tagList.at("EnvironmentalDamage") },
			{ "Projectile", tagList.at("Projectile") },
			{ "InteractableItem", tagList.at("InteractableItem") },
			{ "InteractableObject", tagList.at("InteractableObject") },
			{ "Item", tagList.at("Item") },
		});

		json ignoreTagsObjectJson = json::object({
			{ "Player", ignoreTagList.at("Player") },
			{ "Enemy", ignoreTagList.at("Enemy") },
			{ "Npc", ignoreTagList.at("Npc") },
			{ "Terrain", ignoreTagList.at("Terrain") },
			{ "PlayerTrigger", ignoreTagList.at("PlayerTrigger") },
			{ "EnemyTrigger", ignoreTagList.at("EnemyTrigger") },
			{ "NpcTrigger", ignoreTagList.at("NpcTrigger") },
			{ "EnvironmentalTrigger", ignoreTagList.at("EnvironmentalTrigger") },
			{ "TerrainTrigger", ignoreTagList.at("TerrainTrigger") },
			{ "PlayerDamage", ignoreTagList.at("PlayerDamage") },
			{ "EnemyDamage", ignoreTagList.at("EnemyDamage") },
			{ "EnvironmentalDamage", ignoreTagList.at("EnvironmentalDamage") },
			{ "Projectile", ignoreTagList.at("Projectile") },
			{ "InteractableItem", ignoreTagList.at("InteractableItem") },
			{ "InteractableObject", ignoreTagList.at("InteractableObject") },
			{ "Item", ignoreTagList.at("Item") },
		});
		
		std::string objectName = currentObject.GetName();
		Vector2 spawnLocation = currentObject.GetPrefabSpawnLocation();
		if (currentObject.HasComponent("Transform"))
		{
			spawnLocation = currentObject.GetTransform()->GetPosition();
		}
		
		json gameObjectJson = json::object({
			{ "_isPersistant", currentObject.IsPersistant() },
			{ "_isPrefab", currentObject.IsPrefab() },
			{ "prefabName", currentObject.GetPrefabName() },
			{ "spawnLocationX", spawnLocation.x },
			{ "spawnLocationY", spawnLocation.y },
			{ "name", objectName },
			{ "id", currentObject.GetID() },
			{ "_isActive", currentObject.IsActive() },
			{ "parent", currentObject.GetParentID() },
			{ "children", childrenArray },
			{ "components", componentsArray },
			{ "tags", tagsObjectJson },
			{ "ignoreTags", ignoreTagsObjectJson },
		});

		return gameObjectJson;
	}

	bool JsonContains(json obj, std::string checkFor, std::string loadedName)
	{
		bool contains = false;
		if (obj.contains(checkFor))
		{
			contains = true;
		}
		else
		{
			LogError("JsonContains() - \"" + loadedName + "\" does not contain a value for \"" + checkFor + "\".");
		}
		return contains;
	}

	float CheckJsonFloat(json obj, std::string checkFor, std::string loadedName)
	{
		float value = -1;
		if (obj.contains(checkFor))
		{
			value = obj[checkFor];
		}
		else
		{
			LogError("CheckJsonFloat() - \"" + loadedName + "\" object does not contain a value for \"" + checkFor + "\".");
		}
		return value;
	}

	float CheckJsonFloat(json obj, std::string checkFor, std::string loadedName, std::string& errorMessage)
	{
		float value = -1;
		if (obj.contains(checkFor))
		{
			value = obj[checkFor];
		}
		else
		{
			errorMessage = "CheckJsonFloat() - \"" + loadedName + "\" object does not contain a value for \"" + checkFor + "\".";
		}
		return value;
	}

	int CheckJsonInt(json obj, std::string checkFor, std::string loadedName)
	{
		int value = -1;
		if (obj.contains(checkFor))
		{
			value = obj[checkFor];
		}
		else
		{
			LogError("CheckJsonInt() - \"" + loadedName + "\" object does not contain a value for \"" + checkFor + "\".");
		}
		return value;
	}

	int CheckJsonInt(json obj, std::string checkFor, std::string loadedName, std::string& errorMessage)
	{
		int value = -1;
		if (obj.contains(checkFor))
		{
			value = obj[checkFor];
		}
		else
		{
			errorMessage = "CheckJsonInt() - \"" + loadedName + "\" object does not contain a value for \"" + checkFor + "\".";
		}
		return value;
	}

	long CheckJsonLong(json obj, std::string checkFor, std::string loadedName)
	{
		long value = -1;
		if (obj.contains(checkFor))
		{
			value = obj[checkFor];
		}
		else
		{
			LogError("CheckJsonLong() - \"" + loadedName + "\" object does not contain a value for \"" + checkFor + "\".");
		}
		return value;
	}

	long CheckJsonLong(json obj, std::string checkFor, std::string loadedName, std::string& errorMessage)
	{
		long value = -1;
		if (obj.contains(checkFor))
		{
			value = obj[checkFor];
		}
		else
		{
			errorMessage = "CheckJsonLong() - \"" + loadedName + "\" object does not contain a value for \"" + checkFor + "\".";
		}
		return value;
	}

	double CheckJsonDouble(json obj, std::string checkFor, std::string loadedName)
	{
		double value = -1;
		if (obj.contains(checkFor))
		{
			value = obj[checkFor];
		}
		else
		{
			LogError("CheckJsonLong() - \"" + loadedName + "\" object does not contain a value for \"" + checkFor + "\".");
		}
		return value;
	}

	double CheckJsonDouble(json obj, std::string checkFor, std::string loadedName, std::string& errorMessage)
	{
		double value = -1;
		if (obj.contains(checkFor))
		{
			value = obj[checkFor];
		}
		else
		{
			errorMessage = "CheckJsonLong() - \"" + loadedName + "\" object does not contain a value for \"" + checkFor + "\".";
		}
		return value;
	}
	bool CheckJsonBool(json obj, std::string checkFor, std::string loadedName)
	{
		bool value = false;
		if (obj.contains(checkFor))
		{
			value = obj[checkFor];
		}
		else
		{
			LogError("CheckJsonBool() - \"" + loadedName + "\" object does not contain a value for \"" + checkFor + "\".");
		}
		return value;
	}

	bool CheckJsonBool(json obj, std::string checkFor, std::string loadedName, std::string& errorMessage)
	{
		bool value = false;
		if (obj.contains(checkFor))
		{
			value = obj[checkFor];
		}
		else
		{
			errorMessage = "CheckJsonBool() - \"" + loadedName + "\" object does not contain a value for \"" + checkFor + "\".";
		}
		return value;
	}

	std::string CheckJsonString(json obj, std::string checkFor, std::string loadedName)
	{		
		std::string value = "";
		if (obj.contains(checkFor))
		{
			value = obj[checkFor];
		}
		else
		{
			LogError("CheckJsonString() - \"" + loadedName + "\" object does not contain a value for \"" + checkFor + "\".");
		}
		return value;
	}

	std::string CheckJsonString(json obj, std::string checkFor, std::string loadedName, std::string& errorMessage)
	{
		std::string value = "";
		if (obj.contains(checkFor))
		{
			value = obj[checkFor];
		}
		else
		{
			errorMessage = "CheckJsonString() - \"" + loadedName + "\" object does not contain a value for \"" + checkFor + "\".";
		}
		return value;
	}

	GameObject *CreateObjectFromJson(json objectJson, Scene* scene)
	{
		GameObject *loadedObject;
		std::string objectName = CheckJsonString(objectJson, "name", "Name");
		bool b_isActive = CheckJsonBool(objectJson, "_isActive", objectName);
		bool b_isPrefab = CheckJsonBool(objectJson, "_isPrefab", objectName);
		bool b_isPersistant = CheckJsonBool(objectJson, "_isPersistant", objectName);
		std::string prefabName = CheckJsonString(objectJson, "prefabName", objectName);
		Vector2 spawnLocation = Vector2(0, 0);
		spawnLocation.x = CheckJsonFloat(objectJson, "spawnLocationX", objectName); // SetOrigin() is taken care of by Instantiate() using parentID
		spawnLocation.y = CheckJsonFloat(objectJson, "spawnLocationY", objectName);
		long loadedID = CheckJsonLong(objectJson, "id", objectName);
		long loadedParentID = CheckJsonLong(objectJson, "parent", objectName);
		std::vector<long> loadedChildrenIDs = std::vector<long>();
		TagList tags = TagList();

		if (JsonContains(objectJson, "children", objectName))
		{
			for (int c = 0; c < objectJson["children"].size(); c++)
			{
				loadedChildrenIDs.push_back(objectJson["children"][c]);
			}
		}

		if (b_isPrefab)
		{
			loadedObject = Instantiate(prefabName, spawnLocation, scene, loadedParentID, loadedID);
			loadedObject->SetName(objectName);
		}
		else
		{
			loadedObject = CreateGameObject(loadedParentID, loadedID, scene);
			
			loadedObject->SetName(objectName);
			loadedObject->SetActive(b_isActive);

			// TagList
			bool b_updateColliderPairs = false;
			if (JsonContains(objectJson, "tags", objectName))
			{
				json tagsObj = objectJson["tags"];
				tags.SetTag("Player", CheckJsonBool(tagsObj, "Player", objectName), b_updateColliderPairs);
				tags.SetTag("Enemy", CheckJsonBool(tagsObj, "Enemy", objectName), b_updateColliderPairs);
				tags.SetTag("Npc", CheckJsonBool(tagsObj, "Npc", objectName), b_updateColliderPairs);
				tags.SetTag("Terrain", CheckJsonBool(tagsObj, "Terrain", objectName), b_updateColliderPairs);
				tags.SetTag("PlayerTrigger", CheckJsonBool(tagsObj, "PlayerTrigger", objectName), b_updateColliderPairs);
				tags.SetTag("EnemyTrigger", CheckJsonBool(tagsObj, "EnemyTrigger", objectName), b_updateColliderPairs);
				tags.SetTag("NpcTrigger", CheckJsonBool(tagsObj, "NpcTrigger", objectName), b_updateColliderPairs);
				tags.SetTag("EnvironmentalTrigger", CheckJsonBool(tagsObj, "EnvironmentalTrigger", objectName), b_updateColliderPairs);
				tags.SetTag("TerrainTrigger", CheckJsonBool(tagsObj, "TerrainTrigger", objectName), b_updateColliderPairs);
				tags.SetTag("Projectile", CheckJsonBool(tagsObj, "Projectile", objectName), b_updateColliderPairs);
				tags.SetTag("PlayerDamage", CheckJsonBool(tagsObj, "PlayerDamage", objectName), b_updateColliderPairs);
				tags.SetTag("EnemyDamage", CheckJsonBool(tagsObj, "EnemyDamage", objectName), b_updateColliderPairs);
				tags.SetTag("EnvironmentalDamage", CheckJsonBool(tagsObj, "EnvironmentalDamage", objectName), b_updateColliderPairs);
				tags.SetTag("Projectile", CheckJsonBool(tagsObj, "Projectile", objectName), b_updateColliderPairs);
				tags.SetTag("InteractableItem", CheckJsonBool(tagsObj, "InteractableItem", objectName), b_updateColliderPairs);
				tags.SetTag("InteractableObject", CheckJsonBool(tagsObj, "InteractableObject", objectName), b_updateColliderPairs);
				tags.SetTag("Item", CheckJsonBool(tagsObj, "Item", objectName), b_updateColliderPairs);
			}
			if (JsonContains(objectJson, "ignoreTags", objectName))
			{
				json ignoreTags = objectJson["ignoreTags"];
				tags.SetIgnore("Player", CheckJsonBool(ignoreTags, "Player", objectName), b_updateColliderPairs);
				tags.SetIgnore("Enemy", CheckJsonBool(ignoreTags, "Enemy", objectName), b_updateColliderPairs);
				tags.SetIgnore("Npc", CheckJsonBool(ignoreTags, "Npc", objectName), b_updateColliderPairs);
				tags.SetIgnore("Terrain", CheckJsonBool(ignoreTags, "Terrain", objectName), b_updateColliderPairs);
				tags.SetIgnore("PlayerTrigger", CheckJsonBool(ignoreTags, "PlayerTrigger", objectName), b_updateColliderPairs);
				tags.SetIgnore("EnemyTrigger", CheckJsonBool(ignoreTags, "EnemyTrigger", objectName), b_updateColliderPairs);
				tags.SetIgnore("NpcTrigger", CheckJsonBool(ignoreTags, "NpcTrigger", objectName), b_updateColliderPairs);
				tags.SetIgnore("EnvironmentalTrigger", CheckJsonBool(ignoreTags, "EnvironmentalTrigger", objectName), b_updateColliderPairs);
				tags.SetIgnore("TerrainTrigger", CheckJsonBool(ignoreTags, "TerrainTrigger", objectName), b_updateColliderPairs);
				tags.SetIgnore("Projectile", CheckJsonBool(ignoreTags, "Projectile", objectName), b_updateColliderPairs);
				tags.SetIgnore("PlayerDamage", CheckJsonBool(ignoreTags, "PlayerDamage", objectName), b_updateColliderPairs);
				tags.SetIgnore("EnemyDamage", CheckJsonBool(ignoreTags, "EnemyDamage", objectName), b_updateColliderPairs);
				tags.SetIgnore("EnvironmentalDamage", CheckJsonBool(ignoreTags, "EnvironmentalDamage", objectName), b_updateColliderPairs);
				tags.SetIgnore("Projectile", CheckJsonBool(ignoreTags, "Projectile", objectName), b_updateColliderPairs);
				tags.SetIgnore("InteractableItem", CheckJsonBool(ignoreTags, "InteractableItem", objectName), b_updateColliderPairs);
				tags.SetIgnore("InteractableObject", CheckJsonBool(ignoreTags, "InteractableObject", objectName), b_updateColliderPairs);
				tags.SetIgnore("Item", CheckJsonBool(ignoreTags, "Item", objectName), b_updateColliderPairs);
			}
			loadedObject->SetTagList(tags);

			float objectRotation = 0;

			// Loop through the components in this GameObjects json
			for (int j = 0; j < objectJson["components"].size(); j++)
			{
				json componentJson = objectJson["components"][j];
				std::string type = CheckJsonString(componentJson, "type", objectName);
				long id = CheckJsonLong(componentJson, "id", objectName);
				bool b_isCollapsed = CheckJsonBool(componentJson, "_isCollapsed", objectName);
				bool b_isActive = CheckJsonBool(componentJson, "_isActive", objectName);

				if (type == "Transform")
				{
					Transform* transform = loadedObject->GetTransform();
					float rotation = CheckJsonFloat(componentJson, "rotation", objectName);
					transform->SetID(id);
					transform->SetActive(b_isActive);
					transform->SetCollapsed(b_isCollapsed);
					transform->SetOrigin(Vector2(CheckJsonFloat(componentJson, "xOrigin", objectName), CheckJsonFloat(componentJson, "yOrigin", objectName)));
					transform->SetInitialPosition(Vector2(CheckJsonFloat(componentJson, "xPos", objectName), CheckJsonFloat(componentJson, "yPos", objectName)));
					transform->SetScale(Vector2(CheckJsonFloat(componentJson, "xScale", objectName), CheckJsonFloat(componentJson, "yScale", objectName)));
					transform->SetRotation(rotation);
					objectRotation = rotation;
				}
				else if (type == "Sprite")
				{
					Sprite* newSprite = loadedObject->AddSprite(id, b_isActive, b_isCollapsed);
					std::string pivotPoint = "Center";
					newSprite->SetPivotPoint(CheckJsonString(componentJson, "pivotPoint", objectName));					
					newSprite->SetScale(Vector2(CheckJsonFloat(componentJson, "xScale", objectName), CheckJsonFloat(componentJson, "yScale", objectName)));				
					newSprite->SetRenderOrder(CheckJsonInt(componentJson, "renderOrder", objectName));
					newSprite->SetTintColor(Vector4(
						CheckJsonFloat(componentJson, "tintColorX", objectName),
						CheckJsonFloat(componentJson, "tintColorY", objectName),
						CheckJsonFloat(componentJson, "tintColorZ", objectName),
						CheckJsonFloat(componentJson, "tintColorW", objectName)
					));
					std::string path = CheckJsonString(componentJson, "path", objectName);
					if (path != "" && !DoesFileExist(path))
					{
						LogError("Sprite file not found for GameObject: \"" + objectName + "\". This may lead to unexpected behavior.  \npath: " + path);
					}
					newSprite->SetTexture(path);
					newSprite->SetOffset(Vector2(CheckJsonFloat(componentJson, "xOffset", objectName), CheckJsonFloat(componentJson, "yOffset", objectName)));
				}
				else if (type == "Camera")
				{
					Camera* newCamera = loadedObject->AddCamera(id, b_isActive, b_isCollapsed);
					bool b_isPrimaryCamera = CheckJsonBool(componentJson, "_isPrimaryCamera", objectName);
					newCamera->SetDimensions(CheckJsonFloat(componentJson, "width", objectName), CheckJsonFloat(componentJson, "height", objectName));
					newCamera->SetPrimaryCamera(b_isPrimaryCamera);
					newCamera->SetZoom(CheckJsonFloat(componentJson, "zoom", objectName));
					newCamera->SetFrustrumColor(Vector4(
						CheckJsonFloat(componentJson, "frustrumRed", objectName),
						CheckJsonFloat(componentJson, "frustrumGreen", objectName),
						CheckJsonFloat(componentJson, "frustrumBlue", objectName),
						CheckJsonFloat(componentJson, "frustrumAlpha", objectName)
					));
					newCamera->SetShouldFollow(CheckJsonBool(componentJson, "_follow", objectName));
					newCamera->SetFollowSmoothing(CheckJsonFloat(componentJson, "followSmoothing", objectName));
					newCamera->SetToFollowID(CheckJsonLong(componentJson, "following", objectName));

					if (b_isPrimaryCamera)
					{
						SetPrimaryCamera(newCamera);
					}
				}
				else if (type == "Script")
				{
					Script* newScript = loadedObject->AddScript(id, b_isActive, b_isCollapsed);
					newScript->SetAttachedScript(CheckJsonString(componentJson, "attachedScript", objectName));

					json scriptParamsJson = componentJson["scriptParameters"];					

					for (int i = 0; i < scriptParamsJson.size(); i++)
					{
						json param = scriptParamsJson[i];
						Animation::S_EventFunctionParam parameter;
						std::string paramName = CheckJsonString(param, "paramName", objectName);
						parameter.type = CheckJsonString(param, "type", objectName);
						parameter.e_string = CheckJsonString(param, "string", objectName);
						parameter.e_int = CheckJsonInt(param, "int", objectName);
						parameter.e_float = CheckJsonFloat(param, "float", objectName);
						parameter.e_long = CheckJsonLong(param, "long", objectName);
						parameter.e_double = CheckJsonDouble(param, "double", objectName);
						parameter.e_boolean = CheckJsonBool(param, "bool", objectName);
						parameter.e_Vector2 = Vector2(CheckJsonFloat(param, "vector2X", objectName), CheckJsonFloat(param, "vector2Y", objectName));

						newScript->AddScriptParam(paramName, parameter);
					}					
				}
				else if (type == "Button")
				{
					Button* newButton = loadedObject->AddButton(id, b_isActive, b_isCollapsed);
					newButton->SetActiveDimensions(CheckJsonFloat(componentJson, "activeWidth", objectName), CheckJsonFloat(componentJson, "activeHeight", objectName));
					newButton->SetActiveOffset(Vector2(CheckJsonFloat(componentJson, "activeOffsetX", objectName), CheckJsonFloat(componentJson, "activeOffsetY", objectName)));
					newButton->SetActiveLayer(CheckJsonInt(componentJson, "activeLayer", objectName));
					newButton->SetLuaFunctionName(CheckJsonString(componentJson, "luaFunctionName", objectName));
					newButton->SetLeftClick(CheckJsonBool(componentJson, "_leftClick", objectName));
					newButton->SetRightClick(CheckJsonBool(componentJson, "_rightClick", objectName));

					json functionParamsJson = componentJson["luaFunctionParameters"];
					std::shared_ptr<Animation::S_Event> functionParams = std::make_shared<Animation::S_Event>();

					for (int i = 0; i < functionParamsJson.size(); i++)
					{
						json param = functionParamsJson[i];
						Animation::S_EventFunctionParam parameter;
						parameter.type = CheckJsonString(param, "type", objectName);
						parameter.e_string = CheckJsonString(param, "string", objectName);
						parameter.e_int = CheckJsonInt(param, "int", objectName);
						parameter.e_float = CheckJsonFloat(param, "float", objectName);
						parameter.e_long = CheckJsonLong(param, "long", objectName);
						parameter.e_double = CheckJsonDouble(param, "double", objectName);
						parameter.e_boolean = CheckJsonBool(param, "bool", objectName);
						parameter.e_Vector2 = Vector2(CheckJsonFloat(param, "vector2X", objectName), CheckJsonFloat(param, "vector2Y", objectName));

						functionParams->parameters.push_back(parameter);
					}

					newButton->SetLuaFunctionParams(functionParams);
				}
				else if (type == "Canvas")
				{
					Canvas* newCanvas = loadedObject->AddCanvas(id, b_isActive, b_isCollapsed);
					newCanvas->SetDimensions(CheckJsonFloat(componentJson, "width", objectName), CheckJsonFloat(componentJson, "height", objectName));
					newCanvas->SetLayerNumber(CheckJsonInt(componentJson, "layerNumber", objectName));
					newCanvas->SetBlocksLayers(CheckJsonBool(componentJson, "_blocksLayers", objectName));
				}
				else if (type == "Animation")
				{
					Animation* newAnimation = loadedObject->AddAnimation(id, b_isActive, b_isCollapsed);				

					if (JsonContains(componentJson, "animationData", objectName))
					{
						for (int anim = 0; anim < componentJson["animationData"].size(); anim++)
						{
							json animationJson = componentJson["animationData"][anim];
							std::string path = CheckJsonString(animationJson, "path", objectName);
							std::string animationName = CheckJsonString(animationJson, "name", objectName);
							if (!DoesFileExist(path))
							{
								LogError("Animation file not found for GameObject: \"" + objectName + "\" - on Animation: \"" + animationName + "\". This may lead to unexpected behavior.  \npath: " + path);
							}																	
							newAnimation->AddAnimation(animationName, path);
						}
					}
				}
				else if (type == "Audio")
				{
					Audio* newAudio = loadedObject->AddAudio(id, b_isActive, b_isCollapsed);				

					if (JsonContains(componentJson, "soundData", objectName))
					{
						for (int sound = 0; sound < componentJson["soundData"].size(); sound++)
						{
							json soundJson = componentJson["soundData"][sound];
							std::string path = CheckJsonString(soundJson, "path", objectName);
							std::string soundName = CheckJsonString(soundJson, "name", objectName);
							if (!DoesFileExist(path))
							{
								LogError("Audio file not found for GameObject: \"" + objectName + "\" - on Audio: \"" + soundName + "\". This may lead to unexpected behavior.  \npath: " + path);
							}						
							bool b_isMusic = CheckJsonBool(soundJson, "b_isMusic", objectName);

							newAudio->AddSound(soundName, path);
						}
					}
				}
				else if (type == "Text")
				{
					Text* newText = loadedObject->AddText(id, b_isActive, b_isCollapsed);
					std::string fontPath = CheckJsonString(componentJson, "fontPath", objectName);
					if (!DoesFileExist(fontPath))
					{
						LogError("Font file not found for GameObject: \"" + objectName + "\". This may lead to unexpected behavior.  \npath: " + fontPath);
					}
					newText->SetFontPath(fontPath);
					newText->SetFontSize(CheckJsonInt(componentJson, "fontSize", objectName));
					newText->SetPivotPoint(CheckJsonString(componentJson, "pivotPoint", objectName));
					newText->SetColor(Vector4(
						CheckJsonFloat(componentJson, "tintColorX", objectName),
						CheckJsonFloat(componentJson, "tintColorY", objectName),
						CheckJsonFloat(componentJson, "tintColorZ", objectName),
						CheckJsonFloat(componentJson, "tintColorW", objectName)
					));
					newText->SetText(CheckJsonString(componentJson, "text", objectName));
					newText->SetRenderOrder(CheckJsonInt(componentJson, "renderOrder", objectName));
					newText->SetOffset(Vector2(CheckJsonFloat(componentJson, "xOffset", objectName), CheckJsonFloat(componentJson, "yOffset", objectName)));
				}
				else if (type == "CharacterController")
				{
					CharacterController* newCharacterController = loadedObject->AddCharacterController(id, b_isActive, b_isCollapsed);
					newCharacterController->SetMaxAcceleration(CheckJsonFloat(componentJson, "maxAcceleration", objectName));
					newCharacterController->SetMaxSpeed(CheckJsonFloat(componentJson, "maxSpeed", objectName));
					newCharacterController->SetAirControl(CheckJsonFloat(componentJson, "airControl", objectName));
				}
				else if (type == "BoxCollider")
				{
					BoxCollider* newBoxCollider = loadedObject->AddBoxCollider(id, b_isActive, b_isCollapsed);
					newBoxCollider->SetTileMapCollider(false);
					newBoxCollider->SetActiveDimensions(CheckJsonFloat(componentJson, "activeWidth", objectName), CheckJsonFloat(componentJson, "activeHeight", objectName));
					newBoxCollider->SetActiveOffset(Vector2(CheckJsonFloat(componentJson, "activeOffsetX", objectName), CheckJsonFloat(componentJson, "activeOffsetY", objectName)));
					newBoxCollider->SetIsContinuous(CheckJsonBool(componentJson, "_isContinuous", objectName));
					newBoxCollider->SetIsStatic(CheckJsonBool(componentJson, "_isStatic", objectName));
					newBoxCollider->SetIsSolid(CheckJsonBool(componentJson, "_isSolid", objectName));
					newBoxCollider->SetActiveLayer(CheckJsonInt(componentJson, "activeLayer", objectName));
					newBoxCollider->SetRotation(objectRotation);
					newBoxCollider->SetShowActiveRadius(CheckJsonBool(componentJson, "_showActiveRadius", objectName));
					newBoxCollider->SetIsComposite(CheckJsonBool(componentJson, "_isComposite", objectName));
				}
				else if (type == "CircleCollider")
				{
					CircleCollider* newCircleCollider = loadedObject->AddCircleCollider(id, b_isActive, b_isCollapsed);
					newCircleCollider->SetActiveRadiusGrid(CheckJsonFloat(componentJson, "activeRadius", objectName));
					newCircleCollider->SetActiveOffset(Vector2(CheckJsonFloat(componentJson, "activeOffsetX", objectName), CheckJsonFloat(componentJson, "activeOffsetY", objectName)));
					newCircleCollider->SetIsContinuous(CheckJsonBool(componentJson, "_isContinuous", objectName));
					newCircleCollider->SetIsStatic(CheckJsonBool(componentJson, "_isStatic", objectName));
					newCircleCollider->SetIsSolid(CheckJsonBool(componentJson, "_isSolid", objectName));
					newCircleCollider->SetActiveLayer(CheckJsonInt(componentJson, "activeLayer", objectName));
					newCircleCollider->SetIsComposite(CheckJsonBool(componentJson, "_isComposite", objectName));
				}
				else if (type == "RigidBody")
				{
					RigidBody* newRigidBody = loadedObject->AddRigidBody(id, b_isActive, b_isCollapsed);
					newRigidBody->SetMass(CheckJsonFloat(componentJson, "mass", objectName));
					newRigidBody->SetAngularDrag(CheckJsonFloat(componentJson, "angularDrag", objectName));
					newRigidBody->SetGravity(CheckJsonFloat(componentJson, "gravity", objectName));
					newRigidBody->SetFallingGravity(CheckJsonFloat(componentJson, "fallingGravity", objectName));
					newRigidBody->SetFriction(CheckJsonFloat(componentJson, "friction", objectName));
					newRigidBody->SetWindResistance(CheckJsonFloat(componentJson, "windResistance", objectName));
					newRigidBody->SetEquilibriumForce(CheckJsonFloat(componentJson, "equilibriumForce", objectName));
					newRigidBody->SetTerminalVelocity(CheckJsonFloat(componentJson, "terminalVelocity", objectName));
					newRigidBody->SetIsStatic(CheckJsonBool(componentJson, "_isStatic", objectName));
					newRigidBody->SetTorquesAllowed(CheckJsonBool(componentJson, "_allowTorques", objectName));
				}
				else if (type == "TileMap")
				{
					TileMap* newTileMap = loadedObject->AddTileMap(id, b_isActive, b_isCollapsed);
					newTileMap->SetWidth(CheckJsonInt(componentJson, "width", objectName));
					newTileMap->SetHeight(CheckJsonInt(componentJson, "height", objectName));
					newTileMap->SetTileWidth(CheckJsonInt(componentJson, "tileWidth", objectName));
					newTileMap->SetTileHeight(CheckJsonInt(componentJson, "tileHeight", objectName));

					// Get used TileSet names
					if (JsonContains(componentJson, "tileSets", objectName))
					{
						for (int tileSet = 0; tileSet < componentJson["tileSets"].size(); tileSet++)
						{
							json tileSetJson = componentJson["tileSets"][tileSet];
							std::string tileSetName = CheckJsonString(tileSetJson, "name", objectName);
							if (GetTileSet(tileSetName) != nullptr)
							{
								newTileMap->AddTileSet(tileSetName);
							}
							else
							{
								LogError("TileSet: \"" + tileSetName + "\" could not be found.");
							}
						}
					}
					// Get Tile data
					if (JsonContains(componentJson, "tiles", objectName))
					{
						for (int tile = 0; tile < componentJson["tiles"].size(); tile++)
						{
							json tileJson = componentJson["tiles"][tile];
							float x = CheckJsonFloat(tileJson, "tileCoordX", objectName);
							float y = CheckJsonFloat(tileJson, "tileCoordY", objectName);
							std::string tileSetName = CheckJsonString(tileJson, "tileSetName", objectName);
							int tileSetIndex = CheckJsonInt(tileJson, "tileSetIndex", objectName);
				
							if (tileSetName != "" && GetTileSet(tileSetName) != nullptr && tileSetIndex != -1)
							{								
								newTileMap->SetTile(Vector2(x, y), GetTileSet(tileSetName), tileSetIndex);
							}
						}
					}
					// Get Collision Area data
					if (JsonContains(componentJson, "collisionAreas", objectName))
					{
						for (int collisionArea = 0; collisionArea < componentJson["collisionAreas"].size(); collisionArea++)
						{
							json colliderAreaJson = componentJson["collisionAreas"][collisionArea];
							json colliderDataJson = colliderAreaJson["areaData"];
							std::string collisionAreaName = CheckJsonString(colliderAreaJson, "name", objectName);
							std::vector<std::pair<Vector2, Vector2>> collisionCoordBuffer;

							for (int colArea = 0; colArea < colliderDataJson.size(); colArea++)
							{											
								Vector2 startCoord = Vector2(CheckJsonFloat(colliderDataJson[colArea], "startCoordX", objectName), CheckJsonFloat(colliderDataJson[colArea], "startCoordY", objectName));
								Vector2 endCoord = Vector2(CheckJsonFloat(colliderDataJson[colArea], "endCoordX", objectName), CheckJsonFloat(colliderDataJson[colArea], "endCoordY", objectName));
								std::pair<Vector2, Vector2> colPair = { startCoord, endCoord };
								collisionCoordBuffer.push_back(colPair);
							}

							if (collisionCoordBuffer.size() > 0)
							{
								newTileMap->SetCollisionAreaValues(collisionAreaName, collisionCoordBuffer);
							}										
						}
					}
				}
			}
		}

		// Update the moment of inertia if applicable
		if (loadedObject != nullptr)
		{
			if (loadedObject->GetRigidBody() != nullptr)
			{
				loadedObject->GetRigidBody()->UpdateI();
			}

			if (loadedObject->GetButton() != nullptr)
			{
				loadedObject->GetButton()->CalculateActiveEdges();
			}
		}

		return loadedObject;
	}
}

// ImGui cheat sheet
// Border around object
//auto wPos = ImGui::GetWindowPos();
//auto wSize = ImGui::GetWindowSize();  // This is the size of the current box, perfect for getting the exact dimensions for a border
//ImGui::GetWindowDrawList()->AddRect({ wPos.x + 2, wPos.y + 2 }, { wPos.x + wSize.x - 2, wPos.y + wSize.y - 2 }, GetColor32("componentBorder"), 2);

