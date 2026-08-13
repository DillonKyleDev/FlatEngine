#include "Types.h"
#include "components/Transform.h"
#include "FlatEngine.h"
#include "GameObject.h"
#include "GuiCore.h"
#include "physics/PhysicsManager.h"
#include "managers/Scene.h"
#include "render/RenderWindow.h"
#include "render/VulkanManager.h"
#include "managers/Assets.h"
#include "managers/Controls.h"
#include "managers/SceneManager.h"
#include "managers/Settings.h"
#include "managers/ProjectManager.h"
#include "SceneView.h"
#include "tools/Pool.h"
#include "tools/Time.h"
#include "tools/Vector2.h"

#include <cmath>
#include "imgui.h"
#include "tools/Vector3.h"

namespace FL = FlatEngine;


namespace FlatEngine 
{		
	namespace SceneView
	{
		std::vector<SceneRenderObject> persistentSceneRenderObjects;
        std::vector<PoolObject<SceneRenderObject>> debugDrawSceneRenderObjects;
		void CleanupSceneRenderObject(SceneRenderObject& renderObject) { renderObject.mesh.Cleanup(); }
        UMapVector<SceneRenderObject> cameraSceneRenderObjects = UMapVector<SceneRenderObject>(&CleanupSceneRenderObject);		
		UMapVector<SceneRenderObject> lightSceneRenderObjects;
		SceneRenderObject transformGizmoRenderObject;
		SceneRenderObject orientationGizmoRenderObject;
		Vector2 finalImageSize;

		SceneRenderObject CreateLineObject()
		{
			SceneRenderObject object;
			object.mesh.CreateUniformBuffers();
			object.mesh.SetMaterial("fl_debugDraw");
			object.mesh.SetModel("../engine/models/line.obj", false);	
			object.mesh.SetUBOVec4("color", Assets::assetManager.GetColor("debug")); 				          
			object.mesh.CreateResources();
			return object;
		}
		SceneRenderObject CreateQuadObject()
		{
			SceneRenderObject object;
			object.mesh.CreateUniformBuffers();
			object.mesh.SetMaterial("fl_debugDraw");
			object.mesh.SetModel("../engine/models/quadLines.obj", false);	
			object.mesh.SetUBOVec4("color", Assets::assetManager.GetColor("debug")); 				          
			object.mesh.CreateResources();
			return object;
		}
		SceneRenderObject CreateCircleObject()
		{
			SceneRenderObject object;
			object.mesh.CreateUniformBuffers();
			object.mesh.SetMaterial("fl_debugDraw");
			object.mesh.SetModel("../engine/models/circle.obj", false);	
			object.mesh.SetUBOVec4("color", Assets::assetManager.GetColor("debug")); 				          
			object.mesh.CreateResources();
			return object;
		}
		std::vector<SceneRenderObject> CreateCapsuleObject()
		{
			std::vector<SceneRenderObject> capsuleShapes = { CreateCircleObject(), CreateCircleObject(), CreateLineObject(), CreateLineObject(), CreateLineObject(), CreateLineObject() };			
			return capsuleShapes;
		}
		std::vector<SceneRenderObject> CreatePolygonObject()
		{
			std::vector<SceneRenderObject> polygonShapes = { CreateLineObject(), CreateLineObject(), CreateLineObject() };			
			return polygonShapes;
		}
		std::vector<SceneRenderObject> CreateChainObject()
		{
			std::vector<SceneRenderObject> chainShapes = { CreateLineObject(), CreateLineObject(), CreateLineObject(), CreateLineObject() };			
			return chainShapes;
		}

		void CleanupPoolObject(SceneRenderObject& object)
		{
			object.mesh.Cleanup();
		}

		Pool<SceneRenderObject> debugLinePool = Pool<SceneRenderObject>(CreateLineObject, CleanupPoolObject, 10);
		Pool<SceneRenderObject> debugQuadPool = Pool<SceneRenderObject>(CreateQuadObject, CleanupPoolObject, 10);
		Pool<SceneRenderObject> debugCirclePool = Pool<SceneRenderObject>(CreateCircleObject, CleanupPoolObject, 10);

		Vector2 sceneViewDimensions = Vector2(600, 400);	
		Vector2 sceneViewCenter = Vector2();
		Vector2 sceneViewScrolling = Vector2();
		Vector2 sceneViewportCenter = Vector2();	
		float sceneViewGridStep = 50.0f; // Used to convert grid space values to pixel values.ie. 2 grid squares = 2 * 50 = 100px.
		bool b_firstSceneRenderPass = true;
		bool b_sceneHasBeenSet = false;
		bool b_sceneViewLockedOnObject = false;
		bool b_sceneViewRightClicked = false;
		long sceneViewLockedObjectID = -1;
		std::vector<Line> sceneLines = std::vector<Line>();	

		Camera sceneViewCamera = Camera();
		Transform sceneViewCameraTransform = Transform();

		// private
		bool b_showGridObjects = true;        
		bool b_gridHorizontal = true;

		// Show cursor position in scene view when pressing Alt
		void RenderSceneViewTooltip()
		{
			ImGuiIO& inputOutput = ImGui::GetIO();
			Vector2 positionInGrid = Vector2((inputOutput.MousePos.x - sceneViewCenter.x) / sceneViewGridStep, -(inputOutput.MousePos.y - sceneViewCenter.y) / sceneViewGridStep);
			std::string cursorXPosGrid = "x: " + std::to_string(positionInGrid.x);
			std::string cursorYPosGrid = "y: " + std::to_string(positionInGrid.y);	
			std::string cursorXPosScreen = "x: " + std::to_string((int)inputOutput.MousePos.x);
			std::string cursorYPosScreen = "y: " + std::to_string((int)inputOutput.MousePos.y);
			GuiCore::BeginToolTip("");		
			ImGui::Text("Cursor Pos World");
			ImGui::Text("%s", cursorXPosGrid.c_str());
			ImGui::Text("%s", cursorYPosGrid.c_str());		
			GuiCore::RenderSeparator(3, 3);
			ImGui::Text("Cursor Pos Screen");
			ImGui::Text("%s", cursorXPosScreen.c_str());
			ImGui::Text("%s", cursorYPosScreen.c_str());				
			GuiCore::EndToolTip();
		}

		void RenderCursorModeButtons()
		{
			Vector2 iconSize = Vector2(24);			
			Vector2 startPos = ImGui::GetCursorScreenPos();
			ImGui::SetCursorScreenPos(Vector2(startPos.x + 10, startPos.y + 5));

			if (GuiCore::cursorMode == GuiCore::CURSOR_MODE::CURSOR_MODE_TRANSLATE)
			{
				Vector2 currentPos = ImGui::GetCursorScreenPos();				
				if (GuiCore::RenderImageButton("#TranslateModeIcon", Assets::assetManager.GetTexture("translate"), iconSize, 0, Vector2(1), "buttonBorder", "selectedCursorModeButtonBg", "imageButtonTint", "cursorModeButtonHoverSelected"))
				{
					GuiCore::cursorMode = GuiCore::CURSOR_MODE::CURSOR_MODE_TRANSLATE;
				}
			}
			else
			{
				if (GuiCore::RenderImageButton("#TranslateModeIcon", Assets::assetManager.GetTexture("translate"), iconSize, 0, Vector2(1), "buttonBorder", "transparent", "imageButtonTint", "cursorModeButtonHover"))
				{
					GuiCore::cursorMode = GuiCore::CURSOR_MODE::CURSOR_MODE_TRANSLATE;
				}
			}
			if (ImGui::IsItemHovered())
			{
				GuiCore::RenderTextToolTip("Translate Mode");
			}
		}

		void RenderGameTimeStats()
		{
			Vector2 currentPos = ImGui::GetCursorScreenPos();

			std::string ellapsedTimeString =          "time (ms): ---";
			static Uint32 frameStart = Time::Time();		
			static long framesCountedAtStart = GetFramesCounted();
			static float fps = 60;
			static float lastFrameFps = 60;
			static int fpsTrackingCounter = 0;
			float smoothing = 0.005f; // smaller = more smoothing

			if (GameLoopStarted())
			{
				// Slows down the display of fps so it is readable
				if (fpsTrackingCounter == 5)
				{
					Uint32 frameTime = Time::Time() - frameStart;
					long frames = GetFramesCounted() - framesCountedAtStart;

					fpsTrackingCounter = 0;
					float measurement = (float)frames / ((float)frameTime / 1000);
					fps = (measurement * smoothing) + (lastFrameFps * (1.0f - smoothing));
					lastFrameFps = measurement;

					framesCountedAtStart = GetFramesCounted();
					frameStart = Time::Time();
				}
				fpsTrackingCounter++;

				ellapsedTimeString =          "time (ms): " + std::to_string(GetFramesCounted());
			}

			ImGui::SetCursorScreenPos(Vector2(currentPos.x, currentPos.y + 25));
			ImGui::PushStyleColor(ImGuiCol_Text, Assets::assetManager.GetColor32("logText"));
			ImGui::Text("%s", ellapsedTimeString.c_str());
			ImGui::PopStyleColor();
		}

		void GetMouseDelta(Vector2& mouseDelta, Vector2 mousePos, Vector2& lastMousePos)
		{
			Vector2 extent = Vector2((float)RenderWindow::window.GetExtent().width, (float)RenderWindow::window.GetExtent().height);
			mouseDelta = Vector2(mousePos.x - lastMousePos.x, mousePos.y - lastMousePos.y);
			lastMousePos = mousePos;
			
			if (mousePos.x > extent.x - 2)
			{
				SDL_WarpMouseInWindow(RenderWindow::window.GetWindow(), 1, (int)mousePos.y);
				lastMousePos = Vector2(0, mousePos.y);
				mouseDelta.x = 1;
			}
			else if (mousePos.x < 1)
			{
				SDL_WarpMouseInWindow(RenderWindow::window.GetWindow(), (int)extent.x - 2, (int)mousePos.y);
				lastMousePos = Vector2(extent.x - 1, mousePos.y);
				mouseDelta.x = -1;
			}
			if (mousePos.y > extent.y - 1)
			{
				SDL_WarpMouseInWindow(RenderWindow::window.GetWindow(), (int)mousePos.x, 1);
				lastMousePos = Vector2(mousePos.x, 0);
				mouseDelta.y = 1;
			}
			else if (mousePos.y < 1)
			{
				SDL_WarpMouseInWindow(RenderWindow::window.GetWindow(), (int)mousePos.x, (int)extent.y - 1);
				lastMousePos = Vector2(mousePos.x, extent.y);
				mouseDelta.y = 1;
			}			
		}

		void AddSceneCameraRotation(Vector2 mouseDelta)
		{
			Vector3 rotation = sceneViewCameraTransform.GetRotation();
			sceneViewCameraTransform.AddYRotation(-mouseDelta.x * 0.25f);			
			
			if (rotation.x + mouseDelta.y * 0.25f >= 90)
			{
				sceneViewCameraTransform.SetRotation(Vector3(89.99f, rotation.y, rotation.z));
			}
			else if (rotation.x + mouseDelta.y * 0.25f <= -90)
			{
				sceneViewCameraTransform.SetRotation(Vector3(-89.99f, rotation.y, rotation.z));
			}
			else
			{
				sceneViewCameraTransform.AddXRotation(mouseDelta.y * 0.25f);
			}
		}

		void AddSceneViewMouseControls(Vector2 startPos, Vector2 size)
		{
			std::string buttonID = "##SceneViewMouseControls";				
			Uint32 rectColor = ImGui::GetColorU32(Vector4()); 
			bool b_filled = false; 
			ImGuiButtonFlags buttonFlags; 
			bool b_allowOverlap = true; 
			bool b_weightedScroll = false; 
			float zoomMultiplier = 2; 
			Vector2 rightMouseDelta = Vector2();
			Vector2 leftMouseDelta = Vector2();
			Vector2 mousePos = ImGui::GetIO().MousePos;
			static Vector2 lastMousePos = ImGui::GetIO().MousePos;

			if (size.x > 0 && size.y > 0)
			{
				ImGuiIO& inputOutput = ImGui::GetIO();
				Vector2 endPos = Vector2(startPos.x + size.x, startPos.y + size.y);

				// For calculating scrolling mouse position and what vector to zoom to
				sceneViewDimensions = endPos - startPos;			

				GuiCore::RenderInvisibleButton(buttonID.c_str(), startPos, size, b_allowOverlap, false, ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight);
				const bool b_isHovered = ImGui::IsItemHovered();
				const bool b_isActive = ImGui::IsItemActive();
				const bool b_isClicked = ImGui::IsItemClicked();

				const float mouse_threshold_for_pan = 0.0f;
				if (ImGui::IsItemClicked(ImGuiMouseButton_Left) || ImGui::IsItemClicked(ImGuiMouseButton_Right))
				{
					lastMousePos = mousePos;
					b_sceneViewRightClicked = true;
					SDL_ShowCursor(SDL_FALSE);
				}
				if (b_isActive)
				{
					if (ImGui::IsMouseDragging(ImGuiMouseButton_Right, mouse_threshold_for_pan))
					{	
						GetMouseDelta(rightMouseDelta, mousePos, lastMousePos);

						float moveDamping = Settings::settings.sceneViewCameraSpeed * 0.00001f;									

						if (IsOrthoGraphic())
						{
							Vector3 cameraPos = sceneViewCameraTransform.GetPosition();
							sceneViewCameraTransform.SetPosition(Vector3(cameraPos.x - (rightMouseDelta.x / (float)sceneViewCamera.gridStep), cameraPos.y + (rightMouseDelta.y / (float)sceneViewCamera.gridStep), cameraPos.z));			
						}
						else 
						{						
							AddSceneCameraRotation(rightMouseDelta);

							Controls::MappingContext* engineContext = FL::Controls::GetMappingContext("EngineContext");
							glm::vec4 lookDir = sceneViewCameraTransform.GetLookDirection();
							Vector2 xzPlane = Vector2(lookDir.x, lookDir.z);							
							Vector2 leftDir = Vector2::Rotate(xzPlane, -90);
							Vector2 rightDir = Vector2::Rotate(xzPlane, 90);
							
							if (engineContext->ActionPressed("MoveCameraLeft"))
							{
								sceneViewCamera.AddVelocity(Vector3(leftDir.x * moveDamping, 0, leftDir.y * moveDamping));
							}
							if (engineContext->ActionPressed("MoveCameraRight"))
							{
								sceneViewCamera.AddVelocity(Vector3(rightDir.x * moveDamping, 0, rightDir.y * moveDamping));
							}
							if (engineContext->ActionPressed("MoveCameraForward"))
							{
								sceneViewCamera.AddVelocity(Vector3(lookDir.x * moveDamping, lookDir.y * moveDamping, lookDir.z * moveDamping));
							}
							if (engineContext->ActionPressed("MoveCameraBack"))
							{
								sceneViewCamera.AddVelocity(Vector3(-lookDir.x * moveDamping, -lookDir.y * moveDamping, -lookDir.z * moveDamping));
							}
							if (engineContext->ActionPressed("MoveCameraUp"))
							{
								sceneViewCamera.AddVelocity(Vector3(0, moveDamping, 0));
							}
							if (engineContext->ActionPressed("MoveCameraDown"))
							{
								sceneViewCamera.AddVelocity(Vector3(0, -moveDamping, 0));
							}
						}				
					}				
					if (ImGui::IsMouseDragging(ImGuiMouseButton_Left, mouse_threshold_for_pan))
					{	
						GetMouseDelta(leftMouseDelta, mousePos, lastMousePos);

						if (IsOrthoGraphic())
						{
							AddSceneCameraRotation(leftMouseDelta);
						}
					}									
				}

				Vector3& cameraVelocity = sceneViewCamera.GetVelocity();
				if (cameraVelocity != 0)
				{				
					Vector3 position = sceneViewCameraTransform.GetPosition();
					sceneViewCameraTransform.SetPosition(position + cameraVelocity);
					cameraVelocity = cameraVelocity * 0.95f;
				}

				if (b_isHovered)
				{
					// Get scroll amount for changing zoom level of scene view
					Vector2 mousePos = Vector2(inputOutput.MousePos.x, inputOutput.MousePos.y);
					float scrollInput = inputOutput.MouseWheel;
					float weight = 0.1f;
					Vector2 signedMousePos = mousePos - sceneViewCenter - (sceneViewDimensions * 0.5f);
					float zoomSpeed = 1;					

					if (inputOutput.KeyCtrl)
					{
						zoomSpeed += zoomMultiplier;
					}

					if (scrollInput != 0)
					{
						if (b_weightedScroll)
						{
							Vector3 currentPos = sceneViewCameraTransform.GetPosition();
							sceneViewCameraTransform.SetPosition(Vector3(currentPos.x + scrollInput * (signedMousePos.x * weight), currentPos.y - scrollInput * (signedMousePos.y * weight), 0));
						}

						if (sceneViewCamera.gridStep + zoomSpeed * scrollInput < maxGridStep && sceneViewCamera.gridStep + zoomSpeed * scrollInput > minGridStep)
						{
							sceneViewCamera.gridStep += zoomSpeed * scrollInput;
						}
					}
				}
				if (ImGui::IsItemDeactivated())
				{				
					b_sceneViewRightClicked = false;
					SDL_ShowCursor(SDL_TRUE);
				}

				// Show cursor position in scene view when pressing Alt
				if (b_isHovered && inputOutput.KeyAlt)
				{
					RenderSceneViewTooltip();
				}
			}
		}

		void PositionTransformGizmo()
        {
			long focusedID = ProjectManager::loadedProject.focusedGameObjectID;
			if (focusedID != -1)
			{
				Transform* focusedTransform = SceneManager::loadedScene.Get<Transform>(focusedID);
				if (focusedTransform == nullptr)
					return;

				Vector3 position = focusedTransform->GetPosition();
				transformGizmoRenderObject.mesh.SetActive(true);
				transformGizmoRenderObject.transform.SetPosition(position);
			}
			else 
			{
				transformGizmoRenderObject.mesh.SetActive(false);
			}		
        }

		void ClearDebugDrawObjects()
		{
			debugLinePool.ReturnAll();
			debugQuadPool.ReturnAll();
			debugCirclePool.ReturnAll();

			debugDrawSceneRenderObjects.clear();
		}

		void RenderSceneView(bool& b_show)
		{		
			if (!debugLinePool.Initialized())
			{
				debugLinePool.Init();
			}
			if (!debugQuadPool.Initialized())
			{
				debugQuadPool.Init();
			}
			if (!debugCirclePool.Initialized())
			{
				debugCirclePool.Init();
			}	
			
			for (Body2D& body2D : SceneManager::loadedScene.GetAll<Body2D>().GetAll())
			{
				body2D.UpdateRenderShapes();
			}
			for (SceneRenderObject& renderCamera : cameraSceneRenderObjects.GetAll())
			{
				Transform* transform = SceneManager::loadedScene.Get<Transform>(renderCamera.ID);
				Transform copy = transform != nullptr ? *transform : Transform();
				copy.SetScale(Vector3(2,3,3));
				renderCamera.transform = copy;
			}

			if (!b_show)
				return;
						
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, Vector2());
			
			if (GuiCore::BeginWindow("Scene View", b_show, ImGuiWindowFlags_NoScrollWithMouse, "black"))
			{				
				PositionTransformGizmo();				

				Vector2 canvas_p0 = ImGui::GetCursorScreenPos();
				Vector2 canvas_sz = ImGui::GetContentRegionAvail();

				if (canvas_sz.x < 50.0f) canvas_sz.x = 50.0f;
				if (canvas_sz.y < 50.0f) canvas_sz.y = 50.0f;
				Vector2 canvas_p1 = Vector2(canvas_p0.x + canvas_sz.x, canvas_p0.y + canvas_sz.y);

				// Set initial viewport dimensions for rendering scene view grid and objects
				if (!b_firstSceneRenderPass)
				{
					// if (!b_sceneHasBeenSet)
					// {
						sceneViewDimensions = canvas_sz;											
						b_sceneHasBeenSet = true;
					// }
				}
				b_firstSceneRenderPass = false;
				
				sceneViewDimensions = canvas_p1 - canvas_p0;	
				sceneViewportCenter = canvas_p0 + sceneViewDimensions;

				ImGuiIO& inputOutput = ImGui::GetIO();
				Vector2 currentPos = ImGui::GetCursorScreenPos();
				Vector2 centerOffset = sceneViewDimensions * 0.5f;
				bool b_weightedScroll = false;
				Vector2 startingPos = ImGui::GetCursorScreenPos();
				
				std::vector<VkDescriptorSet> descriptors = VulkanManager::vulkan.GetSceneViewDescriptorSets();

				if (descriptors.size() > 0 && descriptors[VulkanManager::currentFrame])
				{
					Vector2 regionAvailable = ImGui::GetContentRegionAvail();
					float targetAspect = 16.0f / 9.0f;
					float regionAspect = regionAvailable.x / regionAvailable.y;

					if (regionAspect > targetAspect)
					{
						// Too wide — pillarbox
						float w = regionAvailable.y * targetAspect;
						float offset = (regionAvailable.x - w) / 2.0f;
						startingPos.x += (int)offset;
						finalImageSize = Vector2((int)w, regionAvailable.y);
					}
					else
					{
						// Too tall — letterbox
						float h = regionAvailable.x / targetAspect;
						float offset = (regionAvailable.y - h) / 2.0f;
						startingPos.y += (int)offset;
						finalImageSize = Vector2(regionAvailable.x, (int)h);
					}

					ImGui::SetCursorScreenPos(startingPos);
					ImGui::Image(descriptors[VulkanManager::currentFrame], finalImageSize);
				}
				
				AddSceneViewMouseControls(canvas_p0, canvas_sz);

				// Drop Target
				int droppedValue = -1;
				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(GuiCore::fileExplorerTarget.c_str()))
					{
						IM_ASSERT(payload->DataSize == sizeof(int));
						droppedValue = *(const int*)payload->Data;
					}
					ImGui::EndDragDropTarget();
				}
				// FIND WAY TO DO THIS IN 3D
				// Create a GameObject from a file in the Explorer by dragging it into the Scene View space
				if (droppedValue != -1 && GuiCore::selectedFiles.size() >= droppedValue)
				{
					ImGuiIO& inputOutput = ImGui::GetIO();
					Vector2 mousePosInGrid = Vector2((inputOutput.MousePos.x - sceneViewCenter.x) / sceneViewGridStep, -(inputOutput.MousePos.y - sceneViewCenter.y) / sceneViewGridStep);
					std::string filePath = GuiCore::selectedFiles[droppedValue - 1];			
				
					//GameObject* newObject = CreateAssetUsingFilePath(filePath, mousePosInGrid);
					//if (newObject != nullptr)
					//{
					//	SetFocusedGameObjectID(newObject->GetID());
					//}
				}

				// Cursor mode select
				ImGui::SetCursorScreenPos(canvas_p0);
				RenderCursorModeButtons();
				ImGui::SameLine();
				
				// Toggle Orthographic / Perspective mode			
				Vector2 iconSize = Vector2(24);
				if (GuiCore::RenderImageButton("#PerspectiveOrthographic", Assets::assetManager.GetTexture("tileDrawColliders"), iconSize, 0, Vector2(1), "buttonBorder", "selectedCursorModeButtonBg", "imageButtonTint", "cursorModeButtonHoverSelected"))
				{
					ToggleOrthographic();
				}
				if (ImGui::IsItemHovered())
				{
					GuiCore::RenderTextToolTip("Perspective/Orthographic");
				}
				ImGui::SameLine();

				// Game Stats in SceneView
				ImGui::SetCursorScreenPos(Vector2(canvas_p0.x + 3, canvas_p1.y - 40)); // was - 54 y
				RenderGameTimeStats();		
			}			
			
			ImGui::PopStyleVar(2);
			GuiCore::EndWindow(); // Scene View
		}

		void RenderStatsOnGameView()
		{		
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, Vector2());
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);		
			ImGui::Begin("Game View", 0, 16 | 8);
			// {

				Vector2 gameViewSize = ImGui::GetWindowSize();
				Vector2 gameViewPos = ImGui::GetWindowPos();
				ImGui::SetCursorScreenPos(Vector2(gameViewPos.x + 3, gameViewPos.y + gameViewSize.y - 10));
				RenderGameTimeStats();

			// }
			ImGui::End(); // Game View		
			ImGui::PopStyleVar();
			ImGui::PopStyleVar();
			ImGui::PopStyleVar();
		}

		void UpdateSceneObjectColors()
        {
            persistentSceneRenderObjects[PersistentSceneObjectIndex_GridH].mesh.SetUBOVec4("color", Assets::assetManager.GetColor("grid"));            
			persistentSceneRenderObjects[PersistentSceneObjectIndex_GridV].mesh.SetUBOVec4("color", Assets::assetManager.GetColor("grid"));            
            persistentSceneRenderObjects[PersistentSceneObjectIndex_XAxis].mesh.SetUBOVec4("color", Assets::assetManager.GetColor("xAxis"));            
            persistentSceneRenderObjects[PersistentSceneObjectIndex_YAxis].mesh.SetUBOVec4("color", Assets::assetManager.GetColor("yAxis"));    
            persistentSceneRenderObjects[PersistentSceneObjectIndex_ZAxis].mesh.SetUBOVec4("color", Assets::assetManager.GetColor("zAxis"));
        }

		long AddSceneViewCameraGizmo(Transform transform, long ownerID)
		{
			SceneRenderObject sceneObject;
			sceneObject.ID = ownerID;
			SceneRenderObject* ref = cameraSceneRenderObjects.Add(sceneObject.ID, sceneObject);			
			ref->mesh.CreateUniformBuffers();
			ref->mesh.SetMaterial("fl_unlit");
            ref->mesh.SetModel("../engine/models/camera.obj", false);
            ref->mesh.AddTexture("../engine/images/textures/camera.png", 0);            
            ref->mesh.CreateResources();
			transform.SetScale(Vector3(2,3,3));
			ref->transform.PutData(transform.GetData(), "Scene Camera Gizmo");
			return ref->ID;
		}

		long AddSceneViewLightGizmo(Transform transform, long ownerID)
		{
			SceneRenderObject sceneObject;
			sceneObject.ID = ownerID;
			SceneRenderObject* ref = lightSceneRenderObjects.Add(sceneObject.ID, sceneObject);			
			ref->mesh.CreateUniformBuffers();
			ref->mesh.SetMaterial("fl_unlit");
            ref->mesh.SetModel("../engine/models/camera.obj", false);
            ref->mesh.AddTexture("../engine/images/textures/camera.png", 0);            
            ref->mesh.CreateResources();
			ref->transform.PutData(transform.GetData(), "Scene Camera Gizmo");
			return ref->ID;
		}

		Transform GetLineTransformForStartEndPos(Vector3 startPos, Vector3 endPos)
		{
			Transform transform;
			Vector3 direction = endPos - startPos;
			transform.SetPosition(startPos);
			transform.SetScale(Vector3(direction.GetMagnitude()));
			transform.SetRotation(Vector3::DirectionToRotation(direction));

			return transform;
		}
		void DebugDrawLine(Vector3 startPos, Vector3 endPos, std::string color)
		{
			AddDebugDrawObject(DebugSceneObjectType_Line, GetLineTransformForStartEndPos(startPos, endPos), color);
		}

		void DebugDrawQuad(Vector3 position, Vector2 scale, std::string color, Vector3 rotation)
		{
			Transform transform;
			transform.SetPosition(position);
			transform.SetScale(Vector3(scale.x, scale.y, 1));
			transform.SetRotation(rotation);
			AddDebugDrawObject(DebugSceneObjectType_Quad, transform, color);
		}

		void DebugDrawCircle(Vector3 position, float radius, std::string color, Vector3 rotation)
		{
			Transform transform;
			transform.SetPosition(position);
			transform.SetScale(Vector3(radius, radius, 1));
			transform.SetRotation(rotation);
			AddDebugDrawObject(DebugSceneObjectType_Circle, transform, color);
		}

		void AddDebugDrawObject(DebugSceneObjectType type, Transform transform, std::string color)
		{
			switch (type)
			{
				case DebugSceneObjectType_Line: 
				{					
					debugDrawSceneRenderObjects.push_back(debugLinePool.Get());
					debugDrawSceneRenderObjects.back().object->transform.PutData(transform.GetData(), "Debug Line Object");
					debugDrawSceneRenderObjects.back().object->mesh.SetUBOVec4("color", Assets::assetManager.GetColor(color));
					break;
				}
				case DebugSceneObjectType_Circle: 
				{					
					debugDrawSceneRenderObjects.push_back(debugCirclePool.Get());
					debugDrawSceneRenderObjects.back().object->transform.PutData(transform.GetData(), "Debug Circle Object");
					debugDrawSceneRenderObjects.back().object->mesh.SetUBOVec4("color", Assets::assetManager.GetColor(color));
					break;
				}
				case DebugSceneObjectType_Quad: 
				{										
					debugDrawSceneRenderObjects.push_back(debugQuadPool.Get());
					debugDrawSceneRenderObjects.back().object->transform.PutData(transform.GetData(), "Debug Quad Object");
					debugDrawSceneRenderObjects.back().object->mesh.SetUBOVec4("color", Assets::assetManager.GetColor(color));
					break;
				}
				case DebugSceneObjectType_Sphere: 
				{					
					
					// AddDebugDrawObject(DebugSceneObjectType_Circle, transform);
					break;
				}
				case DebugSceneObjectType_Cube: 
				{					
					
					break;
				}
				default: break;
			}
		}

        void LoadSceneViewObjects()
        {            
			json persistentSceneObjectJson = JsonHelper::LoadFileData("../engine/scene_view_grid_objects.json");
			persistentSceneRenderObjects.resize(persistentSceneObjectJson.size());

			for (auto jsonData : persistentSceneObjectJson)
			{
				SceneRenderObject sceneObject;
				long ID = JsonHelper::CheckJsonLong(jsonData, "id", "Persistent Scene Render Object");				
				if (persistentSceneRenderObjects.size() >= ID)
				{
					persistentSceneRenderObjects[ID] = sceneObject;
				}
				persistentSceneRenderObjects[ID].PutData(jsonData);
			}

			transformGizmoRenderObject.mesh.CreateUniformBuffers();
			transformGizmoRenderObject.mesh.SetMaterial("fl_transformGizmo");
			transformGizmoRenderObject.mesh.SetModel("../engine/models/transform_gizmo.obj");			
			transformGizmoRenderObject.mesh.AddTexture("../engine/images/textures/transform_gizmo.png", 0);
			transformGizmoRenderObject.mesh.CreateResources();

			orientationGizmoRenderObject.mesh.CreateUniformBuffers();
			orientationGizmoRenderObject.mesh.SetMaterial("fl_unlit");
			orientationGizmoRenderObject.mesh.SetModel("../engine/models/orientation_gizmo.obj");			
			orientationGizmoRenderObject.mesh.AddTexture("../engine/images/textures/orientation_gizmo.png", 0);
			orientationGizmoRenderObject.mesh.CreateResources();
        }

		bool ShowSceneViewGridObjects()
        {
            return b_showGridObjects;
        }

        void SetShowSceneViewGridObjects(bool b_setShowGridObjects)
        {
            b_showGridObjects = b_setShowGridObjects;
			
			// And other things...
        }

        void ToggleShowSceneViewGridObjects()
        {
            b_showGridObjects = !b_showGridObjects;
        }

		const bool ShouldShowSceneViewGridObjects()
		{
			return b_showGridObjects;
		}

		void SetOrthographic(bool b_isOrthographic)
        {
            sceneViewCamera.b_orthographic = b_isOrthographic;
			Vector3 cameraPos = sceneViewCameraTransform.GetPosition();
			sceneViewCameraTransform.SetPosition(Vector3((int)cameraPos.x, (int)cameraPos.y, (int)cameraPos.z));

			// Position and rotate the grid as necessary
			if (sceneViewCamera.b_orthographic)
			{					
				persistentSceneRenderObjects[PersistentSceneObjectIndex_GridH].mesh.SetActive(false);
				persistentSceneRenderObjects[PersistentSceneObjectIndex_GridV].mesh.SetActive(true);	
				persistentSceneRenderObjects[PersistentSceneObjectIndex_GridV].transform.SetPosition(Vector3(0,0,-1)); // Check in the shader
				persistentSceneRenderObjects[PersistentSceneObjectIndex_XAxis].transform.SetPosition(Vector3(0,0,-1));
				persistentSceneRenderObjects[PersistentSceneObjectIndex_YAxis].transform.SetPosition(Vector3(0,0,-1));				
				// sceneViewCamera.orthoHorizontalViewAngle = 180;
				// sceneViewCamera.orthoVerticalViewAngle = 0;
			}
			else
			{				
				persistentSceneRenderObjects[PersistentSceneObjectIndex_XAxis].transform.SetPosition(Vector3(0,0,0));
				persistentSceneRenderObjects[PersistentSceneObjectIndex_YAxis].transform.SetPosition(Vector3(0,0,0));				
				SetGridHorizontal(b_gridHorizontal); // Handles activation/deactivation logic
			} 
        }

        void ToggleOrthographic()
        {
            SetOrthographic(!sceneViewCamera.b_orthographic);
        }

		const bool IsOrthoGraphic()
		{
			return sceneViewCamera.b_orthographic;
		}

		extern void ToggleGridHorizontal()
		{
			SetGridHorizontal(!b_gridHorizontal);
		}

		void SetGridHorizontal(bool b_horizontal)
		{
			b_gridHorizontal = b_horizontal;

			if (b_gridHorizontal)
			{
				persistentSceneRenderObjects[0].mesh.SetActive(true);
				persistentSceneRenderObjects[1].mesh.SetActive(false);
				
			}
			else
			{
				persistentSceneRenderObjects[0].mesh.SetActive(false);
				persistentSceneRenderObjects[1].mesh.SetActive(true);
			}
		}

		const bool IsGridHorizontal()
		{
			return b_gridHorizontal;
		}

		// Converts from world grid space in Scene View to screen space
		Vector2 Scene_ConvertWorldToScreen(Vector2 positionInWorld)
		{
			float x = sceneViewCenter.x + (positionInWorld.x * sceneViewGridStep);
			float y = sceneViewCenter.y - (positionInWorld.y * sceneViewGridStep);

			return Vector2(x, y);
		}

		// Converts from screen space to world grid space in Scene View
		Vector2 Scene_ConvertScreenToWorld(Vector2 positionOnScreen)
		{
			float x = (positionOnScreen.x - sceneViewCenter.x) / sceneViewGridStep;
			float y = (sceneViewCenter.y - positionOnScreen.y) / sceneViewGridStep;

			return Vector2(x, y);
		}

		Vector2 Scene_GetMousePosWorld()
		{
			return Scene_ConvertScreenToWorld(ImGui::GetIO().MousePos);
		}

		void RenderGridView(FL::Vector2& centerPoint, FL::Vector2 &scrolling, bool b_weightedScroll, FL::Vector2 canvasP0, FL::Vector2 canvasP1, FL::Vector2 canvasSize, FL::Vector2& gridStep, FL::Vector2 centerOffset, bool b_showAxis)
		{
			ImDrawList* drawList = ImGui::GetWindowDrawList();
			drawList->AddRectFilled(canvasP0, canvasP1, IM_COL32(50, 50, 50, 255));

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
				FL::Logger::log.DrawLine(FL::Vector2(x, canvasP0.y), FL::Vector2(x, canvasP1.y), "grid", 1.0f, drawList);
			}
			// Draw vertical grid lines
			for (float y = trunc(fmodf(scrolling.y + canvasP0.y, gridStep.y)); y < canvasP0.y + canvasSize.y; y += gridStep.y)
			{
				FL::Logger::log.DrawLine(FL::Vector2(canvasP0.x, y), FL::Vector2(canvasP1.x, y), "grid", 1.0f, drawList);
			}

			// Draw our x and y axis blue and green lines
			float divX = trunc(scrolling.x / gridStep.x);
			float modX = fmodf(scrolling.x, gridStep.x);
			float offsetX = (gridStep.x * divX) + modX;
			float divY = trunc(scrolling.y / gridStep.y);
			float modY = fmodf(scrolling.y, gridStep.y);
			float offsetY = (gridStep.y * divY) + modY;

			centerPoint = FL::Vector2(offsetX + canvasP0.x, offsetY + canvasP0.y);

			if (b_showAxis)
			{
				// Blue, green and pink colors for axis and center
				std::string xColor = "xAxis";
				std::string yColor = "yAxis";
				std::string centerDotColor = "centerDot";

				float drawYAxisAt = centerPoint.x;
				float drawXAxisAt = centerPoint.y;

				// x axis bounds check + color change (lighten) if out of bounds
				if (centerPoint.x > canvasP1.x - 1)
				{
					drawYAxisAt = canvasP1.x - 1;
					yColor = "xAxisDark";
				}
				else if (centerPoint.x < canvasP0.x)
				{
					drawYAxisAt = canvasP0.x;
					yColor = "xAxisDark";
				}
				// y axis bounds check + color change (lighten) if out of bounds
				if (centerPoint.y > canvasP1.y - 1)
				{
					drawXAxisAt = canvasP1.y - 1;
					xColor = "yAxisDark";
				}
				else if (centerPoint.y < canvasP0.y)
				{
					drawXAxisAt = canvasP0.y;
					xColor = "yAxisDark";
				}


				// Draw the axis and center point
				FL::Logger::log.DrawLine(FL::Vector2(drawYAxisAt, canvasP0.y), FL::Vector2(drawYAxisAt, canvasP1.y), xColor, 1.0f, drawList);
				FL::Logger::log.DrawLine(FL::Vector2(canvasP0.x, drawXAxisAt), FL::Vector2(canvasP1.x, drawXAxisAt), yColor, 1.0f, drawList);
				FL::Logger::log.DrawPoint(FL::Vector2(centerPoint.x, centerPoint.y), centerDotColor, drawList);
			}
		}

		void RenderViewObject(FL::GameObject &self, FL::Vector2 centerPoint, FL::Vector2 canvasP0, FL::Vector2 canvasSize, float gridStep, ImDrawList* drawList, ImDrawListSplitter* drawSplitter)
		{
			FL::Transform* transform = self.Get<FL::Transform>();
			FL::Animation* animation = self.Get<FL::Animation>();
			FL::Sprite* sprite = self.Get<FL::Sprite>();
			FL::CharacterController* characterController = self.Get<FL::CharacterController>();
			FL::Camera* camera = self.Get<FL::Camera>();
			FL::Button* button = self.Get<FL::Button>();
			FL::Canvas* canvas = self.Get<FL::Canvas>();
			FL::Text* text = self.Get<FL::Text>();
			FL::Body* body = self.Get<FL::Body>();
			FL::TileMap* tileMap = self.Get<FL::TileMap>();

			bool b_spriteButtonAdded = false;

			/*

			if (transform != nullptr)
			{
				long focusedObjectID = GetFocusedGameObjectID();
				FL::Vector2 position = transform->GetAbsolutePosition();
				float rotation = transform->GetAbsoluteRotation();						
				FL::Vector2 scale = transform->GetAbsoluteScale();						
						
				if (self.GetID() != focusedObjectID)
				{
					drawSplitter->SetCurrentChannel(drawList, FL::FL::VulkanManager::maxSpriteLayers + 4);

					float selectObjectWidth = (float)FL::GetTextureObject("selectGameObject")->GetWidth();
					float selectObjectHeight = (float)FL::GetTextureObject("selectGameObject")->GetHeight();
					FL::Vector2 selectObjectOffset = FL::Vector2(selectObjectWidth / 2, selectObjectHeight / 2);

					FL::AddImageToDrawList(FL::GetTexture("selectGameObject"), position, *FL::F_sceneViewCenter, selectObjectWidth, selectObjectHeight, selectObjectOffset, FL::Vector2(1), false, FL::F_sceneViewGridStep->x, drawList);
					FL::RenderInvisibleButton("##TransformSelect_" + std::to_string(transform->GetID()), FL::Scene_ConvertWorldToScreen(position) - selectObjectOffset, FL::Vector2(selectObjectWidth, selectObjectHeight), false);
					const bool b_isItemClicked = ImGui::IsItemClicked();
					const bool b_isItemHovered = ImGui::IsItemHovered();
					if (b_isItemClicked || b_isItemHovered)
					{
						ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
					}
					if (b_isItemClicked)
					{
						SetFocusedGameObjectID(transform->GetParentID());
					}
				}

				if (sprite != nullptr && sprite->GetTexture() != nullptr && sprite->IsActive())
				{
					b_spriteButtonAdded = true;
					VkDescriptorSet spriteTexture = sprite->GetTexture();
					float spriteTextureWidth = (float)sprite->GetTextureWidth();
					float spriteTextureHeight = (float)sprite->GetTextureHeight();
					FL::Vector2 spriteScale = sprite->GetScale();
					FL::Vector2 offset = sprite->GetOffset();				
					bool b_spriteScalesWithZoom = true;
					int renderOrder = sprite->GetRenderOrder();
					FL::Vector4 tintColor = sprite->GetTintColor();
					std::string invisibleButtonID = "GameObjectSelectorButtonSprite_" + std::to_string(sprite->GetID());	
					ImGuiIO& inputOutput = ImGui::GetIO();

					spriteScale.x *= scale.x;
					spriteScale.y *= scale.y;

					FL::Vector2 positionOnScreen = FL::Vector2(FG_sceneViewCenter.x + (position.x * gridStep) - ((offset.x * FL::F_spriteScaleMultiplier * gridStep) * spriteScale.x), FG_sceneViewCenter.y - (position.y * gridStep) - ((offset.y * FL::F_spriteScaleMultiplier * gridStep) * spriteScale.y));
					FL::Vector2 buttonSize = FL::Vector2(spriteTextureWidth * FL::F_spriteScaleMultiplier * gridStep * spriteScale.x, spriteTextureHeight * FL::F_spriteScaleMultiplier * gridStep * spriteScale.y);
					
					AddSceneViewMouseControls(invisibleButtonID, positionOnScreen, buttonSize, FG_sceneViewScrolling, FG_sceneViewCenter, FG_sceneViewGridStep, FL::GetColor32("transparent"), false, 0, true);			
					const bool b_isClicked = ImGui::IsItemClicked();
					if (b_isClicked && (FL::F_CursorMode == FL::F_CURSOR_MODE::TRANSLATE || FL::F_CursorMode == FL::F_CURSOR_MODE::SCALE || FL::F_CursorMode == FL::F_CURSOR_MODE::ROTATE))
					{
						SetFocusedGameObjectID(sprite->GetParentID());
					}
					
					if (renderOrder <= FL::FL::VulkanManager::maxSpriteLayers && renderOrder >= 0)
					{
						drawSplitter->SetCurrentChannel(drawList, renderOrder);
					}
					else
					{
						drawSplitter->SetCurrentChannel(drawList, 0);
					}

					if (spriteScale.x != 0 && spriteScale.y != 0 && spriteTexture != nullptr)
					{
						FL::AddImageToDrawList(spriteTexture, position, centerPoint, spriteTextureWidth, spriteTextureHeight, offset, spriteScale, b_spriteScalesWithZoom, gridStep, drawList, rotation, ImGui::GetColorU32(tintColor));
					}
				}

				if (characterController != nullptr)
				{
					Capsule& capsule = characterController->GetCapsule();
					b2Capsule b2Capsule = capsule.GetB2Capsule();
					bool b_isActive = characterController->IsActive();				
					Shape::ShapeProps shapeProps = capsule.GetShapeProps();
					bool b_isSensor = shapeProps.b_isSensor;				
					float length = shapeProps.capsuleLength;
					float radius = shapeProps.radius;
					float radiusScreen = radius * FG_sceneViewGridStep.x;
					FL::Vector2 offset = shapeProps.positionOffset;
					float rotation = FL::RadiansToDegrees(b2Rot_GetAngle(shapeProps.rotationOffset));

					FL::Vector2 center1 = Scene_ConvertWorldToScreen(position + b2Capsule.center1);
					FL::Vector2 center2 = Scene_ConvertWorldToScreen(position + b2Capsule.center2);
					FL::Vector2 difference = center2 - center1;
					FL::Vector2 diffN = FL::Vector2::Normalize(difference);
					FL::Vector2 diffNR = diffN * radiusScreen;
					FL::Vector2 diffPerp = FL::Vector2::Rotate(diffNR, 90);
					FL::Vector2 flippedDiffPerp = FL::Vector2::Rotate(diffNR, -90);

					FL::Vector4 color;
					FL::Vector4 colorLight;

					if (b_isActive)
					{
						color = FL::GetColor("capsuleColliderActive");
						colorLight = FL::GetColor("capsuleColliderActiveLight");				
					}
					else
					{
						color = FL::GetColor("capsuleColliderInactive");
						colorLight = FL::GetColor("capsuleColliderInactiveLight");
					}

					drawSplitter->SetCurrentChannel(drawList, FL::FL::VulkanManager::maxSpriteLayers + 2);

					FL::DrawCircle(center1, radiusScreen, colorLight, drawList, 2.0f);
					FL::DrawCircle(center1, radiusScreen, color, drawList);

					FL::DrawCircle(center2, radiusScreen, colorLight, drawList, 2.0f);
					FL::DrawCircle(center2, radiusScreen, color, drawList);

					FL::DrawLine(center1 - diffNR, center1 + diffNR, colorLight, 2.0f, drawList);
					FL::DrawLine(center2 - diffNR, center2 + diffNR, colorLight, 2.0f, drawList);
					FL::DrawLine(center1 - diffPerp, center1 + diffPerp, colorLight, 2.0f, drawList);
					FL::DrawLine(center2 - diffPerp, center2 + diffPerp, colorLight, 2.0f, drawList);

					// Sides
					FL::DrawLine(center1 + diffPerp, center1 + diffPerp + difference, colorLight, 2.0f, drawList);
					FL::DrawLine(center1 + diffPerp, center1 + diffPerp + difference, color, 1.0f, drawList);

					FL::DrawLine(center1 + flippedDiffPerp, center1 + flippedDiffPerp + difference, colorLight, 2.0f, drawList);
					FL::DrawLine(center1 + flippedDiffPerp, center1 + flippedDiffPerp + difference, color, 1.0f, drawList);
				}

				if (text != nullptr && text->IsActive())
				{
					std::shared_ptr<Texture> textTexture = text->GetTexture();
					float textWidth = (float)textTexture->GetWidth();
					float textHeight = (float)textTexture->GetHeight();
					FL::Vector2 offset = text->GetOffset();
					int renderOrder = text->GetRenderOrder();				
					bool b_spriteScalesWithZoom = true;
					FL::Vector4 tintColor = text->GetColor();
					std::string invisibleButtonID = "GameObjectSelectorButtonText_" + std::to_string(text->GetID());
					ImGuiIO& inputOutput = ImGui::GetIO();
					
					FL::Vector2 newScale = FL::Vector2(scale.x * FL::F_spriteScaleMultiplier, scale.y * FL::F_spriteScaleMultiplier);

					if (textTexture->GetTexture() != nullptr)
					{					
						FL::Vector2 positionOnScreen = FL::Vector2(FG_sceneViewCenter.x + (position.x * gridStep) - ((offset.x * FL::F_spriteScaleMultiplier * gridStep) * newScale.x), FG_sceneViewCenter.y - (position.y * gridStep) - ((offset.y * FL::F_spriteScaleMultiplier * gridStep) * newScale.y));
						FL::Vector2 buttonSize = FL::Vector2(textWidth * FL::F_spriteScaleMultiplier * gridStep * newScale.x, textHeight * FL::F_spriteScaleMultiplier * gridStep * newScale.y);
						
						AddSceneViewMouseControls(invisibleButtonID, positionOnScreen, buttonSize, FG_sceneViewScrolling, FG_sceneViewCenter, FG_sceneViewGridStep, FL::GetColor32("transparent"), false, 0, true);						
						const bool b_isClicked = ImGui::IsItemClicked();
						if (b_isClicked && (FL::F_CursorMode == FL::F_CURSOR_MODE::TRANSLATE || FL::F_CursorMode == FL::F_CURSOR_MODE::SCALE || FL::F_CursorMode == FL::F_CURSOR_MODE::ROTATE))
						{
							SetFocusedGameObjectID(text->GetParentID());
						}
						
						if (renderOrder <= FL::FL::VulkanManager::maxSpriteLayers && renderOrder >= 0)
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
				
				if (camera != nullptr && camera->IsActive())
				{
					float cameraWidth = camera->GetWidth();
					float cameraHeight = camera->GetHeight();

					float cameraLeftEdge = centerPoint.x + (position.x * FG_sceneViewGridStep.x) - (cameraWidth * FG_sceneViewGridStep.x / 2 * scale.x);
					float cameraTopEdge = centerPoint.y + (-position.y * FG_sceneViewGridStep.y) - (cameraHeight * FG_sceneViewGridStep.y / 2 * scale.y);
					float cameraRightEdge = centerPoint.x + (position.x * FG_sceneViewGridStep.x) + (cameraWidth * FG_sceneViewGridStep.x / 2 * scale.x);
					float cameraBottomEdge = centerPoint.y + (-position.y * FG_sceneViewGridStep.y) + (cameraHeight * FG_sceneViewGridStep.y / 2 * scale.y);

					FL::Vector2 topLeftCorner = FL::Vector2(cameraLeftEdge, cameraTopEdge);
					FL::Vector2 bottomRightCorner = FL::Vector2(cameraRightEdge, cameraBottomEdge);
					FL::Vector2 topRightCorner = FL::Vector2(cameraRightEdge, cameraTopEdge);
					FL::Vector2 bottomLeftCorner = FL::Vector2(cameraLeftEdge, cameraBottomEdge);

					float cameraTextureWidth = (float)FL::GetTextureObject("camera")->GetWidth() / 4;
					float cameraTextureHeight = (float)FL::GetTextureObject("camera")->GetHeight() / 4;
					bool b_scalesWithZoom = false;
					FL::Vector2 cameraTextureOffset = { cameraTextureWidth / 2, cameraTextureHeight / 2 };
					FL::Vector2 cameraTextureScale = { 1, 1 };
					FL::Vector2 offsetPosition = FL::Vector2(position.x - cameraTextureWidth / 2, position.y + cameraTextureHeight / 2);

					// Draw channel 2 for Lower UI
					drawSplitter->SetCurrentChannel(drawList, FL::FL::VulkanManager::maxSpriteLayers + 2);

					// Draw a rectangle to the scene view to represent the camera frustrum
					FL::DrawRectangle(topLeftCorner, bottomRightCorner, canvasP0, canvasSize, FL::GetColor("cameraBox"), 2.0f, drawList);
					FL::DrawLine(topLeftCorner, bottomRightCorner, FL::GetColor("cameraBox"), 2.0f, drawList);
					FL::DrawLine(topRightCorner, bottomLeftCorner, FL::GetColor("cameraBox"), 2.0f, drawList);
					
					FL::AddImageToDrawList(FL::GetTexture("camera"), position, centerPoint, cameraTextureWidth, cameraTextureHeight, cameraTextureOffset, cameraTextureScale, b_scalesWithZoom, gridStep, drawList, 0, IM_COL32(255, 255, 255, FG_iconTransparency));
				}

				if (canvas != nullptr && canvas->IsActive())
				{
					float activeWidth = canvas->GetWidth();
					float activeHeight = canvas->GetHeight();
					int layerNumber = canvas->GetLayerNumber();				

					float renderXStart = FG_sceneViewCenter.x + ((position.x - (activeWidth * scale.x / 2)) * FG_sceneViewGridStep.x);
					float renderYStart = FG_sceneViewCenter.y - ((position.y + (activeHeight * scale.y / 2)) * FG_sceneViewGridStep.x);
					FL::Vector2 renderStart = FL::Vector2(renderXStart, renderYStart);
					FL::Vector2 renderEnd = FL::Vector2(renderXStart + ((activeWidth * scale.x) * FG_sceneViewGridStep.x), renderYStart + ((activeHeight * scale.y) * FG_sceneViewGridStep.x));

					drawSplitter->SetCurrentChannel(drawList, FL::FL::VulkanManager::maxSpriteLayers + 2);

					FL::DrawRectangle(renderStart, renderEnd, canvasP0, canvasSize, FL::GetColor("canvasBox"), 2.0f, drawList);
				}

				if (button != nullptr)
				{
					float activeWidth = button->GetActiveWidth();
					float activeHeight = button->GetActiveHeight();
					FL::Vector2 activeOffset = button->GetActiveOffset();
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

					float activeLeft = FG_sceneViewCenter.x + ((position.x - (activeWidth * scale.x / 2) + activeOffset.x * scale.x) * FG_sceneViewGridStep.x);
					float activeRight = FG_sceneViewCenter.x + ((position.x + (activeWidth * scale.x / 2) + activeOffset.x * scale.x) * FG_sceneViewGridStep.x);
					float activeTop = FG_sceneViewCenter.y - ((position.y + (activeHeight * scale.y / 2) + activeOffset.y * scale.y) * FG_sceneViewGridStep.y);
					float activeBottom = FG_sceneViewCenter.y - ((position.y - (activeHeight * scale.y / 2) + activeOffset.y * scale.y) * FG_sceneViewGridStep.y);

					FL::Vector2 center = FL::Vector2(activeLeft + (activeRight - activeLeft) / 2, activeTop + (activeBottom - activeTop) / 2);

					FL::Vector2 topLeft = { activeLeft, activeTop };
					FL::Vector2 bottomRight = { activeRight, activeBottom };
					FL::Vector2 topRight = { activeRight, activeTop };
					FL::Vector2 bottomLeft = { activeLeft, activeBottom };

					drawSplitter->SetCurrentChannel(drawList, FL::FL::VulkanManager::maxSpriteLayers + 2);

					if (rotation != 0)
					{
						float cosA = cosf(rotation * 2.0f * (float)M_PI / 360.0f); // Convert degrees into radians
						float sinA = sinf(rotation * 2.0f * (float)M_PI / 360.0f);

						topLeft = ImRotate(FL::Vector2(-activeWidth * FG_sceneViewGridStep.x / 2 * scale.x, -activeHeight * FG_sceneViewGridStep.x / 2 * scale.y), cosA, sinA);
						topRight = ImRotate(FL::Vector2(+activeWidth * FG_sceneViewGridStep.x / 2 * scale.x, -activeHeight * FG_sceneViewGridStep.x / 2 * scale.y), cosA, sinA);
						bottomRight = ImRotate(FL::Vector2(+activeWidth * FG_sceneViewGridStep.x / 2 * scale.x, +activeHeight * FG_sceneViewGridStep.x / 2 * scale.y), cosA, sinA);
						bottomLeft = ImRotate(FL::Vector2(-activeWidth * FG_sceneViewGridStep.x / 2 * scale.x, +activeHeight * FG_sceneViewGridStep.x / 2 * scale.y), cosA, sinA);

						FL::Vector2 pos[4] =
						{
							FL::Vector2(center.x + topLeft.x, center.y + topLeft.y),
							FL::Vector2(center.x + topRight.x, center.y + topRight.y),
							FL::Vector2(center.x + bottomRight.x, center.y + bottomRight.y),
							FL::Vector2(center.x + bottomLeft.x, center.y + bottomLeft.y),
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

				if (body != nullptr)
				{
					std::list<Box>& boxes = body->GetBoxes();
					std::list<Circle> circles = body->GetCircles();
					std::list<Capsule>& capsules = body->GetCapsules();
					std::list<FL::Polygon>& polygons = body->GetPolygons();
					std::list<Chain>& chains = body->GetChains();

					for (Box& box : boxes)
					{
						bool b_isActive = body->IsActive();
						FL::Physics::BodyProps bodyProps = body->GetBodyProps();
						Shape::ShapeProps shapeProps = box.GetShapeProps();
						bool b_isSensor = shapeProps.b_isSensor;
						box.UpdateCorners();

						std::vector<FL::Vector2> cornersVec = box.GetCornersScreen();
						FL::Vector2 corners[4] = {
							cornersVec[0],
							cornersVec[1],
							cornersVec[2],
							cornersVec[3]
						};

						drawSplitter->SetCurrentChannel(drawList, FL::FL::VulkanManager::maxSpriteLayers + 2);

						if (b_isActive)
						{
							if (b_isSensor)
							{
								FL::DrawRectangleFromLines(corners, FL::GetColor("sensorActiveLight"), 2.0f, drawList);
								FL::DrawRectangleFromLines(corners, FL::GetColor("sensorActive"), 1.0f, drawList);					
							}
							else
							{
								FL::DrawRectangleFromLines(corners, FL::GetColor("boxColliderActiveLight"), 2.0f, drawList);
								FL::DrawRectangleFromLines(corners, FL::GetColor("boxColliderActive"), 1.0f, drawList);
							}
						}
						else
						{
							if (b_isSensor)
							{
								FL::DrawRectangleFromLines(corners, FL::GetColor("sensorInactiveLight"), 2.0f, drawList);
								FL::DrawRectangleFromLines(corners, FL::GetColor("sensorInactive"), 1.0f, drawList);							
							}
							else
							{
								FL::DrawRectangleFromLines(corners, FL::GetColor("boxColliderInactiveLight"), 2.0f, drawList);
								FL::DrawRectangleFromLines(corners, FL::GetColor("boxColliderInactive"), 1.0f, drawList);
							}
						}
					}

					for (Circle& circle : circles)
					{
						bool b_isActive = body->IsActive();
						FL::Physics::BodyProps bodyProps = body->GetBodyProps();
						Shape::ShapeProps shapeProps = circle.GetShapeProps();
						bool b_isSensor = shapeProps.b_isSensor;
						float radius = shapeProps.radius * FG_sceneViewGridStep.x;
						FL::Vector2 offset = shapeProps.positionOffset;
						FL::Vector2 center = Scene_ConvertWorldToScreen(position + FL::Vector2::Rotate(offset, rotation));

						drawSplitter->SetCurrentChannel(drawList, FL::FL::VulkanManager::maxSpriteLayers + 2);

						if (b_isActive)
						{
							if (b_isSensor)
							{
								FL::DrawCircle(center, radius, FL::GetColor("sensorActiveLight"), drawList, 2.0f);
								FL::DrawCircle(center, radius, FL::GetColor("sensorActive"), drawList);
							}
							else
							{
								FL::DrawCircle(center, radius, FL::GetColor("circleColliderActiveLight"), drawList, 2.0f);
								FL::DrawCircle(center, radius, FL::GetColor("circleColliderActive"), drawList);							
							}
						}
						else
						{
							if (b_isSensor)
							{
								FL::DrawCircle(center, radius, FL::GetColor("sensorInactiveLight"), drawList, 2.0f);
								FL::DrawCircle(center, radius, FL::GetColor("sensorInactive"), drawList);
							}
							else
							{
								FL::DrawCircle(center, radius, FL::GetColor("circleColliderInactiveLight"), drawList, 2.0f);
								FL::DrawCircle(center, radius, FL::GetColor("circleColliderInactive"), drawList);
							}
						}
					}

					for (Capsule& capsule : capsules)
					{
						bool b_isActive = body->IsActive();
						FL::Physics::BodyProps bodyProps = body->GetBodyProps();
						Shape::ShapeProps shapeProps = capsule.GetShapeProps();
						bool b_isSensor = shapeProps.b_isSensor;
						b2Capsule capsuleShape = b2Shape_GetCapsule(capsule.GetShapeID());
						float length = shapeProps.capsuleLength;
						float radius = shapeProps.radius;
						float radiusScreen = radius * FG_sceneViewGridStep.x;
						FL::Vector2 offset = shapeProps.positionOffset;
						float rotation = FL::RadiansToDegrees(b2Rot_GetAngle(shapeProps.rotationOffset));

						FL::Vector2 center1 = Scene_ConvertWorldToScreen(FL::Vector2(b2Body_GetWorldPoint(body->GetBodyID(), capsuleShape.center1)));
						FL::Vector2 center2 = Scene_ConvertWorldToScreen(FL::Vector2(b2Body_GetWorldPoint(body->GetBodyID(), capsuleShape.center2)));
						FL::Vector2 difference = center2 - center1;
						FL::Vector2 diffN = FL::Vector2::Normalize(difference);
						FL::Vector2 diffNR = diffN * radiusScreen;
						FL::Vector2 diffPerp = FL::Vector2::Rotate(diffNR, 90);
						FL::Vector2 flippedDiffPerp = FL::Vector2::Rotate(diffNR, -90);

						FL::Vector4 color;
						FL::Vector4 colorLight;

						if (b_isActive)
						{
							if (b_isSensor)
							{
								color = FL::GetColor("sensorActive");
								colorLight = FL::GetColor("sensorActiveLight");
							}
							else
							{
								color = FL::GetColor("capsuleColliderActive");
								colorLight = FL::GetColor("capsuleColliderActiveLight");
							}
						}
						else
						{
							if (b_isSensor)
							{
								color = FL::GetColor("sensorInactive");
								colorLight = FL::GetColor("sensorInactiveLight");
							}
							else
							{
								color = FL::GetColor("capsuleColliderInactive");
								colorLight = FL::GetColor("capsuleColliderInactiveLight");
							}
						}


						drawSplitter->SetCurrentChannel(drawList, FL::FL::VulkanManager::maxSpriteLayers + 2);

						FL::DrawCircle(center1, radiusScreen, colorLight, drawList, 2.0f);
						FL::DrawCircle(center1, radiusScreen, color, drawList);

						FL::DrawCircle(center2, radiusScreen, colorLight, drawList, 2.0f);
						FL::DrawCircle(center2, radiusScreen, color, drawList);

						FL::DrawLine(center1 - diffNR, center1 + diffNR, colorLight, 2.0f, drawList);
						FL::DrawLine(center2 - diffNR, center2 + diffNR, colorLight, 2.0f, drawList);
						FL::DrawLine(center1 - diffPerp, center1 + diffPerp, colorLight, 2.0f, drawList);
						FL::DrawLine(center2 - diffPerp, center2 + diffPerp, colorLight, 2.0f, drawList);

						// Sides
						FL::DrawLine(center1 + diffPerp, center1 + diffPerp + difference, colorLight, 2.0f, drawList);
						FL::DrawLine(center1 + diffPerp, center1 + diffPerp + difference, color, 1.0f, drawList);

						FL::DrawLine(center1 + flippedDiffPerp, center1 + flippedDiffPerp + difference, colorLight, 2.0f, drawList);
						FL::DrawLine(center1 + flippedDiffPerp, center1 + flippedDiffPerp + difference, color, 1.0f, drawList);
					}

					for (FL::Polygon& polygon : polygons)
					{
						b2BodyId polygonBodyID = body->GetBodyID();
						bool b_isActive = body->IsActive();
						FL::Physics::BodyProps& bodyProps = body->GetBodyProps();
						Shape::ShapeProps& shapeProps = polygon.GetShapeProps();
						bool b_isSensor = shapeProps.b_isSensor;
						bool b_isLoop = shapeProps.b_isLoop;
						std::vector<FL::Vector2>& points = shapeProps.points;
						int pointCount = (int)points.size();
						float cornerRadius = shapeProps.cornerRadius;
						int minPolygonBodyVertices = 3;
						int maxPolygonBodyVertices = 8;
						bool b_editingPoints = polygon.IsEditingPoints();

						FL::Vector4 color;
						FL::Vector4 colorLight;

						if (b_isActive)
						{
							if (b_isSensor)
							{
								color = FL::GetColor("sensorActive");
								colorLight = FL::GetColor("sensorActiveLight");
							}
							else
							{
								color = FL::GetColor("polygonColliderActive");
								colorLight = FL::GetColor("polygonColliderActiveLight");
							}
						}
						else
						{
							if (b_isSensor)
							{
								color = FL::GetColor("sensorInactive");
								colorLight = FL::GetColor("sensorInactiveLight");
							}
							else
							{
								color = FL::GetColor("polygonColliderInactive");
								colorLight = FL::GetColor("polygonColliderInactiveLight");
							}
						}

						//ImGui::GetWindowDrawList()->AddPolyline();

						for (int i = 0; i < pointCount; i++)
						{
							FL::Vector2 rPerpStart = points[i] + FL::Vector2::Rotate(FL::Vector2::Normalize(points[FL::Fmod(i + 1, pointCount)] - points[i]) * cornerRadius, -90);
							FL::Vector2 rPerpEnd = rPerpStart + (points[FL::Fmod(i + 1, pointCount)] - points[i]);
							FL::Vector2 rotatedStart = FL::Vector2::Rotate(rPerpStart, rotation);
							FL::Vector2 rotatedEnd = FL::Vector2::Rotate(rPerpEnd, rotation);
							FL::Vector2 lineStart = FL::Scene_ConvertWorldToScreen(position + rotatedStart);
							FL::Vector2 lineEnd = FL::Scene_ConvertWorldToScreen(position + rotatedEnd);

							FL::DrawLine(lineStart, lineEnd, colorLight, 2.0f, drawList);
							FL::DrawLine(lineStart, lineEnd, color, 1.0f, drawList);

							if (cornerRadius > 0)
							{
								FL::Vector2 rotatedCircleStart = FL::Vector2::Rotate(points[i], rotation);
								FL::Vector2 rotatedCircleEnd = FL::Vector2::Rotate(points[FL::Fmod(i + 1, pointCount)], rotation);
								FL::Vector2 circleStart = FL::Scene_ConvertWorldToScreen(position + rotatedStart);
								FL::Vector2 circleEnd = FL::Scene_ConvertWorldToScreen(position + rotatedEnd);

								FL::DrawCircle(circleStart, cornerRadius* FG_sceneViewGridStep.x, colorLight, drawList, 2.0f);
								FL::DrawCircle(circleStart, cornerRadius * FG_sceneViewGridStep.x, color, drawList);

								FL::DrawLine(circleStart, circleEnd, colorLight, 2.0f, drawList);
								FL::DrawLine(circleStart, circleEnd, color, 1.0f, drawList);
							}
						}

						if (b_editingPoints)
						{
							bool b_pointDeleted = false;

							for (int i = 0; i < pointCount; i++)
							{
								b_pointDeleted = RenderPointWidget(body, &polygon, points[i], i, minPolygonBodyVertices);

								if (b_pointDeleted)
								{
									break;
								}
							}

							if (!b_pointDeleted && pointCount < maxPolygonBodyVertices)
							{
								for (int i = 0; i < pointCount; i++)
								{
									FL::Vector2 midPoint = points[i] + ((points[FL::Fmod(i + 1, pointCount)] - points[i]) * 0.5f);

									if (RenderAddPointWidget(body, &polygon, midPoint, i))
									{
										break;
									}
								}
							}
						}
					}

					for (Chain& chain : chains)
					{
						FL::Physics::BodyProps& bodyProps = body->GetBodyProps();
						Shape::ShapeProps& shapeProps = chain.GetShapeProps();
						bool b_isActive = body->IsActive();
						bool b_isLoop = shapeProps.b_isLoop;
						std::vector<FL::Vector2>& points = shapeProps.points;
						int pointCount = (int)points.size();
						int minChainBodyVertices = 4;
						bool b_editingPoints = chain.IsEditingPoints();

						FL::Vector4 mainColor = FL::GetColor("chainColliderInactive");
						FL::Vector4 mainColorLight = FL::GetColor("chainColliderInactiveLight");
						FL::Vector4 endColor = FL::GetColor("chainColliderEndSegmentsInactive");

						if (b_isActive)
						{
							mainColor = FL::GetColor("chainColliderActive");
							mainColorLight = FL::GetColor("chainColliderActiveLight");
							endColor = FL::GetColor("chainColliderEndSegmentsActive");						
						}

						for (int i = 0; i < pointCount; i++)
						{
							if (i < pointCount - 1 || b_isLoop)
							{
								FL::Vector2 start = FL::Scene_ConvertWorldToScreen(position + FL::Vector2::Rotate(points[i], rotation));
								FL::Vector2 end = FL::Scene_ConvertWorldToScreen(position + FL::Vector2::Rotate(points[FL::Fmod(i + 1, pointCount)], rotation));

								if (b_isLoop || (i > 0 && i < pointCount - 2))
								{
									FL::DrawLine(start, end, mainColorLight, 2.0f, drawList);
									FL::DrawLine(start, end, mainColor, 1.0f, drawList);
								}
								else
								{
									FL::DrawLine(start, end, endColor, 1.0f, drawList);
								}
							}
						}


						if (b_editingPoints)
						{
							bool b_pointDeleted = false;

							for (int i = 0; i < pointCount; i++)
							{
								b_pointDeleted = RenderPointWidget(body, &chain, points[i], i, minChainBodyVertices);

								if (b_pointDeleted)
								{
									break;
								}
							}

							if (!b_pointDeleted)
							{
								for (int i = 0; i < pointCount; i++)
								{
									if (i != 0 && i < pointCount - 2)
									{
										FL::Vector2 midPoint = points[i] + ((points[i + 1] - points[i]) * 0.5f);

										if (RenderAddPointWidget(body, &chain, midPoint, i))
										{
											break;
										}
									}
								}
							}
						}
					}
				}

				if (tileMap != nullptr && tileMap->IsActive())
				{
					long id = tileMap->GetID();				
					float width = (float)tileMap->GetWidth();							// in tiles
					float height = (float)tileMap->GetHeight();							// in tiles
					float tileWidth = (float)tileMap->GetTileWidth();
					float tileHeight = (float)tileMap->GetTileHeight();
					float gridWidth = width * tileWidth / FL::F_pixelsPerGridSpace;		// in grid tiles
					float gridHeight = height * tileHeight / FL::F_pixelsPerGridSpace;	// in grid tiles
					int renderOrder = tileMap->GetRenderOrder();
					std::map<int, std::map<int, FL::Tile>> tiles = tileMap->GetTiles();

					static std::vector<FL::Vector2> hoveredTiles = std::vector<FL::Vector2>();
					static std::vector<FL::Vector2> selectedTiles = std::vector<FL::Vector2>();
						
					// For Drawing TileMap border and background color
					float renderXStart = FG_sceneViewCenter.x + ((position.x - (gridWidth * scale.x / 2)) * FG_sceneViewGridStep.x);
					float renderYStart = FG_sceneViewCenter.y - ((position.y + (gridHeight * scale.y / 2)) * FG_sceneViewGridStep.x);
					FL::Vector2 renderStart = FL::Vector2(renderXStart, renderYStart);
					FL::Vector2 renderEnd = FL::Vector2(renderXStart + ((gridWidth * scale.x) * FG_sceneViewGridStep.x), renderYStart + ((gridHeight * scale.y) * FG_sceneViewGridStep.x));
					FL::Vector2 focusObjectButtonSize = FL::Vector2(renderEnd.x - renderStart.x, renderEnd.y - renderStart.y);

					float tileWidthInPx = FG_sceneViewGridStep.x * (tileWidth / FL::F_pixelsPerGridSpace);
					float tileHeightInPx = FG_sceneViewGridStep.x * (tileHeight / FL::F_pixelsPerGridSpace);
					FL::Vector2 tileSize = FL::Vector2(tileWidthInPx, tileHeightInPx);

					// For selecting multiple tiles
					static FL::Vector2 multiSelectStartTile = FL::Vector2(-1, -1);
					static FL::Vector2 multiSelectEndTile = FL::Vector2(-1, -1);
					static FL::Vector2 multiSelectCurrentHoveredTile = FL::Vector2(-1, -1);
					static FL::Vector2 savedMultiSelectStartTile = FL::Vector2(-1, -1);

					// For Moving MultiSelected Tiles
					static FL::Vector2 moveStartTile = FL::Vector2(-1, -1);
					static FL::Vector2 moveEndTile = FL::Vector2(-1, -1);

					// For selecting Collision Area coordinates
					static std::vector<std::pair<FL::Vector2, FL::Vector2>> selectedCollisionCoords = std::vector<std::pair<FL::Vector2, FL::Vector2>>();
					static FL::Vector2 colAreaStartTile = FL::Vector2(-1, -1);
					static FL::Vector2 colAreaEndTile = FL::Vector2(-1, -1);

					// "Focus on this TileMap GameObject" button
					if (focusedObjectID != self.GetID() && (FL::F_CursorMode == FL::F_CURSOR_MODE::TRANSLATE || FL::F_CursorMode == FL::F_CURSOR_MODE::SCALE || FL::F_CursorMode == FL::F_CURSOR_MODE::ROTATE))
					{
						if (focusObjectButtonSize.x <= 0 || focusObjectButtonSize.y <= 0)
						{
							focusObjectButtonSize = FL::Vector2(1, 1);
						}
						std::string focusObjectButtonID = "##SelectThisTileMapObjectButton" + std::to_string(self.GetID()) + "-" + std::to_string(id);
						AddSceneViewMouseControls(focusObjectButtonID, renderStart, focusObjectButtonSize, FG_sceneViewScrolling, FG_sceneViewCenter, FG_sceneViewGridStep, FL::GetColor32("transparent"), false, 0, true);
						if (ImGui::IsItemClicked())
						{						
							SetFocusedGameObjectID(self.GetID());
						}
					}

					drawSplitter->SetCurrentChannel(drawList, 0);

					// TileMap background color and border
					if (focusedObjectID == self.GetID())
					{
						ImGui::GetWindowDrawList()->AddRectFilled(renderStart, renderEnd, FL::GetColor32("tileMapGridBgFocused"));
						FL::DrawRectangle(renderStart, renderEnd, canvasP0, canvasSize, FL::GetColor("tileMapBoxFocused"), 2.0f, drawList);
					}
					else
					{
						ImGui::GetWindowDrawList()->AddRectFilled(renderStart, renderEnd, FL::GetColor32("tileMapGridBgUnfocused"));
						FL::DrawRectangle(renderStart, renderEnd, canvasP0, canvasSize, FL::GetColor("tileMapBoxUnfocused"), 2.0f, drawList);
					}

					if (focusedObjectID == self.GetID())
					{
						// Handle Tiles
						for (float w = 0; w < width; w++)
						{
							for (float h = 0; h < height; h++)
							{
								// TileMap interactions							
								std::string tileButtonID = "##tileMapIndexButton" + std::to_string(id) + "-" + std::to_string(w) + std::to_string(h);
								TileSet* activeTileSet = nullptr;

								// Get active TileSet for texture dimensions
								std::string activeTileSetName = FL::F_tileSetAndIndexOnBrush.first;
								if (activeTileSetName != "")
								{
									activeTileSet = FL::GetTileSet(activeTileSetName);
								}

								// tileStart = viewport center + top left corner in pixel screen space + tile offset
								float tileStartX = FG_sceneViewCenter.x + ((position.x - (gridWidth * scale.x / 2)) * FG_sceneViewGridStep.x) + (w * tileWidthInPx);
								float tileStartY = FG_sceneViewCenter.y - ((position.y + (gridHeight * scale.y / 2)) * FG_sceneViewGridStep.x) + (h * tileHeightInPx);

								FL::Vector2 tileStart = FL::Vector2(tileStartX, tileStartY);
								FL::Vector2 tileEnd = FL::Vector2(tileStartX + tileWidthInPx, tileStartY + tileHeightInPx);

								// Catch interactions on the TileMap container
								if ((focusedObjectID == self.GetID()) &&
									((FL::F_CursorMode == FL::F_CURSOR_MODE::TILE_BRUSH && activeTileSet != nullptr && FL::F_tileSetAndIndexOnBrush.second != -1) ||
									(FL::F_CursorMode == FL::F_CURSOR_MODE::TILE_ERASE) ||
									(FL::F_CursorMode == FL::F_CURSOR_MODE::TILE_COLLIDER_DRAW && tileMap->GetSelectedCollisionArea() != "")) ||
									(FL::F_CursorMode == FL::F_CURSOR_MODE::TILE_MULTISELECT) ||
									(FL::F_CursorMode == FL::F_CURSOR_MODE::TILE_MOVE))
								{
									// Set Draw Channel to 2 for lower level UI
									drawSplitter->SetCurrentChannel(drawList, FL::FL::VulkanManager::maxSpriteLayers + 2);

									AddSceneViewMouseControls(tileButtonID, tileStart, tileSize, FG_sceneViewScrolling, FG_sceneViewCenter, FG_sceneViewGridStep, FL::GetColor32("tileMapGridLines"));
									// _RectOnly flag enables the buttons to work when dragging the mouse over them in a clicked state // https://github.com/ocornut/imgui/commit/564ff2dfd379d40568879a5bc89e8cfea7e51d2f
									const bool b_isHovered = ImGui::IsItemHovered(ImGuiHoveredFlags_RectOnly);



									// Collect all boxes between multiSelectStartTile tile and multiSelectCurrentlyHovered tile
									if (focusedObjectID == self.GetID() &&
										multiSelectStartTile.x != -1 &&
										multiSelectStartTile.y != -1 &&
										(FL::F_CursorMode == FL::F_CURSOR_MODE::TILE_COLLIDER_DRAW || FL::F_CursorMode == FL::F_CURSOR_MODE::TILE_MULTISELECT) &&
										ImGui::IsKeyDown(ImGuiKey_MouseLeft) &&
										(multiSelectStartTile.x <= w && multiSelectCurrentHoveredTile.x >= w || multiSelectCurrentHoveredTile.x <= w && multiSelectStartTile.x >= w) &&
										(multiSelectStartTile.y <= h && multiSelectCurrentHoveredTile.y >= h || multiSelectCurrentHoveredTile.y <= h && multiSelectStartTile.y >= h))
									{
										if (FL::F_CursorMode == FL::F_CURSOR_MODE::TILE_COLLIDER_DRAW)
										{
											ImGui::GetWindowDrawList()->AddRectFilled(tileStart, tileEnd, FL::GetColor32("tileBoxColHighlight"));
										}
										else if (FL::F_CursorMode == FL::F_CURSOR_MODE::TILE_MULTISELECT)
										{
											ImGui::GetWindowDrawList()->AddRectFilled(tileStart, tileEnd, FL::GetColor32("tileMultiSelectHighlightDragging"));

											bool b_containsTile = false;
											for (FL::Vector2 tile : hoveredTiles)
											{
												if (tile.x == w && tile.y == h)
													b_containsTile = true;
											}
											if (!b_containsTile)
											{
												hoveredTiles.push_back(FL::Vector2(w, h));
											}
										}
									}

									if (b_isHovered)
									{
										// Mouse down
										if (ImGui::IsKeyDown(ImGuiKey_MouseLeft))
										{
											// Record the tile that is currently under the clicked mouse
											multiSelectCurrentHoveredTile = FL::Vector2(w, h);

											if (activeTileSet != nullptr && FL::F_CursorMode == FL::F_CURSOR_MODE::TILE_BRUSH)
											{
												ImGui::GetWindowDrawList()->AddRectFilled(tileStart, tileEnd, FL::GetColor32("tileSetHoldingTile"));
												tileMap->SetTile(FL::Vector2(w, h), activeTileSet, FL::F_tileSetAndIndexOnBrush.second);
											}
											else if (FL::F_CursorMode == FL::F_CURSOR_MODE::TILE_ERASE)
											{
												ImGui::GetWindowDrawList()->AddRectFilled(tileStart, tileEnd, FL::GetColor32("tileSetEraseModeClick"));
												ImGui::GetWindowDrawList()->AddRect(tileStart, tileEnd, FL::GetColor32("tileSetEraseModeClickBorder"));
												tileMap->EraseTile(FL::Vector2(w, h));
											}
											else if (FL::F_CursorMode == FL::F_CURSOR_MODE::TILE_COLLIDER_DRAW)
											{
												ImGui::GetWindowDrawList()->AddRectFilled(tileStart, tileEnd, FL::GetColor32("tileBoxColHighlight"));
											}
											else if (FL::F_CursorMode == FL::F_CURSOR_MODE::TILE_MULTISELECT)
											{
												ImGui::GetWindowDrawList()->AddRect(tileStart, tileEnd, FL::GetColor32("tileSelectModeClickBorder"));
											}
											else if (FL::F_CursorMode == FL::F_CURSOR_MODE::TILE_MOVE)
											{
												ImGui::GetWindowDrawList()->AddRect(tileStart, tileEnd, FL::GetColor32("tileMoveModeClickBorder"));
											}
										}
										// Mouse not down
										else
										{
											if (FL::F_CursorMode == FL::F_CURSOR_MODE::TILE_BRUSH)
											{
												ImGui::GetWindowDrawList()->AddRectFilled(tileStart, tileEnd, FL::GetColor32("tileSetHoveredTile"));
												ImGui::GetWindowDrawList()->AddRect(tileStart, tileEnd, FL::GetColor32("tileSetHoveredTileBorder"));
											}
											else if (FL::F_CursorMode == FL::F_CURSOR_MODE::TILE_ERASE)
											{
												ImGui::GetWindowDrawList()->AddRectFilled(tileStart, tileEnd, FL::GetColor32("tileSetEraseModeHover"));
												ImGui::GetWindowDrawList()->AddRect(tileStart, tileEnd, FL::GetColor32("tileSetEraseModeHoverBorder"));
											}
											else if (FL::F_CursorMode == FL::F_CURSOR_MODE::TILE_COLLIDER_DRAW)
											{
												ImGui::GetWindowDrawList()->AddRectFilled(tileStart, tileEnd, FL::GetColor32("tileBoxColHoveredHighlight"));
											}
											else if (FL::F_CursorMode == FL::F_CURSOR_MODE::TILE_MULTISELECT)
											{
												ImGui::GetWindowDrawList()->AddRectFilled(tileStart, tileEnd, FL::GetColor32("tileSetHoveredTile"));
												ImGui::GetWindowDrawList()->AddRect(tileStart, tileEnd, FL::GetColor32("tileSetHoveredTileBorder"));
											}
										}

										// Click pressed this frame
										if (ImGui::IsKeyPressed(ImGuiKey_MouseLeft, false))
										{
											if (FL::F_CursorMode != FL::F_CURSOR_MODE::TILE_MOVE)
											{
												// Record the tile that was clicked on	
												multiSelectStartTile = FL::Vector2(w, h);
												savedMultiSelectStartTile = FL::Vector2(w, h);
											}

											if (FL::F_CursorMode == FL::F_CURSOR_MODE::TILE_COLLIDER_DRAW)
											{
												colAreaStartTile = FL::Vector2(w, h);
											}
											else if (FL::F_CursorMode == FL::F_CURSOR_MODE::TILE_MULTISELECT)
											{
												hoveredTiles.clear();
												selectedTiles.clear();
											}
											else if (FL::F_CursorMode == FL::F_CURSOR_MODE::TILE_MOVE)
											{
												moveStartTile = FL::Vector2(w, h);
											}
										}

										// Click released this frame
										if (ImGui::IsKeyReleased(ImGuiKey_MouseLeft))
										{
											if (FL::F_CursorMode != FL::F_CURSOR_MODE::TILE_MOVE)
											{
												// Record the tile that the mouse released on
												multiSelectEndTile = FL::Vector2(w, h);
											}


											if (FL::F_CursorMode == FL::F_CURSOR_MODE::TILE_COLLIDER_DRAW)
											{
												colAreaEndTile = FL::Vector2(w, h);

												if (colAreaStartTile.x != -1 && colAreaStartTile.y != -1)
												{
													std::pair<FL::Vector2, FL::Vector2> newPair = { colAreaStartTile, colAreaEndTile };
													bool b_alreadyContains = false;

													for (std::vector<std::pair<FL::Vector2, FL::Vector2>>::iterator coordPair = FG_collisionAreasBuffer.begin(); coordPair != FG_collisionAreasBuffer.end(); coordPair++)
													{
														if ((coordPair->first == colAreaStartTile && coordPair->second == colAreaEndTile) || (coordPair->first == colAreaEndTile && coordPair->second == colAreaStartTile))
														{
															FG_collisionAreasBuffer.erase(coordPair);
															b_alreadyContains = true;
															break;
														}
													}

													if (!b_alreadyContains)
													{
														FG_collisionAreasBuffer.push_back(newPair);
													}
												}

												colAreaStartTile = FL::Vector2(-1, -1);
												colAreaEndTile = FL::Vector2(-1, -1);
											}
											else if (FL::F_CursorMode == FL::F_CURSOR_MODE::TILE_MULTISELECT)
											{
												selectedTiles = hoveredTiles;
												hoveredTiles.clear();
											}
											else if (FL::F_CursorMode == FL::F_CURSOR_MODE::TILE_MOVE)
											{
												moveEndTile = FL::Vector2(w, h);

												if (moveStartTile.x != -1 && moveStartTile.y != -1)
												{
													float xMove = moveEndTile.x - moveStartTile.x;
													float yMove = moveEndTile.y - moveStartTile.y;

													tileMap->MoveTiles(selectedTiles, FL::Vector2(xMove, yMove));
												}

												moveStartTile = FL::Vector2(-1, -1);
												moveEndTile = FL::Vector2(-1, -1);
											}

											if (FL::F_CursorMode != FL::F_CURSOR_MODE::TILE_MOVE)
											{
												// Reset the starting tile
												multiSelectStartTile = FL::Vector2(-1, -1);
											}
										}
									}
								}
							}
						}

						// Draw box around selected multiselect tiles
						if ((selectedTiles.size() > 0) && (FL::F_CursorMode == FL::F_CURSOR_MODE::TILE_MULTISELECT || FL::F_CursorMode == FL::F_CURSOR_MODE::TILE_MOVE))
						{
							float startPosX = FG_sceneViewCenter.x + ((position.x - (gridWidth * scale.x / 2)) * FG_sceneViewGridStep.x) + (savedMultiSelectStartTile.x * tileWidthInPx);
							float startPosY = FG_sceneViewCenter.y - ((position.y + (gridHeight * scale.y / 2)) * FG_sceneViewGridStep.y) + (savedMultiSelectStartTile.y * tileHeightInPx);
							float selectWidth = multiSelectEndTile.x - savedMultiSelectStartTile.x;
							if (selectWidth < 0)
							{
								selectWidth *= -1;
							}
							selectWidth += 1;
							float selectHeight = multiSelectEndTile.y - savedMultiSelectStartTile.y;
							if (selectHeight < 0)
							{
								selectHeight *= -1;
							}
							selectHeight += 1;

							FL::Vector2 startTileScreenPos = FL::Vector2(startPosX, startPosY);
							FL::Vector2 endTileScreenPos = FL::Vector2(startPosX + (tileWidthInPx * selectWidth), startPosY + (tileHeightInPx * selectHeight));

							ImGui::GetWindowDrawList()->AddRectFilled(startTileScreenPos, endTileScreenPos, FL::GetColor32("tileMultiSelectHighlight"));
							ImGui::GetWindowDrawList()->AddRect(startTileScreenPos, endTileScreenPos, FL::GetColor32("tileMultiSelectHighlightBorder"));

							if (FL::F_CursorMode == FL::F_CURSOR_MODE::TILE_MOVE)
							{
								FL::RenderInvisibleButton("##MultiSelectDraggableBox", startTileScreenPos, FL::Vector2(endTileScreenPos.x - startTileScreenPos.x, endTileScreenPos.y - startTileScreenPos.y));
								if (ImGui::IsItemHovered())
								{
									ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
								}
							}
						}
					}

					// Draw box around each of the collision areas in the buffer
					if (focusedObjectID == self.GetID() && FL::F_CursorMode == FL::F_CURSOR_MODE::TILE_COLLIDER_DRAW)
					{
						for (std::pair<FL::Vector2, FL::Vector2> collAreaBuffer : FG_collisionAreasBuffer)
						{
							FL::Vector2 startCoord = collAreaBuffer.first;
							FL::Vector2 endCoord = collAreaBuffer.second;

							float startPosX = FG_sceneViewCenter.x + ((position.x - (gridWidth * scale.x / 2)) * FG_sceneViewGridStep.x) + (startCoord.x * tileWidthInPx);
							float startPosY = FG_sceneViewCenter.y - ((position.y + (gridHeight * scale.y / 2)) * FG_sceneViewGridStep.y) + (startCoord.y * tileHeightInPx);
							float selectWidth = endCoord.x - startCoord.x;
							if (selectWidth < 0)
							{
								selectWidth *= -1;
							}
							selectWidth += 1;
							float selectHeight = endCoord.y - startCoord.y;
							if (selectHeight < 0)
							{
								selectHeight *= -1;
							}
							selectHeight += 1;

							FL::Vector2 startTileScreenPos = FL::Vector2(startPosX, startPosY);
							FL::Vector2 endTileScreenPos = FL::Vector2(startPosX + (tileWidthInPx * selectWidth), startPosY + (tileHeightInPx * selectHeight));

							ImGui::GetWindowDrawList()->AddRectFilled(startTileScreenPos, endTileScreenPos, FL::GetColor32("tileMultiSelectHighlight"));
							ImGui::GetWindowDrawList()->AddRect(startTileScreenPos, endTileScreenPos, FL::GetColor32("tileMultiSelectHighlightBorder"));

							if (FL::F_CursorMode == FL::F_CURSOR_MODE::TILE_MOVE)
							{
								FL::RenderInvisibleButton("##MultiSelectDraggableBox", startTileScreenPos, FL::Vector2(endTileScreenPos.x - startTileScreenPos.x, endTileScreenPos.y - startTileScreenPos.y));
								if (ImGui::IsItemHovered())
								{
									ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
								}
							}
						}
					}

					// Draw TileMap indice Textures
					for (std::pair<int, std::map<int, FL::Tile>> xPair : tiles)
					{
						float x = (float)xPair.first;
							
						if (x <= width)
						{
							for (std::pair<int, FL::Tile> yPair : xPair.second)
							{
								float y = (float)yPair.first;

								if (y <= height)
								{
									FL::Tile tile = yPair.second;
									TileSet* usedTileSet = nullptr;
									std::string tileSetName = tile.tileSetName;

									if (tileSetName != "")
									{
										usedTileSet = FL::GetTileSet(tileSetName);
									}

									// this many grid spaces fit into a single tiles width (if tileWidth is 16: 16 / 8 is 2 grid spaces in for a single tile 
									float gridWidthsInATile = tileWidth / FL::F_pixelsPerGridSpace;
									float gridHeightsInATile = tileHeight / FL::F_pixelsPerGridSpace;

									VkDescriptorSet texture = tile.tileSetTexture;
									float textureWidth = (float)usedTileSet->GetTexture()->GetWidth();
									float textureHeight = (float)usedTileSet->GetTexture()->GetHeight();
									FL::Vector2 uvStart = FL::Vector2(tile.uvStart.x / textureWidth, tile.uvStart.y / textureHeight);
									FL::Vector2 uvEnd = FL::Vector2(tile.uvEnd.x / textureWidth, tile.uvEnd.y / textureHeight);
									float gridXPosition = (position.x - (gridWidth / 2)) + gridWidthsInATile * x;
									float gridYPosition = (position.y + (gridHeight / 2)) - gridHeightsInATile * y;
									FL::Vector2 tilePosition = FL::Vector2(gridXPosition, gridYPosition);


									// Change the draw channel for the scene object
									if (renderOrder <= FL::FL::VulkanManager::maxSpriteLayers && renderOrder >= 0)
									{
										drawSplitter->SetCurrentChannel(drawList, renderOrder);
									}
									else
									{
										drawSplitter->SetCurrentChannel(drawList, 0);
									}

									FL::AddImageToDrawList(texture, tilePosition, FG_sceneViewCenter, tileWidth, tileHeight, FL::Vector2(0, 0), scale, true, FG_sceneViewGridStep.x, drawList, 0, FL::GetColor32("white"), uvStart, uvEnd);
								}
							}
						}
					}
				}
			}

			*/
		}
		
		void RenderViewObjects(std::map<long, FL::GameObject>& objects, FL::Vector2 centerPoint, FL::Vector2 canvasP0, FL::Vector2 canvasSize, float gridStep)
		{
			// Split our drawlist into multiple channels for different rendering orders
			ImDrawList* drawList = ImGui::GetWindowDrawList();
			ImDrawListSplitter* drawSplitter = new ImDrawListSplitter();

			// 4 channels for now in this scene view. 0 = scene objects, 1 & 2 = other UI (camera icon, etc), 4 = transform arrow
			drawSplitter->Split(drawList, FL::VulkanManager::maxSpriteLayers + 5);
			

			for (std::pair<long, FL::GameObject> object : objects)
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

		// bool RenderAddPointWidget(Body* body, Shape* shape, FL::Vector2 midPoint, int startIndex)
		// {
		// 	bool b_pointAdded = false;
		// 	FL::Vector2 position = body->GetPosition();
		// 	float rotation = body->GetRotation();		

		// 	std::vector<FL::Vector2> points = shape->GetShapeProps().points;			
		// 	VkDescriptorSet chainAddJointTexture = FL::GetTexture("addJoint");
		// 	float textureWidth = (float)FL::GetTextureObject("addJoint")->GetWidth();
		// 	float textureHeight = (float)FL::GetTextureObject("addJoint")->GetHeight();
		// 	FL::Vector2 jointOffset = { textureWidth / 2, textureHeight / 2 };
		// 	FL::Vector2 adjustedPoint = position + FL::Vector2::Rotate(midPoint, rotation);
		// 	ImGuiIO& inputOutput = ImGui::GetIO();
		// 	FL::Vector2 jointScreenPos = FL::Scene_ConvertWorldToScreen(adjustedPoint);		
		// 	jointScreenPos = jointScreenPos - jointOffset;

		// 	FL::RenderInvisibleButton("##bodyAddJoint_" + std::to_string(body->GetID()) + "_" + std::to_string(startIndex), jointScreenPos, FL::Vector2(textureWidth, textureHeight), false);
		// 	const bool b_jointHovered = ImGui::IsItemHovered();
		// 	const bool b_jointActive = ImGui::IsItemActive();
		// 	const bool b_jointClicked = ImGui::IsItemClicked();
		// 	if (b_jointHovered || b_jointActive)
		// 	{
		// 		chainAddJointTexture = FL::GetTexture("addJointHovered");
		// 		ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);			
		// 		FL::RenderTextToolTip("Add new joint");
		// 	}

		// 	if (b_jointClicked)
		// 	{
		// 		std::vector<FL::Vector2> newPoints;
				
		// 		for (int i = 0; i < points.size(); i++)
		// 		{
		// 			newPoints.push_back(points[i]);

		// 			if (i == startIndex)
		// 			{
		// 				newPoints.push_back(midPoint);
		// 			}
		// 		}			

		// 		shape->SetPoints(newPoints);
		// 		b_pointAdded = true;
		// 	}

		// 	FL::Vector2 jointScale = { 1, 1 };
		// 	bool b_scalesWithZoom = false;

		// 	FL::AddImageToDrawList(chainAddJointTexture, adjustedPoint, FG_sceneViewCenter, textureWidth, textureHeight, jointOffset, jointScale, b_scalesWithZoom, FG_sceneViewGridStep.x, ImGui::GetWindowDrawList());

		// 	return b_pointAdded;
		// }

		// bool RenderPointWidget(Body* body, Shape* shape, FL::Vector2& point, int index, int minShapeVerticies)
		// {
		// 	bool b_pointDeleted = false;
		// 	float rotation = body->GetRotation();
		// 	FL::Vector2 position = body->GetPosition();

		// 	std::vector<FL::Vector2> points = shape->GetShapeProps().points;		
		// 	VkDescriptorSet chainJointTexture = FL::GetTexture("joint");		
		// 	float textureWidth = (float)FL::GetTextureObject("joint")->GetWidth();
		// 	float textureHeight = (float)FL::GetTextureObject("joint")->GetHeight();
		// 	FL::Vector2 jointOffset = { textureWidth / 2, textureHeight / 2 };
		// 	FL::Vector2 adjustedPoint = position + FL::Vector2::Rotate(point, rotation);		
		// 	FL::Vector2 jointScreenPos = FL::Scene_ConvertWorldToScreen(adjustedPoint);
		// 	jointScreenPos = jointScreenPos - jointOffset;		
		// 	std::string invisibleButtonID = "##bodyJoint_" + std::to_string(body->GetID()) + "_" + std::to_string(index);

		// 	FL::RenderInvisibleButton(invisibleButtonID, jointScreenPos, FL::Vector2(textureWidth, textureHeight), false, false, ImGuiButtonFlags_MouseButtonRight | ImGuiButtonFlags_MouseButtonLeft);
		// 	const bool b_jointHovered = ImGui::IsItemHovered();
		// 	const bool b_jointActive = ImGui::IsItemActive();
		// 	const bool b_jointRightClicked = ImGui::IsItemClicked(ImGuiMouseButton_Right);		

		// 	if (points.size() > minShapeVerticies && ImGui::BeginPopupContextItem(invisibleButtonID.c_str(), ImGuiPopupFlags_MouseButtonRight))
		// 	{
		// 		FL::PushMenuStyles();

		// 		if (ImGui::MenuItem("Delete"))
		// 		{
		// 			std::vector<FL::Vector2> newPoints;

		// 			for (int i = 0; i < points.size(); i++)
		// 			{						
		// 				if (i != index)
		// 				{
		// 					newPoints.push_back(points[i]);
		// 				}
		// 			}

		// 			shape->SetPoints(newPoints);					
		// 			b_pointDeleted = true;
		// 		}

		// 		FL::PopMenuStyles();

		// 		ImGui::EndPopup();
		// 	}		

		// 	if (b_jointHovered || b_jointActive)
		// 	{			
		// 		chainJointTexture = FL::GetTexture("jointHovered");
		// 		ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
		// 		FL::RenderTextToolTip("Point index: " + std::to_string(index));
		// 	}		

		// 	if (b_jointActive && ImGui::IsMouseDragging(ImGuiMouseButton_Left))
		// 	{
		// 		FL::Vector2 mousePosInGrid = FL::Scene_GetMousePosWorld();
		// 		point = FL::Vector2(b2Body_GetLocalPoint(body->GetBodyID(), b2Vec2(mousePosInGrid.x, mousePosInGrid.y)));			
		// 		shape->UpdatePoints();
		// 	}
			
		// 	FL::Vector2 jointScale = { 1, 1 };
		// 	bool b_scalesWithZoom = false;

		// 	FL::AddImageToDrawList(chainJointTexture, adjustedPoint, FG_sceneViewCenter, textureWidth, textureHeight, jointOffset, jointScale, b_scalesWithZoom, FG_sceneViewGridStep.x, ImGui::GetWindowDrawList());

		// 	return b_pointDeleted;
		// }

		// void RenderTransformArrowWidget()
		// {
		// 	GameObject* focusedObject = FL::GetObjectByID(FG_FocusedGameObjectID);

		// 	// Renders Transform Arrow widget
		// 	if (FL::F_CursorMode == FL::F_CURSOR_MODE::TRANSLATE && focusedObject != nullptr)
		// 	{	
		// 		Transform* transform = focusedObject->GetTransform();
		// 		FL::Vector2 position = FL::Vector2(0, 0);
		// 		Body* body = focusedObject->GetBody();

		// 		if (transform != nullptr)
		// 		{
		// 			position = transform->GetAbsolutePosition();
		// 		}

		// 		if (body != nullptr)
		// 		{
		// 			position = body->GetPosition();
		// 		}

		// 		VkDescriptorSet arrowToRender = FL::GetTexture("transformArrow");
		// 		// * 3 because the texture is so small. If we change the scale, it will change the render starting position. We only want to change the render ending position so we adjust dimensions only
		// 		float arrowWidth = (float)FL::GetTextureObject("transformArrow")->GetWidth() * 3;
		// 		float arrowHeight = (float)FL::GetTextureObject("transformArrow")->GetHeight() * 3;
		// 		FL::Vector2 arrowScale = { 1, 1 };
		// 		FL::Vector2 arrowOffset = { 3, arrowHeight - 3 };
		// 		bool b_scalesWithZoom = false;
		// 		float transformMoveModifier = 0.02f;
		// 		ImGuiIO& inputOutput = ImGui::GetIO();
		// 		FL::Vector2 positionOnScreen = FL::Vector2(FG_sceneViewCenter.x + (position.x * FG_sceneViewGridStep.x), FG_sceneViewCenter.y - (position.y * FG_sceneViewGridStep.x));

		// 		// Invisible button for Transform Arrow Move X and Y
		// 		FL::Vector2 moveAllStartPos = FL::Vector2(positionOnScreen.x - 4, positionOnScreen.y - 23);
		// 		FL::RenderInvisibleButton("##TransformBaseArrowButton", moveAllStartPos, FL::Vector2(28, 28), false);
		// 		const bool b_baseHovered = ImGui::IsItemHovered();
		// 		const bool b_baseActive = ImGui::IsItemActive();
		// 		const bool b_baseClicked = ImGui::IsItemClicked();

		// 		if (b_baseHovered || b_baseActive)
		// 		{
		// 			arrowToRender = FL::GetTexture("transformArrowAllWhite");
		// 			ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
		// 		}

		// 		// Invisible button for X arrow
		// 		FL::Vector2 moveXStartPos = FL::Vector2(positionOnScreen.x + 24, positionOnScreen.y - 30);
		// 		FL::RenderInvisibleButton("##TransformBaseArrowXButton", moveXStartPos, FL::Vector2(63, 35), false);
		// 		const bool b_xHovered = ImGui::IsItemHovered();
		// 		const bool b_xActive = ImGui::IsItemActive();
		// 		const bool b_xClicked = ImGui::IsItemClicked();

		// 		if (b_xHovered || b_xActive)
		// 		{
		// 			arrowToRender = FL::GetTexture("transformArrowXWhite");
		// 			ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
		// 		}

		// 		// Invisible button for Y arrow
		// 		FL::Vector2 moveYStartPos = FL::Vector2(positionOnScreen.x - 4, positionOnScreen.y - 86);
		// 		FL::RenderInvisibleButton("TransformBaseArrowYButton", moveYStartPos, FL::Vector2(35, 63), false);
		// 		const bool b_yHovered = ImGui::IsItemHovered();
		// 		const bool b_yActive = ImGui::IsItemActive();
		// 		const bool b_yClicked = ImGui::IsItemClicked();

		// 		if (b_yHovered || b_yActive)
		// 		{
		// 			arrowToRender = FL::GetTexture("transformArrowYWhite");
		// 			ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
		// 		}

		// 		static FL::Vector2 transformOffsetFromMouse;
				
		// 		if (b_baseClicked || b_xClicked || b_yClicked)
		// 		{
		// 			transformOffsetFromMouse = position - FL::Scene_GetMousePosWorld();
		// 		}

		// 		//if (b_baseActive && ImGui::IsMouseDragging(ImGuiMouseButton_Left))
		// 		//{			
		// 		//	FL::Vector2 newPosition = FL::Scene_GetMousePosWorld() + transformOffsetFromMouse;
		// 		//	transform->SetPosition(newPosition - transform->GetPositionOrigin());
		// 		//}
		// 		//else if (b_xActive && ImGui::IsMouseDragging(ImGuiMouseButton_Left))
		// 		//{
		// 		//	FL::Vector2 newPosition = FL::Scene_GetMousePosWorld() + transformOffsetFromMouse;
		// 		//	transform->SetPosition(FL::Vector2(newPosition.x - transform->GetPositionOrigin().x, position.y - transform->GetPositionOrigin().x));
		// 		//}
		// 		//else if (b_yActive && ImGui::IsMouseDragging(ImGuiMouseButton_Left))
		// 		//{
		// 		//	FL::Vector2 newPosition = FL::Scene_GetMousePosWorld() + transformOffsetFromMouse;
		// 		//	transform->SetPosition(FL::Vector2(position.x - transform->GetPositionOrigin().x, newPosition.y - transform->GetPositionOrigin().y));
		// 		//}


		// 		// Draw channel maxSpriteLayers + 3 for Upper UI Transform Arrow			
		// 		FL::AddImageToDrawList(arrowToRender, position, FG_sceneViewCenter, arrowWidth, arrowHeight, arrowOffset, arrowScale, b_scalesWithZoom, FG_sceneViewGridStep.x, ImGui::GetWindowDrawList());
		// 	}
		// }

		// void PositionTransformWidget()
		// {
		// 	GameObject* focusedObject = FL::GetObjectByID(FG_FocusedGameObjectID);

		// 	// Renders Transform Arrow widget
		// 	if (FL::F_CursorMode == FL::F_CURSOR_MODE::TRANSLATE && focusedObject != nullptr)
		// 	{	
		// 		Transform* transform = focusedObject->GetTransform();
		// 		Vector3 position = Vector3(0);
		// 		Body* body = focusedObject->GetBody();

		// 		if (transform != nullptr)
		// 		{
		// 			position = transform->GetAbsolutePosition();
		// 		}

		// 		if (body != nullptr)
		// 		{
		// 			position = body->GetPosition();
		// 		}

		// 		VkDescriptorSet arrowToRender = FL::GetTexture("transformArrow");
		// 		// * 3 because the texture is so small. If we change the scale, it will change the render starting position. We only want to change the render ending position so we adjust dimensions only
		// 		float arrowWidth = (float)FL::GetTextureObject("transformArrow")->GetWidth() * 3;
		// 		float arrowHeight = (float)FL::GetTextureObject("transformArrow")->GetHeight() * 3;
		// 		FL::Vector2 arrowScale = { 1, 1 };
		// 		FL::Vector2 arrowOffset = { 3, arrowHeight - 3 };
		// 		bool b_scalesWithZoom = false;
		// 		float transformMoveModifier = 0.02f;
		// 		ImGuiIO& inputOutput = ImGui::GetIO();
		// 		FL::Vector2 positionOnScreen = FL::Vector2(FG_sceneViewCenter.x + (position.x * FG_sceneViewGridStep.x), FG_sceneViewCenter.y - (position.y * FG_sceneViewGridStep.x));

		// 		// Invisible button for Transform Arrow Move X and Y
		// 		FL::Vector2 moveAllStartPos = FL::Vector2(positionOnScreen.x - 4, positionOnScreen.y - 23);
		// 		FL::RenderInvisibleButton("##TransformBaseArrowButton", moveAllStartPos, FL::Vector2(28, 28), false);
		// 		const bool b_baseHovered = ImGui::IsItemHovered();
		// 		const bool b_baseActive = ImGui::IsItemActive();
		// 		const bool b_baseClicked = ImGui::IsItemClicked();

		// 		if (b_baseHovered || b_baseActive)
		// 		{
		// 			arrowToRender = FL::GetTexture("transformArrowAllWhite");
		// 			ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
		// 		}

		// 		// Invisible button for X arrow
		// 		FL::Vector2 moveXStartPos = FL::Vector2(positionOnScreen.x + 24, positionOnScreen.y - 30);
		// 		FL::RenderInvisibleButton("##TransformBaseArrowXButton", moveXStartPos, FL::Vector2(63, 35), false);
		// 		const bool b_xHovered = ImGui::IsItemHovered();
		// 		const bool b_xActive = ImGui::IsItemActive();
		// 		const bool b_xClicked = ImGui::IsItemClicked();

		// 		if (b_xHovered || b_xActive)
		// 		{
		// 			arrowToRender = FL::GetTexture("transformArrowXWhite");
		// 			ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
		// 		}

		// 		// Invisible button for Y arrow
		// 		FL::Vector2 moveYStartPos = FL::Vector2(positionOnScreen.x - 4, positionOnScreen.y - 86);
		// 		FL::RenderInvisibleButton("TransformBaseArrowYButton", moveYStartPos, FL::Vector2(35, 63), false);
		// 		const bool b_yHovered = ImGui::IsItemHovered();
		// 		const bool b_yActive = ImGui::IsItemActive();
		// 		const bool b_yClicked = ImGui::IsItemClicked();

		// 		if (b_yHovered || b_yActive)
		// 		{
		// 			arrowToRender = FL::GetTexture("transformArrowYWhite");
		// 			ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
		// 		}

		// 		static FL::Vector2 transformOffsetFromMouse;
				
		// 		// if (b_baseClicked || b_xClicked || b_yClicked)
		// 		// {
		// 		// 	transformOffsetFromMouse = position - FL::Scene_GetMousePosWorld();
		// 		// }

		// 		//if (b_baseActive && ImGui::IsMouseDragging(ImGuiMouseButton_Left))
		// 		//{			
		// 		//	FL::Vector2 newPosition = FL::Scene_GetMousePosWorld() + transformOffsetFromMouse;
		// 		//	transform->SetPosition(newPosition - transform->GetPositionOrigin());
		// 		//}
		// 		//else if (b_xActive && ImGui::IsMouseDragging(ImGuiMouseButton_Left))
		// 		//{
		// 		//	FL::Vector2 newPosition = FL::Scene_GetMousePosWorld() + transformOffsetFromMouse;
		// 		//	transform->SetPosition(FL::Vector2(newPosition.x - transform->GetPositionOrigin().x, position.y - transform->GetPositionOrigin().x));
		// 		//}
		// 		//else if (b_yActive && ImGui::IsMouseDragging(ImGuiMouseButton_Left))
		// 		//{
		// 		//	FL::Vector2 newPosition = FL::Scene_GetMousePosWorld() + transformOffsetFromMouse;
		// 		//	transform->SetPosition(FL::Vector2(position.x - transform->GetPositionOrigin().x, newPosition.y - transform->GetPositionOrigin().y));
		// 		//}


		// 		// Draw channel maxSpriteLayers + 3 for Upper UI Transform Arrow			
		// 		// FL::AddImageToDrawList(arrowToRender, position, FG_sceneViewCenter, arrowWidth, arrowHeight, arrowOffset, arrowScale, b_scalesWithZoom, FG_sceneViewGridStep.x, ImGui::GetWindowDrawList());
		// 	}
		// }
	}
}