#include "Allocator.h"
#include "VulkanManager.h"
#include "FlatEngine.h"

#include <stdexcept>


namespace FlatEngine
{
	Allocator::Allocator()
	{
		m_type = AllocatorType::Null;
		m_sizePerPool = 0;
		m_startingPools = 1;
		m_textureCount = 0;
		m_currentPoolIndex = 0;
		m_descriptorSetLayout = VK_NULL_HANDLE;
		m_bindings = {};
		m_layoutInfo = {};
		m_layoutInfo.bindingCount = 0;
		m_poolSizes = {};
		m_poolInfo = {};
		m_deviceHandle = nullptr;
	}

	Allocator::~Allocator()
	{
		//CleanupPools();
	}

	void Allocator::CleanupPools()
	{
		switch (m_type)
		{
		case AllocatorType::DescriptorPool:
			for (uint32_t i = 0; i < m_descriptorPools.size(); i++)
			{
				vkDestroyDescriptorPool(m_deviceHandle->GetDevice(), m_descriptorPools[i], nullptr);
			}
			break;
		case AllocatorType::CommandPool:
			break;
		default:
			break;
		}
	}

	void Allocator::Init(AllocatorType type, uint32_t textureCount, LogicalDevice& logicalDevice, uint32_t perPool)
	{
		m_type = type;
		m_textureCount = textureCount;
		m_deviceHandle = &logicalDevice;
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
		// Default Descriptor Set Layout config
		VkDescriptorSetLayoutBinding uboLayoutBinding{};
		uboLayoutBinding.binding = 0;
		uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		uboLayoutBinding.descriptorCount = 1;
		uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		uboLayoutBinding.pImmutableSamplers = nullptr; // Optional

		m_bindings.push_back(uboLayoutBinding);

		for (uint32_t i = 0; i < m_textureCount; i++)
		{
			VkDescriptorSetLayoutBinding samplerLayoutBinding{};
			samplerLayoutBinding.binding = i + 1;
			samplerLayoutBinding.descriptorCount = 1;
			samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			samplerLayoutBinding.pImmutableSamplers = nullptr;
			samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

			m_bindings.push_back(samplerLayoutBinding);
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
		}
		else
		{
			m_layoutInfo.bindingCount = 0;
		}
	}

	void Allocator::CreateDescriptorSetLayout()
	{
		// Refer to - https://vulkan-tutorial.com/en/Uniform_buffers/Descriptor_layout_and_buffer

		std::vector<VkDescriptorSetLayoutBinding> bindings{};

		if (m_layoutInfo.bindingCount == 0)
		{
			SetDefaultDescriptorSetLayoutConfig();
		}

		if (vkCreateDescriptorSetLayout(m_deviceHandle->GetDevice(), &m_layoutInfo, nullptr, &m_descriptorSetLayout) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create descriptor set layout!");
		}
	}

	void Allocator::CleanupDescriptorSetLayout()
	{
		vkDestroyDescriptorSetLayout(m_deviceHandle->GetDevice(), m_descriptorSetLayout, nullptr);
	}

	VkDescriptorSetLayout& Allocator::GetDescriptorSetLayout()
	{
		return m_descriptorSetLayout;
	}

	void Allocator::FillPools()
	{
		if (m_poolSizes.size() == 0)
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
		m_poolSizes = std::vector<VkDescriptorPoolSize>(m_textureCount + 1, {});
		m_poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		m_poolSizes[0].descriptorCount = static_cast<uint32_t>(m_sizePerPool);
		for (uint32_t j = 1; j < m_textureCount + 1; j++)
		{
			m_poolSizes[j].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			m_poolSizes[j].descriptorCount = static_cast<uint32_t>(m_sizePerPool);
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

		if (vkCreateDescriptorPool(m_deviceHandle->GetDevice(), &m_poolInfo, nullptr, &descriptorPool) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create descriptor pool!");
		}
	}

	void Allocator::AllocateDescriptorSets(std::vector<VkDescriptorSet>& descriptorSets, Model& model, std::vector<Texture>& textures)
	{
		if (m_type != AllocatorType::Null)
		{
			descriptorSets.resize(VM_MAX_FRAMES_IN_FLIGHT);

			CheckPoolAvailability();

			std::vector<VkDescriptorSetLayout> layouts(VM_MAX_FRAMES_IN_FLIGHT, m_descriptorSetLayout);
			VkDescriptorSetAllocateInfo allocInfo{};
			allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			allocInfo.descriptorPool = m_descriptorPools[m_currentPoolIndex];
			allocInfo.descriptorSetCount = static_cast<uint32_t>(VM_MAX_FRAMES_IN_FLIGHT);
			allocInfo.pSetLayouts = layouts.data();

			VkResult err = vkAllocateDescriptorSets(m_deviceHandle->GetDevice(), &allocInfo, descriptorSets.data());
			if (err != VK_SUCCESS)
			{
				FlatEngine::LogError("failed to allocate descriptor sets!");
			}

			for (int i = 0; i < VM_MAX_FRAMES_IN_FLIGHT; i++)
			{
				int descriptorCounter = 0;
				int newSize = textures.size();
				std::vector<VkWriteDescriptorSet> descriptorWrites{};
				descriptorWrites.resize(newSize);

				if (model.GetModelPath() != "")
				{
					descriptorWrites.resize(newSize + 1);

					VkDescriptorBufferInfo bufferInfo{};
					bufferInfo.buffer = model.GetUniformBuffers()[i];
					bufferInfo.offset = 0;
					bufferInfo.range = sizeof(UniformBufferObject);

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
				imageInfos.resize(m_textureCount);

				for (size_t j = 0; j < textures.size(); j++)
				{
					imageInfos[j].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
					imageInfos[j].imageView = textures[j].GetImageView();
					imageInfos[j].sampler = textures[j].GetTextureSampler();

					descriptorWrites[descriptorCounter].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
					descriptorWrites[descriptorCounter].dstSet = descriptorSets[i];
					descriptorWrites[descriptorCounter].dstBinding = descriptorCounter;
					descriptorWrites[descriptorCounter].dstArrayElement = 0;
					descriptorWrites[descriptorCounter].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
					descriptorWrites[descriptorCounter].descriptorCount = 1;
					descriptorWrites[descriptorCounter].pImageInfo = &imageInfos[j];

					textures[j].SetAllocationIndex((int)m_currentPoolIndex);
					descriptorCounter++;
				}

				vkUpdateDescriptorSets(m_deviceHandle->GetDevice(), static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
				m_allocationsRemainingByPool[m_currentPoolIndex] -= descriptorCounter;
			}
		}
		else
		{			
			for (size_t j = 0; j < m_textureCount; j++)
			{
				textures[j].SetAllocationIndex(-1);
			}
			FlatEngine::LogError("Allocator has not been initialized yet is trying to allocate DescriptorSets!");
		}
	}

	void Allocator::CheckPoolAvailability()
	{
		uint32_t availableSets = m_allocationsRemainingByPool[m_currentPoolIndex];

		switch (m_type)
		{
		case AllocatorType::DescriptorPool:

			if (availableSets == 0)
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

				vkDestroyDescriptorPool(m_deviceHandle->GetDevice(), m_descriptorPools[freedFrom], nullptr);

				break;
			case AllocatorType::CommandPool:
				break;
			default:
				break;
			}
		}
	}

	//void Allocator::AllocateCommandBuffers(std::vector<VkCommandBuffer>& commandBuffers, uint32_t& allocatedFrom)
	//{

	//}
}