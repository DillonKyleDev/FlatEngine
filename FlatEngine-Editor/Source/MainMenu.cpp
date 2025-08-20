#include "FlatEngine.h"
#include "FlatGui.h"
#include "GameObject.h"
#include "AssetManager.h"
#include "Sprite.h"
#include "SceneManager.h"
#include "Project.h"
#include "Vector2.h"
#include "PrefabManager.h"
#include "TagList.h"
#include "WindowManager.h"

#include "imgui.h"


namespace FL = FlatEngine;

namespace FlatGui 
{
	void MainMenuBar()
	{
		static bool b_openScriptModal = false;
		static bool b_openAnimationModal = false;
		static bool b_openMappingContextModal = false;
		static bool b_openTileSetModal = false;
		static bool b_openProjectModal = false;
		std::string projectName = "";

		if (FL::RenderInputModal("Create New Project", "Project name", projectName, b_openProjectModal))
		{
			CreateNewProject(projectName);
		}

		FL::PushMenuStyles();

		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Project Select"))
				{					
					FL::F_b_projectSelected = false;					
					FL::F_Window->ResizeWindow(800, 500);
					RecreateProjectNameTexts();
				}
				ImGui::Separator();
				//if (ImGui::MenuItem("Open Project..."))
				//{					
				//	std::string projectPath = FL::OpenLoadFileExplorer();
				//	if (projectPath != "")
				//	{
				//		LoadProject(projectPath);
				//	}
				//}
				if (ImGui::MenuItem("Save Project"))
				{					
					std::string projectPath = FL::F_LoadedProject.GetPath();
					SaveProject(FL::F_LoadedProject, projectPath);
				}
				//if (ImGui::MenuItem("Save Project As..."))
				//{					
				//	std::string projectPath = FL::OpenSaveFileExplorer();
				//	if (projectPath != "")
				//	{
				//		SaveProject(FL::F_LoadedProject, projectPath);
				//	}
				//}

				ImGui::Separator();

				if (ImGui::MenuItem("New Scene..."))
				{					
					FL::F_SceneManager.SaveCurrentScene();

					std::string scenePath = FL::OpenSaveFileExplorer();
					if (scenePath != "")
					{
						FL::F_SceneManager.SaveTempScene();
						FL::F_SceneManager.SaveScene(FL::F_SceneManager.CreateAndLoadNewScene(), scenePath);
						FL::F_SceneManager.LoadScene(scenePath);
						FL::F_LoadedProject.SetLoadedScenePath(scenePath);
						SaveCurrentProject();
					}
				}
				if (ImGui::IsItemHovered())
				{
					FL::RenderTextToolTip("You must add '.scn' to the end of your scene files or FlatEngine won't see them.");
				}

				if (ImGui::MenuItem("Load Scene...", "Ctrl+L"))
				{					
					std::string scenePath = FL::OpenLoadFileExplorer();
					if (scenePath != "")
					{
						OpenFileContextually(scenePath);
						//FL::F_SceneManager.LoadScene(scenePath);
						//FL::F_LoadedProject.SetLoadedScenePath(scenePath);						
						//SaveCurrentProject();
					}
				}
				
				if (ImGui::MenuItem("Save Scene", "Ctrl+S"))
				{
					FL::F_SceneManager.SaveCurrentScene();
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

				ImGui::Separator();
				if (ImGui::MenuItem("Quit", "Alt+F4"))
				{
					FL::F_b_closeProgramQueued = true;
				}

				ImGui::EndMenu();
			}
			//if (ImGui::BeginMenu("Edit"))
			//{
			//	if (ImGui::MenuItem("Undo", "CTRL+Z")) {}
			//	if (ImGui::MenuItem("Redo", "CTRL+Y", false, false)) {}  // Disabled item
			//	ImGui::Separator();
			//	if (ImGui::MenuItem("Cut", "CTRL+X")) {}
			//	if (ImGui::MenuItem("Copy", "CTRL+C")) {}
			//	if (ImGui::MenuItem("Paste", "CTRL+V")) {}
			//	ImGui::EndMenu();
			//}
			if (ImGui::BeginMenu("Viewports"))
			{
				//if (ImGui::MenuItem("ImGui Demo Window", NULL, FG_b_showDemoWindow))
				//{
				//	FG_b_showDemoWindow = !FG_b_showDemoWindow;
				//	SaveProject(FL::F_LoadedProject, FL::F_LoadedProject.GetPath());
				//}
				//ImGui::Separator();
				ImGui::Text("- Main Panels -");
				ImGui::Separator();
				if (ImGui::MenuItem("Scene View", NULL, FG_b_showSceneView))
				{
					FG_b_showSceneView = !FG_b_showSceneView;
					SaveProject(FL::F_LoadedProject, FL::F_LoadedProject.GetPath());
				}
				if (ImGui::MenuItem("Game View", NULL, FG_b_showGameView))
				{
					FG_b_showGameView = !FG_b_showGameView;
					SaveProject(FL::F_LoadedProject, FL::F_LoadedProject.GetPath());
				}
				if (ImGui::MenuItem("Hierarchy", NULL, FG_b_showHierarchy))
				{
					FG_b_showHierarchy = !FG_b_showHierarchy;
					SaveProject(FL::F_LoadedProject, FL::F_LoadedProject.GetPath());
				}
				if (ImGui::MenuItem("Persistant Hierarchy", NULL, FG_b_showPersistantHierarchy))
				{
					FG_b_showPersistantHierarchy = !FG_b_showPersistantHierarchy;
					SaveProject(FL::F_LoadedProject, FL::F_LoadedProject.GetPath());
				}
				if (ImGui::MenuItem("Inspector", NULL, FG_b_showInspector))
				{
					FG_b_showInspector = !FG_b_showInspector;
					SaveProject(FL::F_LoadedProject, FL::F_LoadedProject.GetPath());
				}
				if (ImGui::MenuItem("File Explorer", NULL, FG_b_showFileExplorer))
				{
					FG_b_showFileExplorer = !FG_b_showFileExplorer;
					SaveProject(FL::F_LoadedProject, FL::F_LoadedProject.GetPath());
				}
				ImGui::Separator();
				if (ImGui::MenuItem("Logger", NULL, FG_b_showLogger))
				{
					FG_b_showLogger = !FG_b_showLogger;
					SaveProject(FL::F_LoadedProject, FL::F_LoadedProject.GetPath());
				}
				//if (ImGui::MenuItem("Profiler", NULL, FG_b_showProfiler))
				//{
				//	FG_b_showProfiler = !FG_b_showProfiler;
				//	SaveProject(FL::F_LoadedProject, FL::F_LoadedProject.GetPath());
				//}
				if (ImGui::MenuItem("Animator", NULL, FG_b_showAnimator))
				{
					FG_b_showAnimator = !FG_b_showAnimator;
					SaveProject(FL::F_LoadedProject, FL::F_LoadedProject.GetPath());
				}
				//if (ImGui::MenuItem("Animation Preview", NULL, FG_b_showAnimationPreview))
				//{
				//	FG_b_showAnimationPreview = !FG_b_showAnimationPreview;
				//	SaveProject(FL::F_LoadedProject, FL::F_LoadedProject.GetPath());
				//}
				ImGui::Separator();
				ImGui::Text("- Editors -");
				ImGui::Separator();
				if (ImGui::MenuItem("Key Frame Editor", NULL, FG_b_showKeyFrameEditor))
				{
					FG_b_showKeyFrameEditor = !FG_b_showKeyFrameEditor;
					SaveProject(FL::F_LoadedProject, FL::F_LoadedProject.GetPath());
				}
				if (ImGui::MenuItem("TileSet Editor", NULL, FG_b_showTileSetEditor))
				{
					FG_b_showTileSetEditor = !FG_b_showTileSetEditor;
					SaveProject(FL::F_LoadedProject, FL::F_LoadedProject.GetPath());
				}
				if (ImGui::MenuItem("Mapping Context Editor", NULL, FG_b_showMappingContextEditor))
				{
					FG_b_showMappingContextEditor = !FG_b_showMappingContextEditor;
					SaveProject(FL::F_LoadedProject, FL::F_LoadedProject.GetPath());
				}

				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Create"))
			{
				if (ImGui::MenuItem("GameObject         "))
				{
					GameObject* newObject = FL::CreateGameObject(-1);
					SetFocusedGameObjectID(newObject->GetID());
				}
				ImGui::Separator();
				if (ImGui::MenuItem("Persistant GameObject"))
				{
					GameObject* newObject = FL::CreatePersistantGameObject(-1);
					SetFocusedGameObjectID(newObject->GetID());
				}
				ImGui::Separator();
				if (ImGui::BeginMenu("Components"))
				{
					if (ImGui::MenuItem("Sprite"))
					{
						GameObject *newObject = FL::CreateGameObject(-1);
						newObject->AddSprite();
						newObject->SetName("Sprite(" + std::to_string(newObject->GetID()) + ")");
						SetFocusedGameObjectID(newObject->GetID());
					}
					if (ImGui::MenuItem("Button"))
					{
						GameObject *newObject = FL::CreateGameObject(-1);
						newObject->AddButton();
						newObject->SetName("Button(" + std::to_string(newObject->GetID()) + ")");
						SetFocusedGameObjectID(newObject->GetID());
					}
					if (ImGui::MenuItem("Camera"))
					{
						GameObject *newObject = FL::CreateGameObject(-1);
						newObject->AddCamera();
						newObject->SetName("Camera(" + std::to_string(newObject->GetID()) + ")");
						SetFocusedGameObjectID(newObject->GetID());
					}
					if (ImGui::MenuItem("Canvas"))
					{
						GameObject *newObject = FL::CreateGameObject(-1);					
						newObject->AddCanvas();
						newObject->SetName("Canvas(" + std::to_string(newObject->GetID()) + ")");
						SetFocusedGameObjectID(newObject->GetID());
					}
					if (ImGui::MenuItem("Animation"))
					{
						GameObject *newObject = FL::CreateGameObject(-1);						
						newObject->AddAnimation();
						Sprite* sprite = newObject->AddSprite();						
						newObject->SetName("Animation(" + std::to_string(newObject->GetID()) + ")");
						SetFocusedGameObjectID(newObject->GetID());
					}
					if (ImGui::MenuItem("Audio"))
					{
						GameObject *newObject = FL::CreateGameObject(-1);
						newObject->AddAudio();
						newObject->SetName("Audio(" + std::to_string(newObject->GetID()) + ")");
						SetFocusedGameObjectID(newObject->GetID());
					}
					if (ImGui::MenuItem("Text"))
					{
						GameObject *newObject = FL::CreateGameObject(-1);						
						newObject->AddText();
						newObject->SetName("Text(" + std::to_string(newObject->GetID()) + ")");
						SetFocusedGameObjectID(newObject->GetID());
					}
					if (ImGui::MenuItem("Script"))
					{
						GameObject *newObject = FL::CreateGameObject(-1);
						newObject->AddScript();
						newObject->SetName("Script(" + std::to_string(newObject->GetID()));
						SetFocusedGameObjectID(newObject->GetID());
					}
					if (ImGui::MenuItem("CharacterController"))
					{
						GameObject *newObject = FL::CreateGameObject(-1);
						
						newObject->AddCharacterController();
						newObject->SetName("CharacterController(" + std::to_string(newObject->GetID()) + ")");
						SetFocusedGameObjectID(newObject->GetID());
					}
					if (ImGui::BeginMenu("Body"))
					{
						if (ImGui::MenuItem("Empty Body"))
						{
							GameObject* newObject = FL::CreateGameObject(-1);
							newObject->AddBody();
							newObject->SetName("Body(" + std::to_string(newObject->GetID()) + ")");
							SetFocusedGameObjectID(newObject->GetID());
						}
						if (ImGui::MenuItem("Box Body"))
						{
							GameObject* newObject = FL::CreateGameObject(-1);
							Body* body = newObject->AddBody();
							body->AddBox();
							newObject->SetName("BoxBody(" + std::to_string(newObject->GetID()) + ")");
							SetFocusedGameObjectID(newObject->GetID());
						}
						if (ImGui::MenuItem("Circle Body"))
						{
							GameObject* newObject = FL::CreateGameObject(-1);
							Body* body = newObject->AddBody();
							body->AddCircle();
							newObject->SetName("CircleBody(" + std::to_string(newObject->GetID()) + ")");
							SetFocusedGameObjectID(newObject->GetID());
						}
						if (ImGui::MenuItem("Capsule Body"))
						{
							GameObject* newObject = FL::CreateGameObject(-1);
							Body* body = newObject->AddBody();
							body->AddCapsule();
							newObject->SetName("CapsuleBody(" + std::to_string(newObject->GetID()) + ")");
							SetFocusedGameObjectID(newObject->GetID());
						}
						if (ImGui::MenuItem("Polygon Body"))
						{
							GameObject* newObject = FL::CreateGameObject(-1);
							Body* body = newObject->AddBody();
							body->AddPolygon();
							newObject->SetName("PolygonBody(" + std::to_string(newObject->GetID()) + ")");
							SetFocusedGameObjectID(newObject->GetID());
						}
						if (ImGui::MenuItem("Chain Body"))
						{
							GameObject* newObject = FL::CreateGameObject(-1);
							Body* body = newObject->AddBody();
							body->AddChain();
							newObject->SetName("ChainBody(" + std::to_string(newObject->GetID()) + ")");
							SetFocusedGameObjectID(newObject->GetID());
						}
						ImGui::EndMenu();
					}
					if (ImGui::MenuItem("Joint Maker"))
					{
						GameObject* newObject = FL::CreateGameObject(-1);
						JointMaker* jointMaker = newObject->AddJointMaker();						
						newObject->SetName("JointMaker(" + std::to_string(newObject->GetID()) + ")");
						SetFocusedGameObjectID(newObject->GetID());
					}
					if (ImGui::MenuItem("TileMap"))
					{
						GameObject* newObject = FL::CreateGameObject(-1);
						newObject->AddTileMap();
						newObject->SetName("TileMap(" + std::to_string(newObject->GetID()) + ")");
						SetFocusedGameObjectID(newObject->GetID());
					}
					ImGui::EndMenu();
				}
				if (ImGui::BeginMenu("Prefabs"))
				{
					std::map<std::string, FL::Prefab> prefabs = FL::F_PrefabManager->GetPrefabs();
					if (prefabs.size() > 0)
					{
						for (std::pair<std::string, FL::Prefab> pair : prefabs)
						{
							if (ImGui::MenuItem(pair.first.c_str()))
							{
								GameObject *instantiatedObject = FL::Instantiate(pair.first, Vector2(0, 0), FL::GetLoadedScene(), -1);
								SetFocusedGameObjectID(instantiatedObject->GetID());
							}
						}
					}
					else
					{
						ImGui::Text("Right click GameObject in hierarchy to create a Prefab");
					}

					ImGui::EndMenu();
				}
				if (ImGui::BeginMenu("Asset files"))
				{
					if (ImGui::MenuItem("Mapping Context"))
					{						
						b_openMappingContextModal = true;
					}
					if (ImGui::MenuItem("Animation"))
					{
						b_openAnimationModal = true;
					}
					if (ImGui::MenuItem("Lua Script"))
					{						
						b_openScriptModal = true;
					}
					if (ImGui::MenuItem("Tile Set"))
					{
						b_openTileSetModal = true;
					}
					ImGui::EndMenu();
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Settings"))
			{
				if (ImGui::MenuItem("Reload Lua Colors"))
				{
					FL::F_AssetManager.CollectColors();					
				}
				if (ImGui::MenuItem("Reload Lua Textures"))
				{
					FL::F_AssetManager.CollectTextures();					
				}
				if (ImGui::MenuItem("Reload Tags"))
				{
					FL::F_AssetManager.CollectTags();
					if (FL::GetLoadedScene() != nullptr)
					{
						for (std::map<long, GameObject>::iterator gameObject = FL::GetLoadedScene()->GetSceneObjects().begin(); gameObject != FL::GetLoadedScene()->GetSceneObjects().end(); gameObject++)
						{
							gameObject->second.GetTagList().UpdateAvailableTags();
						}
					}
					if (FL::F_LoadedProject.GetPersistantGameObjectScene() != nullptr)
					{
						for (std::map<long, GameObject>::iterator gameObject = FL::F_LoadedProject.GetPersistantGameObjectScene()->GetSceneObjects().begin(); gameObject != FL::F_LoadedProject.GetPersistantGameObjectScene()->GetSceneObjects().end(); gameObject++)
						{
							gameObject->second.GetTagList().UpdateAvailableTags();
						}
					}
				}
				ImGui::Separator();
				if (ImGui::BeginMenu("Widgets"))
				{
					if (ImGui::BeginMenu("Scene View Icon Transparency"))
					{					
						FL::RenderSliderInt("##SceneViewIconTransparency", FG_iconTransparency, 1, 0, 255, 300);
						ImGui::EndMenu();
					}
					ImGui::EndMenu();
				}
				ImGui::Separator();
				if (ImGui::MenuItem("Project Settings", NULL))
				{
					FG_b_showSettings = !FG_b_showSettings;
				}
				if (ImGui::MenuItem("Build Project", NULL))
				{
					FL::BuildProject();
				}
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}

		// New Lua Script Modal
		std::string newScriptName = "";
		if (FL::RenderInputModal("Create Lua Script", "Enter a name for the Lua script:", newScriptName, b_openScriptModal))
		{
			FL::CreateNewLuaScript(newScriptName);
			FG_currentDirectory = FL::GetDir("projectDir") + "\\projects\\" + FL::GetFilenameFromPath(FL::F_LoadedProject.GetPath()) + "\\scripts";
		}
		// New Animation Modal
		std::string animationName = "";
		if (FL::RenderInputModal("Create New Animation", "Enter a name for the new Animation file", animationName, b_openAnimationModal))
		{
			std::shared_ptr<Animation::S_AnimationProperties> animationProperties = std::make_shared<Animation::S_AnimationProperties>();
			animationProperties->animationName = animationName;
			FL::CreateNewAnimationFile(animationName);
			SaveAnimationFile(animationProperties, FL::GetDir("animations") + "/" + animationName + ".anm");
			SetFocusedAnimation(animationProperties);
			FG_currentDirectory = FL::GetDir("projectDir") + "\\projects\\" + FL::GetFilenameFromPath(FL::F_LoadedProject.GetPath()) + "\\animations";
			FG_b_showAnimator = true;
			//FG_b_showAnimationPreview = true;
		}
		// New Mapping Context Modal
		std::string mappingContextName = "";
		if (FL::RenderInputModal("Create New Mapping Context", "Enter a name for the new Mapping Context", mappingContextName, b_openMappingContextModal))
		{
			FL::CreateNewMappingContextFile(mappingContextName);
			FL::F_selectedMappingContextName = mappingContextName;
			FG_b_showMappingContextEditor = true;
			FG_currentDirectory = FL::GetDir("projectDir") + "\\projects\\" + FL::GetFilenameFromPath(FL::F_LoadedProject.GetPath()) + "\\mappingContexts";
		}
		// New TileSet Modal
		std::string tileSetName = "";
		if (FL::RenderInputModal("Create New TileSet", "Enter a name for the new TileSet", tileSetName, b_openTileSetModal))
		{
			FL::CreateNewTileSetFile(tileSetName, FL::GetDir("tileSets"));
			FG_b_showTileSetEditor = true;
			FG_currentDirectory = FL::GetDir("projectDir") + "\\projects\\" + FL::GetFilenameFromPath(FL::F_LoadedProject.GetPath()) + "\\tileSets";
		}

		FL::PopMenuStyles();
	}
}