#include "Application.h"
#include "FlatEngine.h"
#include "GuiCore.h"
#include "managers/SceneManager.h"
#include "render/VulkanManager.h"

namespace FL = FlatEngine;


namespace FlatEngine
{
	void Application::BeginRender()
	{
		FL::GuiCore::BeginImGuiRender();
	}

	void Application::EndRender()
	{
		FL::GuiCore::EndImGuiRender();
		
		if (m_b_windowResized)
		{
			m_b_windowResized = false;
			VulkanManager::vulkan.RecreateSwapChainAndFrameBuffers();
		}

		if (b_loadNewScene)
		{
			b_loadNewScene = false;
			SceneManager::LoadScene(sceneToBeLoaded);
			sceneToBeLoaded = "";
		}
	}
}