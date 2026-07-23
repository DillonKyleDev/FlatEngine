#pragma once

#include "vulkan/vulkan_core.h"


namespace FlatEngine
{
	namespace DeviceManager
	{
		class PhysicalDevice
        {
        public:
            PhysicalDevice();            

            VkPhysicalDevice GetDevice();
            void Init(VkInstance instance, VkSurfaceKHR surface);
            void PickPhysicalDevice(VkInstance instance, VkSurfaceKHR surface);
            bool IsDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface);
            int RateDeviceSuitability(VkPhysicalDevice device);
            uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
            VkBool32 FillModeNonSolidEnabled();
            void Cleanup();

        private:
            VkPhysicalDevice m_physicalDevice;
            VkPhysicalDeviceProperties m_physicalDeviceProperties;
            VkBool32 m_b_fillModeNonSolid;
        };


		class LogicalDevice
		{
		public:
			LogicalDevice();			

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

		extern PhysicalDevice physicalDevice;
		extern LogicalDevice logicalDevice; 
	}
}

