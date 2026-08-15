#include "Application.h"
#include "EntryPoint.h"
#include "tools/FileHelper.h"
#include "FlatEngine.h"
#include "GameLoop.h"
#include "render/GameView.h"
#include "GuiCore.h"
#include "managers/ProjectManager.h"

#include <string>
#include <memory>

namespace FL = FlatEngine;


int main(int argc, char* args[])
{
	return FL::Main(argc, args);
}


class RuntimeApplication : public FL::Application
{
public:
	RuntimeApplication()
	{
		gameloop = std::make_shared<FL::GameLoop>();
	}

	void Init()
	{
		// Remove the reference to the imgui.ini file for layout since we only need that in Editor mode
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.IniFilename = NULL;

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
	}
	void Run()
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
	void BeginRender()
	{
		Application::BeginRender();

		bool b_inRuntime = true;
		FL::GuiCore::SetNextViewportToFillWindow();
		FL::GameView::RenderGameView(b_inRuntime);
	}
	void EndRender()
	{
		Application::EndRender();
	}
	void OnLoadScene(std::string sceneName)
	{
		if (gameloop->IsStarted())
		{
			FL::LuaManager::RunSceneAwakeAndStart();
		}
	}
};


// Define our CreateApplication() for the Runtime
std::shared_ptr<FL::Application> FL::CreateApplication(int argc, char** argv)
{
	std::shared_ptr<RuntimeApplication> RuntimeApp = std::make_shared<RuntimeApplication>();
	RuntimeApp->SetWindowDimensions(1920, 1080);
	return RuntimeApp;
}