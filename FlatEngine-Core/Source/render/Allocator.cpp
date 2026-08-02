#include "render/Allocator.h"
#include "render/DeviceManager.h"
#include "render/VulkanManager.h"
#include "tools/Logger.h"


namespace FlatEngine
{
	Allocator::Allocator()
	{
		m_type = AllocatorType::Null;
		m_sizePerPool = 0;
		m_startingPools = 1;		
		m_currentPoolIndex = 0;
		m_descriptorSetLayout = VK_NULL_HANDLE;
		m_bindings = {};
		m_layoutInfo = {};
		m_layoutInfo.bindingCount = 0;
		m_poolSizes = {};
		m_poolInfo = {};		
		m_b_imguiAllocator = false;
		m_texturePipelineData = nullptr;
	}

	void Allocator::CleanupPools()
	{
		if (DeviceManager::logicalDevice.GetDevice() != VK_NULL_HANDLE)
		{
			switch (m_type)
			{
			case AllocatorType::DescriptorPool:
				for (uint32_t i = 0; i < m_descriptorPools.size(); i++)
				{
					VulkanManager::vulkan.QueueDescriptorPoolDeletion(m_descriptorPools[i]);
				}
				break;
			case AllocatorType::CommandPool:
				break;
			default:
				break;
			}
		}
	}

	void Allocator::Init(AllocatorType type, std::map<uint32_t, TexturePipelineData>* texturesShaderStages, uint32_t perPool)
	{
		CleanupPools();

		m_type = type;
		m_texturePipelineData = texturesShaderStages;		

		if (m_poolSizes.size() == 0)
		{
			m_sizePerPool = perPool;
			m_allocationsRemainingByPool = std::vector<uint32_t>(m_startingPools, m_sizePerPool);
		}

		m_setsFreedByPool = std::vector<uint32_t>(m_startingPools, 0);
		m_descriptorPools = std::vector<VkDescriptorPool>();
		m_commandPools = std::vector<VkCommandPool>();
		m_descriptorPools.resize(m_startingPools);
		m_commandPools.resize(m_startingPools);

		CreateDescriptorSetLayout();
		FillPools();
	}

	void Allocator::SetType(AllocatorType type)
	{
		m_type = type;
	}

	void Allocator::SetSizePerPool(uint32_t size)
	{
		m_sizePerPool = size;
	}

	void Allocator::SetDefaultDescriptorSetLayoutConfig()
	{
		m_bindings.clear();

		// Default Descriptor Set Layout config
		VkDescriptorSetLayoutBinding uboLayoutBinding{};
		uboLayoutBinding.binding = 0;
		uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		uboLayoutBinding.descriptorCount = 1;
		uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		uboLayoutBinding.pImmutableSamplers = nullptr;

		m_bindings.push_back(uboLayoutBinding);

		uint32_t descriptorCount = 1;
		for (uint32_t i = 0; i < m_texturePipelineData->size(); i++)
		{
			VkShaderStageFlags shaderStage = m_texturePipelineData->at(i).shaderStage;
			VkDescriptorType descriptorType = m_texturePipelineData->at(i).descriptorType;

			VkDescriptorSetLayoutBinding descriptorBinding{};
			descriptorBinding.binding = i + 1;
			descriptorBinding.descriptorCount = 1;
			descriptorBinding.descriptorType = descriptorType;
			descriptorBinding.pImmutableSamplers = nullptr;
			descriptorBinding.stageFlags = shaderStage;

			m_bindings.push_back(descriptorBinding);
			descriptorCount++;
		}

		m_layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		m_layoutInfo.bindingCount = static_cast<uint32_t>(m_bindings.size());
		m_layoutInfo.pBindings = m_bindings.data();
	}

	void Allocator::ConfigureDescriptorSetLayout(std::vector<VkDescriptorSetLayoutBinding> bindings, VkDescriptorSetLayoutCreateInfo layoutInfo)
	{
		if (bindings.size() > 0)
		{
			m_bindings = bindings;
			m_layoutInfo = layoutInfo;
			m_layoutInfo.bindingCount = static_cast<uint32_t>(m_bindings.size());
			m_layoutInfo.pBindings = m_bindings.data();
			m_b_imguiAllocator = true;
		}
		else
		{
			m_layoutInfo.bindingCount = 0;
		}
	}

	void Allocator::CreateDescriptorSetLayout()
	{
		// Refer to - https://vulkan-tutorial.com/en/Uniform_buffers/Descriptor_layout_and_buffer

		if (DeviceManager::logicalDevice.GetDevice() != VK_NULL_HANDLE)
		{
			if (!m_b_imguiAllocator)
			{
				SetDefaultDescriptorSetLayoutConfig();
			}

			if (vkCreateDescriptorSetLayout(DeviceManager::logicalDevice.GetDevice(), &m_layoutInfo, nullptr, &m_descriptorSetLayout) != VK_SUCCESS)
			{
				throw std::runtime_error("failed to create descriptor set layout!");
			}
		}
	}

	void Allocator::CleanupDescriptorSetLayout()
	{
		vkDestroyDescriptorSetLayout(DeviceManager::logicalDevice.GetDevice(), m_descriptorSetLayout, nullptr);
	}

	VkDescriptorSetLayout& Allocator::GetDescriptorSetLayout()
	{
		return m_descriptorSetLayout;
	}

	void Allocator::FillPools()
	{
		if (!m_b_imguiAllocator)
		{
			SetDefaultDescriptorPoolConfig();
		}
		else
		{
			m_sizePerPool = m_poolInfo.maxSets;
			m_allocationsRemainingByPool = std::vector<uint32_t>(m_startingPools, m_poolInfo.maxSets);
		}

		switch (m_type)
		{
		case AllocatorType::DescriptorPool:

			for (uint32_t i = 0; i < m_startingPools; i++)
			{
				CreateDescriptorPool(m_descriptorPools[i]);
			}

			break;
		case AllocatorType::CommandPool:
			break;
		default:
			break;
		}
	}

	void Allocator::SetDefaultDescriptorPoolConfig()
	{
		// Default Descriptor Pool Settings	
		m_poolSizes = std::vector<VkDescriptorPoolSize>();
		m_poolSizes.resize(m_texturePipelineData->size() + 1);
		m_poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		m_poolSizes[0].descriptorCount = static_cast<uint32_t>(m_sizePerPool);
		for (uint32_t j = 0; j < m_texturePipelineData->size(); j++)
		{
			m_poolSizes[j + 1].type = m_texturePipelineData->at(j).descriptorType;
			m_poolSizes[j + 1].descriptorCount = static_cast<uint32_t>(m_sizePerPool);
		}
		m_poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		m_poolInfo.poolSizeCount = static_cast<uint32_t>(m_poolSizes.size());
		m_poolInfo.pPoolSizes = m_poolSizes.data();
		m_poolInfo.maxSets = static_cast<uint32_t>(m_sizePerPool);
	}

	void Allocator::ConfigureDescriptorPools(std::vector<VkDescriptorPoolSize> poolSizes, VkDescriptorPoolCreateInfo poolInfo)
	{
		m_poolSizes = poolSizes;
		m_poolInfo = poolInfo;
		m_poolInfo.poolSizeCount = static_cast<uint32_t>(m_poolSizes.size());
		m_poolInfo.pPoolSizes = m_poolSizes.data();
	}

	VkDescriptorPool Allocator::CreateDescriptorPool()
	{
		VkDescriptorPool descriptorPool{};
		CreateDescriptorPool(descriptorPool);
		return descriptorPool;
	}

	void Allocator::CreateDescriptorPool(VkDescriptorPool& descriptorPool)
	{
		// Refer to - https://vulkan-tutorial.com/en/Uniform_buffers/Descriptor_pool_and_sets
		// And for combined sampler - https://vulkan-tutorial.com/en/Texture_mapping/Combined_image_sampler

		if (DeviceManager::logicalDevice.GetDevice() != VK_NULL_HANDLE && vkCreateDescriptorPool(DeviceManager::logicalDevice.GetDevice(), &m_poolInfo, nullptr, &descriptorPool) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create descriptor pool!");
		}
	}

	void Allocator::AllocateDescriptorSets(std::vector<VkDescriptorSet>& descriptorSets, std::vector<VkBuffer>& uniformBuffers, std::map<uint32_t, TexturePipelineData>& materialTextures, std::map<uint32_t, Texture>& meshTextures)
	{
		if (m_type != AllocatorType::Null)
		{
			descriptorSets.resize(VulkanManager::MAX_FRAMES_IN_FLIGHT);

			CheckPoolAvailability();

			std::vector<VkDescriptorSetLayout> layouts(VulkanManager::MAX_FRAMES_IN_FLIGHT, m_descriptorSetLayout);
			VkDescriptorSetAllocateInfo allocInfo{};
			allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			allocInfo.descriptorPool = m_descriptorPools[m_currentPoolIndex];
			allocInfo.descriptorSetCount = static_cast<uint32_t>(VulkanManager::MAX_FRAMES_IN_FLIGHT);
			allocInfo.pSetLayouts = layouts.data();

			VkResult err = vkAllocateDescriptorSets(DeviceManager::logicalDevice.GetDevice(), &allocInfo, descriptorSets.data());
			if (err != VK_SUCCESS)
			{
				FlatEngine::Logger::log.Critical("failed to allocate descriptor sets!");
				return;
			}

			for (int i = 0; i < VulkanManager::MAX_FRAMES_IN_FLIGHT; i++)
			{
				int descriptorCounter = 0;
				size_t newSize = materialTextures.size();
				std::vector<VkWriteDescriptorSet> descriptorWrites{};
				descriptorWrites.resize(newSize);

				if (uniformBuffers.size())
				{
					descriptorWrites.resize(newSize + (size_t)1);

					VkDescriptorBufferInfo bufferInfo{};
					bufferInfo.buffer = uniformBuffers[i];
					bufferInfo.offset = 0;
					bufferInfo.range = sizeof(CustomUBO);

					descriptorWrites[descriptorCounter].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
					descriptorWrites[descriptorCounter].dstSet = descriptorSets[i];
					descriptorWrites[descriptorCounter].dstBinding = 0;
					descriptorWrites[descriptorCounter].dstArrayElement = 0;
					descriptorWrites[descriptorCounter].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
					descriptorWrites[descriptorCounter].descriptorCount = 1;
					descriptorWrites[descriptorCounter].pBufferInfo = &bufferInfo;

					descriptorCounter++;
				}

				std::vector<VkDescriptorImageInfo> imageInfos{};
				imageInfos.resize(m_texturePipelineData->size());

				int imageIndex = 0;
				for (std::map<uint32_t, TexturePipelineData>::iterator iter = materialTextures.begin(); iter != materialTextures.end(); iter++)
				{
					if (meshTextures.count(iter->first))
					{
						imageInfos[imageIndex].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
						imageInfos[imageIndex].imageView = meshTextures.at(iter->first).GetImageViews()[i];
						imageInfos[imageIndex].sampler = meshTextures.at(iter->first).GetSampler();

						descriptorWrites[descriptorCounter].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
						descriptorWrites[descriptorCounter].dstSet = descriptorSets[i];
						descriptorWrites[descriptorCounter].dstBinding = descriptorCounter;
						descriptorWrites[descriptorCounter].dstArrayElement = 0;
						descriptorWrites[descriptorCounter].descriptorType = meshTextures.at(iter->first).GetDescriptorType();
						descriptorWrites[descriptorCounter].descriptorCount = 1;
						descriptorWrites[descriptorCounter].pImageInfo = &imageInfos[imageIndex];

						meshTextures.at(iter->first).SetAllocationIndex((int)m_currentPoolIndex);
						descriptorCounter++;
						imageIndex++;
					}
				}

				if (descriptorWrites.size() != descriptorCounter)
				{
					descriptorWrites.resize((size_t)descriptorCounter);
				}

				vkUpdateDescriptorSets(DeviceManager::logicalDevice.GetDevice(), static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
				m_allocationsRemainingByPool[m_currentPoolIndex] -= 1;
			}
		}
		else
		{			
			for (std::map<uint32_t, TexturePipelineData>::iterator iter = materialTextures.begin(); iter != materialTextures.end(); iter++)
			{
				if (meshTextures.count(iter->first))
				{
					meshTextures.at(iter->first).SetAllocationIndex(-1);
				}
			}

			FlatEngine::Logger::log.Err("Allocator has not been initialized yet is trying to allocate DescriptorSets!");
		}
	}

	void Allocator::CheckPoolAvailability()
	{
		uint32_t availableSets = m_allocationsRemainingByPool[m_currentPoolIndex];

		switch (m_type)
		{
		case AllocatorType::DescriptorPool:

			if (availableSets < VulkanManager::MAX_FRAMES_IN_FLIGHT)
			{
				VkDescriptorPool descriptorPool{};
				CreateDescriptorPool(descriptorPool);
				m_descriptorPools.push_back(descriptorPool);
				m_allocationsRemainingByPool.push_back(m_sizePerPool);
				m_setsFreedByPool.push_back(0);
				m_currentPoolIndex++;
			}

			break;
		case AllocatorType::CommandPool:
			if (availableSets == 0)
			{
				VkDescriptorPool descriptorPool{};
				// CreateCommandPool(descriptorPool);
				// m_descriptorPools.push_back(descriptorPool);
				// m_allocationsRemainingByPool.push_back(m_sizePerPool);
				// m_setsFreedByPool.push_back(0);
				// m_currentPoolIndex++;
			}
			break;
		default:
			break;
		}
	}

	void Allocator::SetFreed(uint32_t freedFrom)
	{
		m_setsFreedByPool[freedFrom]++;

		if (m_setsFreedByPool[freedFrom] == m_sizePerPool)
		{
			switch (m_type)
			{
			case AllocatorType::DescriptorPool:

				VulkanManager::vulkan.QueueDescriptorPoolDeletion(m_descriptorPools[freedFrom]);				

				break;
			case AllocatorType::CommandPool:
				break;
			default:
				break;
			}
		}
	}
}