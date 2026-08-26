#include "Application.h"
#include "FlatEngine.h"
#include "FlatEngine-Editor.h"
#include "FlatGui.h"
#include "managers/ProjectManager.h"
#include "managers/SceneManager.h"
#include "render/RenderWindow.h"
#include "tools/FileHelper.h"
#include "tools/Profiler.h"


namespace FL = FlatEngine;


int main(int argc, char* args[])
{
	return FL::Main(argc, args);
}




EditorGameLoop::EditorGameLoop() 
{
	m_startedScenePath = "";
	m_startedPersistentScriptsPath = "";
}

void EditorGameLoop::Start()
{
	FL::Profiler::AddProfilerProcess("GameLoop (variable executions)");		
	FL::Profiler::AddProfilerProcess("Not GameLoop");
	FL::Profiler::AddProfilerProcess("Collision Testing");	
	FL::SceneManager::CreateSceneBackup(); // Backup existing scene save
	m_startedScenePath = FL::SceneManager::loadedScene.path;
	FL::SceneManager::SaveScene(&FL::SceneManager::loadedScene, "../engine/tempFiles/" + FL::SceneManager::loadedScene.name + "_start_snapshot.scn");
	FL::GameLoop::Start();
}
void EditorGameLoop::Stop()
{
	FL::Profiler::RemoveProfilerProcess("GameLoop (variable executions)");
	FL::Profiler::RemoveProfilerProcess("Not GameLoop");
	FL::Profiler::RemoveProfilerProcess("Collision Testing");
	FL::GameLoop::Stop();
	FL::SceneManager::LoadScene("../engine/tempFiles/" + FL::FileHelper::GetFilenameFromPath(m_startedScenePath, false) + "_start_snapshot.scn", m_startedScenePath);
};


// Editor Application
EditorApplication::EditorApplication()
{
	gameloop = std::make_shared<EditorGameLoop>();
	m_b_recreateWindow = false;
	b_editorMode = true;
}

void EditorApplication::Init()
{
	FL::Profiler::SetupProfilerProcesses();
}

void EditorApplication::Run()
{
	bool b_hasQuit = false;
	
	while (!b_hasQuit)
	{
		BeginRender();	
		FL::HandleEvents();
		gameloop->Update();						
		EndRender();

		if (FL::b_closeProgramQueued)
		{
			b_hasQuit = true;
		}
	}
}

void EditorApplication::Cleanup() 
{
	FL::Profiler::CleanupProfilerProcesses();
}

void EditorApplication::BeginRender()
{
	if (m_b_recreateWindow)
	{
		FL::RenderWindow::window.ResizeWindow(1920, 1006);		
		// FL::F_Window->SetFullscreen(true);
		m_b_recreateWindow = false;
	}

	Application::BeginRender();

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

void EditorApplication::EndRender()
{
	Application::EndRender();
	
	// If window was recreated this frame ( for after selecting a project )
	if (m_b_recreateWindow)
	{
		FL::ProjectManager::LoadProject(m_startupProject);
	}
}

void EditorApplication::OnLoadScene(std::string sceneName)
{
	if (FL::ProjectManager::loadedProject.loadedScenePath != sceneName)
	{
		FL::ProjectManager::loadedProject.focusedGameObjectIDs.clear();
	}
	if (gameloop->IsStarted())
	{
		FL::LuaManager::RunSceneAwakeAndStart();
	}
}

// Define our CreateApplication() for the Editor
std::shared_ptr<FL::Application> FL::CreateApplication(int argc, char** argv)
{
	std::shared_ptr<EditorApplication> EditorApp = std::make_shared<EditorApplication>();
	//EditorApp->SetWindowDimensions(800, 500);	
	return EditorApp;
}