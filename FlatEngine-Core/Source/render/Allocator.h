#pragma once
#include "Texture.h"

#include <map>
#include <vector>


namespace FlatEngine
{
	enum AllocatorType {
		DescriptorPool,
		CommandPool,
		Null
	};

	class Allocator
	{
	public:
		Allocator();

		void Init(AllocatorType type, std::map<uint32_t, TexturePipelineData>* texturesShaderStages, uint32_t perPool = 100);
		void SetFreed(uint32_t freedFrom);
		void AllocateDescriptorSets(std::vector<VkDescriptorSet>& descriptorSets, std::vector<VkBuffer>& uniformBuffers, std::map<uint32_t, TexturePipelineData>& materialTextures, std::map<uint32_t, Texture>& meshTextures);
		void ConfigureDescriptorSetLayout(std::vector<VkDescriptorSetLayoutBinding> bindings, VkDescriptorSetLayoutCreateInfo layoutInfo);
		void CreateDescriptorSetLayout();
		void CleanupDescriptorSetLayout();
		VkDescriptorSetLayout& GetDescriptorSetLayout();
		void ConfigureDescriptorPools(std::vector<VkDescriptorPoolSize> poolSizes, VkDescriptorPoolCreateInfo poolInfo);
		VkDescriptorPool CreateDescriptorPool();		

	private:
		void CleanupPools();
		void SetType(AllocatorType type);
		void SetSizePerPool(uint32_t size);
		void SetDefaultDescriptorSetLayoutConfig();
		void SetDefaultDescriptorPoolConfig();
		void FillPools();
		void CreateDescriptorPool(VkDescriptorPool& descriptorPool);		
		void CheckPoolAvailability();

		AllocatorType m_type;
		uint32_t m_sizePerPool;
		uint32_t m_startingPools;
		std::map<uint32_t, TexturePipelineData>* m_texturePipelineData;
		uint32_t m_currentPoolIndex;
		std::vector<uint32_t> m_allocationsRemainingByPool;
		std::vector<uint32_t> m_setsFreedByPool;
		std::vector<VkDescriptorPool> m_descriptorPools;
		std::vector<VkCommandPool> m_commandPools;
		VkDescriptorSetLayout m_descriptorSetLayout;
		std::vector<VkDescriptorSetLayoutBinding> m_bindings;
		VkDescriptorSetLayoutCreateInfo m_layoutInfo;
		std::vector<VkDescriptorPoolSize> m_poolSizes;
		VkDescriptorPoolCreateInfo m_poolInfo;		
		bool m_b_imguiAllocator;
	};
}