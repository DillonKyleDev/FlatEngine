#pragma once
#include "FlatEngine.h"
#include "FlatGui.h"
#include "EntryPoint.h"
#include "Application.h"
#include "GameLoop.h"
#include "AssetManager.h"
#include "WindowManager.h"
#include "Project.h"
#include "Process.h"
#include "GameObject.h"
#include "Component.h"
#include "Script.h"
#include "PrefabManager.h"
#include "Scene.h"

#include <vector>
#include <SDL_mixer.h>
#include <string>
#include <memory>
#include "implot.h"


namespace FL = FlatEngine;

using GameObject = FL::GameObject;
using Component = FL::Component;
using Script = FL::Script;


int main(int argc, char* args[])
{
	// Initializes FlatEngine
	return FL::Main(argc, args);
}


// Define our Applications main GameLoop
class EditorGameLoop : public FL::GameLoop
{
public:
	EditorGameLoop() 
	{
		m_startedScenePath = "";
		m_startedPersistantScenePath = "";
	};
	~EditorGameLoop() {};

	void Start()
	{
		FL::AddProfilerProcess("GameLoop (variable executions)");		
		FL::AddProfilerProcess("Not GameLoop");
		FL::AddProfilerProcess("Collision Testing");	
		m_startedScenePath = FL::GetLoadedScenePath();
		m_startedPersistantScenePath = FL::GetLoadedProject().GetPersistantGameObjectsScenePath();
		FL::SaveScene(FL::GetLoadedScene(), "..\\engine\\tempFiles\\" + FL::GetLoadedScene()->GetName() + ".scn");
		if (m_startedPersistantScenePath != "" && FL::GetLoadedProject().GetPersistantGameObjectScene() != nullptr)
		{
			FL::F_LoadedProject.SavePersistantScene("..\\engine\\tempFiles\\" + FL::GetLoadedProject().GetPersistantGameObjectScene()->GetName() + ".scn");
		}
		FL::GameLoop::Start();
	};
	void Stop()
	{
		FL::RemoveProfilerProcess("GameLoop (variable executions)");
		FL::RemoveProfilerProcess("Not GameLoop");
		FL::RemoveProfilerProcess("Collision Testing");
		FL::GameLoop::Stop();
		if (m_startedPersistantScenePath != "")
		{
			FL::GetLoadedProject().LoadPersistantScene("..\\engine\\tempFiles\\" + FL::GetFilenameFromPath(m_startedPersistantScenePath, true));
		}
		FL::LoadScene("..\\engine\\tempFiles\\" + FL::GetFilenameFromPath(m_startedScenePath, true), m_startedScenePath);		
	};
	void Update()
	{
		// Call base class GameLoop Update function
		FL::GameLoop::Update(FlatGui::FG_sceneViewGridStep.x, FlatGui::FG_sceneViewCenter);
		
		// Other, application specific updates here if needed
		//
	};
private:
	std::string m_startedScenePath;
	std::string m_startedPersistantScenePath;
};



// Define our Application
class EditorApplication : public FL::Application
{
public:
	EditorApplication()
	{
		A_GameLoop = new EditorGameLoop();
		m_b_recreateWindow = false;
	}
	~EditorApplication()
	{
		delete A_GameLoop;
		A_GameLoop = nullptr;
	}

	void Init()
	{
	}
	void Run()
	{
		bool& b_hasQuit = HasQuit();
		while (!b_hasQuit)
		{
			RunOnceAfterInitialization();

			static Uint32 frameStart = FL::GetEngineTime();
			Uint32 renderStartTime = 0;
			renderStartTime = FL::GetEngineTime(); // Profiler

			BeginRender();
			FL::AddProcessData("Render", (float)(FL::GetEngineTime() - renderStartTime)); // Profiler

			if ((GameLoopStarted() && !GameLoopPaused()) || (GameLoopPaused() && A_GameLoop->IsFrameSkipped()))
			{
				int iterations = 0;
				int minIter = 1;
				static int framesSkipped = 0;		

				if (GameLoopPaused() && A_GameLoop->IsFrameSkipped())
				{
					if (framesSkipped < A_GameLoop->GetFramesToSkip())
					{
						framesSkipped++;
					}
					else
					{
						framesSkipped = 0;
						A_GameLoop->SetFrameSkipped(false);
					}
				}

				// Profiler
				Uint32 updateLoopStart = 0;
				static Uint32 updateLoopEnd = 0;
				updateLoopStart = FL::GetEngineTime();
				Uint32 everythingElseHangTime = updateLoopStart - updateLoopEnd;
				FL::AddProcessData("Not GameLoop", (float)everythingElseHangTime);
				updateLoopEnd = updateLoopStart;

				float frameTime = (float)(FL::GetEngineTime() - frameStart) / 1000.0f; // actual deltaTime (in seconds)

				// Only add accumulated time if the GameLoop is not paused or if a frame was skipped while paused, then add a small fixed amount of time
				if (!GameLoopPaused())
				{
					A_GameLoop->m_accumulator += frameTime;
				}
				else if (A_GameLoop->IsFrameSkipped())
				{
					A_GameLoop->m_accumulator += A_GameLoop->m_deltaTime;
				}

				if (!GameLoopPaused() || A_GameLoop->IsFrameSkipped())
				{
					while (iterations < minIter || A_GameLoop->m_accumulator >= A_GameLoop->m_deltaTime)
					{
						FL::HandleEvents(b_hasQuit);
						A_GameLoop->Update();

						A_GameLoop->m_time += A_GameLoop->m_deltaTime;
						if (A_GameLoop->m_accumulator >= A_GameLoop->m_deltaTime)
						{
							A_GameLoop->m_accumulator -= A_GameLoop->m_deltaTime;
						}

						iterations++;
					}
				}
				
				// Get time it took to get back to GameLoopUpdate()
				frameStart = FL::GetEngineTime();

				// Artificially slow GameLoop if frameTime is less than 
				if (!FL::F_LoadedProject.IsVsyncEnabled() && frameTime < A_GameLoop->m_deltaTime)
				{
					SDL_Delay((Uint32)(A_GameLoop->m_deltaTime - frameTime) * 1000);
				}

				Uint32 hangTime = FL::GetEngineTime() - updateLoopStart;
				FL::AddProcessData("GameLoop (variable executions)", (float)hangTime);				
				updateLoopEnd = FL::GetEngineTime();				
			}
			else
			{
				FL::HandleEvents(b_hasQuit);
			}

			// If gameloop isn't running, make sure our framestart keeps up with current engine time otherwise it will cause a freeze on initially starting gameloop
			if (!A_GameLoop->IsStarted() || A_GameLoop->IsPaused())
			{
				frameStart = FL::GetEngineTime();
			}

			EndRender();


			A_GameLoop->DeleteObjectsInDeleteQueue();

			if (FL::F_b_closeProgramQueued)
			{
				Quit();
			}
		}
	}
	void RunOnceAfterInitialization()
	{
		static bool b_hasRunOnce = false;

		if (!b_hasRunOnce)
		{
			FlatGui::RunOnceAfterInitialization();

			b_hasRunOnce = true;
		}
	}
	void BeginRender()
	{
		if (m_b_recreateWindow)
		{
			FL::F_Window->ResizeWindow(1500, 850);
			m_b_recreateWindow = false;
		}

		Application::BeginRender();


		// Application specific rendering


		// Render the project selection screen
		if (!FL::F_b_projectSelected)
		{			
			FlatGui::RenderProjectHub(FL::F_b_projectSelected, m_startupProject);
			if (FL::F_b_projectSelected)
			{
				m_b_recreateWindow = true;				
			}
		}
		else
		{
			FlatGui::AddViewports();
		}
	}
	void EndRender()
	{
		Application::EndRender();


		// Application specific rendering tasks
		
		// If window was recreated this frame ( for after selecting a project )
		if (m_b_recreateWindow)
		{
			FlatGui::LoadProject(m_startupProject);
		}
	}
	void Quit()
	{
		FL::Application::Quit();
		FlatGui::Cleanup();
	}
	void OnLoadScene(std::string sceneName)
	{
		if (FL::GetObjectByID(FlatGui::GetFocusedGameObjectID()) == nullptr)
		{
			FlatGui::SetFocusedGameObjectID(-1);
		}
		if (GameLoopStarted())
		{
			FL::RunSceneAwakeAndStart();
		}
	}
	FL::GameLoop* GetGameLoop()
	{
		return A_GameLoop;
	};
	bool GameLoopStarted() 
	{
		return A_GameLoop->IsStarted();
	};
	bool GameLoopPaused() 
	{
		return A_GameLoop->IsPaused();
	};
	void StartGameLoop()
	{
		A_GameLoop->Start();
	};
	void UpdateGameLoop()
	{
		A_GameLoop->Update();
	};
	void PauseGameLoop()
	{
		if (A_GameLoop->IsPaused())
		{
			A_GameLoop->Unpause();
		}
		else
		{
			A_GameLoop->Pause();
		}
	};
	void StopGameLoop()
	{
		A_GameLoop->Stop();
	};
	void PauseGame()
	{
		if (A_GameLoop->IsGamePaused())
		{
			A_GameLoop->UnpauseGame();
		}
		else
		{
			A_GameLoop->PauseGame();
		}
	}

	bool m_b_recreateWindow;
	std::string m_startupProject;

private:
	EditorGameLoop* A_GameLoop;
};


// Define our CreateApplication() for the Editor
std::shared_ptr<FL::Application> FL::CreateApplication(int argc, char** argv)
{
	std::shared_ptr<EditorApplication> EditorApp = std::make_shared<EditorApplication>();
	EditorApp->SetWindowDimensions(800, 500);
	return EditorApp;
}