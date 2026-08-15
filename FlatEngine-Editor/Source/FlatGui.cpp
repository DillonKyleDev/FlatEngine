#include "FlatGui.h"
#include "managers/ProjectManager.h"
#include "managers/SceneManager.h"
#include "managers/Settings.h"
#include "render/GameView.h"
#include "render/SceneView.h"

#include "imgui.h"
#include <string>

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

	void RenderInspectors()
	{
		int index = 0;
		long queuedForDelete = -1;

		for (long focusedID : FL::ProjectManager::loadedProject.focusedGameObjectIDs)
		{
			bool b_keepIDFocused = FL::Settings::settings.b_showInspector;
			RenderInspector(b_keepIDFocused, std::to_string(index), focusedID);
			index++;

			if (!b_keepIDFocused)
			{
				if (FL::ProjectManager::loadedProject.focusedGameObjectIDs.size() > 1)
					queuedForDelete = focusedID;				
				else
				{
					FL::Settings::settings.b_showInspector = false;
					FL::Settings::settings.SaveSettings();
				}	
			}
		}

		if (queuedForDelete != -1)
		{
			FL::ProjectManager::loadedProject.RemoveFocusedObjectID(queuedForDelete);
		}

		if (FL::ProjectManager::loadedProject.focusedGameObjectIDs.size() == 0)
		{
			RenderInspector(FL::Settings::settings.b_showInspector, std::to_string(index), -1);
		}
	}

	void AddViewports()
	{		
		ImGui::ShowDemoWindow(&FL::Settings::settings.b_showDemoWindow);

		RenderModals();
		MainMenuBar(FL::Settings::settings.b_showMainMenuBar);
		RenderToolbar();
		RenderFileExplorer(FL::Settings::settings.b_showFileExplorer);
		RenderHierarchy(FL::Settings::settings.b_showHierarchy);
		RenderInspectors();
		RenderPersistentScript(FL::Settings::settings.b_showPersistentScript);		
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