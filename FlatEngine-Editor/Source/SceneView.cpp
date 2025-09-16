#include "FlatEngine.h"
#include "FlatGui.h"
#include "GameObject.h"
#include "Transform.h"
#include "SceneManager.h"
#include "Scene.h"
#include "Project.h"
#include "VulkanManager.h"

#include "imgui_internal.h"
#include <cmath> // trunc


namespace FlatGui 
{	
	Vector2 FG_sceneViewGridStep = Vector2(50,50); // Used to convert grid space values to pixel values.ie. 2 grid squares = 2 * 10 = 20px.
	Vector2 FG_sceneViewDimensions = Vector2(600, 400);	
	float DYNAMIC_VIEWPORT_WIDTH = 600;
	float DYNAMIC_VIEWPORT_HEIGHT = 400;
	bool FG_b_firstSceneRenderPass = true;
	bool FG_b_sceneHasBeenSet = false;
	Vector2 FG_sceneViewScrolling = Vector2(0,0);
	Vector2 FG_sceneViewCenter = Vector2(0, 0);
	bool FG_b_sceneViewLockedOnObject = false;
	long FG_sceneViewLockedObjectID = -1;
	Vector2 FG_sceneViewportCenter = Vector2(0, 0);

	void Scene_RenderView()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, Vector2(0, 0));

		FlatEngine::PushWindowStyles();
		ImGui::Begin("Scene View", &FG_b_showSceneView, 8 | 16); // Window flags 	ImGuiWindowFlags_NoScrollWithMouse
		FlatEngine::PopWindowStyles();
		// {
		
			FL::F_b_sceneViewFocused = ImGui::IsWindowFocused();

			Vector2 canvas_p0 = ImGui::GetCursorScreenPos();
			Vector2 canvas_sz = ImGui::GetContentRegionAvail();

			if (canvas_sz.x < 50.0f) canvas_sz.x = 50.0f;
			if (canvas_sz.y < 50.0f) canvas_sz.y = 50.0f;
			Vector2 canvas_p1 = Vector2(canvas_p0.x + canvas_sz.x, canvas_p0.y + canvas_sz.y);

			// Set initial viewport dimensions for rendering scene view grid and objects
			if (!FG_b_firstSceneRenderPass)
			{
				if (!FG_b_sceneHasBeenSet)
				{
					FG_sceneViewDimensions = canvas_sz;					
					FL::F_sceneViewDimensions = canvas_sz;
					FG_b_sceneHasBeenSet = true;
				}
			}
			FG_b_firstSceneRenderPass = false;
			
			DYNAMIC_VIEWPORT_WIDTH = trunc(canvas_p1.x - canvas_p0.x);
			DYNAMIC_VIEWPORT_HEIGHT = trunc(canvas_p1.y - canvas_p0.y);
			
			FG_sceneViewportCenter = Vector2(canvas_p0.x + DYNAMIC_VIEWPORT_WIDTH, canvas_p0.y + DYNAMIC_VIEWPORT_HEIGHT);
			FL::F_sceneViewCenter = &FG_sceneViewCenter;

			ImGuiIO& inputOutput = ImGui::GetIO();
			Vector2 currentPos = ImGui::GetCursorScreenPos();
			Vector2 centerOffset = FG_sceneViewDimensions * 0.5f;
			bool b_weightedScroll = false;
			Vector2 size;
			Vector2 startingPos = ImGui::GetCursorScreenPos();
			
			std::vector<VkDescriptorSet> descriptors = FL::F_VulkanManager->GetSceneViewDescriptorSets();

			if (descriptors.size() > 0 && descriptors[FL::VM_currentFrame])
			{
				Vector2 regionAvailable = ImGui::GetContentRegionAvail();		
				if (regionAvailable.x > regionAvailable.y)
				{
					size = Vector2(regionAvailable.x);
					float heightAdjust = (regionAvailable.x - regionAvailable.y) / 2;
					startingPos.y -= heightAdjust;
					ImGui::SetCursorScreenPos(startingPos);
				}
				else
				{
					size = Vector2(regionAvailable.y);
					float widthAdjust = (regionAvailable.y - regionAvailable.x) / 2;
					startingPos.x -= widthAdjust;
					ImGui::SetCursorScreenPos(startingPos);
				}

				ImGui::Image(descriptors[FL::VM_currentFrame], size);
			}

			// Need both the center of the viewport and the center of the world to reference when drawing imagese to the scene view
			AddSceneViewMouseControls("SceneViewCanvas", currentPos, canvas_sz, FG_sceneViewScrolling, FG_sceneViewCenter, FG_sceneViewGridStep);
			//RenderGridView(FG_sceneViewCenter, FG_sceneViewScrolling, b_weightedScroll, canvas_p0, canvas_p1, canvas_sz, FG_sceneViewGridStep, centerOffset);


			// Drop Target
			int droppedValue = -1;
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(FL::F_fileExplorerTarget.c_str()))
				{
					IM_ASSERT(payload->DataSize == sizeof(int));
					droppedValue = *(const int*)payload->Data;
				}
				ImGui::EndDragDropTarget();
			}
			// Create a GameObject from a file in the Explorer by dragging it into the Scene View space
			if (droppedValue != -1 && FL::F_selectedFiles.size() >= droppedValue)
			{
				ImGuiIO& inputOutput = ImGui::GetIO();
				Vector2 mousePosInGrid = Vector2((inputOutput.MousePos.x - FG_sceneViewCenter.x) / FG_sceneViewGridStep.x, -(inputOutput.MousePos.y - FG_sceneViewCenter.y) / FG_sceneViewGridStep.y);
				std::string filePath = FL::F_selectedFiles[droppedValue - 1];			
			
				GameObject* newObject = FL::CreateAssetUsingFilePath(filePath, mousePosInGrid);
				if (newObject != nullptr)
				{
					SetFocusedGameObjectID(newObject->GetID());
				}
			}
			
			Scene* loadedScene = FL::F_SceneManager.GetLoadedScene();
			std::map<long, GameObject> sceneObjects = std::map<long, GameObject>();
			std::map<long, GameObject> persistantObjects = std::map<long, GameObject>();

			if (loadedScene != nullptr)
			{
				sceneObjects = loadedScene->GetSceneObjects();
			}
			if (FL::GetLoadedProject().GetPersistantGameObjectScene() != nullptr)
			{
				persistantObjects = FL::GetLoadedProject().GetPersistantGameObjectScene()->GetSceneObjects();
			}

			//RenderViewObjects(sceneObjects, FG_sceneViewCenter, canvas_p0, canvas_sz, FG_sceneViewGridStep.x);
			//RenderViewObjects(persistantObjects, FG_sceneViewCenter, canvas_p0, canvas_sz, FG_sceneViewGridStep.x);
			//RenderTransformArrowWidget();


			//// For centering on focused GameObject
			//GameObject* lockedObject = FL::GetObjectByID(FG_sceneViewLockedObjectID);
			//if (FG_b_sceneViewLockedOnObject && lockedObject != nullptr)
			//{
			//	Transform* transform = lockedObject->GetTransform();
			//	Vector2 position = transform->GetAbsoluteScale();
			//	FG_sceneViewScrolling = Vector2(position.x * -FG_sceneViewGridStep.x + (ImGui::GetWindowWidth() / 2), position.y * FG_sceneViewGridStep.y + (ImGui::GetWindowHeight() / 2));
			//}

			// Cursor mode select
			ImGui::SetCursorScreenPos(canvas_p0);
			RenderCursorModeButtons();

			// Game Stats in SceneView
			ImGui::SetCursorScreenPos(Vector2(canvas_p0.x + 3, canvas_p1.y - 40)); // was - 54 y
			RenderGameTimeStats();

			// Game Stats in GameView
			//RenderStatsOnGameView();
		
			FL::RenderSceneLines();

		// }
		ImGui::PopStyleVar();
		ImGui::PopStyleVar();
		ImGui::End(); // Scene View
	}

	// Show cursor position in scene view when pressing Alt
	void RenderSceneViewTooltip()
	{
		ImGuiIO& inputOutput = ImGui::GetIO();
		Vector2 positionInGrid = Vector2((inputOutput.MousePos.x - FG_sceneViewCenter.x) / FG_sceneViewGridStep.x, -(inputOutput.MousePos.y - FG_sceneViewCenter.y) / FG_sceneViewGridStep.y);
		std::string cursorXPosGrid = "x: " + std::to_string(positionInGrid.x);
		std::string cursorYPosGrid = "y: " + std::to_string(positionInGrid.y);	
		std::string cursorXPosScreen = "x: " + std::to_string((int)inputOutput.MousePos.x);
		std::string cursorYPosScreen = "y: " + std::to_string((int)inputOutput.MousePos.y);
		FL::BeginToolTip("");		
		ImGui::Text("Cursor Pos World");
		ImGui::Text(cursorXPosGrid.c_str());
		ImGui::Text(cursorYPosGrid.c_str());		
		FL::RenderSeparator(3, 3);
		ImGui::Text("Cursor Pos Screen");
		ImGui::Text(cursorXPosScreen.c_str());
		ImGui::Text(cursorYPosScreen.c_str());				
		FL::EndToolTip();
	}

	void RenderCursorModeButtons()
	{
		Vector2 iconSize = Vector2(24, 24);
		Vector2 buttonSize = Vector2(33, 31);
		Vector2 startPos = ImGui::GetCursorScreenPos();
		//ImGui::GetWindowDrawList()->AddRectFilled(startPos, Vector2(startPos.x + 250, startPos.y + 40), FL::GetColor32("cursorModeSelectBg"), 0);
		//ImGui::GetWindowDrawList()->AddRect(Vector2(startPos.x - 1, startPos.y - 1), Vector2(startPos.x + 250, startPos.y + 40), FL::GetColor32("cursorModeSelectBorder"), 0);
		ImGui::SetCursorScreenPos(Vector2(startPos.x + 10, startPos.y + 5));


		if (FL::F_CursorMode == FL::F_CURSOR_MODE::TRANSLATE)
		{
			Vector2 currentPos = ImGui::GetCursorScreenPos();
			//ImGui::GetWindowDrawList()->AddRect(Vector2(currentPos.x - 1, currentPos.y - 1), Vector2(currentPos.x + buttonSize.x, currentPos.y + buttonSize.y), FL::GetColor32("cursorModeButtonSelectedBorder"));
			if (FL::RenderImageButton("#TranslateModeIcon", FL::GetTexture("translate"), iconSize, 0, Vector2(1, 1), FL::GetColor("selectedCursorModeButtonBg"), FL::GetColor("imageButtonTint"), FL::GetColor("cursorModeButtonHoverSelected")))
			{
				FL::F_CursorMode = FL::F_CURSOR_MODE::TRANSLATE;
			}
		}
		else
		{
			if (FL::RenderImageButton("#TranslateModeIcon", FL::GetTexture("translate"), iconSize, 0, Vector2(1, 1), FL::GetColor("transparent"), FL::GetColor("imageButtonTint"), FL::GetColor("cursorModeButtonHover")))
			{
				FL::F_CursorMode = FL::F_CURSOR_MODE::TRANSLATE;
			}
		}
		if (ImGui::IsItemHovered())
		{
			FL::RenderTextToolTip("Translate Mode");
		}
		ImGui::SameLine();

		if (FL::F_CursorMode == FL::F_CURSOR_MODE::TILE_BRUSH)
		{
			Vector2 currentPos = ImGui::GetCursorScreenPos();
			//ImGui::GetWindowDrawList()->AddRect(Vector2(currentPos.x - 1, currentPos.y - 1), Vector2(currentPos.x + buttonSize.x, currentPos.y + buttonSize.y), FL::GetColor32("cursorModeButtonSelectedBorder"));
			if (FL::RenderImageButton("#TileBrushModeIcon", FL::GetTexture("tileBrush"), iconSize, 0, Vector2(1, 1), FL::GetColor("selectedCursorModeButtonBg"), FL::GetColor("imageButtonTint"), FL::GetColor("cursorModeButtonHoverSelected")))
			{
				FL::F_CursorMode = FL::F_CURSOR_MODE::TILE_BRUSH;
			}
		}
		else
		{
			if (FL::RenderImageButton("#TileBrushModeIcon", FL::GetTexture("tileBrush"), iconSize, 0, Vector2(1, 1), FL::GetColor("transparent"), FL::GetColor("imageButtonTint"), FL::GetColor("cursorModeButtonHover")))
			{
				FL::F_CursorMode = FL::F_CURSOR_MODE::TILE_BRUSH;
			}
		}
		if (ImGui::IsItemHovered())
		{
			FL::RenderTextToolTip("Tile Brush Mode");
		}
		ImGui::SameLine();


		if (FL::F_CursorMode == FL::F_CURSOR_MODE::TILE_ERASE)
		{
			Vector2 currentPos = ImGui::GetCursorScreenPos();
			//ImGui::GetWindowDrawList()->AddRect(Vector2(currentPos.x - 1, currentPos.y - 1), Vector2(currentPos.x + buttonSize.x, currentPos.y + buttonSize.y), FL::GetColor32("cursorModeButtonSelectedBorder"));
			if (FL::RenderImageButton("#TileEraseModeIcon", FL::GetTexture("tileErase"), iconSize, 0, Vector2(1, 1), FL::GetColor("selectedCursorModeButtonBg"), FL::GetColor("imageButtonTint"), FL::GetColor("cursorModeButtonHoverSelected")))
			{
				FL::F_CursorMode = FL::F_CURSOR_MODE::TILE_ERASE;
			}
		}
		else
		{
			if (FL::RenderImageButton("#TileEraseModeIcon", FL::GetTexture("tileErase"), iconSize, 0, Vector2(1, 1), FL::GetColor("transparent"), FL::GetColor("imageButtonTint"), FL::GetColor("cursorModeButtonHover")))
			{
				FL::F_CursorMode = FL::F_CURSOR_MODE::TILE_ERASE;
			}
		}
		if (ImGui::IsItemHovered())
		{
			FL::RenderTextToolTip("Tile Erase Mode");
		}
		ImGui::SameLine();


		//if (FL::F_CursorMode == FL::F_CURSOR_MODE::TILE_COLLIDER_DRAW)
		//{
		//	Vector2 currentPos = ImGui::GetCursorScreenPos();
		//	//ImGui::GetWindowDrawList()->AddRect(Vector2(currentPos.x - 1, currentPos.y - 1), Vector2(currentPos.x + buttonSize.x, currentPos.y + buttonSize.y), FL::GetColor32("cursorModeButtonSelectedBorder"));
		//	if (FL::RenderImageButton("#BoxColliderTileDrawModeIcon", FL::GetTexture("tileDrawColliders"), iconSize, 0, Vector2(1, 1), FL::GetColor("selectedCursorModeButtonBg"), FL::GetColor("imageButtonTint"), FL::GetColor("cursorModeButtonHoverSelected")))
		//	{
		//		FL::F_CursorMode = FL::F_CURSOR_MODE::TILE_COLLIDER_DRAW;
		//	}
		//}
		//else
		//{
		//	if (FL::RenderImageButton("#BoxColliderTileDrawModeIcon", FL::GetTexture("tileDrawColliders"), iconSize, 0, Vector2(1, 1), FL::GetColor("transparent"), FL::GetColor("imageButtonTint"), FL::GetColor("cursorModeButtonHover")))
		//	{
		//		FL::F_CursorMode = FL::F_CURSOR_MODE::TILE_COLLIDER_DRAW;
		//	}
		//}
		//if (ImGui::IsItemHovered())
		//{
		//	FL::RenderTextToolTip("BoxCollider Draw Mode (4)");
		//}
		//ImGui::SameLine();

		if (FL::F_CursorMode == FL::F_CURSOR_MODE::TILE_MULTISELECT)
		{
			Vector2 currentPos = ImGui::GetCursorScreenPos();
			//ImGui::GetWindowDrawList()->AddRect(Vector2(currentPos.x - 1, currentPos.y - 1), Vector2(currentPos.x + buttonSize.x, currentPos.y + buttonSize.y), FL::GetColor32("cursorModeButtonSelectedBorder"));
			if (FL::RenderImageButton("#SelectTilesModeIcon", FL::GetTexture("tileSelect"), iconSize, 0, Vector2(1, 1), FL::GetColor("selectedCursorModeButtonBg"), FL::GetColor("imageButtonTint"), FL::GetColor("cursorModeButtonHoverSelected")))
			{
				FL::F_CursorMode = FL::F_CURSOR_MODE::TILE_MULTISELECT;
			}
		}
		else
		{
			if (FL::RenderImageButton("#SelectTilesModeIcon", FL::GetTexture("tileSelect"), iconSize, 0, Vector2(1, 1), FL::GetColor("transparent"), FL::GetColor("imageButtonTint"), FL::GetColor("cursorModeButtonHover")))
			{
				FL::F_CursorMode = FL::F_CURSOR_MODE::TILE_MULTISELECT;
			}
		}
		if (ImGui::IsItemHovered())
		{
			FL::RenderTextToolTip("Select Tiles Mode");
		}
		ImGui::SameLine();

		if (FL::F_CursorMode == FL::F_CURSOR_MODE::TILE_MOVE)
		{
			Vector2 currentPos = ImGui::GetCursorScreenPos();
			//ImGui::GetWindowDrawList()->AddRect(Vector2(currentPos.x - 1, currentPos.y - 1), Vector2(currentPos.x + buttonSize.x, currentPos.y + buttonSize.y), FL::GetColor32("cursorModeButtonSelectedBorder"));
			if (FL::RenderImageButton("#MoveTilesModeIcon", FL::GetTexture("tileMove"), iconSize, 0, Vector2(1, 1), FL::GetColor("selectedCursorModeButtonBg"), FL::GetColor("imageButtonTint"), FL::GetColor("cursorModeButtonHoverSelected")))
			{
				FL::F_CursorMode = FL::F_CURSOR_MODE::TILE_MOVE;
			}
		}
		else
		{
			if (FL::RenderImageButton("#MoveTilesModeIcon", FL::GetTexture("tileMove"), iconSize, 0, Vector2(1, 1), FL::GetColor("transparent"), FL::GetColor("imageButtonTint"), FL::GetColor("cursorModeButtonHover")))
			{
				FL::F_CursorMode = FL::F_CURSOR_MODE::TILE_MOVE;
			}
		}
		if (ImGui::IsItemHovered())
		{
			FL::RenderTextToolTip("Move Tiles Mode *experimental*");
		}
	}

	void RenderGameTimeStats()
	{
		Vector2 currentPos = ImGui::GetCursorScreenPos();

		std::string ellapsedTimeString =          "time (ms): ---";
		static Uint32 frameStart = FL::GetEngineTime();		
		static long framesCountedAtStart = FL::GetFramesCounted();
		static float fps = 60;
		static float lastFrameFps = 60;
		static int fpsTrackingCounter = 0;
		float smoothing = 0.005f; // smaller = more smoothing

		if (FL::GameLoopStarted())
		{
			// Slows down the display of fps so it is readable
			if (fpsTrackingCounter == 5)
			{
				Uint32 frameTime = FL::GetEngineTime() - frameStart;
				long frames = FL::GetFramesCounted() - framesCountedAtStart;

				fpsTrackingCounter = 0;
				float measurement = (float)frames / ((float)frameTime / 1000);
				fps = (measurement * smoothing) + (lastFrameFps * (1.0f - smoothing));
				lastFrameFps = measurement;

				framesCountedAtStart = FL::GetFramesCounted();
				frameStart = FL::GetEngineTime();
			}
			fpsTrackingCounter++;

			ellapsedTimeString =          "time (ms): " + std::to_string(FL::GetFramesCounted());
		}

		ImGui::SetCursorScreenPos(Vector2(currentPos.x, currentPos.y + 25));
		ImGui::PushStyleColor(ImGuiCol_Text, FL::GetColor32("logText"));
		ImGui::Text(ellapsedTimeString.c_str());
		ImGui::PopStyleColor();
	}

	void RenderStatsOnGameView()
	{		
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, Vector2(0, 0));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		FL::PushWindowStyles();
		ImGui::Begin("Game View", 0, 16 | 8);
		// {

			Vector2 gameViewSize = ImGui::GetWindowSize();
			Vector2 gameViewPos = ImGui::GetWindowPos();
			ImGui::SetCursorScreenPos(Vector2(gameViewPos.x + 3, gameViewPos.y + gameViewSize.y - 10));
			RenderGameTimeStats();

		// }
		ImGui::End(); // Game View
		FL::PopWindowStyles();
		ImGui::PopStyleVar();
		ImGui::PopStyleVar();
		ImGui::PopStyleVar();
	}
}