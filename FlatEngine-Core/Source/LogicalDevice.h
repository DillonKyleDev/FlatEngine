#pragma once
#include "PhysicalDevice.h"

#include "vulkan/vulkan_core.h"


namespace FlatEngine
{
	class LogicalDevice
	{
	public:
		LogicalDevice();
		~LogicalDevice();

		VkDevice& GetDevice();
		VkQueue& GetGraphicsQueue();
		VkQueue& GetPresentQueue();
		void SetGraphicsIndex(uint32_t index);
		uint32_t GetGraphicsIndex();
		void SetGraphicsPipelineCache(VkPipelineCache& cache);
		VkPipelineCache& GetGraphicsPipelineCache();
		void Init(PhysicalDevice& physicalDevice, VkSurfaceKHR& surface);
		void Cleanup();

	private:
		VkDevice m_device;
		VkQueue m_graphicsQueue;
		VkQueue m_presentQueue;
		uint32_t m_graphicsQueueFamilyIndex;
		VkPipelineCache m_graphicsPipelineCache;
	};
}

