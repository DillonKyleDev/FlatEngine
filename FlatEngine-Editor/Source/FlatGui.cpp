#include "FlatGui.h"
#include "Project.h"
#include "Logger.h"
#include "Scene.h"
#include "Vector2.h"
#include "Vector4.h"
#include "Matrix.h"
#include "box2d.h"

#include <string>
#include "imgui_internal.h"
#include <fstream>
#include <vector>
#include <filesystem>
#include <windows.h> // For getting directory name


namespace FL = FlatEngine;

/*
######################################
######							######
######    Gui Implimentation    ######
######                          ######
######################################
*/

using GameObject = FL::GameObject;
using Button = FL::Button;
using Component = FL::Component;
using MappingContext = FL::MappingContext;
using Transform = FL::Transform;
using Sprite = FL::Sprite;
using Camera = FL::Camera;
using Canvas = FL::Canvas;
using Text = FL::Text;
using Body = FL::Body;
using BoxBody = FL::BoxBody;
using CircleBody = FL::CircleBody;
using CapsuleBody = FL::CapsuleBody;
using PolygonBody = FL::PolygonBody;
using Sound = FL::Sound;
using TileMap = FL::TileMap;

namespace FlatGui 
{
	ImDrawList* FG_DrawList = nullptr;

	std::shared_ptr<Animation::S_AnimationProperties> FG_FocusedAnimation = nullptr;
	std::string FG_FocusedAnimationName = "";
	GameObject *objectForFocusedAnimation = nullptr;
	std::shared_ptr<Animation::S_Property> FG_SelectedKeyFrameToEdit = nullptr;
	long FG_previewAnimationStartTime = 0;
	long FG_previewAnimationTime = 0;
	bool FG_b_playPreviewAnimation = false;
	long FG_FocusedGameObjectID = -1;
	std::shared_ptr<GameObject> playerObject = nullptr;

	// TileMap
	std::string FG_currentSelectedColliderArea = "";
	std::vector<std::pair<Vector2, Vector2>> FG_collisionAreasBuffer = std::vector<std::pair<Vector2, Vector2>>();

	// Frame Counter
	int framesDrawn = 0;

	// Settings
	int FG_iconTransparency = 50;

	// Hierarchy	
	std::map<long, bool> FG_leafExpandedTracker = std::map<long, bool>();

	bool FG_b_showDemoWindow = false;
	bool FG_b_showScriptEditor = false;
	bool FG_b_showTileSetEditor = false;
	bool FG_b_showFileExplorer = true;
	bool FG_b_showSceneView = true;
	bool FG_b_showGameView = true;
	bool FG_b_showHierarchy = true;
	bool FG_b_showPersistantHierarchy = true;
	bool FG_b_showInspector = true;
	bool FG_b_showAnimator = false;
	bool FG_b_showAnimationPreview = false;
	bool FG_b_showKeyFrameEditor = false;
	bool FG_b_showLogger = true;
	bool FG_b_showProfiler = false;
	bool FG_b_showMappingContextEditor = false;
	bool FG_b_showSettings = false;


	void Init()
	{
		FL::CreateNewScene();
	}

	void SetupProfilerProcesses()
	{
		if (FG_b_showProfiler)
		{					
			FL::AddProfilerProcess("Render");
			FL::AddProfilerProcess("Render Present");

			if (FG_b_showFileExplorer)
			{
				FL::AddProfilerProcess("File Explorer");
			}
			else
			{
				FL::RemoveProfilerProcess("File Explorer");
			}


			if (FG_b_showTileSetEditor)
			{
				FL::AddProfilerProcess("TileSet Editor");
			}
			else
			{
				FL::RemoveProfilerProcess("TileSet Editor");
			}

			if (FG_b_showScriptEditor)
			{
				FL::AddProfilerProcess("Script Editor");
			}
			else
			{
				FL::RemoveProfilerProcess("Script Editor");
			}

			if (FG_b_showHierarchy)
			{
				FL::AddProfilerProcess("Scene Hierarchy");
			}
			else
			{
				FL::RemoveProfilerProcess("Scene Hierarchy");
			}

			if (FG_b_showPersistantHierarchy)
			{
				FL::AddProfilerProcess("Persistant Hierarchy");
			}
			else
			{
				FL::RemoveProfilerProcess("Persistant Hierarchy");
			}

			if (FG_b_showInspector)
			{
				FL::AddProfilerProcess("Inspector");
			}
			else
			{
				FL::RemoveProfilerProcess("Inspector");
			}

			if (FG_b_showGameView)
			{
				FL::AddProfilerProcess("Game View");
			}
			else
			{
				FL::RemoveProfilerProcess("Game View");
			}

			if (FG_b_showSceneView)
			{
				FL::AddProfilerProcess("Scene View");
			}
			else
			{
				FL::RemoveProfilerProcess("Scene View");
			}

			if (FG_b_showAnimator)
			{
				FL::AddProfilerProcess("Animator");
			}
			else
			{
				FL::RemoveProfilerProcess("Animator");
			}

			if (FG_b_showAnimationPreview)
			{
				FL::AddProfilerProcess("Animation Preview");
			}
			else
			{
				FL::RemoveProfilerProcess("Animation Preview");
			}

			if (FG_b_showKeyFrameEditor)
			{
				FL::AddProfilerProcess("Key Frame Editor");
			}
			else
			{
				FL::RemoveProfilerProcess("Key Frame Editor");
			}

			if (FG_b_showLogger)
			{
				FL::AddProfilerProcess("Log");
			}
			else
			{
				FL::RemoveProfilerProcess("Log");
			}

			if (FG_b_showProfiler)
			{
				FL::AddProfilerProcess("Profiler");
			}
			else
			{
				FL::RemoveProfilerProcess("Profiler");
			}

			if (FG_b_showMappingContextEditor)
			{
				FL::AddProfilerProcess("Mapping Context Editor");
			}
			else
			{
				FL::RemoveProfilerProcess("Mapping Context Editor");
			}
		
			if (FG_b_showSettings)
			{
				FL::AddProfilerProcess("Settings");
			}
			else
			{
				FL::RemoveProfilerProcess("Settings");
			}

			FL::AddProfilerProcess("Collision Testing");
		}
	}

	void RunOnceAfterInitialization()
	{
		SetupProfilerProcesses();
	}

	void Cleanup()
	{		
		FL::RemoveProfilerProcess("Render");
		FL::RemoveProfilerProcess("Render Present");
	}

	void SetFocusedGameObjectID(long ID)
	{
		FL::F_FocusedGameObjectID = ID;
		if (ID != -1)
		{
			GameObject *focusedObject = FL::GetObjectByID(ID);
			std::string animationPath = "";
		}
	}

	long GetFocusedGameObjectID()
	{
		return FL::F_FocusedGameObjectID;
	}

	void DestroySelf(std::shared_ptr<FL::GameObject> thisObject)
	{
		if (thisObject)
		FL::DeleteGameObject(thisObject->GetID());
	}

	// Project Management
	void LoadProject(std::string path)
	{
		json projectJson;
		FL::LoadGameProject(path, projectJson);

		if (projectJson["Project Properties"][0] != "NULL")
		{			
			for (int i = 0; i < projectJson["Project Properties"].size(); i++)
			{
				json currentObjectJson = projectJson["Project Properties"][i];

				if (currentObjectJson.contains("_showSceneView"))
				{
					FG_b_showSceneView = currentObjectJson["_showSceneView"];
				}
				if (currentObjectJson.contains("_showGameView"))
				{
					FG_b_showGameView = currentObjectJson["_showGameView"];
				}
				if (currentObjectJson.contains("_showFileExplorer"))
				{
					FG_b_showFileExplorer = currentObjectJson["_showFileExplorer"];
				}
				if (currentObjectJson.contains("_showTileSetEditor"))
				{
					FG_b_showTileSetEditor = currentObjectJson["_showTileSetEditor"];
				}
				if (currentObjectJson.contains("_showScriptEditor"))
				{
					FG_b_showScriptEditor = currentObjectJson["_showScriptEditor"];
				}
				if (currentObjectJson.contains("_showHierarchy"))
				{
					FG_b_showHierarchy = currentObjectJson["_showHierarchy"];
				}
				if (currentObjectJson.contains("_showHierarchy"))
				{
					FG_b_showPersistantHierarchy = currentObjectJson["_showPersistantHierarchy"];
				}				
				if (currentObjectJson.contains("_showInspector"))
				{
					FG_b_showInspector = currentObjectJson["_showInspector"];
				}
				if (currentObjectJson.contains("_showAnimator"))
				{
					FG_b_showAnimator = currentObjectJson["_showAnimator"];
				}
				if (currentObjectJson.contains("_showAnimationPreview"))
				{
					FG_b_showAnimationPreview = currentObjectJson["_showAnimationPreview"];
				}
				if (currentObjectJson.contains("_showKeyFrameEditor"))
				{
					FG_b_showKeyFrameEditor = currentObjectJson["_showKeyFrameEditor"];
				}
				if (currentObjectJson.contains("_showLogger"))
				{
					FG_b_showLogger = currentObjectJson["_showLogger"];
				}
				if (currentObjectJson.contains("_showProfiler"))
				{
					FG_b_showProfiler = currentObjectJson["_showProfiler"];
				}
				if (currentObjectJson.contains("_showMappingContextEditor"))
				{
					FG_b_showMappingContextEditor = currentObjectJson["_showMappingContextEditor"];
				}
				
				if (currentObjectJson.contains("currentFileDirectory"))
				{
					FG_currentDirectory = currentObjectJson["currentFileDirectory"];
				}
				
				if (currentObjectJson.contains("_clearLogBuffer"))
				{
					FG_b_clearBufferEveryFrame = currentObjectJson["_clearLogBuffer"];
					if (FG_b_clearBufferEveryFrame)
					{
						FL::F_Logger.ClearBuffer();
					}
				}
			}
		}

		Vector2 scrolling = FL::F_LoadedProject.GetSceneViewScrolling();
		FG_sceneViewScrolling = scrolling;
		Vector2 gridStep = FL::F_LoadedProject.GetSceneViewGridStep();
		FG_sceneViewGridStep = gridStep;
		FL::F_sceneViewGridStep = FG_sceneViewGridStep;

		if (FL::F_LoadedProject.GetFocusedGameObjectID() != -1 && FL::GetObjectByID(FL::F_LoadedProject.GetFocusedGameObjectID()) != nullptr)
		{
			SetFocusedGameObjectID(FL::F_LoadedProject.GetFocusedGameObjectID());
		}
	}

	void CreateNewProject(std::string projectName)
	{
		SaveProject(FL::F_LoadedProject, FL::F_LoadedProject.GetPath());

		if (projectName != "")
		{
			Project newProject = Project();
			std::string directoryPath = "..\\projects\\" + projectName;
			std::string projectFilePath = directoryPath + "\\" + projectName + ".prj";
			std::string persistantGameObjectsScenePath = directoryPath + "\\persistant_objects_" + projectName + ".scn";

			CreateProjectDirectory(directoryPath);

			newProject.SetPersistantGameObjectsScenePath(persistantGameObjectsScenePath);
			newProject.SaveScene();

			SaveProject(newProject, projectFilePath);
			LoadProject(projectFilePath);
		}
	}

	void CreateProjectDirectory(std::string path)
	{
		std::filesystem::create_directory(path);
		std::filesystem::create_directory(path + "\\animations");
		std::filesystem::create_directory(path + "\\audio");
		std::filesystem::create_directory(path + "\\images");
		std::filesystem::create_directory(path + "\\images\\tileTextures");
		std::filesystem::create_directory(path + "\\mappingContexts");
		std::filesystem::create_directory(path + "\\prefabs");
		std::filesystem::create_directory(path + "\\scenes");
		std::filesystem::create_directory(path + "\\scripts");
		std::filesystem::create_directory(path + "\\scripts\\cpp");
		std::filesystem::create_directory(path + "\\scripts\\lua");
		std::filesystem::create_directory(path + "\\tileSets");
	}

	void SaveProject(Project& project, std::string path)
	{		
		project.SaveScene();

		std::ofstream file_obj;
		std::ifstream ifstream(path);

		// Delete old file contents
		file_obj.open(path, std::ofstream::out | std::ofstream::trunc);
		file_obj.close();

		file_obj.open(path, std::ios::app);

		json projectProperties;
		
		json properties = json::object({
			{ "path", path },
			{ "loadedScenePath", project.GetLoadedScenePath()},
			{ "loadedAnimationPath", project.GetLoadedPreviewAnimationPath() },
			{ "sceneToLoadAtRuntime", project.GetRuntimeScene() },
			{ "persistantGameObjectsScenePath", project.GetPersistantGameObjectsScenePath() },
			{ "buildPath", project.GetBuildPath() },
			{ "_autoSave", project.AutoSaveOn() },
			{ "resolutionWidth", project.GetResolution().x },
			{ "resolutionHeight", project.GetResolution().y },
			{ "_fullscreen", project.IsFullscreen() },
			{ "_vsyncEnabled", project.IsVsyncEnabled() },
			{ "musicVolume", project.GetMusicVolume() },
			{ "effectsVolume", project.GetEffectsVolume() },
			{ "currentFileDirectory", FG_currentDirectory },
			{ "focusedGameObjectID", GetFocusedGameObjectID() },
			{ "sceneViewScrollingX", FG_sceneViewScrolling.x },
			{ "sceneViewScrollingY", FG_sceneViewScrolling.y },
			{ "sceneViewGridStepX", FG_sceneViewGridStep.x },
			{ "sceneViewGridStepY", FG_sceneViewGridStep.y },
			{ "_showFileExplorer", FG_b_showFileExplorer },
			{ "_showTileSetEditor", FG_b_showTileSetEditor },
			{ "_showScriptEditor", FG_b_showScriptEditor },
			{ "_showSceneView", FG_b_showSceneView },
			{ "_showGameView", FG_b_showGameView },
			{ "_showHierarchy", FG_b_showHierarchy },
			{ "_showPersistantHierarchy", FG_b_showPersistantHierarchy },
			{ "_showInspector", FG_b_showInspector },
			{ "_showAnimator", FG_b_showAnimator },
			{ "_showAnimationPreview", FG_b_showAnimationPreview },
			{ "_showKeyFrameEditor", FG_b_showKeyFrameEditor },
			{ "_showLogger", FG_b_showLogger },
			{ "_showProfiler", FG_b_showProfiler },
			{ "_showMappingContextEditor", FG_b_showMappingContextEditor },
			{ "_clearLogBuffer", FG_b_clearBufferEveryFrame }
		});

		projectProperties.push_back(properties);
		json newFileObject = json::object({ {"Project Properties", projectProperties } });
		file_obj << newFileObject.dump(4).c_str() << std::endl;
		file_obj.close();
	}

	void SaveCurrentProject()
	{
		SaveProject(FL::F_LoadedProject, FL::F_LoadedProject.GetPath());
	}

	void AddViewports()
	{		
		if (FG_b_showDemoWindow)
		{
			ImGui::ShowDemoWindow(&FG_b_showDemoWindow);
		}

		MainMenuBar();
		RenderToolbar();

		float startTime = (float)FL::GetEngineTime();

		if (FG_b_showScriptEditor)
		{
			startTime = (float)FL::GetEngineTime();
			RenderScriptEditor();
			FL::AddProcessData("Script Editor", (float)FL::GetEngineTime() - startTime);
		}

		if (FG_b_showTileSetEditor)
		{
			startTime = (float)FL::GetEngineTime();
			RenderTileSetEditor();
			FL::AddProcessData("TileSet Editor", (float)FL::GetEngineTime() - startTime);
		}

		if (FG_b_showFileExplorer)
		{
			startTime = (float)FL::GetEngineTime();
			RenderFileExplorer();
			FL::AddProcessData("File Explorer", (float)FL::GetEngineTime() - startTime);
		}
		
		if (FG_b_showHierarchy)
		{
			startTime = (float)FL::GetEngineTime();
			RenderHierarchy();
			FL::AddProcessData("Hierarchy", (float)FL::GetEngineTime() - startTime);
		}

		if (FG_b_showPersistantHierarchy)
		{
			startTime = (float)FL::GetEngineTime();
			RenderPersistantHierarchy();
			FL::AddProcessData("Persistant Hierarchy", (float)FL::GetEngineTime() - startTime);
		}

		if (FG_b_showInspector)
		{
			startTime = (float)FL::GetEngineTime();
			RenderInspector();
			FL::AddProcessData("Inspector", (float)FL::GetEngineTime() - startTime);
		}

		if (FG_b_showGameView)
		{
			startTime = (float)FL::GetEngineTime();
			FL::Game_RenderView();
			FL::AddProcessData("Game View", (float)FL::GetEngineTime() - startTime);
		}

		if (FG_b_showSceneView)
		{
			startTime = (float)FL::GetEngineTime();
			Scene_RenderView();
			FL::AddProcessData("Scene View", (float)FL::GetEngineTime() - startTime);
		}

		if (FG_b_showAnimator)
		{
			startTime = (float)FL::GetEngineTime();
			RenderAnimator();
			FL::AddProcessData("Animator", (float)FL::GetEngineTime() - startTime);
		}
		
		if (FG_b_showAnimationPreview)
		{
			startTime = (float)FL::GetEngineTime();
			RenderAnimationPreview();
			FL::AddProcessData("Animation Preview", (float)FL::GetEngineTime() - startTime);
		}
		
		if (FG_b_showKeyFrameEditor)
		{
			startTime = (float)FL::GetEngineTime();
			RenderKeyFrameEditor();
			FL::AddProcessData("Key Frame Editor", (float)FL::GetEngineTime() - startTime);
		}

		if (FG_b_showLogger)
		{
			startTime = (float)FL::GetEngineTime();
			RenderLog();
			FL::AddProcessData("Log", (float)FL::GetEngineTime() - startTime);
		}
	
		//if (FG_b_showProfiler)
		//{
		//	startTime = (float)FL::GetEngineTime();
		//	RenderProfiler();
		//	FL::AddProcessData("Profiler", (float)FL::GetEngineTime() - startTime);
		//}

		if (FG_b_showMappingContextEditor)
		{
			startTime = (float)FL::GetEngineTime();
			RenderMappingContextEditor();
			FL::AddProcessData("Mapping Context Editor", (float)FL::GetEngineTime() - startTime);
		}

		if (FG_b_showSettings)
		{
			startTime = (float)FL::GetEngineTime();
			RenderSettings();
			FL::AddProcessData("Settings", (float)FL::GetEngineTime() - startTime);
		}
	}


	void RenderGridView(Vector2& centerPoint, Vector2 &scrolling, bool b_weightedScroll, Vector2 canvasP0, Vector2 canvasP1, Vector2 canvasSize, Vector2& gridStep, Vector2 centerOffset, bool b_showAxis)
	{
		FG_DrawList = ImGui::GetWindowDrawList();
		FG_DrawList->AddRectFilled(canvasP0, canvasP1, IM_COL32(50, 50, 50, 255));

		// Our grid gridStep determines the largest gap between each grid point so our centerpoints must fall on
		// one of those gridStep locations. We get the total grid gridSteps that will render given the current viewport
		// size and divide that by two to get the closest spot to the center of the viewport. It's okay that this
		// is not exactly center at all, the viewport width will never be the perfect size, we just need a starting
		// point and for that point. We need to update this value every pass of the scene view because the gridStep
		// value will change over time and we need to keep these in sync.          
		// 
		//                   V
		// |  |  |  |  |  |  |  |  |  |  |  |  |

		// X = 0 starts the drawing at the left most edge of the entire app window.

		// Draw horizontal grid lines
		for (float x = trunc(fmodf(scrolling.x + canvasP0.x, gridStep.y)); x < canvasP0.x + canvasSize.x; x += gridStep.y)
		{
			FL::DrawLine(Vector2(x, canvasP0.y), Vector2(x, canvasP1.y), Vector4(0.8f, 0.8f, 0.8f, 0.15f), 1.0f, FG_DrawList);
		}
		// Draw vertical grid lines
		for (float y = trunc(fmodf(scrolling.y + canvasP0.y, gridStep.y)); y < canvasP0.y + canvasSize.y; y += gridStep.y)
		{
			FL::DrawLine(Vector2(canvasP0.x, y), Vector2(canvasP1.x, y), Vector4(0.8f, 0.8f, 0.8f, 0.15f), 1.0f, FG_DrawList);
		}

		// Draw our x and y axis blue and green lines
		float divX = trunc(scrolling.x / gridStep.x);
		float modX = fmodf(scrolling.x, gridStep.x);
		float offsetX = (gridStep.x * divX) + modX;
		float divY = trunc(scrolling.y / gridStep.y);
		float modY = fmodf(scrolling.y, gridStep.y);
		float offsetY = (gridStep.y * divY) + modY;

		centerPoint = Vector2(offsetX + canvasP0.x, offsetY + canvasP0.y);

		if (b_showAxis)
		{
			// Blue, green and pink colors for axis and center
			Vector4 xColor = Vector4(0.2f, 0.2f, 0.9f, 1.0f);
			Vector4 yColor = Vector4(0, 0.82f, 0.14f, 1.0f);
			Vector4 centerColor = Vector4(1.0f, 0, 0.96f, 1.0f);

			float drawYAxisAt = centerPoint.x;
			float drawXAxisAt = centerPoint.y;

			// x axis bounds check + color change (lighten) if out of bounds
			if (centerPoint.x > canvasP1.x - 1)
			{
				drawYAxisAt = canvasP1.x - 1;
				yColor = Vector4(0, 0.82f, 0.14f, 0.4f);
			}
			else if (centerPoint.x < canvasP0.x)
			{
				drawYAxisAt = canvasP0.x;
				yColor = Vector4(0, 0.82f, 0.14f, 0.4f);
			}
			// y axis bounds check + color change (lighten) if out of bounds
			if (centerPoint.y > canvasP1.y - 1)
			{
				drawXAxisAt = canvasP1.y - 1;
				xColor = Vector4(0.07f, 0.07f, 0.8f, 0.58f);
			}
			else if (centerPoint.y < canvasP0.y)
			{
				drawXAxisAt = canvasP0.y;
				xColor = Vector4(0.07f, 0.07f, 0.8f, 0.58f);
			}


			// Draw the axis and center point
			FL::DrawLine(Vector2(drawYAxisAt, canvasP0.y), Vector2(drawYAxisAt, canvasP1.y), yColor, 1.0f, FG_DrawList);
			FL::DrawLine(Vector2(canvasP0.x, drawXAxisAt), Vector2(canvasP1.x, drawXAxisAt), xColor, 1.0f, FG_DrawList);
			FL::DrawPoint(Vector2(centerPoint.x, centerPoint.y), centerColor, FG_DrawList);
		}
	}

	void RenderViewObjects(std::map<long, GameObject>& objects, Vector2 centerPoint, Vector2 canvasP0, Vector2 canvasSize, float gridStep)
	{
		// Split our drawlist into multiple channels for different rendering orders
		ImDrawList* drawList = ImGui::GetWindowDrawList();
		ImDrawListSplitter* drawSplitter = new ImDrawListSplitter();

		// 4 channels for now in this scene view. 0 = scene objects, 1 & 2 = other UI (camera icon, etc), 4 = transform arrow
		drawSplitter->Split(drawList, FL::F_maxSpriteLayers + 5);
		

		for (std::pair<long, GameObject> object : objects)
		{
			if (object.second.IsActive())
			{
				RenderViewObject(object.second, centerPoint, canvasP0, canvasSize, gridStep, drawList, drawSplitter);
			}
		}


		drawSplitter->Merge(drawList);
		delete drawSplitter;
		drawSplitter = nullptr;
	}

	void RenderViewObject(GameObject &self, Vector2 scrolling, Vector2 canvasP0, Vector2 canvasSize, float gridStep, ImDrawList* drawList, ImDrawListSplitter* drawSplitter)
	{
		Transform* transform = self.GetTransform();
		Animation* animation = self.GetAnimation();
		Sprite* sprite = self.GetSprite();
		Camera* camera = self.GetCamera();
		Button* button = self.GetButton();
		Canvas* canvas = self.GetCanvas();
		Text* text = self.GetText();
		BoxBody* boxBody = self.GetBoxBody();
		CircleBody* circleBody = self.GetCircleBody();
		CapsuleBody* capsuleBody = self.GetCapsuleBody();
		PolygonBody* polygonBody = self.GetPolygonBody();
		//TileMap* tileMap = self.GetTileMap();

		bool b_spriteButtonAdded = false;


		if (transform != nullptr)
		{
			long focusedObjectID = GetFocusedGameObjectID();
			Vector2 position = transform->GetTruePosition();
			float rotation = transform->GetRotation();
			Body* body = self.GetBody();
			BoxBody* boxBody = self.GetBoxBody();
			CircleBody* circleBody = self.GetCircleBody();
			CapsuleBody* capsuleBody = self.GetCapsuleBody();
			PolygonBody* polygonBody = self.GetPolygonBody();
			if (body != nullptr)
			{
				position = body->GetPosition();
				rotation = body->GetRotation();
			}
			Vector2 relativePosition = transform->GetPosition();
			Vector2 origin = transform->GetOrigin();
			Vector2 transformScale = transform->GetScale();		
			Vector2 scale = transform->GetScale();
			//Vector2 baseScale = transform->GetBaseScale();
			
			if (sprite != nullptr && sprite->GetTexture() != nullptr && sprite->IsActive())
			{
				b_spriteButtonAdded = true;
				SDL_Texture* spriteTexture = sprite->GetTexture();
				float spriteTextureWidth = (float)sprite->GetTextureWidth();
				float spriteTextureHeight = (float)sprite->GetTextureHeight();
				Vector2 spriteScale = sprite->GetScale();
				Vector2 offset = sprite->GetOffset();				
				bool b_spriteScalesWithZoom = true;
				int renderOrder = sprite->GetRenderOrder();
				Vector4 tintColor = sprite->GetTintColor();
				std::string invisibleButtonID = "GameObjectSelectorButton_" + std::to_string(sprite->GetID());	
				ImGuiIO& inputOutput = ImGui::GetIO();

				spriteScale.x *= scale.x;
				spriteScale.y *= scale.y;

				Vector2 positionOnScreen = Vector2(FG_sceneViewCenter.x + (position.x * gridStep) - ((offset.x * FL::F_spriteScaleMultiplier * gridStep) * spriteScale.x), FG_sceneViewCenter.y - (position.y * gridStep) - ((offset.y * FL::F_spriteScaleMultiplier * gridStep) * spriteScale.y));
				Vector2 buttonSize = Vector2(spriteTextureWidth * FL::F_spriteScaleMultiplier * gridStep * spriteScale.x, spriteTextureHeight * FL::F_spriteScaleMultiplier * gridStep * spriteScale.y);
				
				AddSceneViewMouseControls(invisibleButtonID, positionOnScreen, buttonSize, FG_sceneViewScrolling, FG_sceneViewCenter, FG_sceneViewGridStep, FL::GetColor32("transparent"), false, 0, true);			
				const bool b_isClicked = ImGui::IsItemClicked();
				if (b_isClicked && (FL::F_CursorMode == FL::F_CURSOR_MODE::TRANSLATE || FL::F_CursorMode == FL::F_CURSOR_MODE::SCALE || FL::F_CursorMode == FL::F_CURSOR_MODE::ROTATE))
				{
					SetFocusedGameObjectID(sprite->GetParentID());
				}
				
				if (renderOrder <= FL::F_maxSpriteLayers && renderOrder >= 0)
				{
					drawSplitter->SetCurrentChannel(drawList, renderOrder);
				}
				else
				{
					drawSplitter->SetCurrentChannel(drawList, 0);
				}

				if (spriteScale.x > 0 && spriteScale.y > 0 && spriteTexture != nullptr)
				{
					FL::AddImageToDrawList(spriteTexture, position, scrolling, spriteTextureWidth, spriteTextureHeight, offset, spriteScale, b_spriteScalesWithZoom, gridStep, drawList, rotation, ImGui::GetColorU32(tintColor));
				}
			}

			if (text != nullptr)
			{
				std::shared_ptr<Texture> textTexture = text->GetTexture();
				float textWidth = (float)textTexture->GetWidth();
				float textHeight = (float)textTexture->GetHeight();
				Vector2 offset = text->GetOffset();
				int renderOrder = text->GetRenderOrder();				
				bool b_spriteScalesWithZoom = true;
				Vector4 tintColor = text->GetColor();
				std::string invisibleButtonID = "GameObjectSelectorButton_" + std::to_string(text->GetID());
				ImGuiIO& inputOutput = ImGui::GetIO();
				
				Vector2 newScale = Vector2(scale.x * FL::F_spriteScaleMultiplier, scale.y * FL::F_spriteScaleMultiplier);

				if (textTexture->GetTexture() != nullptr)
				{
					//Vector2 scaledPosition = Vector2(origin.x + (relativePosition.x * baseScale.x), origin.y + (relativePosition.y * baseScale.y));
					Vector2 positionOnScreen = Vector2(FG_sceneViewCenter.x + (position.x * gridStep) - ((offset.x * FL::F_spriteScaleMultiplier * gridStep) * newScale.x), FG_sceneViewCenter.y - (position.y * gridStep) - ((offset.y * FL::F_spriteScaleMultiplier * gridStep) * newScale.y));
					Vector2 buttonSize = Vector2(textWidth * FL::F_spriteScaleMultiplier * gridStep * newScale.x, textHeight * FL::F_spriteScaleMultiplier * gridStep * newScale.y);
					
					AddSceneViewMouseControls(invisibleButtonID, positionOnScreen, buttonSize, FG_sceneViewScrolling, FG_sceneViewCenter, FG_sceneViewGridStep, FL::GetColor32("transparent"), false, 0, true);						
					const bool b_isClicked = ImGui::IsItemClicked();
					if (b_isClicked && (FL::F_CursorMode == FL::F_CURSOR_MODE::TRANSLATE || FL::F_CursorMode == FL::F_CURSOR_MODE::SCALE || FL::F_CursorMode == FL::F_CURSOR_MODE::ROTATE))
					{
						SetFocusedGameObjectID(text->GetParentID());
					}
					
					if (renderOrder <= FL::F_maxSpriteLayers && renderOrder >= 0)
					{
						drawSplitter->SetCurrentChannel(drawList, renderOrder);
					}
					else
					{
						drawSplitter->SetCurrentChannel(drawList, 0);
					}
									
					FL::AddImageToDrawList(textTexture->GetTexture(), position, FG_sceneViewCenter, textWidth, textHeight, offset, newScale, b_spriteScalesWithZoom, FG_sceneViewGridStep.x, drawList, rotation, ImGui::GetColorU32(tintColor));
				}
			}
			
			if (camera != nullptr)
			{
				float cameraWidth = camera->GetWidth();
				float cameraHeight = camera->GetHeight();

				float cameraLeftEdge = scrolling.x + (position.x * FG_sceneViewGridStep.x) - (cameraWidth * FG_sceneViewGridStep.x / 2 * scale.x);
				float cameraTopEdge = scrolling.y + (-position.y * FG_sceneViewGridStep.y) - (cameraHeight * FG_sceneViewGridStep.y / 2 * scale.y);
				float cameraRightEdge = scrolling.x + (position.x * FG_sceneViewGridStep.x) + (cameraWidth * FG_sceneViewGridStep.x / 2 * scale.x);
				float cameraBottomEdge = scrolling.y + (-position.y * FG_sceneViewGridStep.y) + (cameraHeight * FG_sceneViewGridStep.y / 2 * scale.y);

				Vector2 topLeftCorner = Vector2(cameraLeftEdge, cameraTopEdge);
				Vector2 bottomRightCorner = Vector2(cameraRightEdge, cameraBottomEdge);
				Vector2 topRightCorner = Vector2(cameraRightEdge, cameraTopEdge);
				Vector2 bottomLeftCorner = Vector2(cameraLeftEdge, cameraBottomEdge);

				float cameraTextureWidth = (float)FL::GetTextureObject("camera")->GetWidth() / 4;
				float cameraTextureHeight = (float)FL::GetTextureObject("camera")->GetHeight() / 4;
				bool b_scalesWithZoom = false;
				Vector2 cameraTextureOffset = { cameraTextureWidth / 2, cameraTextureHeight / 2 };
				Vector2 cameraTextureScale = { 1, 1 };
				Vector2 offsetPosition = Vector2(position.x - cameraTextureWidth / 2, position.y + cameraTextureHeight / 2);

				// Draw channel 2 for Lower UI
				drawSplitter->SetCurrentChannel(drawList, FL::F_maxSpriteLayers + 2);

				// Draw a rectangle to the scene view to represent the camera frustrum
				FL::DrawRectangle(topLeftCorner, bottomRightCorner, canvasP0, canvasSize, FL::GetColor("cameraBox"), 2.0f, drawList);
				FL::DrawLine(topLeftCorner, bottomRightCorner, FL::GetColor("cameraBox"), 2.0f, drawList);
				FL::DrawLine(topRightCorner, bottomLeftCorner, FL::GetColor("cameraBox"), 2.0f, drawList);
				
				FL::AddImageToDrawList(FL::GetTexture("camera"), position, scrolling, cameraTextureWidth, cameraTextureHeight, cameraTextureOffset, cameraTextureScale, b_scalesWithZoom, gridStep, drawList, 0, IM_COL32(255, 255, 255, FG_iconTransparency));
			}

			if (canvas != nullptr)
			{
				float activeWidth = canvas->GetWidth();
				float activeHeight = canvas->GetHeight();
				int layerNumber = canvas->GetLayerNumber();				

				float renderXStart = FG_sceneViewCenter.x + ((position.x - (activeWidth * transformScale.x / 2)) * FG_sceneViewGridStep.x);
				float renderYStart = FG_sceneViewCenter.y - ((position.y + (activeHeight * transformScale.y / 2)) * FG_sceneViewGridStep.x);
				Vector2 renderStart = Vector2(renderXStart, renderYStart);
				Vector2 renderEnd = Vector2(renderXStart + ((activeWidth * transformScale.x) * FG_sceneViewGridStep.x), renderYStart + ((activeHeight * transformScale.y) * FG_sceneViewGridStep.x));

				drawSplitter->SetCurrentChannel(drawList, FL::F_maxSpriteLayers + 2);

				FL::DrawRectangle(renderStart, renderEnd, canvasP0, canvasSize, FL::GetColor("canvasBox"), 2.0f, drawList);
			}

			if (button != nullptr)
			{
				float activeWidth = button->GetActiveWidth();
				float activeHeight = button->GetActiveHeight();
				Vector2 activeOffset = button->GetActiveOffset();
				bool b_isActive = button->IsActive();
				bool b_isHovered = button->MouseIsOver();
				std::string buttonStateColor = "buttonComponentInactive";

				if (b_isActive)
				{
					buttonStateColor = "buttonComponentActive";

					if (b_isHovered)
					{
						buttonStateColor = "buttonComponentHovered";
					}
				}

				float activeLeft = FG_sceneViewCenter.x + ((position.x - (activeWidth * transformScale.x / 2) + activeOffset.x * transformScale.x) * FG_sceneViewGridStep.x);
				float activeRight = FG_sceneViewCenter.x + ((position.x + (activeWidth * transformScale.x / 2) + activeOffset.x * transformScale.x) * FG_sceneViewGridStep.x);
				float activeTop = FG_sceneViewCenter.y - ((position.y + (activeHeight * transformScale.y / 2) + activeOffset.y * transformScale.y) * FG_sceneViewGridStep.y);
				float activeBottom = FG_sceneViewCenter.y - ((position.y - (activeHeight * transformScale.y / 2) + activeOffset.y * transformScale.y) * FG_sceneViewGridStep.y);

				Vector2 center = Vector2(activeLeft + (activeRight - activeLeft) / 2, activeTop + (activeBottom - activeTop) / 2);

				Vector2 topLeft = { activeLeft, activeTop };
				Vector2 bottomRight = { activeRight, activeBottom };
				Vector2 topRight = { activeRight, activeTop };
				Vector2 bottomLeft = { activeLeft, activeBottom };

				drawSplitter->SetCurrentChannel(drawList, FL::F_maxSpriteLayers + 2);

				if (rotation != 0)
				{
					float cosA = cosf(rotation * 2.0f * (float)M_PI / 360.0f); // Convert degrees into radians
					float sinA = sinf(rotation * 2.0f * (float)M_PI / 360.0f);

					topLeft = ImRotate(Vector2(-activeWidth * FG_sceneViewGridStep.x / 2 * transformScale.x, -activeHeight * FG_sceneViewGridStep.x / 2 * transformScale.y), cosA, sinA);
					topRight = ImRotate(Vector2(+activeWidth * FG_sceneViewGridStep.x / 2 * transformScale.x, -activeHeight * FG_sceneViewGridStep.x / 2 * transformScale.y), cosA, sinA);
					bottomRight = ImRotate(Vector2(+activeWidth * FG_sceneViewGridStep.x / 2 * transformScale.x, +activeHeight * FG_sceneViewGridStep.x / 2 * transformScale.y), cosA, sinA);
					bottomLeft = ImRotate(Vector2(-activeWidth * FG_sceneViewGridStep.x / 2 * transformScale.x, +activeHeight * FG_sceneViewGridStep.x / 2 * transformScale.y), cosA, sinA);

					Vector2 pos[4] =
					{
						Vector2(center.x + topLeft.x, center.y + topLeft.y),
						Vector2(center.x + topRight.x, center.y + topRight.y),
						Vector2(center.x + bottomRight.x, center.y + bottomRight.y),
						Vector2(center.x + bottomLeft.x, center.y + bottomLeft.y),
					};

					FL::DrawLine(pos[0], pos[1], FL::GetColor(buttonStateColor), 2.0f, drawList);
					FL::DrawLine(pos[1], pos[2], FL::GetColor(buttonStateColor), 2.0f, drawList);
					FL::DrawLine(pos[2], pos[3], FL::GetColor(buttonStateColor), 2.0f, drawList);
					FL::DrawLine(pos[3], pos[0], FL::GetColor(buttonStateColor), 2.0f, drawList);
				}
				else
				{
					FL::DrawRectangle(topLeft, bottomRight, canvasP0, canvasSize, FL::GetColor(buttonStateColor), 1.0f, drawList);
				}
			}

			if (boxBody != nullptr)
			{				
				b2BodyId boxBodyID = boxBody->GetBodyID();
				bool b_isActive = boxBody->IsActive();
				FL::Physics::BodyProps bodyProps = boxBody->GetBodyProps();
				Vector2 dimensions = bodyProps.dimensions;
				Vector2 center = ConvertWorldToScreen(position, FG_sceneViewCenter, FG_sceneViewGridStep.x);
				boxBody->UpdateCorners();
				std::vector<Vector2> cornersVec = boxBody->GetCornersScreen();
				Vector2 corners[4] = {
					cornersVec[0],
					cornersVec[1],
					cornersVec[2],
					cornersVec[3]
				};
				bool b_isColliding = false;

				drawSplitter->SetCurrentChannel(drawList, FL::F_maxSpriteLayers + 2);

				if (b_isActive && !b_isColliding)
				{
					FL::DrawRectangleFromLines(corners, FL::GetColor("colliderActive"), 1.0f, drawList);
				}
				else if (!b_isActive)
				{
					FL::DrawRectangleFromLines(corners, FL::GetColor("colliderInactive"), 1.0f, drawList);
				}
				else if (b_isColliding)
				{
					FL::DrawRectangleFromLines(corners, FL::GetColor("colliderColliding"), 1.0f, drawList);
				}
			}

			if (circleBody != nullptr)
			{								
				b2BodyId circleBodyID = circleBody->GetBodyID();
				bool b_isActive = circleBody->IsActive();
				FL::Physics::BodyProps bodyProps = circleBody->GetBodyProps();				
				float radius = bodyProps.radius * FG_sceneViewGridStep.x;
				Vector2 center = ConvertWorldToScreen(position, FG_sceneViewCenter, FG_sceneViewGridStep.x);

				drawSplitter->SetCurrentChannel(drawList, FL::F_maxSpriteLayers + 2);

				if (b_isActive)
				{
					FL::DrawCircle(center, radius, FL::GetColor("colliderActive"), drawList);
				}
				else if (!b_isActive)
				{
					FL::DrawCircle(center, radius, FL::GetColor("colliderInactive"), drawList);
				}
				//else if (b_isColliding)
				//{
				//	FL::DrawCircle(center, activeRadius, FL::GetColor("colliderColliding"), drawList);
				//}
			}

			if (capsuleBody != nullptr)
			{
				b2BodyId capsuleBodyID = capsuleBody->GetBodyID();
				bool b_isActive = capsuleBody->IsActive();
				FL::Physics::BodyProps bodyProps = capsuleBody->GetBodyProps();
				b2Capsule capsule = b2Shape_GetCapsule(capsuleBody->GetShapeIDs().back());
				float length = bodyProps.capsuleLength;
				float radius = bodyProps.radius;
				float radiusScreen = radius * FG_sceneViewGridStep.x;	
				Vector2 center1 = ConvertWorldToScreen(Vector2(b2Body_GetWorldPoint(capsuleBodyID, capsule.center1)), FG_sceneViewCenter, FG_sceneViewGridStep.x);
				Vector2 center2 = ConvertWorldToScreen(Vector2(b2Body_GetWorldPoint(capsuleBodyID, capsule.center2)), FG_sceneViewCenter, FG_sceneViewGridStep.x);								
				Vector2 difference = center2 - center1;
				Vector2 diffN = Vector2::Normalize(difference);
				Vector2 diffNR = diffN * radiusScreen;
				Vector2 diffPerp = Vector2::Rotate(diffNR, 90);
				Vector2 flippedDiffPerp = Vector2::Rotate(diffNR, -90);

				drawSplitter->SetCurrentChannel(drawList, FL::F_maxSpriteLayers + 2);

				if (b_isActive)
				{
					FL::DrawCircle(center1, radiusScreen, FL::GetColor("colliderActive"), drawList);
					FL::DrawCircle(center2, radiusScreen, FL::GetColor("colliderActive"), drawList);
					FL::DrawLine(center1 - diffNR, center1 + diffNR, FL::GetColor("colliderActiveLight"), 1, drawList);
					FL::DrawLine(center2 - diffNR, center2 + diffNR, FL::GetColor("colliderActiveLight"), 1, drawList);
					FL::DrawLine(center1 - diffPerp, center1 + diffPerp, FL::GetColor("colliderActiveLight"), 1, drawList);
					FL::DrawLine(center2 - diffPerp, center2 + diffPerp, FL::GetColor("colliderActiveLight"), 1, drawList);
					// Sides
					FL::DrawLine(center1 + diffPerp, center1 + diffPerp + difference, FL::GetColor("colliderActive"), 1, drawList);
					FL::DrawLine(center1 + flippedDiffPerp, center1 + flippedDiffPerp + difference, FL::GetColor("colliderActive"), 1, drawList);
				}
				else if (!b_isActive)
				{
					FL::DrawCircle(center1, radiusScreen, FL::GetColor("colliderInactive"), drawList);
					FL::DrawCircle(center2, radiusScreen, FL::GetColor("colliderInactive"), drawList);
					FL::DrawLine(center1 - diffNR, center1 + diffNR, FL::GetColor("colliderInactiveLight"), 1, drawList);
					FL::DrawLine(center2 - diffNR, center2 + diffNR, FL::GetColor("colliderInactiveLight"), 1, drawList);
					FL::DrawLine(center1 - diffPerp, center1 + diffPerp, FL::GetColor("colliderInactiveLight"), 1, drawList);
					FL::DrawLine(center2 - diffPerp, center2 + diffPerp, FL::GetColor("colliderInactiveLight"), 1, drawList);
					// Sides
					FL::DrawLine(center1 + diffPerp, center1 + diffPerp + difference, FL::GetColor("colliderInactiveLight"), 1, drawList);
					FL::DrawLine(center1 + flippedDiffPerp, center1 + flippedDiffPerp + difference, FL::GetColor("colliderInactiveLight"), 1, drawList);
				}
				//else if (b_isColliding)
				//{
				//	FL::DrawCircle(center, activeRadius, FL::GetColor("colliderColliding"), drawList);
				//}
			}

			//if (tileMap != nullptr && tileMap->IsActive())
			//{
			//	long id = tileMap->GetID();				
			//	float width = (float)tileMap->GetWidth();							// in tiles
			//	float height = (float)tileMap->GetHeight();							// in tiles
			//	float tileWidth = (float)tileMap->GetTileWidth();
			//	float tileHeight = (float)tileMap->GetTileHeight();
			//	float gridWidth = width * tileWidth / FL::F_pixelsPerGridSpace;		// in grid tiles
			//	float gridHeight = height * tileHeight / FL::F_pixelsPerGridSpace;	// in grid tiles
			//	int renderOrder = tileMap->GetRenderOrder();
			//	std::map<int, std::map<int, FL::Tile>> tiles = tileMap->GetTiles();

			//	static std::vector<Vector2> hoveredTiles = std::vector<Vector2>();
			//	static std::vector<Vector2> selectedTiles = std::vector<Vector2>();
			//	
			//	// For Drawing TileMap border and background color
			//	float renderXStart = FG_sceneViewCenter.x + ((position.x - (gridWidth * transformScale.x / 2)) * FG_sceneViewGridStep.x);
			//	float renderYStart = FG_sceneViewCenter.y - ((position.y + (gridHeight * transformScale.y / 2)) * FG_sceneViewGridStep.x);
			//	Vector2 renderStart = Vector2(renderXStart, renderYStart);
			//	Vector2 renderEnd = Vector2(renderXStart + ((gridWidth * transformScale.x) * FG_sceneViewGridStep.x), renderYStart + ((gridHeight * transformScale.y) * FG_sceneViewGridStep.x));
			//	Vector2 focusObjectButtonSize = Vector2(renderEnd.x - renderStart.x, renderEnd.y - renderStart.y);

			//	float tileWidthInPx = FG_sceneViewGridStep.x * (tileWidth / FL::F_pixelsPerGridSpace);
			//	float tileHeightInPx = FG_sceneViewGridStep.x * (tileHeight / FL::F_pixelsPerGridSpace);
			//	Vector2 tileSize = Vector2(tileWidthInPx, tileHeightInPx);

			//	// For selecting multiple tiles
			//	static Vector2 multiSelectStartTile = Vector2(-1, -1);
			//	static Vector2 multiSelectEndTile = Vector2(-1, -1);
			//	static Vector2 multiSelectCurrentHoveredTile = Vector2(-1, -1);
			//	static Vector2 savedMultiSelectStartTile = Vector2(-1, -1);

			//	// For Moving MultiSelected Tiles
			//	static Vector2 moveStartTile = Vector2(-1, -1);
			//	static Vector2 moveEndTile = Vector2(-1, -1);

			//	// For selecting Collision Area coordinates
			//	static std::vector<std::pair<Vector2, Vector2>> selectedCollisionCoords = std::vector<std::pair<Vector2, Vector2>>();
			//	static Vector2 colAreaStartTile = Vector2(-1, -1);
			//	static Vector2 colAreaEndTile = Vector2(-1, -1);

			//	// "Focus on this TileMap GameObject" button
			//	if (focusedObjectID != self.GetID() && (FL::F_CursorMode == FL::F_CURSOR_MODE::TRANSLATE || FL::F_CursorMode == FL::F_CURSOR_MODE::SCALE || FL::F_CursorMode == FL::F_CURSOR_MODE::ROTATE))
			//	{
			//		if (focusObjectButtonSize.x <= 0 || focusObjectButtonSize.y <= 0)
			//		{
			//			focusObjectButtonSize = Vector2(1, 1);
			//		}
			//		std::string focusObjectButtonID = "##SelectThisTileMapObjectButton" + std::to_string(self.GetID()) + "-" + std::to_string(id);
			//		AddSceneViewMouseControls(focusObjectButtonID, renderStart, focusObjectButtonSize, FG_sceneViewScrolling, FG_sceneViewCenter, FG_sceneViewGridStep, FL::GetColor32("transparent"), false, 0, true);
			//		if (ImGui::IsItemClicked())
			//		{						
			//			SetFocusedGameObjectID(self.GetID());
			//		}
			//	}

			//	drawSplitter->SetCurrentChannel(drawList, 0);

			//	// TileMap background color and border
			//	if (focusedObjectID == self.GetID())
			//	{
			//		ImGui::GetWindowDrawList()->AddRectFilled(renderStart, renderEnd, FL::GetColor32("tileMapGridBgFocused"));
			//		FL::DrawRectangle(renderStart, renderEnd, canvasP0, canvasSize, FL::GetColor("tileMapBoxFocused"), 2.0f, drawList);
			//	}
			//	else
			//	{
			//		ImGui::GetWindowDrawList()->AddRectFilled(renderStart, renderEnd, FL::GetColor32("tileMapGridBgUnfocused"));
			//		FL::DrawRectangle(renderStart, renderEnd, canvasP0, canvasSize, FL::GetColor("tileMapBoxUnfocused"), 2.0f, drawList);
			//	}

			//	if (focusedObjectID == self.GetID())
			//	{
			//		// Handle Tiles
			//		for (float w = 0; w < width; w++)
			//		{
			//			for (float h = 0; h < height; h++)
			//			{
			//				// TileMap interactions							
			//				std::string tileButtonID = "##tileMapIndexButton" + std::to_string(id) + "-" + std::to_string(w) + std::to_string(h);
			//				TileSet* activeTileSet = nullptr;

			//				// Get active TileSet for texture dimensions
			//				std::string activeTileSetName = FL::F_tileSetAndIndexOnBrush.first;
			//				if (activeTileSetName != "")
			//				{
			//					activeTileSet = FL::GetTileSet(activeTileSetName);
			//				}

			//				// tileStart = viewport center + top left corner in pixel screen space + tile offset
			//				float tileStartX = FG_sceneViewCenter.x + ((position.x - (gridWidth * transformScale.x / 2)) * FG_sceneViewGridStep.x) + (w * tileWidthInPx);
			//				float tileStartY = FG_sceneViewCenter.y - ((position.y + (gridHeight * transformScale.y / 2)) * FG_sceneViewGridStep.x) + (h * tileHeightInPx);

			//				Vector2 tileStart = Vector2(tileStartX, tileStartY);
			//				Vector2 tileEnd = Vector2(tileStartX + tileWidthInPx, tileStartY + tileHeightInPx);

			//				// Catch interactions on the TileMap container
			//				if ((focusedObjectID == self.GetID()) &&
			//					((FL::F_CursorMode == FL::F_CURSOR_MODE::TILE_BRUSH && activeTileSet != nullptr && FL::F_tileSetAndIndexOnBrush.second != -1) ||
			//					(FL::F_CursorMode == FL::F_CURSOR_MODE::TILE_ERASE) ||
			//					(FL::F_CursorMode == FL::F_CURSOR_MODE::TILE_COLLIDER_DRAW && tileMap->GetSelectedCollisionArea() != "")) ||
			//					(FL::F_CursorMode == FL::F_CURSOR_MODE::TILE_MULTISELECT) ||
			//					(FL::F_CursorMode == FL::F_CURSOR_MODE::TILE_MOVE))
			//				{
			//					// Set Draw Channel to 2 for lower level UI
			//					drawSplitter->SetCurrentChannel(drawList, FL::F_maxSpriteLayers + 2);

			//					AddSceneViewMouseControls(tileButtonID, tileStart, tileSize, FG_sceneViewScrolling, FG_sceneViewCenter, FG_sceneViewGridStep, FL::GetColor32("tileMapGridLines"));
			//					// _RectOnly flag enables the buttons to work when dragging the mouse over them in a clicked state // https://github.com/ocornut/imgui/commit/564ff2dfd379d40568879a5bc89e8cfea7e51d2f
			//					const bool b_isHovered = ImGui::IsItemHovered(ImGuiHoveredFlags_RectOnly);



			//					// Collect all boxes between multiSelectStartTile tile and multiSelectCurrentlyHovered tile
			//					if (focusedObjectID == self.GetID() &&
			//						multiSelectStartTile.x != -1 &&
			//						multiSelectStartTile.y != -1 &&
			//						(FL::F_CursorMode == FL::F_CURSOR_MODE::TILE_COLLIDER_DRAW || FL::F_CursorMode == FL::F_CURSOR_MODE::TILE_MULTISELECT) &&
			//						ImGui::IsKeyDown(ImGuiKey_MouseLeft) &&
			//						(multiSelectStartTile.x <= w && multiSelectCurrentHoveredTile.x >= w || multiSelectCurrentHoveredTile.x <= w && multiSelectStartTile.x >= w) &&
			//						(multiSelectStartTile.y <= h && multiSelectCurrentHoveredTile.y >= h || multiSelectCurrentHoveredTile.y <= h && multiSelectStartTile.y >= h))
			//					{
			//						if (FL::F_CursorMode == FL::F_CURSOR_MODE::TILE_COLLIDER_DRAW)
			//						{
			//							ImGui::GetWindowDrawList()->AddRectFilled(tileStart, tileEnd, FL::GetColor32("tileBoxColHighlight"));
			//						}
			//						else if (FL::F_CursorMode == FL::F_CURSOR_MODE::TILE_MULTISELECT)
			//						{
			//							ImGui::GetWindowDrawList()->AddRectFilled(tileStart, tileEnd, FL::GetColor32("tileMultiSelectHighlightDragging"));

			//							bool b_containsTile = false;
			//							for (Vector2 tile : hoveredTiles)
			//							{
			//								if (tile.x == w && tile.y == h)
			//									b_containsTile = true;
			//							}
			//							if (!b_containsTile)
			//							{
			//								hoveredTiles.push_back(Vector2(w, h));
			//							}
			//						}
			//					}

			//					if (b_isHovered)
			//					{
			//						// Mouse down
			//						if (ImGui::IsKeyDown(ImGuiKey_MouseLeft))
			//						{
			//							// Record the tile that is currently under the clicked mouse
			//							multiSelectCurrentHoveredTile = Vector2(w, h);

			//							if (activeTileSet != nullptr && FL::F_CursorMode == FL::F_CURSOR_MODE::TILE_BRUSH)
			//							{
			//								ImGui::GetWindowDrawList()->AddRectFilled(tileStart, tileEnd, FL::GetColor32("tileSetHoldingTile"));
			//								tileMap->SetTile(Vector2(w, h), activeTileSet, FL::F_tileSetAndIndexOnBrush.second);
			//							}
			//							else if (FL::F_CursorMode == FL::F_CURSOR_MODE::TILE_ERASE)
			//							{
			//								ImGui::GetWindowDrawList()->AddRectFilled(tileStart, tileEnd, FL::GetColor32("tileSetEraseModeClick"));
			//								ImGui::GetWindowDrawList()->AddRect(tileStart, tileEnd, FL::GetColor32("tileSetEraseModeClickBorder"));
			//								tileMap->EraseTile(Vector2(w, h));
			//							}
			//							else if (FL::F_CursorMode == FL::F_CURSOR_MODE::TILE_COLLIDER_DRAW)
			//							{
			//								ImGui::GetWindowDrawList()->AddRectFilled(tileStart, tileEnd, FL::GetColor32("tileBoxColHighlight"));
			//							}
			//							else if (FL::F_CursorMode == FL::F_CURSOR_MODE::TILE_MULTISELECT)
			//							{
			//								ImGui::GetWindowDrawList()->AddRect(tileStart, tileEnd, FL::GetColor32("tileSelectModeClickBorder"));
			//							}
			//							else if (FL::F_CursorMode == FL::F_CURSOR_MODE::TILE_MOVE)
			//							{
			//								ImGui::GetWindowDrawList()->AddRect(tileStart, tileEnd, FL::GetColor32("tileMoveModeClickBorder"));
			//							}
			//						}
			//						// Mouse not down
			//						else
			//						{
			//							if (FL::F_CursorMode == FL::F_CURSOR_MODE::TILE_BRUSH)
			//							{
			//								ImGui::GetWindowDrawList()->AddRectFilled(tileStart, tileEnd, FL::GetColor32("tileSetHoveredTile"));
			//								ImGui::GetWindowDrawList()->AddRect(tileStart, tileEnd, FL::GetColor32("tileSetHoveredTileBorder"));
			//							}
			//							else if (FL::F_CursorMode == FL::F_CURSOR_MODE::TILE_ERASE)
			//							{
			//								ImGui::GetWindowDrawList()->AddRectFilled(tileStart, tileEnd, FL::GetColor32("tileSetEraseModeHover"));
			//								ImGui::GetWindowDrawList()->AddRect(tileStart, tileEnd, FL::GetColor32("tileSetEraseModeHoverBorder"));
			//							}
			//							else if (FL::F_CursorMode == FL::F_CURSOR_MODE::TILE_COLLIDER_DRAW)
			//							{
			//								ImGui::GetWindowDrawList()->AddRectFilled(tileStart, tileEnd, FL::GetColor32("tileBoxColHoveredHighlight"));
			//							}
			//							else if (FL::F_CursorMode == FL::F_CURSOR_MODE::TILE_MULTISELECT)
			//							{
			//								ImGui::GetWindowDrawList()->AddRectFilled(tileStart, tileEnd, FL::GetColor32("tileSetHoveredTile"));
			//								ImGui::GetWindowDrawList()->AddRect(tileStart, tileEnd, FL::GetColor32("tileSetHoveredTileBorder"));
			//							}
			//						}

			//						// Click pressed this frame
			//						if (ImGui::IsKeyPressed(ImGuiKey_MouseLeft, false))
			//						{
			//							if (FL::F_CursorMode != FL::F_CURSOR_MODE::TILE_MOVE)
			//							{
			//								// Record the tile that was clicked on	
			//								multiSelectStartTile = Vector2(w, h);
			//								savedMultiSelectStartTile = Vector2(w, h);
			//							}

			//							if (FL::F_CursorMode == FL::F_CURSOR_MODE::TILE_COLLIDER_DRAW)
			//							{
			//								colAreaStartTile = Vector2(w, h);
			//							}
			//							else if (FL::F_CursorMode == FL::F_CURSOR_MODE::TILE_MULTISELECT)
			//							{
			//								hoveredTiles.clear();
			//								selectedTiles.clear();
			//							}
			//							else if (FL::F_CursorMode == FL::F_CURSOR_MODE::TILE_MOVE)
			//							{
			//								moveStartTile = Vector2(w, h);
			//							}
			//						}

			//						// Click released this frame
			//						if (ImGui::IsKeyReleased(ImGuiKey_MouseLeft))
			//						{
			//							if (FL::F_CursorMode != FL::F_CURSOR_MODE::TILE_MOVE)
			//							{
			//								// Record the tile that the mouse released on
			//								multiSelectEndTile = Vector2(w, h);
			//							}


			//							if (FL::F_CursorMode == FL::F_CURSOR_MODE::TILE_COLLIDER_DRAW)
			//							{
			//								colAreaEndTile = Vector2(w, h);

			//								if (colAreaStartTile.x != -1 && colAreaStartTile.y != -1)
			//								{
			//									std::pair<Vector2, Vector2> newPair = { colAreaStartTile, colAreaEndTile };
			//									bool b_alreadyContains = false;

			//									for (std::vector<std::pair<Vector2, Vector2>>::iterator coordPair = FG_collisionAreasBuffer.begin(); coordPair != FG_collisionAreasBuffer.end(); coordPair++)
			//									{
			//										if ((coordPair->first == colAreaStartTile && coordPair->second == colAreaEndTile) || (coordPair->first == colAreaEndTile && coordPair->second == colAreaStartTile))
			//										{
			//											FG_collisionAreasBuffer.erase(coordPair);
			//											b_alreadyContains = true;
			//											break;
			//										}
			//									}

			//									if (!b_alreadyContains)
			//									{
			//										FG_collisionAreasBuffer.push_back(newPair);
			//									}
			//								}

			//								colAreaStartTile = Vector2(-1, -1);
			//								colAreaEndTile = Vector2(-1, -1);
			//							}
			//							else if (FL::F_CursorMode == FL::F_CURSOR_MODE::TILE_MULTISELECT)
			//							{
			//								selectedTiles = hoveredTiles;
			//								hoveredTiles.clear();
			//							}
			//							else if (FL::F_CursorMode == FL::F_CURSOR_MODE::TILE_MOVE)
			//							{
			//								moveEndTile = Vector2(w, h);

			//								if (moveStartTile.x != -1 && moveStartTile.y != -1)
			//								{
			//									float xMove = moveEndTile.x - moveStartTile.x;
			//									float yMove = moveEndTile.y - moveStartTile.y;

			//									tileMap->MoveTiles(selectedTiles, Vector2(xMove, yMove));
			//								}

			//								moveStartTile = Vector2(-1, -1);
			//								moveEndTile = Vector2(-1, -1);
			//							}

			//							if (FL::F_CursorMode != FL::F_CURSOR_MODE::TILE_MOVE)
			//							{
			//								// Reset the starting tile
			//								multiSelectStartTile = Vector2(-1, -1);
			//							}
			//						}
			//					}
			//				}
			//			}
			//		}

			//		// Draw box around selected multiselect tiles
			//		if ((selectedTiles.size() > 0) && (FL::F_CursorMode == FL::F_CURSOR_MODE::TILE_MULTISELECT || FL::F_CursorMode == FL::F_CURSOR_MODE::TILE_MOVE))
			//		{
			//			float startPosX = FG_sceneViewCenter.x + ((position.x - (gridWidth * transformScale.x / 2)) * FG_sceneViewGridStep.x) + (savedMultiSelectStartTile.x * tileWidthInPx);
			//			float startPosY = FG_sceneViewCenter.y - ((position.y + (gridHeight * transformScale.y / 2)) * FG_sceneViewGridStep.y) + (savedMultiSelectStartTile.y * tileHeightInPx);
			//			float selectWidth = multiSelectEndTile.x - savedMultiSelectStartTile.x;
			//			if (selectWidth < 0)
			//			{
			//				selectWidth *= -1;
			//			}
			//			selectWidth += 1;
			//			float selectHeight = multiSelectEndTile.y - savedMultiSelectStartTile.y;
			//			if (selectHeight < 0)
			//			{
			//				selectHeight *= -1;
			//			}
			//			selectHeight += 1;

			//			Vector2 startTileScreenPos = Vector2(startPosX, startPosY);
			//			Vector2 endTileScreenPos = Vector2(startPosX + (tileWidthInPx * selectWidth), startPosY + (tileHeightInPx * selectHeight));

			//			ImGui::GetWindowDrawList()->AddRectFilled(startTileScreenPos, endTileScreenPos, FL::GetColor32("tileMultiSelectHighlight"));
			//			ImGui::GetWindowDrawList()->AddRect(startTileScreenPos, endTileScreenPos, FL::GetColor32("tileMultiSelectHighlightBorder"));

			//			if (FL::F_CursorMode == FL::F_CURSOR_MODE::TILE_MOVE)
			//			{
			//				FL::RenderInvisibleButton("##MultiSelectDraggableBox", startTileScreenPos, Vector2(endTileScreenPos.x - startTileScreenPos.x, endTileScreenPos.y - startTileScreenPos.y));
			//				if (ImGui::IsItemHovered())
			//				{
			//					ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
			//				}
			//			}
			//		}
			//	}

			//	// Draw box around each of the collision areas in the buffer
			//	if (focusedObjectID == self.GetID() && FL::F_CursorMode == FL::F_CURSOR_MODE::TILE_COLLIDER_DRAW)
			//	{
			//		for (std::pair<Vector2, Vector2> collAreaBuffer : FG_collisionAreasBuffer)
			//		{
			//			Vector2 startCoord = collAreaBuffer.first;
			//			Vector2 endCoord = collAreaBuffer.second;

			//			float startPosX = FG_sceneViewCenter.x + ((position.x - (gridWidth * transformScale.x / 2)) * FG_sceneViewGridStep.x) + (startCoord.x * tileWidthInPx);
			//			float startPosY = FG_sceneViewCenter.y - ((position.y + (gridHeight * transformScale.y / 2)) * FG_sceneViewGridStep.y) + (startCoord.y * tileHeightInPx);
			//			float selectWidth = endCoord.x - startCoord.x;
			//			if (selectWidth < 0)
			//			{
			//				selectWidth *= -1;
			//			}
			//			selectWidth += 1;
			//			float selectHeight = endCoord.y - startCoord.y;
			//			if (selectHeight < 0)
			//			{
			//				selectHeight *= -1;
			//			}
			//			selectHeight += 1;

			//			Vector2 startTileScreenPos = Vector2(startPosX, startPosY);
			//			Vector2 endTileScreenPos = Vector2(startPosX + (tileWidthInPx * selectWidth), startPosY + (tileHeightInPx * selectHeight));

			//			ImGui::GetWindowDrawList()->AddRectFilled(startTileScreenPos, endTileScreenPos, FL::GetColor32("tileMultiSelectHighlight"));
			//			ImGui::GetWindowDrawList()->AddRect(startTileScreenPos, endTileScreenPos, FL::GetColor32("tileMultiSelectHighlightBorder"));

			//			if (FL::F_CursorMode == FL::F_CURSOR_MODE::TILE_MOVE)
			//			{
			//				FL::RenderInvisibleButton("##MultiSelectDraggableBox", startTileScreenPos, Vector2(endTileScreenPos.x - startTileScreenPos.x, endTileScreenPos.y - startTileScreenPos.y));
			//				if (ImGui::IsItemHovered())
			//				{
			//					ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
			//				}
			//			}
			//		}
			//	}

			//	// Draw TileMap indice Textures
			//	for (std::pair<int, std::map<int, FL::Tile>> xPair : tiles)
			//	{
			//		float x = (float)xPair.first;
			//		
			//		if (x <= width)
			//		{
			//			for (std::pair<int, FL::Tile> yPair : xPair.second)
			//			{
			//				float y = (float)yPair.first;

			//				if (y <= height)
			//				{
			//					FL::Tile tile = yPair.second;
			//					TileSet* usedTileSet = nullptr;
			//					std::string tileSetName = tile.tileSetName;

			//					if (tileSetName != "")
			//					{
			//						usedTileSet = FL::GetTileSet(tileSetName);
			//					}

			//					// this many grid spaces fit into a single tiles width (if tileWidth is 16: 16 / 8 is 2 grid spaces in for a single tile 
			//					float gridWidthsInATile = tileWidth / FL::F_pixelsPerGridSpace;
			//					float gridHeightsInATile = tileHeight / FL::F_pixelsPerGridSpace;

			//					SDL_Texture* texture = tile.tileSetTexture;
			//					float textureWidth = (float)usedTileSet->GetTexture()->GetWidth();
			//					float textureHeight = (float)usedTileSet->GetTexture()->GetHeight();
			//					Vector2 uvStart = Vector2(tile.uvStart.x / textureWidth, tile.uvStart.y / textureHeight);
			//					Vector2 uvEnd = Vector2(tile.uvEnd.x / textureWidth, tile.uvEnd.y / textureHeight);
			//					float gridXPosition = (position.x - (gridWidth / 2)) + gridWidthsInATile * x;
			//					float gridYPosition = (position.y + (gridHeight / 2)) - gridHeightsInATile * y;
			//					Vector2 tilePosition = Vector2(gridXPosition, gridYPosition);


			//					// Change the draw channel for the scene object
			//					if (renderOrder <= FL::F_maxSpriteLayers && renderOrder >= 0)
			//					{
			//						drawSplitter->SetCurrentChannel(drawList, renderOrder);
			//					}
			//					else
			//					{
			//						drawSplitter->SetCurrentChannel(drawList, 0);
			//					}

			//					FL::AddImageToDrawList(texture, tilePosition, FG_sceneViewCenter, tileWidth, tileHeight, Vector2(0, 0), scale, true, FG_sceneViewGridStep.x, drawList, 0, FL::GetColor32("white"), uvStart, uvEnd);
			//				}
			//			}
			//		}
			//	}
			//}
		}
	}

	void RenderTransformArrowWidget()
	{
		GameObject* focusedObject = FL::GetObjectByID(GetFocusedGameObjectID());

		// Renders Transform Arrow widget
		if (FL::F_CursorMode == FL::F_CURSOR_MODE::TRANSLATE && focusedObject != nullptr)
		{	
			Transform* transform = focusedObject->GetTransform();
			Vector2 position = Vector2(0, 0);
			Body* body = focusedObject->GetBody();
			BoxBody* boxBody = focusedObject->GetBoxBody();
			CircleBody* circleBody = focusedObject->GetCircleBody();
			CapsuleBody* capsuleBody = focusedObject->GetCapsuleBody();
			PolygonBody* polygonBody = focusedObject->GetPolygonBody();

			if (transform != nullptr)
			{
				position = transform->GetTruePosition();
			}

			if (body != nullptr)
			{
				position = body->GetPosition();
			}

			SDL_Texture* arrowToRender = FL::GetTexture("transformArrow");
			// * 3 because the texture is so small. If we change the scale, it will change the render starting position. We only want to change the render ending position so we adjust dimensions only
			float arrowWidth = (float)FL::GetTextureObject("transformArrow")->GetWidth() * 3;
			float arrowHeight = (float)FL::GetTextureObject("transformArrow")->GetHeight() * 3;
			Vector2 arrowScale = { 1, 1 };
			Vector2 arrowOffset = { 3, arrowHeight - 3 };
			bool b_scalesWithZoom = false;
			float transformMoveModifier = 0.02f;
			ImGuiIO& inputOutput = ImGui::GetIO();
			Vector2 positionOnScreen = Vector2(FG_sceneViewCenter.x + (position.x * FG_sceneViewGridStep.x), FG_sceneViewCenter.y - (position.y * FG_sceneViewGridStep.x));

			// Invisible button for Transform Arrow Move X and Y
			Vector2 moveAllStartPos = Vector2(positionOnScreen.x - 4, positionOnScreen.y - 23);
			FL::RenderInvisibleButton("##TransformBaseArrowButton", moveAllStartPos, Vector2(28, 28), false);
			const bool b_baseHovered = ImGui::IsItemHovered();
			const bool b_baseActive = ImGui::IsItemActive();
			const bool b_baseClicked = ImGui::IsItemClicked();

			if (b_baseHovered || b_baseActive)
			{
				arrowToRender = FL::GetTexture("transformArrowAllWhite");
				ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
			}

			// Invisible button for X arrow
			Vector2 moveXStartPos = Vector2(positionOnScreen.x + 24, positionOnScreen.y - 30);
			FL::RenderInvisibleButton("##TransformBaseArrowXButton", moveXStartPos, Vector2(63, 35), false);
			const bool b_xHovered = ImGui::IsItemHovered();
			const bool b_xActive = ImGui::IsItemActive();
			const bool b_xClicked = ImGui::IsItemClicked();

			if (b_xHovered || b_xActive)
			{
				arrowToRender = FL::GetTexture("transformArrowXWhite");
				ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
			}

			// Invisible button for Y arrow
			Vector2 moveYStartPos = Vector2(positionOnScreen.x - 4, positionOnScreen.y - 86);
			FL::RenderInvisibleButton("TransformBaseArrowYButton", moveYStartPos, Vector2(35, 63), false);
			const bool b_yHovered = ImGui::IsItemHovered();
			const bool b_yActive = ImGui::IsItemActive();
			const bool b_yClicked = ImGui::IsItemClicked();

			if (b_yHovered || b_yActive)
			{
				arrowToRender = FL::GetTexture("transformArrowYWhite");
				ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
			}

			static Vector2 transformScreenPos = Vector2(0, 0);
			static Vector2 cursorPosAtClick = inputOutput.MousePos;
			Vector2 relativePosition = transform->GetPosition();

			if (body != nullptr)
			{
				relativePosition = position;
			}

			if (b_baseClicked || b_xClicked || b_yClicked)
			{
				cursorPosAtClick = inputOutput.MousePos;
				transformScreenPos = Vector2(position.x + (relativePosition.x * FG_sceneViewGridStep.x), position.y - (relativePosition.y * FG_sceneViewGridStep.x));
			}

			Vector2 transformPosOffsetFromMouse = Vector2((cursorPosAtClick.x - transformScreenPos.x) / FG_sceneViewGridStep.x, (cursorPosAtClick.y - transformScreenPos.y) / FG_sceneViewGridStep.x);
			Vector2 mousePosInGrid = Vector2((inputOutput.MousePos.x - position.x) / FG_sceneViewGridStep.x, (position.y - inputOutput.MousePos.y) / FG_sceneViewGridStep.x);
			Vector2 newTransformPos = Vector2(mousePosInGrid.x - transformPosOffsetFromMouse.x, mousePosInGrid.y + transformPosOffsetFromMouse.y);

			if (b_baseActive && ImGui::IsMouseDragging(ImGuiMouseButton_Left))
			{
				transform->SetPosition(newTransformPos);
			}
			else if (b_xActive && ImGui::IsMouseDragging(ImGuiMouseButton_Left))
			{
				transform->SetPosition(Vector2(newTransformPos.x, relativePosition.y));
			}
			else if (b_yActive && ImGui::IsMouseDragging(ImGuiMouseButton_Left))
			{
				transform->SetPosition(Vector2(relativePosition.x, newTransformPos.y));
			}


			// Draw channel maxSpriteLayers + 3 for Upper UI Transform Arrow			
			FL::AddImageToDrawList(arrowToRender, position, FG_sceneViewCenter, arrowWidth, arrowHeight, arrowOffset, arrowScale, b_scalesWithZoom, FG_sceneViewGridStep.x, ImGui::GetWindowDrawList());
		}
	}

	void AddSceneViewMouseControls(std::string buttonID, Vector2 startPos, Vector2 size, Vector2 &scrolling, Vector2 centerPoint, Vector2 &gridStep, Uint32 rectColor, bool b_filled, ImGuiButtonFlags buttonFlags, bool b_allowOverlap, bool b_weightedScroll, float zoomMultiplier, float minGridStep, float maxGridStep)
	{
		int sdl_x;
		int sdl_y;
		SDL_GetMouseState(&sdl_x, &sdl_y);
		Vector2 sdlState = Vector2((float)sdl_x, (float)sdl_y);
		Vector2 mouseDelta = Vector2(0, 0);
		static Vector2 mouseLastPos = sdlState;
		ImGui::GetIO().MousePos = sdlState;

		if (size.x > 0 && size.y > 0)
		{
			ImGuiIO& inputOutput = ImGui::GetIO();
			Vector2 endPos = Vector2(startPos.x + size.x, startPos.y + size.y);

			// For calculating scrolling mouse position and what vector to zoom to
			DYNAMIC_VIEWPORT_WIDTH = trunc(endPos.x - startPos.x);
			DYNAMIC_VIEWPORT_HEIGHT = trunc(endPos.y - startPos.y);

			if (b_filled)
			{
				ImGui::GetWindowDrawList()->AddRectFilled(startPos, Vector2(startPos.x + size.x, startPos.y + size.y), rectColor);
			}
			else
			{
				ImGui::GetWindowDrawList()->AddRect(startPos, Vector2(startPos.x + size.x, startPos.y + size.y), rectColor);
			}

			FL::RenderInvisibleButton(buttonID.c_str(), startPos, size, b_allowOverlap, false, ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight);
			const bool b_isHovered = ImGui::IsItemHovered();
			const bool b_isActive = ImGui::IsItemActive();
			const bool b_isClicked = ImGui::IsItemClicked();

			const float mouse_threshold_for_pan = 0.0f;
			if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
			{
				mouseLastPos = sdlState;
			}
			if (b_isActive && ImGui::IsMouseDragging(ImGuiMouseButton_Right, mouse_threshold_for_pan))
			{
				mouseDelta = Vector2(sdlState.x - mouseLastPos.x, sdlState.y - mouseLastPos.y);
				scrolling.x += mouseDelta.x;
				scrolling.y += mouseDelta.y;
				mouseLastPos = sdlState;
			}

			// Show cursor position in scene view when pressing Alt
			if (b_isHovered && inputOutput.KeyAlt)
			{
				RenderSceneViewTooltip();
			}

			// Get scroll amount for changing zoom level of scene view
			Vector2 mousePos = Vector2(inputOutput.MousePos.x, inputOutput.MousePos.y);
			float scrollInput = inputOutput.MouseWheel;
			float weight = 0.01f;
			float signedMousePosX = mousePos.x - centerPoint.x - (DYNAMIC_VIEWPORT_WIDTH / 2);
			float signedMousePosY = mousePos.y - centerPoint.y - (DYNAMIC_VIEWPORT_HEIGHT / 2);
			float zoomSpeed = 1;
			float finalZoomSpeed = zoomSpeed * zoomMultiplier;

			if (inputOutput.KeyCtrl)
			{
				finalZoomSpeed += zoomMultiplier;
			}

			if (b_isHovered)
			{
				if (scrollInput > 0)
				{
					if (b_weightedScroll)
					{
						scrolling.x -= trunc(signedMousePosX * weight);
						scrolling.y -= trunc(signedMousePosY * weight);
					}
					if (gridStep.x + finalZoomSpeed < maxGridStep)
					{
						gridStep.x += finalZoomSpeed;
					}
					else
					{
						gridStep.x = maxGridStep;
					}
					if (gridStep.y + finalZoomSpeed < maxGridStep)
					{
						gridStep.y += finalZoomSpeed;
					}
					else
					{
						gridStep.y = maxGridStep;
					}
				}
				else if (scrollInput < 0)
				{
					if (b_weightedScroll)
					{
						scrolling.x += trunc(signedMousePosX * weight);
						scrolling.y += trunc(signedMousePosY * weight);
					}
					if (gridStep.x - finalZoomSpeed > minGridStep)
					{
						gridStep.x -= finalZoomSpeed;
					}
					else
					{
						gridStep.x = minGridStep;
					}
					if (gridStep.y - finalZoomSpeed > minGridStep)
					{
						gridStep.y -= finalZoomSpeed;
					}
					else
					{
						gridStep.y = minGridStep;
					}
				}

				FL::F_sceneViewGridStep = gridStep;
			}
		}
	}
}