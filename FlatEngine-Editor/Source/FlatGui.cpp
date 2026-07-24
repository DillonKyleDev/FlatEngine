#include "FlatGui.h"
#include "managers/SceneManager.h"
#include "managers/Settings.h"
#include "render/GameView.h"
#include "render/SceneView.h"

#include "imgui.h"

#ifdef _WINDOWS
    #include <windows.h> // For getting directory name
#endif

/*
######################################
######							######
######    Gui Implimentation    ######
######                          ######
######################################
*/

namespace FL = FlatEngine;


namespace FlatGui 
{
	ImDrawList* drawList = nullptr;

	void Init()
	{
		FlatEngine::SceneManager::CreateAndLoadNewScene();
	}

	void AddViewports()
	{		
		RenderModals();
		MainMenuBar(FL::Settings::settings.b_showMainMenuBar);
		RenderToolbar();

		ImGui::ShowDemoWindow(&FL::Settings::settings.b_showDemoWindow);

		RenderFileExplorer(FL::Settings::settings.b_showFileExplorer);
		RenderHierarchy(FL::Settings::settings.b_showHierarchy);
		RenderPersistentScript(FL::Settings::settings.b_showPersistentScript);
		RenderInspector(FL::Settings::settings.b_showInspector);
		FL::GameView::RenderGameView(FL::Settings::settings.b_showGameView, false);
		FL::SceneView::RenderSceneView(FL::Settings::settings.b_showSceneView);
		RenderAnimator(FL::Settings::settings.b_showAnimator);			
		RenderKeyFrameEditor(FL::Settings::settings.b_showKeyFrameEditor);	
		RenderLog(FL::Settings::settings.b_showLog);			
		RenderMappingContextEditor(FL::Settings::settings.b_showMappingContextEditor);
		RenderMaterialEditor(FL::Settings::settings.b_showMaterialEditor);			
		RenderSettings(FL::Settings::settings.b_showSettings);
	}	
}