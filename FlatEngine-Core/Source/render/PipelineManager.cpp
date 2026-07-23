#include "render/PipelineManager.h"


namespace FlatEngine
{
	PipelineManager::PipelineManager()
	{
		m_renderPass = RenderPass();
		m_commandPool = VK_NULL_HANDLE;

		m_renderTexture = Texture();
		m_imageFormat = VK_FORMAT_R32G32B32A32_SFLOAT;
		m_mipLevels = 1;
	}

	//RenderPass& PipelineManager::GetRenderPass()
	//{
	//	return m_renderPass;
	//}
}