#pragma once
#include "Vector2.h"
#include "Vector4.h"
#include "Animation.h"
#include "Texture.h"

#include <sstream>
#include <memory>
#include <map>
#include <filesystem>
#include "imgui.h"
#include "SDL.h"

/*
######################################
######							######
######     Gui Declarations     ######
######                          ######
######################################
*/

namespace FL = FlatEngine;

namespace FlatEngine 
{
	class Vector2;
	class Vector4;
	class GameObject;
	class TagList;
	class Texture;
	class Project;
	class SceneManager;
	class Scene;
	class Process;
	class Component;
	class Transform;
	class Sprite;
	class Camera;
	class Script;
	class Button;
	class Canvas;
	class Animation;
	class Audio;
	class Text;
	class CharacterController;
	class BoxCollider;
	class CircleCollider;
	class RigidBody;
	class TileMap;
	class TileSet;
}

using Vector2 = FL::Vector2;
using Vector4 = FL::Vector4;
using GameObject = FL::GameObject;
using TagList = FL::TagList;
using Texture = FL::Texture;
using Animation = FL::Animation;
using Project = FL::Project;
using SceneManager = FL::SceneManager;
using Scene = FL::Scene;
using Process = FL::Process;
using Component = FL::Component;
using Transform = FL::Transform;
using Sprite = FL::Sprite;
using Camera = FL::Camera;
using Script = FL::Script;
using Button = FL::Button;
using Canvas = FL::Canvas;
using Animation = FL::Animation;
using Audio = FL::Audio;
using Text = FL::Text;
using CharacterController = FL::CharacterController;
using BoxCollider = FL::BoxCollider;
using CircleCollider = FL::CircleCollider;
using RigidBody = FL::RigidBody;
using TileMap = FL::TileMap;
using TileSet = FL::TileSet;

namespace FlatGui
{
	extern ImDrawList* FG_DrawList;

	// Project Management 
	extern void CreateNewProject(std::string projectName);
	extern void CreateProjectDirectory(std::string path);
	extern void LoadProject(std::string path);
	extern void SaveProject(Project& project, std::string path);
	extern void SaveCurrentProject();	

	// File Explorer
	extern std::map<std::string, std::shared_ptr<Texture>> FG_visibleThumbnails;
	extern std::string FG_currentDirectory;

	// Focused GameObject
	extern long FG_FocusedGameObjectID;
	extern void SetFocusedGameObjectID(long ID);
	extern long GetFocusedGameObjectID();

	// Animator
	extern std::shared_ptr<FL::Animation::S_AnimationProperties> FG_FocusedAnimation;
	extern GameObject *objectForFocusedAnimation;
	extern std::shared_ptr<FL::Animation::S_Property> FG_SelectedKeyFrameToEdit;
	extern long FG_previewAnimationStartTime;
	extern long FG_previewAnimationTime;
	extern bool FG_b_playPreviewAnimation;
	extern void SetFocusedAnimation(std::shared_ptr<FL::Animation::S_AnimationProperties> animation);
	extern std::shared_ptr<FL::Animation::S_AnimationProperties> GetFocusedAnimation();
	// Global Animation Event Functions
	extern void DestroySelf(std::shared_ptr<FL::GameObject> thisObject);

	// Scene view
	extern Vector2 FG_sceneViewportCenter;
	extern float SCENE_VIEWPORT_WIDTH;
	extern float SCENE_VIEWPORT_HEIGHT;
	extern float DYNAMIC_VIEWPORT_WIDTH;
	extern float DYNAMIC_VIEWPORT_HEIGHT;
	extern bool FG_b_firstSceneRenderPass;
	extern bool FG_b_sceneHasBeenSet;
	extern Vector2 FG_sceneViewGridStep;
	extern Vector2 FG_sceneViewScrolling;
	extern Vector2 FG_sceneViewCenter;
	extern bool FG_b_sceneViewLockedOnObject;
	extern long FG_sceneViewLockedObjectID;

	// TileMap
	extern std::string FG_currentSelectedColliderArea;
	extern std::vector<std::pair<Vector2, Vector2>> FG_collisionAreasBuffer;

	// Settings
	extern int FG_iconTransparency;
	extern bool FG_b_clearBufferEveryFrame;

	// Hierarchy
	extern std::map<long, bool> FG_leafExpandedTracker;
	extern std::map<std::string, bool> FG_fileExplorerLeafTracker;

	// Window Visibility
	extern bool FG_b_showDemoWindow;
	extern bool FG_b_showScriptEditor;
	extern bool FG_b_showTileSetEditor;
	extern bool FG_b_showFileExplorer;
	extern bool FG_b_showSceneView;
	extern bool FG_b_showGameView;
	extern bool FG_b_showHierarchy;
	extern bool FG_b_showPersistantHierarchy;
	extern bool FG_b_showInspector;
	extern bool FG_b_showAnimator;
	extern bool FG_b_showAnimationPreview;
	extern bool FG_b_showKeyFrameEditor;
	extern bool FG_b_showLogger;
	extern bool FG_b_showProfiler;
	extern bool FG_b_showMappingContextEditor;
	extern bool FG_b_showSettings;

	extern void Init();

	// Gui Rendering
	extern void SetupProfilerProcesses();
	extern void RunOnceAfterInitialization();

	// Viewports
	extern void RenderProjectHub(bool& b_projectSelected, std::string& projectPath);
	extern void MainMenuBar();
	extern void AddViewports();
	extern void RenderToolbar();
	extern void RenderHierarchy();
	extern void RenderPersistantHierarchy();
	extern void RenderInspector();
	extern void Scene_RenderView();
	extern void RenderAnimator();
	extern void RenderAnimationPreview();
	extern void RenderAnimationTimelineGrid(Vector2& zeroPoint, Vector2 scrolling, Vector2 canvasP0, Vector2 canvasP1, Vector2 canvasSize, float gridStep);
	extern void RenderKeyFrameEditor();
	extern void RenderLog();
	extern void RenderProfiler();
	extern void RenderMappingContextEditor();
	extern void RenderSettings();
	extern void Cleanup();
	extern void RenderGridView(Vector2& centerPoint, Vector2& scrolling, bool b_weightedScroll, Vector2 canvasP0, Vector2 canvasP1, Vector2 canvasSize, Vector2& gridStep, Vector2 centerOffset, bool b_showAxis = true);
	extern void RenderViewObjects(std::map<long, GameObject>& objects, Vector2 centerPoint, Vector2 canvasP0, Vector2 canvasSize, float gridStep);
	extern void RenderViewObject(GameObject& self, Vector2 scrolling, Vector2 canvasP0, Vector2 canvasSize, float gridStep, ImDrawList* drawList, ImDrawListSplitter* drawSplitter);
	extern void RenderTransformArrowWidget();
	extern void RenderFileExplorer();
	extern void RenderFilesTopBar();
	extern void RenderDirNodes(std::string dir);
	extern void RenderDirNode(std::filesystem::path fsPath);
	extern void RenderDirItems();
	extern void RenderFileIcon(std::filesystem::path fsPath, Vector2 currentPos);
	extern void OpenFileContextually(std::filesystem::path fsPath);
	extern void RenderTileSetEditor();
	extern void RenderTileSetEditorTile(std::pair<int, std::pair<Vector2, Vector2>> tile, Vector2 tileSize, Vector2& scrolling, Vector2 canvasP0, Vector2 canvasSize, Vector2& step, TileSet* tileSet);
	extern void RenderCursorModeButtons();
	extern void RenderGameTimeStats();
	extern void RenderStatsOnGameView();
	extern void RenderScriptEditor();

	// *** Don't forget to add flags to the window for preventing scrolling *** ImGuiWindowFlags flags = 8 | 16 or ImGuiWindowFlags_NoScrollbar 
	extern void AddSceneViewMouseControls(std::string buttonID, Vector2 startPos, Vector2 size, Vector2& scrolling, Vector2 centerPoint, Vector2& gridStep, Uint32 rectColor = ImGui::GetColorU32(Vector4(0,0,0,0)), bool b_filled = false, ImGuiButtonFlags buttonFlags = 0, bool b_allowOverlap = true, bool b_weightedScroll = false, float zoomMultiplier = 1, float minGridStep = 5, float maxGridStep = 100);
	// Component Wrappers	
	extern bool RenderIsActiveCheckbox(bool& b_isActive);
	extern void BeginComponent(Component* component, FL::Component*& queuedForDelete, std::string typeNameOverride = "");
	extern void EndComponent(Component* component);
	extern void RenderTransformComponent(Transform* transform);
	extern void RenderSpriteComponent(Sprite* sprite);
	extern bool RenderPivotSelectionButtons(std::string componentType, FL::Pivot& pivot);
	extern void RenderCameraComponent(Camera* camera);
	extern void RenderScriptComponent(Script* script);
	extern void RenderButtonComponent(Button* button);
	extern void RenderCanvasComponent(Canvas* canvas);
	extern void RenderAnimationComponent(Animation* animation);
	extern void RenderAudioComponent(Audio* audio);
	extern void RenderTextComponent(Text* text);
	extern void RenderCharacterControllerComponent(CharacterController* characterController);
	extern void RenderBoxColliderComponent(BoxCollider* boxCollider, TileMap* tileMap = nullptr, std::string collisionAreaName = "");
	extern void RenderCircleColliderComponent(CircleCollider* circleCollider);
	extern void RenderRigidBodyComponent(RigidBody* rigidBody);
	extern void RenderTileMapComponent(TileMap* tileMap);

	// SceneView
	extern void RenderSceneViewTooltip();
	// Hierarchy
	extern void AddObjectToHierarchy(GameObject& currentObject, const char* charName, int& nodeClicked, long& queuedForDelete, float indent);
	// Profiler
	void Sparkline(const char* ID, const float* values, int count, float min, float max, int offset, const Vector4& color, const Vector2& size);
}