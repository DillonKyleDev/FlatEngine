#pragma once
#include "tools/Vector2.h"

#include <memory>
#include <SDL_events.h>
#include <string>
#include <vector>


/*
######################################
######							######
######       FlatEngine         ######
######                          ######
######################################
*/

class Application;

namespace FlatEngine
{		
	extern std::shared_ptr<Application> application;
	extern std::string rootDir;
	extern bool b_loadNewScene;
	extern std::string sceneToBeLoaded;	
	extern bool b_closeProgramQueued;
	extern std::vector<SDL_Event> events;
	extern Vector2 mouseDelta;
	extern Vector2 mousePos;
	extern Vector2 lastMousePos;

	extern bool Init(int windowWidth, int windowHeight);
	extern void Cleanup();
	extern void HandleEvents();	
};