#pragma once
#include "PipelineManager.h"

#include <vector>
#include <memory>


namespace FlatEngine
{
	class Texture;
	class Model;
	class Material;

	class ViewportManager : public PipelineManager
	{
	public:
		ViewportManager();
		~ViewportManager();
		void Cleanup();

		void Setup(VkInstance& instance, WinSys& winSystem, PhysicalDevice& physicalDevice, LogicalDevice& logicalDevice, VkCommandPool& commandPool);
		void HandleRenderPass(uint32_t imageIndex);
		void CreateDescriptorSets(std::shared_ptr<Material> material, std::vector<VkDescriptorSet>& descriptorSets, Model& model, std::vector<Texture>& textures);
		void CreateRenderPassResources();
		void CreateImageResources();
		std::vector<VkDescriptorSet> GetDescriptorSets();
		void OnWindowResized();


	private:		
		std::vector<VkDescriptorSet> m_viewportDescriptorSets;
	};
}
