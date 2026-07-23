#include "Application.h"
#include "EntryPoint.h"
#include "tools/FileHelper.h"
#include "FlatEngine.h"
#include "GameLoop.h"
#include "render/GameView.h"
#include "GuiCore.h"
#include "managers/ProjectManager.h"
#include "managers/Settings.h"
#include "tools/Time.h"

#include <string>
#include <memory>

namespace FL = FlatEngine;


int main(int argc, char* args[])
{
	return FL::Main(argc, args);
}


// Define our Applications main GameLoop
class RuntimeGameLoop : public FL::GameLoop
{
public:
	RuntimeGameLoop() {};
	~RuntimeGameLoop() {};

	void Start()
	{
		FL::GameLoop::Start();
	};
	void Stop()
	{
		FL::GameLoop::Stop();
	};
	void Update()
	{
		// Call base class GameLoop Update function
		FL::GameLoop::Update();

		// Other, application specific updates here if needed
		//
	};
private:
};



// Define our Application
class RuntimeApplication : public FL::Application
{
public:
	RuntimeApplication()
	{
		A_GameLoop = new RuntimeGameLoop();
		m_recreateWindow = false;
	}
	~RuntimeApplication()
	{
		delete A_GameLoop;
		A_GameLoop = nullptr;
	}

	void Init()
	{
		// Remove the reference to the imgui.ini file for layout since we only need that in Editor mode
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.IniFilename = NULL;

		//Window::SetFullscreen(false);
	}
	void Run()
	{
		bool& b_hasQuit = HasQuit();
		while (!b_hasQuit)
		{
			RunOnceAfterInitialization();

			static Uint32 frameStart = FL::Time::Time();	

			int iterations = 0;
			int minIter = 1;

			BeginRender();

			if (!A_GameLoop->IsStarted())
			{
				A_GameLoop->Start();
			}

			if ((GameLoopStarted() && !GameLoopPaused()) || (GameLoopPaused() && A_GameLoop->IsFrameSkipped()))
			{
				float frameTime = (float)(FL::Time::Time() - frameStart) / 1000.0f; // actual deltaTime (in seconds)

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
				frameStart = FL::Time::Time();

				// Artificially slow GameLoop if frameTime is less than 
				if (!FL::Settings::settings.b_vsyncEnabled && frameTime < A_GameLoop->m_deltaTime)
				{
					SDL_Delay((Uint32)(A_GameLoop->m_deltaTime - frameTime) * 1000);
				}
			}
			else
			{
				FL::HandleEvents(b_hasQuit);
			}

			// If gameloop isn't running, make sure our framestart keeps up with current engine time otherwise it will cause a freeze on initially starting gameloop
			if (!GameLoopStarted())
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
			FL::ProjectManager::LoadProject(FL::FileHelper::FindAllFilesWithExtension("../", ".prj").front());
			//FL::F_AssetManager.CollectDirectories();
			//FL::F_AssetManager.UpdateProjectDirs(m_startupProject);
			//FL::F_AssetManager.CollectColors();
			//FL::F_AssetManager.CollectTextures();
			//FL::InitializeTileSets();
			//FL::F_PrefabManager->InitializePrefabs();
			//FL::RetrieveLuaScriptPaths();
			//FL::RetrieveCPPScriptNames();
			//FL::InitializeMappingContexts();

			b_hasRunOnce = true;
		}
	}
	void BeginRender()
	{
		Application::BeginRender();

		// Application specific rendering	
		FL::GuiCore::SetNextViewportToFillWindow();  // Maximize viewport
		bool b_inRuntime = true;
		FL::GameView::RenderGameView(b_inRuntime);
			
	}
	void EndRender()
	{
		Application::EndRender();


		// Application specific rendering tasks
	}
	void Quit()
	{
		FL::Application::Quit();
	}
	void OnLoadScene(std::string sceneName)
	{
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
			A_GameLoop->Unpause();
		else
			A_GameLoop->Pause();
	};
	void StopGameLoop()
	{
		A_GameLoop->Stop();
	};
	void PauseGame()
	{
		if (GameLoopPaused())
			A_GameLoop->UnpauseGame();
		else
			A_GameLoop->PauseGame();
	}

	bool m_recreateWindow;
	std::string m_startupProject;

private:
	RuntimeGameLoop* A_GameLoop;
};


// Define our CreateApplication() for the Runtime
std::shared_ptr<FL::Application> FL::CreateApplication(int argc, char** argv)
{
	std::shared_ptr<RuntimeApplication> RuntimeApp = std::make_shared<RuntimeApplication>();
	RuntimeApp->SetWindowDimensions(1920, 1080);
	return RuntimeApp;
}