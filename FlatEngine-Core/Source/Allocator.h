#pragma once
#include "LogicalDevice.h"
#include "Model.h"
#include "Texture.h"

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
		~Allocator();

		void Init(AllocatorType type, uint32_t textureCount, LogicalDevice& logicalDevice, uint32_t perPool = 50);
		void SetFreed(uint32_t freedFrom);
		void AllocateDescriptorSets(std::vector<VkDescriptorSet>& descriptorSets, Model& model, std::vector<Texture>& textures);
		void ConfigureDescriptorSetLayout(std::vector<VkDescriptorSetLayoutBinding> bindings, VkDescriptorSetLayoutCreateInfo layoutInfo);
		void CreateDescriptorSetLayout();
		void CleanupDescriptorSetLayout();
		VkDescriptorSetLayout& GetDescriptorSetLayout();
		void ConfigureDescriptorPools(std::vector<VkDescriptorPoolSize> poolSizes, VkDescriptorPoolCreateInfo poolInfo);
		VkDescriptorPool CreateDescriptorPool();

		//void AllocateCommandBuffers(std::vector<VkCommandBuffer>& commandBuffers, uint32_t& allocatedFrom);

	private:
		void CleanupPools();
		void SetType(AllocatorType type);
		void SetSizePerPool(uint32_t size);
		void SetDefaultDescriptorSetLayoutConfig();
		void SetDefaultDescriptorPoolConfig();
		void FillPools();
		void CreateDescriptorPool(VkDescriptorPool& descriptorPool);
		//void CreateCommandPool();
		void CheckPoolAvailability();

		AllocatorType m_type;
		uint32_t m_sizePerPool;
		uint32_t m_startingPools;
		uint32_t m_textureCount;
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
		LogicalDevice* m_deviceHandle;
	};
}