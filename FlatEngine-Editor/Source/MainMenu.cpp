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
		
		if (ImGui::BeginMainMenuBar())
		{
			FL::GuiCore::MoveScreenCursor(-7,0);
			if (ImGui::BeginMenu("File"))
			{				
				if (ImGui::MenuItem(" Save All"))
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
				FL::GuiCore::RenderSeparator(0,0,"menuSeparatorLight");
				if (ImGui::MenuItem(" Save Project"))
				{										
					FL::ProjectManager::SaveProject(FL::ProjectManager::loadedProject, FL::ProjectManager::loadedProject.path);
				}	
				FL::GuiCore::RenderSeparator(0,0,"menuSeparatorLight");
				if (ImGui::MenuItem(" Load Project"))
				{					
					// FL::ProjectManager::b_projectSelected = false;					
					// FL::ProjectManager::RecreateProjectNameTexts();
				}						
				FL::GuiCore::RenderSeparator(0,0,"menuSeparatorLight");
				if (ImGui::MenuItem(" Save Scene"))
				{
					FL::SceneManager::SaveCurrentScene();
				}
				FL::GuiCore::RenderSeparator(0,0,"menuSeparatorLight");
				if (ImGui::MenuItem(" Load Scene")) // if (ImGui::MenuItem(" Load Scene...", "Ctrl+L"))
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
				FL::GuiCore::RenderSeparator(0,0,"menuSeparatorLight");
				if (ImGui::MenuItem(" New Scene"))
				{					
					FL::SceneManager::SaveCurrentScene();
					Modals::b_openSceneModal = true;
				}				
				FL::GuiCore::RenderSeparator(0,0,"menuSeparatorLight");
				if (ImGui::MenuItem(" Save Engine Settings"))
				{					
					FL::Settings::settings.SaveSettings();
				}
				//if (ImGui::MenuItem("Save Scene As..."))
				//{					
				//	std::string scenePath = FL::OpenSaveFileExplorer();
				//	if (scenePath != "")
				//	{
				//		Scene* currentScene = FL::GetLoadedScene();
				//		FL::F_SceneManager.SaveScene(currentScene, scenePath);
				//	}
				//}
				FL::GuiCore::RenderSeparator(0,0,"menuSeparatorLight");
				if (ImGui::MenuItem(" Quit", "Alt+F4"))
				{
					FL::b_closeProgramQueued = true;
				}

				ImGui::EndMenu();
			}
			//if (ImGui::BeginMenu("Edit"))
			//{
			//	if (ImGui::MenuItem("Undo", "CTRL+Z")) {}
			//	if (ImGui::MenuItem("Redo", "CTRL+Y", false, false)) {}  // Disabled item
			//	FL::GuiCore::RenderSeparator(0,0,"menuSeparator");
			//	if (ImGui::MenuItem("Cut", "CTRL+X")) {}
			//	if (ImGui::MenuItem("Copy", "CTRL+C")) {}
			//	if (ImGui::MenuItem("Paste", "CTRL+V")) {}
			//	ImGui::EndMenu();
			//}			
			if (ImGui::BeginMenu("Viewports"))
			{
				// FL::GuiCore::MoveScreenCursor(0,-1);
				if (ImGui::MenuItem(" ImGui Demo Window", NULL, FL::Settings::settings.b_showDemoWindow))
				{
					FL::Settings::settings.b_showDemoWindow = !FL::Settings::settings.b_showDemoWindow;
					FL::Settings::settings.SaveSettings();	
				}
				FL::GuiCore::RenderSeparator(0,0,"menuSeparator");
				ImGui::MenuItem("    - MAIN VIEWPORTS -", NULL, false, false);
				FL::GuiCore::RenderSeparator(0,0,"menuSeparator");
				if (ImGui::MenuItem(" Scene View", NULL, FL::Settings::settings.b_showSceneView))
				{
					FL::Settings::settings.b_showSceneView = !FL::Settings::settings.b_showSceneView;
					FL::Settings::settings.SaveSettings();	
				}
				FL::GuiCore::RenderSeparator(0,0,"menuSeparatorLight");
				if (ImGui::MenuItem(" Game View", NULL, FL::Settings::settings.b_showGameView))
				{
					FL::Settings::settings.b_showGameView = !FL::Settings::settings.b_showGameView;
					FL::Settings::settings.SaveSettings();	
				}
				FL::GuiCore::RenderSeparator(0,0,"menuSeparatorLight");
				if (ImGui::MenuItem(" Hierarchy", NULL, FL::Settings::settings.b_showHierarchy))
				{
					FL::Settings::settings.b_showHierarchy = !FL::Settings::settings.b_showHierarchy;
					FL::Settings::settings.SaveSettings();	
				}
				FL::GuiCore::RenderSeparator(0,0,"menuSeparatorLight");
				if (ImGui::MenuItem(" Persistent Scripts", NULL, FL::Settings::settings.b_showPersistentScript))
				{
					FL::Settings::settings.b_showPersistentScript = !FL::Settings::settings.b_showPersistentScript;
					FL::Settings::settings.SaveSettings();	
				}
				FL::GuiCore::RenderSeparator(0,0,"menuSeparatorLight");
				if (ImGui::MenuItem(" Inspector", NULL, FL::Settings::settings.b_showInspector))
				{
					FL::Settings::settings.b_showInspector = !FL::Settings::settings.b_showInspector;
					FL::Settings::settings.SaveSettings();	
				}
				FL::GuiCore::RenderSeparator(0,0,"menuSeparatorLight");
				if (ImGui::MenuItem(" File Explorer", NULL, FL::Settings::settings.b_showFileExplorer))
				{
					FL::Settings::settings.b_showFileExplorer = !FL::Settings::settings.b_showFileExplorer;
					FL::Settings::settings.SaveSettings();	
				}				
				FL::GuiCore::RenderSeparator(0,0,"menuSeparatorLight");
				if (ImGui::MenuItem(" Log", NULL, FL::Settings::settings.b_showLog))
				{
					FL::Settings::settings.b_showLog = !FL::Settings::settings.b_showLog;
					FL::Settings::settings.SaveSettings();	
				}
				//if (ImGui::MenuItem("Profiler", NULL, FG_b_showProfiler))
				//{
				//	FG_b_showProfiler = !FG_b_showProfiler;
				//	FL::Settings::settings.SaveSettings();	
				//}
				FL::GuiCore::RenderSeparator(0,0,"menuSeparatorLight");
				if (ImGui::MenuItem(" Animator", NULL, FL::Settings::settings.b_showAnimator))
				{
					FL::Settings::settings.b_showAnimator = !FL::Settings::settings.b_showAnimator;
					FL::Settings::settings.SaveSettings();	
				}
				//if (ImGui::MenuItem("Animation Preview", NULL, FG_b_showAnimationPreview))
				//{
				//	FG_b_showAnimationPreview = !FG_b_showAnimationPreview;
				//	FL::Settings::settings.SaveSettings();	
				//}
				FL::GuiCore::RenderSeparator(0,0,"menuSeparator");
				ImGui::MenuItem("       - EDITORS -", NULL, false, false);				
				FL::GuiCore::RenderSeparator(0,0,"menuSeparator");
				if (ImGui::MenuItem(" Key Frame Editor", NULL, FL::Settings::settings.b_showKeyFrameEditor))
				{
					FL::Settings::settings.b_showKeyFrameEditor = !FL::Settings::settings.b_showKeyFrameEditor;
					FL::Settings::settings.SaveSettings();	
				}
				FL::GuiCore::RenderSeparator(0,0,"menuSeparatorLight");
				if (ImGui::MenuItem(" TileSet Editor", NULL, FL::Settings::settings.b_showTileSetEditor))
				{
					FL::Settings::settings.b_showTileSetEditor = !FL::Settings::settings.b_showTileSetEditor;
					FL::Settings::settings.SaveSettings();	
				}
				FL::GuiCore::RenderSeparator(0,0,"menuSeparatorLight");
				if (ImGui::MenuItem(" Mapping Context Editor", NULL, FL::Settings::settings.b_showMappingContextEditor))
				{
					FL::Settings::settings.b_showMappingContextEditor = !FL::Settings::settings.b_showMappingContextEditor;
					FL::Settings::settings.SaveSettings();	
				}
				FL::GuiCore::RenderSeparator(0,0,"menuSeparatorLight");
				if (ImGui::MenuItem(" Material Editor", NULL, FL::Settings::settings.b_showMaterialEditor))
				{
					FL::Settings::settings.b_showMaterialEditor = !FL::Settings::settings.b_showMaterialEditor;
					FL::Settings::settings.SaveSettings();	
				}

				ImGui::EndMenu();
			}			
			if (ImGui::BeginMenu("Create"))
			{
				// FL::GuiCore::MoveScreenCursor(0,-1);
				if (ImGui::MenuItem(" GameObject         "))
				{
					FL::ProjectManager::loadedProject.focusedGameObjectID = FL::SceneManager::loadedScene.CreateGameObject(-1)->GetID();				
				}
				FL::GuiCore::RenderSeparator(0,0,"menuSeparatorLight");
				if (ImGui::MenuItem(" Persistent Script"))
				{
					FL::ProjectManager::loadedProject.persistentScript.AddScript();
				}
				FL::GuiCore::RenderSeparator(0,0,"menuSeparatorLight");
				if (ImGui::BeginMenu(" Components"))
				{
					for (int i = 1; i < FL::ComponentType_Size; i++)
					{						
						std::string componentTypeString = " " + FL::ComponentTypeStrings[i];
						if ((FL::ComponentType)i == FL::ComponentType_Body2D)
						{
							if (ImGui::BeginMenu(" Body2D"))
							{
								if (ImGui::MenuItem(" Empty"))
								{
									FL::GameObject* newObject = FL::SceneManager::loadedScene.CreateGameObject(-1);							
									newObject->Add<FL::Body2D>();
									newObject->SetName("Body2D");
									FL::ProjectManager::loadedProject.focusedGameObjectID = newObject->GetID();
								}
								if (ImGui::MenuItem(" Box"))
								{
									FL::GameObject* newObject = FL::SceneManager::loadedScene.CreateGameObject(-1);							
									FL::Body2D* body = newObject->Add<FL::Body2D>();
									body->AddShape(FL::ShapeType_Box);
									newObject->SetName("BoxBody2D");
									FL::ProjectManager::loadedProject.focusedGameObjectID = newObject->GetID();
								}
								if (ImGui::MenuItem(" Circle"))
								{
									FL::GameObject* newObject = FL::SceneManager::loadedScene.CreateGameObject(-1);							
									FL::Body2D* body = newObject->Add<FL::Body2D>();
									body->AddShape(FL::ShapeType_Circle);
									newObject->SetName("CircleBody2D");
									FL::ProjectManager::loadedProject.focusedGameObjectID = newObject->GetID();
								}
								if (ImGui::MenuItem(" Capsule"))
								{
									FL::GameObject* newObject = FL::SceneManager::loadedScene.CreateGameObject(-1);							
									FL::Body2D* body = newObject->Add<FL::Body2D>();
									body->AddShape(FL::ShapeType_Capsule);
									newObject->SetName("CapsuleBody2D");
									FL::ProjectManager::loadedProject.focusedGameObjectID = newObject->GetID();
								}
								if (ImGui::MenuItem(" Polygon"))
								{
									FL::GameObject* newObject = FL::SceneManager::loadedScene.CreateGameObject(-1);							
									FL::Body2D* body = newObject->Add<FL::Body2D>();
									body->AddShape(FL::ShapeType_Polygon);
									newObject->SetName("PolygonBody2D");
									FL::ProjectManager::loadedProject.focusedGameObjectID = newObject->GetID();
								}
								if (ImGui::MenuItem(" Chain"))
								{
									FL::GameObject* newObject = FL::SceneManager::loadedScene.CreateGameObject(-1);							
									FL::Body2D* body = newObject->Add<FL::Body2D>();
									body->AddShape(FL::ShapeType_Chain);
									newObject->SetName("ChainBody2D");
									FL::ProjectManager::loadedProject.focusedGameObjectID = newObject->GetID();
								}
								ImGui::EndMenu();
							}
						}
						else if (ImGui::MenuItem(componentTypeString.c_str()))
						{
							FL::GameObject* newObject = FL::SceneManager::loadedScene.CreateGameObject(-1);						 
							newObject->AddComponent((FL::ComponentType)i);
							newObject->SetName(FL::ComponentTypeStrings[i]);
							FL::ProjectManager::loadedProject.focusedGameObjectID = newObject->GetID();
							ImGui::CloseCurrentPopup();
						}
						if (i < FL::ComponentType_Size - 1)
							FL::GuiCore::RenderSeparator(0,0,"menuSeparatorLight");
					}
					ImGui::EndMenu();
				}
				FL::GuiCore::RenderSeparator(0,0,"menuSeparatorLight");
				if (ImGui::BeginMenu(" Prefabs"))
				{
					std::map<std::string, FL::PrefabManager::Prefab> prefabs = FL::PrefabManager::GetPrefabs();
					if (prefabs.size() > 0)
					{
						for (std::map<std::string, FL::PrefabManager::Prefab>::iterator iter = prefabs.begin(); iter != prefabs.end();)
						{
							std::string prefabLabel = " " + iter->first;
							if (ImGui::MenuItem(prefabLabel.c_str()))
							{
								FL::GameObject *instantiatedObject = FL::PrefabManager::Instantiate(iter->first);
								FL::ProjectManager::loadedProject.focusedGameObjectID = instantiatedObject->GetID();
							}
							iter++;
							if (iter != prefabs.end())
								FL::GuiCore::RenderSeparator(0,0,"menuSeparatorLight");
						}
					}
					else
					{
						ImGui::Text(" Right click GameObject in hierarchy to create a Prefab");
					}

					ImGui::EndMenu();
				}
				FL::GuiCore::RenderSeparator(0,0,"menuSeparatorLight");
				if (ImGui::BeginMenu(" Asset files"))
				{
					if (ImGui::MenuItem(" Mapping Context"))
					{						
						Modals::b_openMappingContextModal = true;
					}
					FL::GuiCore::RenderSeparator(0,0,"menuSeparatorLight");
					if (ImGui::MenuItem(" Animation"))
					{
						Modals::b_openAnimationModal = true;
					}
					FL::GuiCore::RenderSeparator(0,0,"menuSeparatorLight");
					if (ImGui::MenuItem(" Lua Script"))
					{						
						Modals::b_openLuaModal = true;
					}
					FL::GuiCore::RenderSeparator(0,0,"menuSeparatorLight");
					if (ImGui::MenuItem(" Tile Set"))
					{
						Modals::b_openTileSetModal = true;
					}
					ImGui::EndMenu();
				}
				ImGui::EndMenu();
			}			
			if (ImGui::BeginMenu("Settings"))
			{
				if (ImGui::MenuItem(" Reload Lua Colors"))
				{
					FL::Assets::assetManager.CollectColors();		
					FL::SceneView::UpdateSceneObjectColors();	
					FL::GuiCore::SetImGuiVars();		
				}
				FL::GuiCore::RenderSeparator(0,0,"menuSeparatorLight");
				if (ImGui::MenuItem(" Reload Lua Textures"))
				{
					FL::Assets::assetManager.CollectTextures();					
				}
				FL::GuiCore::RenderSeparator(0,0,"menuSeparatorLight");
				if (ImGui::MenuItem(" Reload Tags"))
				{
					FL::Assets::assetManager.CollectTags();

					for (FL::GameObject sceneObject : FL::SceneManager::loadedScene.GetSceneObjects())
					{
						sceneObject.GetTagList().UpdateAvailableTags();
					}
				}
				FL::GuiCore::RenderSeparator(0,0,"menuSeparatorLight");
				if (ImGui::MenuItem(" Reload Shaders"))
				{
					FL::VulkanManager::vulkan.ReloadShaders();
				}
				FL::GuiCore::RenderSeparator(0,0,"menuSeparator");
				if (ImGui::BeginMenu(" Scene Viewport"))
				{
					if (ImGui::MenuItem(" Show Grid", NULL, FL::SceneView::ShouldShowSceneViewGridObjects()))
					{
						FL::SceneView::ToggleShowSceneViewGridObjects();
						FL::Settings::settings.SaveSettings();
					}			
					FL::GuiCore::RenderSeparator(0,0,"menuSeparatorLight");		
					if (ImGui::MenuItem(" Horizontal Grid", NULL, FL::SceneView::IsGridHorizontal()))
					{						
						FL::SceneView::ToggleGridHorizontal();
						FL::Settings::settings.SaveSettings();
					}								
					FL::GuiCore::RenderSeparator(0,0,"menuSeparator");
					if (ImGui::MenuItem(" Orthographic View", NULL, FL::SceneView::IsOrthoGraphic()))
					{
						FL::SceneView::ToggleOrthographic();
						FL::Settings::settings.SaveSettings();
					}
					FL::GuiCore::RenderSeparator(0,0,"menuSeparatorLight");		
					if (ImGui::MenuItem(" Reset View"))
					{
						FL::SceneView::sceneViewCameraTransform.SetRotation(FL::Vector3(0,180,0));
						FL::Settings::settings.SaveSettings();
					}
					FL::GuiCore::RenderSeparator(0,0,"menuSeparatorLight");						
					if (ImGui::BeginMenu(" Pixels/Grid Square"))
					{					
						int gridStep = (int)FL::SceneView::sceneViewCamera.gridStep;
						if (FL::GuiCore::RenderDragInt("##SceneViewCameraGridStep", 100.0f, gridStep, 1, FL::SceneView::minGridStep, FL::SceneView::maxGridStep))
						{
							if (gridStep > 0) FL::SceneView::sceneViewCamera.gridStep = (uint32_t)gridStep;
							FL::Settings::settings.SaveSettings();	
						}
						ImGui::EndMenu();
					}
					FL::GuiCore::RenderSeparator(0,0,"menuSeparatorLight");
					if (ImGui::BeginMenu(" Near Clip (Ortho)"))
					{					
						if (FL::GuiCore::RenderDragFloat("##SceneViewCameraNearClipOrtho", 100.0f, FL::SceneView::sceneViewCamera.orthoNearClippingDistance, 0.1f, -20000, 20000))
							FL::Settings::settings.SaveSettings();	
						ImGui::EndMenu();
					}
					FL::GuiCore::RenderSeparator(0,0,"menuSeparatorLight");
					if (ImGui::BeginMenu(" Far Clip (Ortho)"))
					{					
						if (FL::GuiCore::RenderDragFloat("##SceneViewCameraFarClipOrtho", 100.0f, FL::SceneView::sceneViewCamera.orthoFarClippingDistance, 0.1f, -20000, 20000))
							FL::Settings::settings.SaveSettings();	
						ImGui::EndMenu();
					}
					FL::GuiCore::RenderSeparator(0,0,"menuSeparator");									
					if (ImGui::BeginMenu(" Perspective Angle"))
					{					
						if (FL::GuiCore::RenderDragFloat("##SceneViewCameraPerspective", 100.0f, FL::SceneView::sceneViewCamera.perspectiveAngle, 0.1f, 0, 359))
							FL::Settings::settings.SaveSettings();	
						ImGui::EndMenu();
					}												
					FL::GuiCore::RenderSeparator(0,0,"menuSeparatorLight");
					if (ImGui::BeginMenu(" Near Clip (Per)"))
					{					
						if (FL::GuiCore::RenderDragFloat("##SceneViewCameraNearClip", 100.0f, FL::SceneView::sceneViewCamera.nearClippingDistance, 0.1f, -20000, 20000))
							FL::Settings::settings.SaveSettings();	
						ImGui::EndMenu();
					}
					FL::GuiCore::RenderSeparator(0,0,"menuSeparatorLight");
					if (ImGui::BeginMenu(" Far Clip (Per)"))
					{					
						if (FL::GuiCore::RenderDragFloat("##SceneViewCameraFarClip", 100.0f, FL::SceneView::sceneViewCamera.farClippingDistance, 0.1f, -20000, 20000))
							FL::Settings::settings.SaveSettings();	
						ImGui::EndMenu();
					}
					FL::GuiCore::RenderSeparator(0,0,"menuSeparatorLight");
					if (ImGui::BeginMenu(" Camera Move Speed"))
					{					
						if (FL::GuiCore::RenderDragFloat("##SceneViewCameraDamping", 100.0f, FL::Settings::settings.sceneViewCameraSpeed, 1, 0, 500))
							FL::Settings::settings.SaveSettings();											
						ImGui::EndMenu();
					}
					ImGui::EndMenu();
				}
				FL::GuiCore::RenderSeparator(0,0,"menuSeparator");
				if (ImGui::MenuItem(" Settings", NULL, FL::Settings::settings.b_showSettings))
				{
					FL::Settings::settings.b_showSettings = !FL::Settings::settings.b_showSettings;
					FL::Settings::settings.SaveSettings();
				}
				FL::GuiCore::RenderSeparator(0,0,"menuSeparator");
				if (ImGui::MenuItem(" Build Project", NULL))
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