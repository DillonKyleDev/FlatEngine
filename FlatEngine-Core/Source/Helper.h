#pragma once
#include "Structs.h"
#include "PhysicalDevice.h"
#include "LogicalDevice.h"

#include "vulkan/vulkan_core.h"

#include <vector>
#include <string>


namespace FlatEngine
{
    class Helper
    {
    public:
        static QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);
        static VkSampleCountFlagBits GetMaxUsableSampleCount(VkPhysicalDevice physicalDevice);
        static SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);
        static bool CheckDeviceExtensionSupport(VkPhysicalDevice physicalDevice);
        static std::vector<std::string> FindAllFilesWithExtension(std::string dir, std::string extension);
        static VkCommandBuffer BeginSingleTimeCommands();
        static void EndSingleTimeCommands(VkCommandBuffer commandBuffer);
        static VkFormat FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features, VkPhysicalDevice physicalDevice);
        static VkFormat FindDepthFormat(VkPhysicalDevice physicalDevice);
        static bool HasStencilComponent(VkFormat format);
        static std::vector<char> ReadFile(const std::string& filename);
        static uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties, VkPhysicalDevice physicalDevice);
    };
}