#pragma once
#include "render/PipelineManager.h"

#include <vector>
#include <vulkan_core.h>


namespace FlatEngine
{
	class Texture;
	class Model;
	class Material;
	class Mesh;

	class ViewportManager : public PipelineManager
	{
	public:
		ViewportManager();		
		void Cleanup();

		// void Setup(VkCommandPool& commandPool);
		//void HandleRenderPass(uint32_t imageIndex);
		//void CreateDescriptorSets(std::shared_ptr<Material> material, std::vector<VkDescriptorSet>& descriptorSets, Model& model, std::vector<Texture>& textures);
		//void CreateRenderPassResources();
		//void CreateImageResources();
		//std::vector<VkDescriptorSet> GetDescriptorSets();
		//void OnWindowResized();
		//void RecordCommandBuffer(uint32_t imageIndex, Mesh& mesh);


	private:		
		std::vector<VkDescriptorSet> m_viewportDescriptorSets;
	};
}
