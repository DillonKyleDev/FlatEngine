#include "PipelineManager.h"


namespace FlatEngine
{
	PipelineManager::PipelineManager()
	{
		m_renderPass = RenderPass();
		m_instance = VK_NULL_HANDLE;
		m_winSystem = VK_NULL_HANDLE;
		m_physicalDevice = VK_NULL_HANDLE;
		m_logicalDevice = VK_NULL_HANDLE;
		m_commandPool = VK_NULL_HANDLE;

		m_renderTexture = Texture();
		m_imageFormat = VK_FORMAT_R32G32B32A32_SFLOAT;
		m_mipLevels = 1;
	}

	PipelineManager::~PipelineManager()
	{
	}

	//RenderPass& PipelineManager::GetRenderPass()
	//{
	//	return m_renderPass;
	//}
}