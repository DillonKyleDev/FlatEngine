#pragma once
#include "render/RenderPass.h"
#include "render/Texture.h"

#include "vulkan/vulkan_core.h"


namespace FlatEngine
{	
	class PipelineManager
	{
		friend class ImGuiManager;
		friend class ViewportManager;		// <-- necessary???

	public:
		PipelineManager();

		virtual void Cleanup()
		{
			m_renderPass.Cleanup();
		};

		virtual void Setup(VkCommandPool& commandPool) 
		{
			m_commandPool = &commandPool;
			//m_renderPass.SetHandles(instance, winSystem, physicalDevice, logicalDevice);
		};
		//virtual void HandleRenderPass(uint32_t imageIndex) {};	
		//virtual void CreateRenderPassResources() {};
		//virtual void CreateImageResources() {};		
		//virtual void CreateDescriptorSets(std::shared_ptr<Material> material, std::vector<VkDescriptorSet>& descriptorSets, Model& model, std::vector<Texture>& textures) {};
		//virtual void OnWindowResized() {};
		//virtual void RecordCommandBuffer(uint32_t imageIndex, Mesh& mesh) {};

		//RenderPass& GetRenderPass();		

	private:
		RenderPass m_renderPass;						
		VkCommandPool* m_commandPool;

		Texture m_renderTexture;
		VkFormat m_imageFormat;
		uint32_t m_mipLevels;
	};
}
