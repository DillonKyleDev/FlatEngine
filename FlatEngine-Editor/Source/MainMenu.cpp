#include "components/Component.h"
#include "FileManager.h"
#include "FlatEngine.h"
#include "FlatGui.h"
#include "GameObject.h"
#include "GuiCore.h"
#include "managers/Assets.h"
#include "managers/Settings.h"
#include "managers/SceneManager.h"
#include "managers/PrefabManager.h"
#include "managers/ProjectManager.h"
#include "Modals.h"
#include "physics/Shape.h"
#include "render/SceneView.h"
#include "render/VulkanManager.h"
#include "TagList.h"
#include "tools/FileHelper.h"

#include "imgui.h"
#include "tools/Vector3.h"
#include <cstdint>

namespace FL = FlatEngine;


namespace FlatGui 
{
	void MainMenuBar(bool& b_show)
	{
		if (!b_show)
			return;

		FL::GuiCore::PushMenuStyles();
		
		if (FL::GuiCore::BeginMainMenuBar())
		{
			FL::GuiCore::MoveScreenCursor(-7,0);
			if (FL::GuiCore::BeginMenu("File"))
			{				
				if (FL::GuiCore::MenuItem("Save All"))
				{
					if (FL::SceneManager::loadedScene.path == "")
					{						
						Modals::b_openSceneModal = true;
					}
					else
					{
						FL::SceneManager::SaveCurrentScene();
					}
					FL::ProjectManager::SaveCurrentProject();
					FL::Settings::settings.SaveSettings();
					ImGui::CloseCurrentPopup();
				}	
				FL::GuiCore::RenderMenuSeparator();
				if (FL::GuiCore::MenuItem("Save Project"))
				{										
					FL::ProjectManager::SaveCurrentProject();
				}	
				FL::GuiCore::RenderMenuSeparator();
				if (FL::GuiCore::MenuItem("Load Project"))
				{					
					// FL::ProjectManager::b_projectSelected = false;					
					// FL::ProjectManager::RecreateProjectNameTexts();
				}						
				FL::GuiCore::RenderMenuSeparator();
				if (FL::GuiCore::MenuItem("Save Scene"))
				{
					FL::SceneManager::SaveCurrentScene();
				}
				FL::GuiCore::RenderMenuSeparator();
				if (FL::GuiCore::MenuItem("Load Scene")) // if (FL::GuiCore::MenuItem("Load Scene...", "Ctrl+L"))
				{					
					std::string scenePath = FL::FileHelper::OpenLoadFileExplorer();
					if (scenePath != "")
					{
						FileManager::OpenFileContextually(scenePath);
						//FL::F_SceneManager.LoadScene(scenePath);
						//FL::ProjectManager::loadedProject.SetLoadedScenePath(scenePath);						
						//SaveCurrentProject();
					}
				}
				FL::GuiCore::RenderMenuSeparator();
				if (FL::GuiCore::MenuItem("New Scene"))
				{					
					FL::SceneManager::SaveCurrentScene();
					Modals::b_openSceneModal = true;
				}				
				FL::GuiCore::RenderMenuSeparator();
				if (FL::GuiCore::MenuItem("Save Engine Settings"))
				{					
					FL::Settings::settings.SaveSettings();
				}
				//if (FL::GuiCore::MenuItem("Save Scene As..."))
				//{					
				//	std::string scenePath = FL::OpenSaveFileExplorer();
				//	if (scenePath != "")
				//	{
				//		Scene* currentScene = FL::GetLoadedScene();
				//		FL::F_SceneManager.SaveScene(currentScene, scenePath);
				//	}
				//}
				FL::GuiCore::RenderMenuSeparator();
				if (FL::GuiCore::MenuItem("Quit", "Alt+F4"))
				{
					FL::b_closeProgramQueued = true;
				}

				ImGui::EndMenu();
			}
			//if (FL::GuiCore::BeginMenu("Edit"))
			//{
			//	if (FL::GuiCore::MenuItem("Undo", "CTRL+Z")) {}
			//	if (FL::GuiCore::MenuItem("Redo", "CTRL+Y", false, false)) {}  // Disabled item			
			//	ImGui::EndMenu();
			//}			
			if (FL::GuiCore::BeginMenu("Viewports"))
			{
				if (FL::GuiCore::MenuItem("ImGui Demo Window", NULL, FL::Settings::settings.b_showDemoWindow))
				{
					FL::Settings::settings.b_showDemoWindow = !FL::Settings::settings.b_showDemoWindow;
					FL::Settings::settings.SaveSettings();	
				}
				FL::GuiCore::RenderMenuSeparator(false);
				FL::GuiCore::MenuItem("  - MAIN VIEWPORTS -", NULL, false, false);
				FL::GuiCore::RenderMenuSeparator(false);
				if (FL::GuiCore::MenuItem("Scene View", NULL, FL::Settings::settings.b_showSceneView))
				{
					FL::Settings::settings.b_showSceneView = !FL::Settings::settings.b_showSceneView;
					FL::Settings::settings.SaveSettings();	
				}
				FL::GuiCore::RenderMenuSeparator();
				if (FL::GuiCore::MenuItem("Game View", NULL, FL::Settings::settings.b_showGameView))
				{
					FL::Settings::settings.b_showGameView = !FL::Settings::settings.b_showGameView;
					FL::Settings::settings.SaveSettings();	
				}
				FL::GuiCore::RenderMenuSeparator();
				if (FL::GuiCore::MenuItem("Hierarchy", NULL, FL::Settings::settings.b_showHierarchy))
				{
					FL::Settings::settings.b_showHierarchy = !FL::Settings::settings.b_showHierarchy;
					FL::Settings::settings.SaveSettings();	
				}
				FL::GuiCore::RenderMenuSeparator();
				if (FL::GuiCore::MenuItem("Persistent Scripts", NULL, FL::Settings::settings.b_showPersistentScript))
				{
					FL::Settings::settings.b_showPersistentScript = !FL::Settings::settings.b_showPersistentScript;
					FL::Settings::settings.SaveSettings();	
				}
				FL::GuiCore::RenderMenuSeparator();
				if (FL::GuiCore::MenuItem("Inspector", NULL, FL::Settings::settings.b_showInspector))
				{
					FL::Settings::settings.b_showInspector = !FL::Settings::settings.b_showInspector;
					FL::Settings::settings.SaveSettings();	
				}
				FL::GuiCore::RenderMenuSeparator();
				if (FL::GuiCore::MenuItem("File Explorer", NULL, FL::Settings::settings.b_showFileExplorer))
				{
					FL::Settings::settings.b_showFileExplorer = !FL::Settings::settings.b_showFileExplorer;
					FL::Settings::settings.SaveSettings();	
				}				
				FL::GuiCore::RenderMenuSeparator();
				if (FL::GuiCore::MenuItem("Log", NULL, FL::Settings::settings.b_showLog))
				{
					FL::Settings::settings.b_showLog = !FL::Settings::settings.b_showLog;
					FL::Settings::settings.SaveSettings();	
				}
				//if (FL::GuiCore::MenuItem("Profiler", NULL, FG_b_showProfiler))
				//{
				//	FG_b_showProfiler = !FG_b_showProfiler;
				//	FL::Settings::settings.SaveSettings();	
				//}
				FL::GuiCore::RenderMenuSeparator();
				if (FL::GuiCore::MenuItem("Animator", NULL, FL::Settings::settings.b_showAnimator))
				{
					FL::Settings::settings.b_showAnimator = !FL::Settings::settings.b_showAnimator;
					FL::Settings::settings.SaveSettings();	
				}
				//if (FL::GuiCore::MenuItem("Animation Preview", NULL, FG_b_showAnimationPreview))
				//{
				//	FG_b_showAnimationPreview = !FG_b_showAnimationPreview;
				//	FL::Settings::settings.SaveSettings();	
				//}
				FL::GuiCore::RenderMenuSeparator(false);
				FL::GuiCore::MenuItem("      - EDITORS -", NULL, false, false);				
				FL::GuiCore::RenderMenuSeparator(false);
				if (FL::GuiCore::MenuItem("Key Frame Editor", NULL, FL::Settings::settings.b_showKeyFrameEditor))
				{
					FL::Settings::settings.b_showKeyFrameEditor = !FL::Settings::settings.b_showKeyFrameEditor;
					FL::Settings::settings.SaveSettings();	
				}
				FL::GuiCore::RenderMenuSeparator();
				if (FL::GuiCore::MenuItem("TileSet Editor", NULL, FL::Settings::settings.b_showTileSetEditor))
				{
					FL::Settings::settings.b_showTileSetEditor = !FL::Settings::settings.b_showTileSetEditor;
					FL::Settings::settings.SaveSettings();	
				}
				FL::GuiCore::RenderMenuSeparator();
				if (FL::GuiCore::MenuItem("Mapping Context Editor", NULL, FL::Settings::settings.b_showMappingContextEditor))
				{
					FL::Settings::settings.b_showMappingContextEditor = !FL::Settings::settings.b_showMappingContextEditor;
					FL::Settings::settings.SaveSettings();	
				}
				FL::GuiCore::RenderMenuSeparator();
				if (FL::GuiCore::MenuItem("Material Editor", NULL, FL::Settings::settings.b_showMaterialEditor))
				{
					FL::Settings::settings.b_showMaterialEditor = !FL::Settings::settings.b_showMaterialEditor;
					FL::Settings::settings.SaveSettings();	
				}

				ImGui::EndMenu();
			}			
			if (FL::GuiCore::BeginMenu("Create"))
			{
				// FL::GuiCore::MoveScreenCursor(0,-1);
				if (FL::GuiCore::MenuItem("GameObject         "))
				{
					FL::SceneManager::loadedScene.CreateGameObject(-1);
				}
				FL::GuiCore::RenderMenuSeparator();
				if (FL::GuiCore::MenuItem("Persistent Script"))
				{
					FL::ProjectManager::loadedProject.persistentScript.AddScript();
				}
				FL::GuiCore::RenderMenuSeparator();
				if (FL::GuiCore::BeginMenu("Components"))
				{
					for (int i = 1; i < FL::ComponentType_Size; i++)
					{						
						std::string componentTypeString = ""+ FL::ComponentTypeStrings[i];
						if ((FL::ComponentType)i == FL::ComponentType_Body2D)
						{
							if (FL::GuiCore::BeginMenu("Body2D"))
							{
								if (FL::GuiCore::MenuItem("Empty"))
								{
									FL::GameObject* newObject = FL::SceneManager::loadedScene.CreateGameObject(-1);							
									newObject->Add<FL::Body2D>();
									newObject->SetName("Body2D");
								}
								if (FL::GuiCore::MenuItem("Box"))
								{
									FL::GameObject* newObject = FL::SceneManager::loadedScene.CreateGameObject(-1);							
									FL::Body2D* body = newObject->Add<FL::Body2D>();
									body->AddShape(FL::ShapeType2D_Box);
									newObject->SetName("BoxBody2D");
								}
								if (FL::GuiCore::MenuItem("Circle"))
								{
									FL::GameObject* newObject = FL::SceneManager::loadedScene.CreateGameObject(-1);							
									FL::Body2D* body = newObject->Add<FL::Body2D>();
									body->AddShape(FL::ShapeType2D_Circle);
									newObject->SetName("CircleBody2D");
								}
								if (FL::GuiCore::MenuItem("Capsule"))
								{
									FL::GameObject* newObject = FL::SceneManager::loadedScene.CreateGameObject(-1);							
									FL::Body2D* body = newObject->Add<FL::Body2D>();
									body->AddShape(FL::ShapeType2D_Capsule);
									newObject->SetName("CapsuleBody2D");
								}
								if (FL::GuiCore::MenuItem("Polygon"))
								{
									FL::GameObject* newObject = FL::SceneManager::loadedScene.CreateGameObject(-1);							
									FL::Body2D* body = newObject->Add<FL::Body2D>();
									body->AddShape(FL::ShapeType2D_Polygon);
									newObject->SetName("PolygonBody2D");
								}
								if (FL::GuiCore::MenuItem("Chain"))
								{
									FL::GameObject* newObject = FL::SceneManager::loadedScene.CreateGameObject(-1);							
									FL::Body2D* body = newObject->Add<FL::Body2D>();
									body->AddShape(FL::ShapeType2D_Chain);
									newObject->SetName("ChainBody2D");
								}
								ImGui::EndMenu();
							}
						}
						else if (FL::GuiCore::MenuItem(componentTypeString.c_str()))
						{
							FL::GameObject* newObject = FL::SceneManager::loadedScene.CreateGameObject(-1);						 
							newObject->AddComponent((FL::ComponentType)i);
							newObject->SetName(FL::ComponentTypeStrings[i]);
							ImGui::CloseCurrentPopup();
						}
						if (i < FL::ComponentType_Size - 1)
							FL::GuiCore::RenderMenuSeparator();
					}
					ImGui::EndMenu();
				}
				FL::GuiCore::RenderMenuSeparator();
				if (FL::GuiCore::BeginMenu("Prefabs"))
				{
					std::map<std::string, FL::PrefabManager::Prefab> prefabs = FL::PrefabManager::GetPrefabs();
					if (prefabs.size() > 0)
					{
						for (std::map<std::string, FL::PrefabManager::Prefab>::iterator iter = prefabs.begin(); iter != prefabs.end();)
						{
							std::string prefabLabel = ""+ iter->first;
							if (FL::GuiCore::MenuItem(prefabLabel.c_str()))
							{
								FL::GameObject *instantiatedObject = FL::PrefabManager::Instantiate(iter->first);
							}
							iter++;
							if (iter != prefabs.end())
								FL::GuiCore::RenderMenuSeparator();
						}
					}
					else
					{
						FL::GuiCore::MenuItem("Right click GameObject in the Hierarchy to create a Prefab.");
					}

					ImGui::EndMenu();
				}
				FL::GuiCore::RenderMenuSeparator();
				if (FL::GuiCore::BeginMenu("Asset files"))
				{
					if (FL::GuiCore::MenuItem("Mapping Context"))
					{						
						Modals::b_openMappingContextModal = true;
					}
					FL::GuiCore::RenderMenuSeparator();
					if (FL::GuiCore::MenuItem("Animation"))
					{
						Modals::b_openAnimationModal = true;
					}
					FL::GuiCore::RenderMenuSeparator();
					if (FL::GuiCore::MenuItem("Lua Script"))
					{						
						Modals::b_openLuaModal = true;
					}
					FL::GuiCore::RenderMenuSeparator();
					if (FL::GuiCore::MenuItem("Tile Set"))
					{
						Modals::b_openTileSetModal = true;
					}
					ImGui::EndMenu();
				}
				ImGui::EndMenu();
			}			
			if (FL::GuiCore::BeginMenu("Settings"))
			{
				if (FL::GuiCore::MenuItem("Reload Lua Colors"))
				{
					FL::Assets::assetManager.CollectColors();		
					FL::SceneView::UpdateSceneObjectColors();	
					FL::GuiCore::SetImGuiVars();		
				}
				FL::GuiCore::RenderMenuSeparator();
				if (FL::GuiCore::MenuItem("Reload Lua Textures"))
				{
					FL::Assets::assetManager.CollectTextures();					
				}
				FL::GuiCore::RenderMenuSeparator();
				if (FL::GuiCore::MenuItem("Reload Tags"))
				{
					FL::Assets::assetManager.CollectTags();

					for (FL::GameObject sceneObject : FL::SceneManager::loadedScene.GetSceneObjects())
					{
						sceneObject.GetTagList().UpdateAvailableTags();
					}
				}
				FL::GuiCore::RenderMenuSeparator();
				if (FL::GuiCore::MenuItem("Reload Shaders"))
				{
					FL::VulkanManager::vulkan.ReloadShaders();
				}
				FL::GuiCore::RenderMenuSeparator(false);
				if (FL::GuiCore::BeginMenu("Scene Viewport"))
				{
					if (FL::GuiCore::MenuItem("Show Grid", NULL, FL::SceneView::ShouldShowSceneViewGridObjects()))
					{
						FL::SceneView::ToggleShowSceneViewGridObjects();
						FL::Settings::settings.SaveSettings();
					}			
					FL::GuiCore::RenderMenuSeparator();		
					if (FL::GuiCore::MenuItem("Horizontal Grid", NULL, FL::SceneView::IsGridHorizontal()))
					{						
						FL::SceneView::ToggleGridHorizontal();
						FL::Settings::settings.SaveSettings();
					}								
					FL::GuiCore::RenderMenuSeparator(false);
					if (FL::GuiCore::MenuItem("Orthographic View", NULL, FL::SceneView::IsOrthoGraphic()))
					{
						FL::SceneView::ToggleOrthographic();
						FL::Settings::settings.SaveSettings();
					}
					FL::GuiCore::RenderMenuSeparator();		
					if (FL::GuiCore::MenuItem("Reset View"))
					{
						FL::SceneView::sceneViewCameraTransform.SetRotation(FL::Vector3(0,180,0));
						FL::Settings::settings.SaveSettings();
					}
					FL::GuiCore::RenderMenuSeparator();						
					if (FL::GuiCore::BeginMenu("Pixels/Grid Square"))
					{					
						int gridStep = (int)FL::SceneView::sceneViewCamera.gridStep;
						if (FL::GuiCore::RenderDragInt("##SceneViewCameraGridStep", 100.0f, gridStep, 1, FL::SceneView::minGridStep, FL::SceneView::maxGridStep))
						{
							if (gridStep > 0) FL::SceneView::sceneViewCamera.gridStep = (uint32_t)gridStep;
							FL::Settings::settings.SaveSettings();	
						}
						ImGui::EndMenu();
					}
					FL::GuiCore::RenderMenuSeparator();
					if (FL::GuiCore::BeginMenu("Near Clip (Ortho)"))
					{					
						if (FL::GuiCore::RenderDragFloat("##SceneViewCameraNearClipOrtho", 100.0f, FL::SceneView::sceneViewCamera.orthoNearClippingDistance, 0.1f, -20000, 20000))
							FL::Settings::settings.SaveSettings();	
						ImGui::EndMenu();
					}
					FL::GuiCore::RenderMenuSeparator();
					if (FL::GuiCore::BeginMenu("Far Clip (Ortho)"))
					{					
						if (FL::GuiCore::RenderDragFloat("##SceneViewCameraFarClipOrtho", 100.0f, FL::SceneView::sceneViewCamera.orthoFarClippingDistance, 0.1f, -20000, 20000))
							FL::Settings::settings.SaveSettings();	
						ImGui::EndMenu();
					}
					FL::GuiCore::RenderMenuSeparator(false);									
					if (FL::GuiCore::BeginMenu("Perspective Angle"))
					{					
						if (FL::GuiCore::RenderDragFloat("##SceneViewCameraPerspective", 100.0f, FL::SceneView::sceneViewCamera.perspectiveAngle, 0.1f, 0, 359))
							FL::Settings::settings.SaveSettings();	
						ImGui::EndMenu();
					}												
					FL::GuiCore::RenderMenuSeparator();
					if (FL::GuiCore::BeginMenu("Near Clip (Per)"))
					{					
						if (FL::GuiCore::RenderDragFloat("##SceneViewCameraNearClip", 100.0f, FL::SceneView::sceneViewCamera.nearClippingDistance, 0.1f, -20000, 20000))
							FL::Settings::settings.SaveSettings();	
						ImGui::EndMenu();
					}
					FL::GuiCore::RenderMenuSeparator();
					if (FL::GuiCore::BeginMenu("Far Clip (Per)"))
					{					
						if (FL::GuiCore::RenderDragFloat("##SceneViewCameraFarClip", 100.0f, FL::SceneView::sceneViewCamera.farClippingDistance, 0.1f, -20000, 20000))
							FL::Settings::settings.SaveSettings();	
						ImGui::EndMenu();
					}
					FL::GuiCore::RenderMenuSeparator();
					if (FL::GuiCore::BeginMenu("Camera Move Speed"))
					{					
						if (FL::GuiCore::RenderDragFloat("##SceneViewCameraDamping", 100.0f, FL::Settings::settings.sceneViewCameraSpeed, 1, 0, 500))
							FL::Settings::settings.SaveSettings();											
						ImGui::EndMenu();
					}
					ImGui::EndMenu();
				}
				FL::GuiCore::RenderMenuSeparator(false);
				if (FL::GuiCore::MenuItem("Settings", NULL, FL::Settings::settings.b_showSettings))
				{
					FL::Settings::settings.b_showSettings = !FL::Settings::settings.b_showSettings;
					FL::Settings::settings.SaveSettings();
				}
				FL::GuiCore::RenderMenuSeparator(false);
				if (FL::GuiCore::MenuItem("Build Project", NULL))
				{
					FL::ProjectManager::BuildProject();
				}
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}

		FL::GuiCore::PopMenuStyles();
	}
}