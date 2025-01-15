#include "FlatEngine.h"
#include "Application.h"
#include "AssetManager.h"
//#include "WindowManager.h"

#include "SDL.h"

namespace FL = FlatEngine;

namespace FlatEngine
{
	void Application::BeginRender()
	{
		FL::BeginImGuiRender();
	}

	void Application::EndRender()
	{
		FL::EndImGuiRender();

		// Handle window resizing and recreating ImGui
		if (m_b_windowResized)
		{
			m_b_windowResized = false;
		}

		if (F_b_loadNewScene)
		{
			F_b_loadNewScene = false;
			FL::LoadScene(F_sceneToBeLoaded);
			F_sceneToBeLoaded = "";
		}
	}
}