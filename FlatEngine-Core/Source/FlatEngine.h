#pragma once
#include "Application.h"

#include <SDL_events.h>
#include <string>


/*
######################################
######							######
######       FlatEngine         ######
######                          ######
######################################
*/


namespace FlatEngine
{		
	extern std::shared_ptr<Application> F_Application;
	extern std::string ROOT_DIR;
	extern std::string F_RuntimeDirectoriesLuaFilepath;
	extern std::string F_EditorDirectoriesLuaFilepath;

	extern bool F_b_loadNewScene;
	extern std::string F_sceneToBeLoaded;
	extern bool F_b_closeProgram;
	extern bool F_b_closeProgramQueued;

	// Engine
	extern bool Init(int windowWidth, int windowHeight);
	extern void Cleanup();
	extern void HandleEvents(bool& b_quit);
	extern void HandleEngineEvents(SDL_Event event);	

	// Game Loop
	extern void StartGameLoop();
	extern void GameLoopUpdate();
	extern void PauseGameLoop();
	extern void PauseGame();
	extern void StopGameLoop();
	extern float GetElapsedGameTimeInSec();
	extern long GetElapsedGameTimeInMs();
	extern bool GameLoopStarted();
	extern bool GameLoopPaused();
	extern long GetFramesCounted();
	extern float GetDeltaTime();
};