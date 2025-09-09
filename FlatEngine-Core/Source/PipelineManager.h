#pragma once
#include "RenderPass.h"
#include "Texture.h"
#include "vulkan/vulkan_core.h"

#include <memory>
#include <vector>


namespace FlatEngine
{	
	class RenderPass;
	class WinSys;
	class PhysicalDevice;
	class LogicalDevice;
	class Texture;
	class Material;
	class Model;

	class PipelineManager
	{
		friend class ImGuiManager;
		friend class ViewportManager;		

	public:
		PipelineManager();
		~PipelineManager();
		virtual void Cleanup()
		{
			m_renderPass.Cleanup(*m_logicalDevice);
		};

		virtual void Setup(VkInstance& instance, WinSys& winSystem, PhysicalDevice& physicalDevice, LogicalDevice& logicalDevice, VkCommandPool& commandPool) 
		{
			m_instance = &instance;
			m_winSystem = &winSystem;
			m_physicalDevice = &physicalDevice;
			m_logicalDevice = &logicalDevice;
			m_commandPool = &commandPool;
			m_renderPass.SetHandles(instance, winSystem, physicalDevice, logicalDevice);
		};
		virtual void HandleRenderPass(uint32_t imageIndex) {};	
		virtual void CreateRenderPassResources() {};
		virtual void CreateImageResources() {};		
		virtual void CreateDescriptorSets(std::shared_ptr<Material> material, std::vector<VkDescriptorSet>& descriptorSets, Model& model, std::vector<Texture>& textures) {};
		virtual void OnWindowResized() {};
		RenderPass& GetRenderPass();

	private:
		RenderPass m_renderPass;						
		VkInstance* m_instance;
		WinSys* m_winSystem;
		PhysicalDevice* m_physicalDevice;
		LogicalDevice* m_logicalDevice;
		VkCommandPool* m_commandPool;

		Texture m_renderTexture;
		VkFormat m_imageFormat;
		uint32_t m_mipLevels;
	};
}
