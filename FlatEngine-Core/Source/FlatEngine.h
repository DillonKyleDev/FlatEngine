#pragma once
#include "Application.h"
#include "Animation.h"
#include "CPPScript.h"
#include "RayCast.h"

#include <stdio.h>
#include <string>
#include <map>
#include <sstream>
#include <vector>
#include "SDL.h"
#include <SDL_syswm.h> // Uint32
#include <SDL_image.h>
#include "SDL_ttf.h"
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"
#include "json.hpp"

using json = nlohmann::json;
using namespace nlohmann::literals;

#define SOL_ALL_SAFETIES_ON 1
#include <sol.hpp>


/*
######################################
######							######
######         Engine           ######
######                          ######
######################################
*/

//widthIncrement   = tileWidthInGrid;     
//heightIncrement  = tileHeightInGrid;
//maxHeight        = tileWidth;        
//offsetIncrementX = tileWidthInPx;        
//offsetIncrementY = tileHeight;
//maxWidth         = offsetIncrementX;
//maxHeight        = offsetIncrementY;

class GameManager;

namespace FlatEngine
{
	class WindowManager;
	class Vector2;
	class Vector4;
	class AssetManager;
	class GameLoop;
	class Project;
	class Scene;
	class Process;
	class Logger;
	class SceneManager;
	class PrefabManager;
	class MappingContext;
	class Texture;
	class GameObject;
	class TagList;
	class Camera;
	class Collider;
	class ScriptComponent;
	class Sound;
	class RigidBody;
	class Script;
	class CharacterController;	
	class ECSManager;
	class TileSet;

	enum F_CURSOR_MODE {
		TRANSLATE,
		SCALE,
		ROTATE,
		TILE_BRUSH,
		TILE_ERASE,
		TILE_COLLIDER_DRAW,
		TILE_MULTISELECT,
		TILE_MOVE,
	};

	extern std::shared_ptr<WindowManager> F_Window;
	extern std::shared_ptr<Application> F_Application;
	extern sol::state F_Lua;
	extern std::map<std::string, sol::protected_function> F_LoadedSceneScriptFiles;
	extern std::map<std::string, sol::protected_function> F_LoadedPersistantScriptFiles;

	extern long F_FocusedGameObjectID;

	extern std::vector<std::string> F_TagsAvailable;

	extern bool F_b_projectSelected;

	extern std::string F_RuntimeDirectoriesLuaFilepath;
	extern std::string F_EditorDirectoriesLuaFilepath;

	extern F_CURSOR_MODE F_CursorMode;
	extern bool F_b_closeProgram;
	extern bool F_b_closeProgramQueued;

	// Assets loaded from files on application start	
	extern std::vector<std::string> F_luaScriptPaths;
	extern std::vector<std::string> F_luaScriptNames;
	extern std::map<std::string, std::string> F_LuaScriptsMap;
	extern std::vector<std::string> F_CPPScriptNames;
	extern bool F_b_shouldReloadProjectFiles;
	extern AssetManager F_AssetManager;
	extern std::vector<std::string> F_selectedFiles;
	extern std::vector<MappingContext> F_MappingContexts;
	extern std::string F_selectedMappingContextName;
	extern std::shared_ptr<PrefabManager> F_PrefabManager;
	extern std::vector<TileSet> F_TileSets;
	extern std::string F_selectedTileSetToEdit;
	extern std::pair<std::string, int> F_tileSetAndIndexOnBrush;

	extern std::vector<std::string> F_KeyBindingsAvailable;

	extern bool F_b_loadNewScene;
	extern std::string F_sceneToBeLoaded;

	// Drag/Drop IDs
	extern std::string F_fileExplorerTarget;
	extern std::string F_hierarchyTarget;

	// Flags
	extern ImGuiChildFlags F_childFlags;
	extern ImGuiChildFlags F_autoResizeChildFlags;
	extern ImGuiChildFlags F_resizeChildFlags;
	extern ImGuiChildFlags F_headerFlags;
	extern ImGuiTableFlags F_tableFlags;
	extern ImGuiTableFlags F_resizeableTableFlags;
	extern ImGuiInputTextFlags F_inputFlags;

	extern float F_childPadding;
	extern bool F_b_sceneViewFocused;

	// Controller Management
	extern std::vector<SDL_Joystick*> F_gamepads;
	const int F_JOYSTICK_DEAD_ZONE = 4000;

	// Managers
	extern SceneManager F_SceneManager;
	extern Logger F_Logger;
	extern std::vector<Process> F_ProfilerProcesses;
	extern Sound F_SoundController;
	extern TTF_Font* F_fontCinzel;

	extern int F_maxSpriteLayers;
	extern float F_spriteScaleMultiplier;
	extern float F_pixelsPerGridSpace;

	// Game view
	extern float F_GAME_VIEWPORT_WIDTH;
	extern float F_GAME_VIEWPORT_HEIGHT;
	extern float F_xGameCenter;
	extern float F_yGameCenter;
	extern Vector2 F_gameViewCenter;
	extern Vector2 F_gameViewGridStep;

	extern Camera* F_primaryCamera;

	// Scene View
	extern Vector2 F_sceneViewCenter;
	extern Vector2 F_sceneViewGridStep; 


	// Collision Detection
	extern std::vector<std::pair<Collider*, Collider*>> F_ColliderPairs;
	extern void UpdateColliderPairs();
	extern RayCast* CastRay(Vector2 initialPos, Vector2 direction, float increment, float length, void(*callback)(RayCast*, GameObject*), bool b_visible = false);

	extern bool LoadFonts();
	extern void FreeFonts();
	extern std::string GetDir(std::string dirName);
	extern std::string GetFilePath(std::string fileName);
	extern std::shared_ptr<Texture> GetTextureObject(std::string textureName);
	extern SDL_Texture* GetTexture(std::string textureName);
	extern Vector4 GetColor(std::string colorName);
	extern Uint32 GetColor32(std::string colorName);

	// ImGui + Events
	extern void SetupImGui();
	extern void RestartImGui();
	extern void QuitImGui();
	extern void SetImGuiColors();
	extern void HandleEvents(bool& b_quit);
	extern void HandleEngineEvents(SDL_Event event);
	extern void HandleContextEvents(MappingContext& context, SDL_Event event, std::vector<std::string>& firedKeys);
	extern void RemapInputAction(std::string contextName, std::string inputAction, Uint32 timeoutTime = 0);

	// Lua / Sol
	enum LuaEventFunction {
		OnBoxCollision,
		OnBoxCollisionEnter,
		OnBoxCollisionLeave,
		OnCircleCollision,
		OnCircleCollisionEnter,
		OnCircleCollisionLeave,
		OnButtonMouseOver,
		OnButtonMouseEnter,
		OnButtonMouseLeave,
		OnButtonLeftClick,
		OnButtonRightClick
	};
	const std::string F_LuaEventNames[11] = {
		"OnBoxCollision",
		"OnBoxCollisionEnter",
		"OnBoxCollisionLeave",
		"OnCircleCollision",
		"OnCircleCollisionEnter",
		"OnCircleCollisionLeave",
		"OnButtonMouseOver",
		"OnButtonMouseEnter",
		"OnButtonMouseLeave",
		"OnButtonLeftClick",
		"OnButtonRightClick"
	};
	extern void InitLua();
	extern void RegisterLuaFunctions();
	extern void RegisterLuaTypes();
	extern bool InitLuaScript(Script* script, std::map<std::string, sol::protected_function>& scriptTracker);
	extern bool ReadyScriptFile(std::string scriptToLoad, std::string& message);
	extern void RunLuaFuncOnAllScripts(std::string functionName);
	extern void RunLuaFuncOnSingleScript(Script* script, std::string functionName);
	extern void RunSceneAwakeAndStart();	
	extern void RunPersistantAwakeAndStart();
	extern void RetrieveLuaScriptPaths();
	extern bool CheckLuaScriptFile(std::string filePath);
	extern void LoadLuaGameObject(GameObject* object, std::string scriptName);
	// Lua/Sol Events
	template <class T>
	extern void CallVoidLuaFunction(std::string functionName, T param);
	template <class T>
	extern void CallVoidLuaFunction(std::string functionName);
	extern void CallLuaCollisionFunction(GameObject* caller, Collider* collidedWith, LuaEventFunction eventFunc);
	extern void CallLuaButtonEventFunction(GameObject* caller, LuaEventFunction eventFunc);
	extern void CallLuaAnimationEventFunction(GameObject* caller, std::string eventFunc);
	extern void CallLuaAnimationEventFunction(GameObject* caller, std::string eventFunc, Animation::S_EventFunctionParam param1, Animation::S_EventFunctionParam param2 = Animation::S_EventFunctionParam(), Animation::S_EventFunctionParam param3 = Animation::S_EventFunctionParam(), Animation::S_EventFunctionParam param4 = Animation::S_EventFunctionParam(), Animation::S_EventFunctionParam param5 = Animation::S_EventFunctionParam());
	extern void CallLuaButtonOnClickFunction(GameObject* caller, std::string eventFunc);
	extern void CallLuaButtonOnClickFunction(GameObject* caller, std::string eventFunc, Animation::S_EventFunctionParam param1, Animation::S_EventFunctionParam param2 = Animation::S_EventFunctionParam(), Animation::S_EventFunctionParam param3 = Animation::S_EventFunctionParam(), Animation::S_EventFunctionParam param4 = Animation::S_EventFunctionParam(), Animation::S_EventFunctionParam param5 = Animation::S_EventFunctionParam());
	
	// C++ Scripts
	extern void RetrieveCPPScriptNames();
	extern std::shared_ptr<CPPScript> InstantiateCPPScript(std::string scriptName);


	// Profiler
	extern void AddProfilerProcess(std::string name);
	extern void AddProcessData(std::string processName, float data);
	extern void RemoveProfilerProcess(std::string toRemove);

	// Project Management
	extern Project F_LoadedProject;
	extern void SetLoadedProject(Project &loadedProject);
	extern Project& GetLoadedProject();
	extern void LoadGameProject(std::string path, json& projectJson);
	extern void BuildProject();
	extern void SetProjectLoadedScenePath(std::string scenePath);
	extern GameObject* CreatePersistantGameObject(long parentID = -1, long myID = -1);
	extern std::map<long, GameObject>& GetPersistantObjects();

	// Player Management
	extern GameObject* GetPlayerObject();
	extern GameObject* F_PlayerObject;

	// Scene Management
	extern Scene* GetLoadedScene();
	extern Scene* CreateNewScene();
	extern void SaveScene(Scene* scene, std::string filePath);
	extern void SaveCurrentScene();
	extern void QueueLoadScene(std::string scenePath);
	extern void LoadScene(std::string actualPath, std::string pointTo = "");
	extern std::string GetLoadedScenePath();
	extern std::map<long, GameObject> &GetSceneObjects();
	extern GameObject* CreateGameObject(long parentID = -1, long myID = -1, Scene* scene = nullptr);
	extern void DeleteGameObject(long sceneObjectID);
	extern Component* GetObjectComponent(long objectID, ComponentTypes type);
	extern GameObject* GetObjectByID(long objectID);
	extern GameObject* GetObjectByName(std::string name);
	extern GameObject* GetObjectByTag(std::string tag);

	extern void SetPrimaryCamera(Camera* primaryCamera);
	extern void RemovePrimaryCamera();
	extern Camera* GetPrimaryCamera();

	extern long GetNextComponentID();
	extern long GetNextGameObjectID();

	// SDL
	extern Vector2 AddImageToDrawList(SDL_Texture* texture, Vector2 positionInGrid, Vector2 relativeCenterPoint, float textureWidthPx, float textureHeightPx, Vector2 offset, Vector2 scale, bool b_scalesWithZoom, float zoomMultiplier, ImDrawList* drawList, float rotation = 0, ImU32 addColor = GetColor32("white"), Vector2 uvStart = Vector2(0, 0), Vector2 uvEnd = Vector2(1, 1));
	extern void SetMusicVolume(int volume);
	extern void SetEffectsVolume(int volume);

	// Engine
	extern bool Init(int windowWidth, int windowHeight);
	extern void CloseProgram();
	extern Uint32 GetEngineTime();
	extern void ManageControllers();

	// Controls Context Management
	extern void SaveMappingContext(std::string path, MappingContext context);
	extern void InitializeMappingContexts();
	extern MappingContext* GetMappingContext(std::string contextName);

	// Prefabs
	extern void CreatePrefab(std::string path, GameObject& gameObject);
	extern void InitializePrefabs();
	extern GameObject *Instantiate(std::string prefabName, Vector2 position, Scene* scene, long parentID = -1, long ID = -1);
	//extern std::map<std::string, Prefab> GetPrefabs();

	// Logging Prettification
	extern void LogError(std::string line = "", std::string from = "[C++]");
	extern void LogString(std::string line = "", std::string from = "[C++]");
	extern void LogFloat(float var, std::string line = "", std::string from = "[C++]");
	extern void LogDouble(double var, std::string line = "", std::string from = "[C++]");
	extern void LogLong(long var, std::string line = "", std::string from = "[C++]");
	extern void LogInt(int var, std::string line = "", std::string from = "[C++]");
	extern void LogVector2(Vector2 vector, std::string line = "", std::string from = "[C++]");
	extern void LogSeparator();
	extern void DrawRectangle(Vector2 startingPoint, Vector2 endingPoint, Vector2 canvasP0, Vector2 canvasSize, Vector4 color, float thickness, ImDrawList* drawList);
	extern void DrawLine(Vector2 startingPoint, Vector2 endingPoint, Vector4 color, float thickness, ImDrawList* drawList);
	extern void DrawLineInScene(Vector2 startingPoint, Vector2 endingPoint, Vector4 color, float thickness);
	extern void DrawRectangleFromLines(Vector2* corners, Vector4 color, float thickness, ImDrawList* drawList);
	extern void DrawCircle(Vector2 center, float radius, Vector4 color, ImDrawList* drawList, float thickness = 1, int segments = 0);
	extern void DrawPoint(Vector2 point, Vector4 color, ImDrawList* drawList);
	extern void DebugRectangle(Vector2 startingPoint, Vector2 endPoint, Vector4 color, float thickness, ImDrawList* drawList);
	extern void SaveDebugLogToFile(std::string path = "");
	extern Vector2 ConvertWorldToScreen(Vector2 positionInWorld, Vector2 relativeCenterPoint, float zoomMultiplier);

	// Game View
	extern void Game_RenderView(bool b_inRuntime = false);
	extern void Game_RenderObjects(Vector2 canvasP0, Vector2 canvasSize);
	extern void Game_RenderObject(GameObject self, Vector2 canvasP0, Vector2 canvasSize, ImDrawList* drawList, ImDrawListSplitter* drawSplitter, Vector2 cameraPosition, float cameraWidth, float cameraHeight);

	// Game Loop Prettification
	extern void StartGameLoop();
	extern void GameLoopUpdate();
	extern void PauseGameLoop();
	extern void PauseGame();
	extern void StopGameLoop();
	extern float GetEllapsedGameTimeInSec();
	extern long GetEllapsedGameTimeInMs();
	extern bool GameLoopStarted();
	extern bool GameLoopPaused();
	extern long GetFramesCounted();
	extern float GetDeltaTime();

	// File Explorer
	extern void CreateNewLuaScript(std::string fileName, std::string path = "");
	extern void CreateNewCPPScript(std::string fileName, std::string path = "");
	extern std::string CreateNewSceneFile(std::string fileName, std::string path = "");
	extern void CreateNewAnimationFile(std::string fileName, std::string path = "");
	extern void AddMappingContext(std::string path);
	extern void CreateNewMappingContextFile(std::string fileName, std::string path = "");
	extern void CreateNewTileSetFile(std::string fileName, std::string path);
	extern GameObject* CreateAssetUsingFilePath(std::string filePath, Vector2 position);

	// Animation
	extern void SaveAnimationFile(std::shared_ptr<Animation::S_AnimationProperties> propertiesObject, std::string path);
	extern std::shared_ptr<Animation::S_AnimationProperties> LoadAnimationFile(std::string path);

	// TileSet / TileMap
	extern void SaveTileSet(TileSet tileSet);
	extern void AddTileSet(std::string path);
	extern void InitializeTileSets();
	extern TileSet* GetTileSet(std::string tileSetName);

	//////////////////////
	// Helper Functions //
	//////////////////////

	// Random
	extern int GetRandInt(int low, int high);
	extern float GetRandFloat(float low, float high);

	// Json parsing
	extern json CreateJsonFromObject(GameObject gameObject);
	extern GameObject* CreateObjectFromJson(json objectJson, Scene* scene);
	extern std::string CheckJsonString(json obj, std::string checkFor, std::string loadedName);
	extern std::string CheckJsonString(json obj, std::string checkFor, std::string loadedName, std::string& errorMessage);
	extern float CheckJsonFloat(json obj, std::string checkFor, std::string loadedName);
	extern float CheckJsonFloat(json obj, std::string checkFor, std::string loadedName, std::string& errorMessage);
	extern int CheckJsonInt(json obj, std::string checkFor, std::string loadedName);
	extern int CheckJsonInt(json obj, std::string checkFor, std::string loadedName, std::string& errorMessage);
	extern long CheckJsonLong(json obj, std::string checkFor, std::string loadedName);
	extern long CheckJsonLong(json obj, std::string checkFor, std::string loadedName, std::string& errorMessage);
	extern double CheckJsonDouble(json obj, std::string checkFor, std::string loadedName);
	extern double CheckJsonDouble(json obj, std::string checkFor, std::string loadedName, std::string& errorMessage);
	extern bool CheckJsonBool(json obj, std::string checkFor, std::string loadedName);
	extern bool CheckJsonBool(json obj, std::string checkFor, std::string loadedName, std::string& errorMessage);
	extern bool JsonContains(json obj, std::string checkFor, std::string loadedName);

	// ImGui Wrappers
	extern void BeginImGuiRender();
	extern void EndImGuiRender();
	extern void SetNextViewportToFillWindow();
	extern void MoveScreenCursor(float x, float y);
	extern void RenderSeparator(float topPadding, float bottomPadding);
	extern void RenderSubTitle(std::string title);
	extern void BeginWindow(std::string name, Vector4 bgColor = GetColor("windowBg"));
	extern void BeginWindow(std::string name, bool& b_isOpen, Vector4 bgColor = GetColor("windowBg"));
	extern void BeginWindow(std::string name, bool& b_isOpen, ImGuiWindowFlags flags, Vector4 bgColor = GetColor("windowBg"));
	extern void EndWindow();
	extern void BeginWindowChild(std::string title, Vector4 bgColor = GetColor("outerWindow"), ImGuiWindowFlags flags = 0);
	extern void BeginResizeWindowChild(std::string title, Vector4 bgColor = GetColor("outerWindow"), ImGuiWindowFlags flags = 0);
	extern void EndWindowChild();
	extern void PushWindowStyles(Vector4 bgColor = GetColor("windowBg"));
	extern void PopWindowStyles();
	extern void PushComboStyles();
	extern void PopComboStyles();
	extern void PushMenuStyles();
	extern void PopMenuStyles();
	extern void PushTableStyles();
	extern void PopTableStyles();
	extern bool PushTable(std::string ID, int columns, ImGuiTableFlags flags = F_tableFlags);
	extern bool RenderFloatDragTableRow(std::string ID, std::string fieldName, float& value, float increment, float min, float max);
	extern bool RenderIntSliderTableRow(std::string ID, std::string fieldName, int& value, int increment, int min, int max);
	extern bool RenderTagListTableRow(std::string ID, std::string fieldName, TagList& tagList);
	extern bool RenderIntDragTableRow(std::string ID, std::string fieldName, int& value, float speed, int min, int max);
	extern bool RenderCheckboxTableRow(std::string ID, std::string fieldName, bool& _value);
	extern void RenderSelectableTableRow(std::string ID, std::string fieldName, std::vector<std::string> options, int& currentOption);
	extern bool RenderInputTableRow(std::string ID, std::string fieldName, std::string& value, bool b_canOpenFiles = false);
	extern void RenderTextTableRow(std::string ID, std::string fieldName, std::string value, std::string value2 = "");
	extern void PopTable();
	extern bool RenderInput(std::string ID, std::string label, std::string& value, bool b_canOpenFiles = false, float inputWidth = -1, ImGuiInputTextFlags flags = 0);
	extern bool DropInput(std::string ID, std::string label, std::string displayValue, std::string dropTargetID, int& droppedValue, std::string tooltip = "", float inputWidth = -1);
	extern bool DropInputCanOpenFiles(std::string ID, std::string label, std::string displayValue, std::string dropTargetID, int& droppedValue, std::string& openedFileValue, std::string tooltip = "", float inputWidth = -1);
	extern bool RenderButton(std::string text, Vector2 size = Vector2(0, 0), float rounding = 1, Vector4 color = GetColor("button"), Vector4 hoverColor = GetColor("buttonHovered"), Vector4 activeColor = GetColor("buttonActive"));
	extern bool RenderImageButton(std::string ID, SDL_Texture* texture, Vector2 size = Vector2(16, 16), float rounding = 1, Vector4 bgColor = GetColor("imageButton"), Vector4 tint = GetColor("imageButtonTint"), Vector4 hoverColor = GetColor("imageButtonHovered"), Vector4 activeColor = GetColor("imageButtonActive"), Vector2 uvStart = Vector2(0,0), Vector2 uvEnd = Vector2(1, 1), Vector2 padding = Vector2(-1, -1));
	extern bool RenderDragFloat(std::string text, float width, float& value, float increment, float min, float max, ImGuiSliderFlags flags = 0, std::string bgColor = "");
	extern bool RenderDragInt(std::string text, float width, int& value, float speed, int min, int max, ImGuiSliderFlags flags = 0, std::string bgColor = "");
	extern bool RenderSliderFloat(std::string label, float& value, float increment = 0.1f, float min = 0.0f, float max = 1000, float width = -1, int digitsAfterDecimal = 3);
	extern bool RenderSliderInt(std::string label, int& value, int increment = 1, int min = 0, int max = 1000, float width = -1);
	extern void PushSliderStyles();
	extern void PopSliderStyles();
	extern bool RenderCheckbox(std::string text, bool& b_toCheck);
	extern void RenderSectionHeader(std::string headerText, float height = 0);
	extern bool RenderInvisibleButton(std::string ID, Vector2 startingPoint, Vector2 size, bool b_allowOverlap = true, bool b_showRect = false, ImGuiButtonFlags flags = ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight);
	extern bool RenderCombo(std::string ID, std::string displayedValue, std::vector<std::string> options, int& currentOption, float width = -1);
	extern bool RenderSelectable(std::string ID, std::vector<std::string> options, int& currentOption);
	extern bool PushTreeList(std::string ID);
	extern void RenderTreeLeaf(std::string name, std::string& nodeClicked);
	extern void PopTreeList();
	extern void RenderTextToolTip(std::string text);
	extern void BeginToolTip(std::string title);
	extern void EndToolTip();
	extern void RenderToolTipText(std::string label, std::string text);
	extern void RenderToolTipFloat(std::string label, float data);
	extern void RenderToolTipLong(std::string label, long data);
	extern void RenderToolTipLongVector(std::string label, std::vector<long> data);
	extern bool RenderInputModal(std::string label, std::string description, std::string& inputValue, bool& b_openModal);
	extern bool RenderConfirmModal(std::string label, std::string description, bool& b_openModal);

	extern bool AreCollidingViewport(Vector4 ObjectA, Vector4 ObjectB);
	extern Vector2 Lerp(Vector2 startPos, Vector2 endPos, float ease);

	// File managing
	extern std::string OpenSaveFileExplorer();
	extern std::string OpenLoadFileExplorer();
	extern std::string GetFilenameFromPath(std::string path, bool b_keepExtension = false);
	extern std::string MakePathRelative(std::string filePath);
	extern std::string GetCurrentDir();
	extern bool DoesFileExist(std::string filePath);
	extern bool FilepathHasExtension(std::string filePath, std::string extension);
	extern json LoadFileData(std::string filePath);
	extern void DeleteFileUsingPath(std::string filePath);
	extern std::vector<std::string> FindAllFilesWithExtension(std::string dirPath, std::string extension);
	extern std::string GetFilePathUsingFileName(std::string dirPath, std::string name);
	extern void WriteStringToFile(std::string path, std::string text);
};