#include "Application.h"
#include "EntryPoint.h"
#include "FlatEngine.h"
#include "FlatGui.h"
#include "GameLoop.h"
#include "managers/ProjectManager.h"
#include "managers/SceneManager.h"
#include "managers/Settings.h"
#include "tools/FileHelper.h"
#include "tools/Profiler.h"
#include "tools/Time.h"

#include <SDL_mixer.h>
#include <string>
#include <memory>

namespace FL = FlatEngine;


int main(int argc, char* args[])
{
	return FL::Main(argc, args);
}

// Define our Applications main GameLoop
class EditorGameLoop : public FL::GameLoop
{
public:
	EditorGameLoop() 
	{
		m_startedScenePath = "";
		m_startedPersistentScriptsPath = "";
	};
	~EditorGameLoop() {};

	void Start()
	{
		FL::Profiler::AddProfilerProcess("GameLoop (variable executions)");		
		FL::Profiler::AddProfilerProcess("Not GameLoop");
		FL::Profiler::AddProfilerProcess("Collision Testing");	
		FL::SceneManager::CreateSceneBackup(); // Backup existing scene save
		m_startedScenePath = FL::SceneManager::loadedScene.path;
		m_startedPersistentScriptsPath = FL::ProjectManager::loadedProject.persistentScriptPath;
		FL::SceneManager::SaveScene(&FL::SceneManager::loadedScene, "../engine/tempFiles/" + FL::SceneManager::loadedScene.name + "_start_snapshot.scn");
		if (m_startedPersistentScriptsPath != "")
		{
			FL::ProjectManager::loadedProject.persistentScriptPath = "../engine/tempFiles/" + FL::SceneManager::loadedScene.name + "_scripts_start_snapshot.json";
			FL::ProjectManager::loadedProject.SavePersistentScript();
		}	
		FL::GameLoop::Start();
	};
	void Stop()
	{
		FL::Profiler::RemoveProfilerProcess("GameLoop (variable executions)");
		FL::Profiler::RemoveProfilerProcess("Not GameLoop");
		FL::Profiler::RemoveProfilerProcess("Collision Testing");
		FL::GameLoop::Stop();
		if (m_startedPersistentScriptsPath != "")
		{
			FL::ProjectManager::loadedProject.LoadPersistentScript("../engine/tempFiles/" + FL::FileHelper::GetFilenameFromPath(m_startedPersistentScriptsPath, false) + "_scripts_start_snapshot.json");
		}
		FL::SceneManager::LoadScene("../engine/tempFiles/" + FL::FileHelper::GetFilenameFromPath(m_startedScenePath, false) + "_start_snapshot.scn", m_startedScenePath);
	};
	void Update()
	{
		// Call base class GameLoop Update function
		FL::GameLoop::Update();
		
		// Other, application specific updates here if needed
		//
	};
private:
	std::string m_startedScenePath;
	std::string m_startedPersistentScriptsPath;
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

			static Uint32 frameStart = FL::Time::Time();
			Uint32 renderStartTime = 0;
			renderStartTime = FL::Time::Time(); // Profiler

			BeginRender();
			FL::Profiler::AddProcessData("Render", (float)(FL::Time::Time() - renderStartTime)); // Profiler

			if ((GameLoopStarted() && !GameLoopPaused()) || (GameLoopPaused() && A_GameLoop->IsFrameSkipped()))
			{
				int iterations = 0;				
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
				updateLoopStart = FL::Time::Time();
				Uint32 everythingElseHangTime = updateLoopStart - updateLoopEnd;
				FL::Profiler::AddProcessData("Not GameLoop", (float)everythingElseHangTime);
				updateLoopEnd = updateLoopStart;

				float frameTime = (float)(FL::Time::Time() - frameStart) / 1000.0f; // actual deltaTime (in seconds)

				// Only add accumulated time if the GameLoop is not paused or if a frame was skipped while paused, then add a small fixed amount of time
				if (!GameLoopPaused())
				{
					A_GameLoop->m_accumulator += frameTime;
					if (A_GameLoop->m_accumulator > 0.25f)
					{
						A_GameLoop->m_accumulator = 0.25f; // prevent death spiral
					}
				}
				else if (A_GameLoop->IsFrameSkipped())
				{
					A_GameLoop->m_accumulator += A_GameLoop->m_deltaTime;
				}

				if (!GameLoopPaused() || A_GameLoop->IsFrameSkipped())
				{
					FL::HandleEvents(b_hasQuit);

					while (A_GameLoop->m_accumulator >= A_GameLoop->m_deltaTime)
					{
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
				frameStart = FL::Time::Time();

				// Artificially slow GameLoop if frameTime is less than 
				if (!FL::Settings::settings.b_vsyncEnabled && frameTime < A_GameLoop->m_deltaTime)
				{
					SDL_Delay((Uint32)(A_GameLoop->m_deltaTime - frameTime) * 1000);
				}

				Uint32 hangTime = FL::Time::Time() - updateLoopStart;
				FL::Profiler::AddProcessData("GameLoop (variable executions)", (float)hangTime);				
				updateLoopEnd = FL::Time::Time();				
			}
			else
			{
				FL::HandleEvents(b_hasQuit);
			}

			// If gameloop isn't running, make sure our framestart keeps up with current engine time otherwise it will cause a freeze on initially starting gameloop
			if (!A_GameLoop->IsStarted() || A_GameLoop->IsPaused())
			{
				frameStart = FL::Time::Time();
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
			FL::Profiler::SetupProfilerProcesses();

			b_hasRunOnce = true;
		}
	}
	void BeginRender()
	{
		if (m_b_recreateWindow)
		{
			//FL::F_Window->ResizeWindow(1920, 1040);
			//FL::F_Window->ResizeWindow(800, 600);
			// FL::F_Window->SetFullscreen(true);
			m_b_recreateWindow = false;
		}

		Application::BeginRender();


		//ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
		//ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, Vector2(0, 0));
		//ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		//FlatEngine::PushWindowStyles();
		//ImGui::Begin("Scene View", &FlatGui::FG_b_showSceneView, 16 | 8); // Window flags 	ImGuiWindowFlags_NoScrollWithMouse
		//FlatEngine::PopWindowStyles();
		// {
		
			// Application specific rendering
			//if (FL::F_VulkanManager->viewportDescriptorSets.size() > 0)
			//{
			//	ImVec2 size = ImGui::GetContentRegionAvail();
			//	ImGui::Image(FL::F_VulkanManager->viewportDescriptorSets[FL::VM_currentFrame], size);
			//}

		// }
		//ImGui::End();
		//ImGui::PopStyleVar();
		//ImGui::PopStyleVar();
		//ImGui::PopStyleVar();


		// Render the project selection screen
		if (!FL::ProjectManager::b_projectSelected)
		{			
			FlatGui::RenderProjectHub(FL::ProjectManager::b_projectSelected, m_startupProject);
			if (FL::ProjectManager::b_projectSelected)
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
			FL::ProjectManager::LoadProject(m_startupProject);
		}
	}
	void Quit()
	{
		FL::Application::Quit();
		FL::Profiler::CleanupProfilerProcesses();
	}
	void OnLoadScene(std::string sceneName)
	{
		if (FL::SceneManager::loadedScene.GetObjectByID(FL::ProjectManager::loadedProject.focusedGameObjectID) == nullptr)
		{
			FL::ProjectManager::loadedProject.focusedGameObjectID = -1;
		}
		if (GameLoopStarted())
		{
			FL::LuaManager::RunSceneAwakeAndStart();
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
	//EditorApp->SetWindowDimensions(800, 500);
	EditorApp->SetWindowDimensions(1800, 1000);
	return EditorApp;
}