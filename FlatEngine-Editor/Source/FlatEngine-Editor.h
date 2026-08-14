#include "Application.h"
#include "EntryPoint.h"
#include "GameLoop.h"

#include <SDL_mixer.h>
#include <string>
#include <memory>

namespace FL = FlatEngine;


class EditorGameLoop : public FL::GameLoop
{
public:
	EditorGameLoop();	
	void Start();
	void Stop();
	void Update();
private:
	std::string m_startedScenePath;
	std::string m_startedPersistentScriptsPath;
};


class EditorApplication : public FL::Application
{
public:
	EditorApplication();
    void Init();
	void Run();	
	void Cleanup();
	void BeginRender();
	void EndRender();
	void OnLoadScene(std::string sceneName);

private:
	bool m_b_recreateWindow;
	std::string m_startupProject;
};

// Define our CreateApplication() for the Editor
extern std::shared_ptr<FL::Application> FL::CreateApplication(int argc, char** argv);