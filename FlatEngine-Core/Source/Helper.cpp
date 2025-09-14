#include "Helper.h"
#include "VulkanManager.h"
#include "FlatEngine.h"

#include <set>
#include <fstream>
#include <filesystem>


namespace FlatEngine
{
    QueueFamilyIndices Helper::FindQueueFamilies(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface)
    {
        // NOTE FROM WIKI: Almost every operation in Vulkan, anything from drawing to uploading textures, requires commands to be submitted to a queue. There are different types of queues that originate from different queue families and each family of queues allows only a subset of commands.

        QueueFamilyIndices indices;

        uint32_t ui_queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &ui_queueFamilyCount, nullptr); // Get number of families

        std::vector<VkQueueFamilyProperties> queueFamilies(ui_queueFamilyCount); // Use that number to initialize a container for familyProperties
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &ui_queueFamilyCount, queueFamilies.data()); // Put queue families into new container

        int i = 0;
        for (const auto& queueFamily : queueFamilies)
        {
            // Graphics
            if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) // Check this queue family flag for graphics and grabs it if it is
            {
                indices.graphicsFamily = i;
            }
            // Surface Presenting
            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &presentSupport); // checks if support for our specific surface is available in the queuefamily
            if (presentSupport) // Check this queue family flag for present support and grabs it if it is
            {
                indices.presentFamily = i;
            }

            // If we find a suitable card, exit early
            if (indices.isComplete())
            {
                break;
            }

            i++;
        }

        return indices;
    }

    VkSampleCountFlagBits Helper::GetMaxUsableSampleCount(VkPhysicalDevice physicalDevice)
    {
        // Refer to - https://vulkan-tutorial.com/Multisampling

        VkPhysicalDeviceProperties physicalDeviceProperties;
        vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);

        VkSampleCountFlags counts = physicalDeviceProperties.limits.framebufferColorSampleCounts & physicalDeviceProperties.limits.framebufferDepthSampleCounts;
        if (counts & VK_SAMPLE_COUNT_64_BIT) { return VK_SAMPLE_COUNT_64_BIT; }
        if (counts & VK_SAMPLE_COUNT_32_BIT) { return VK_SAMPLE_COUNT_32_BIT; }
        if (counts & VK_SAMPLE_COUNT_16_BIT) { return VK_SAMPLE_COUNT_16_BIT; }
        if (counts & VK_SAMPLE_COUNT_8_BIT) { return VK_SAMPLE_COUNT_8_BIT; }
        if (counts & VK_SAMPLE_COUNT_4_BIT) { return VK_SAMPLE_COUNT_4_BIT; }
        if (counts & VK_SAMPLE_COUNT_2_BIT) { return VK_SAMPLE_COUNT_2_BIT; }

        return VK_SAMPLE_COUNT_1_BIT;
    }

    SwapChainSupportDetails Helper::QuerySwapChainSupport(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface)
    {
        SwapChainSupportDetails details;

        // Capabilities
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &details.capabilities);

        // Formats
        uint32_t ui_formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &ui_formatCount, nullptr); // Get number of formats
        if (ui_formatCount != 0)
        {
            details.formats.resize(ui_formatCount); // resize our details.format member with that number
            vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &ui_formatCount, details.formats.data()); // Fill details.formats with available formats on device
        }

        // Present Modes
        uint32_t ui_presentModesCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &ui_presentModesCount, nullptr); // Get number of present modes
        if (ui_presentModesCount != 0)
        {
            details.presentModes.resize(ui_presentModesCount); // resize our details.presentModes member with that number
            vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &ui_presentModesCount, details.presentModes.data()); //  Fill details.presentModes with available formats on device
        }

        return details;
    }

    bool Helper::CheckDeviceExtensionSupport(VkPhysicalDevice physicalDevice)
    {
        // Checking for the ability to use the swap chain
        // NOTE FROM WIKI: Since image presentation is heavily tied into the window system and the surfaces associated with windows, it is not actually part of the Vulkan core. You have to enable the VK_KHR_swapchain device extension after querying for its support.

        uint32_t ui_extensionCount;
        vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &ui_extensionCount, nullptr); // Get number of available extensions

        std::vector<VkExtensionProperties> availableExtensions(ui_extensionCount);// Use that number to initialize a container
        vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &ui_extensionCount, availableExtensions.data()); // Fill that container with available extensions for this device

        std::set<std::string> requiredExtensions(DEVICE_EXTENSIONS.begin(), DEVICE_EXTENSIONS.end()); // Get list of extensions required for presenting images to window

        // Go through available extensions and if it is a required extension, erase it from requiredExtensions (like checking it off a list) Any extensions left in required after are not available on the device
        for (const auto& extension : availableExtensions)
        {
            requiredExtensions.erase(extension.extensionName);
        }

        return requiredExtensions.empty();
    }

    std::vector<std::string> Helper::FindAllFilesWithExtension(std::string dirPath, std::string extension)
    {
        std::vector<std::string> files;

        for (auto& p : std::filesystem::recursive_directory_iterator(dirPath))
        {
            if (p.path().extension() == extension || p.path().string().find(extension) != std::string::npos)
            {
                files.push_back(p.path().string());
            }
        }

        return files;
    }

    VkCommandBuffer Helper::BeginSingleTimeCommands()
    {
        VkCommandPool& commandPool = F_VulkanManager->GetCommandPool();
        LogicalDevice& logicalDevice = F_VulkanManager->GetLogicalDevice();

        // For copying our cpu staging buffer over to our actual device buffer
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.pNext = 0;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = commandPool;
        allocInfo.commandBufferCount = 1;

        VkCommandBuffer commandBuffer{};
        vkAllocateCommandBuffers(logicalDevice.GetDevice(), &allocInfo, &commandBuffer);

        // Then start recording the command buffer:
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.pNext = 0;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        vkBeginCommandBuffer(commandBuffer, &beginInfo);

        return commandBuffer;
    }

    void Helper::EndSingleTimeCommands(VkCommandBuffer commandBuffer)
    {
        VkCommandPool& commandPool = F_VulkanManager->GetCommandPool();
        LogicalDevice& logicalDevice = F_VulkanManager->GetLogicalDevice();

        // Stop recording
        vkEndCommandBuffer(commandBuffer);
        // Now execute the command buffer to complete the transfer
        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;
        // We just want to execute the transfer on the buffers immediately.
        VkResult err;
        err = vkQueueSubmit(logicalDevice.GetGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
        //assert(err == 0);
        vkQueueWaitIdle(logicalDevice.GetGraphicsQueue());
        // Don't forget to clean up the command buffer used for the transfer operation.
        vkFreeCommandBuffers(logicalDevice.GetDevice(), commandPool, 1, &commandBuffer);
    }

    VkFormat Helper::FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features, VkPhysicalDevice physicalDevice)
    {
        // Refer to - https://vulkan-tutorial.com/en/Depth_buffering

        for (VkFormat format : candidates) {
            VkFormatProperties props;
            vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &props);

            if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
                return format;
            }
            else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
                return format;
            }
        }

        throw std::runtime_error("failed to find supported format!");
    }


    VkFormat Helper::FindDepthFormat(VkPhysicalDevice physicalDevice)
    {
        return FindSupportedFormat(
            { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
            VK_IMAGE_TILING_OPTIMAL,
            VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT,
            physicalDevice
        );
    }

    bool Helper::HasStencilComponent(VkFormat format)
    {
        return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
    }

    std::vector<char> Helper::ReadFile(const std::string& filename)
    {
        // ate : Start reading at the end of the file - used to determine the size of the file to allocate a buffer
        // binary : Read the file as binary file(avoid text transformations)
        std::ifstream file(filename, std::ios::ate | std::ios::binary);

        if (!file.is_open())
        {
            throw std::runtime_error("failed to open file.");
        }

        size_t fileSize = (size_t)file.tellg();
        std::vector<char> buffer(fileSize);

        // Once we have the buffer size needed, go back to the beginning of the file and read again
        file.seekg(0);
        file.read(buffer.data(), fileSize);
        file.close();

        return buffer;
    }

    uint32_t Helper::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties, VkPhysicalDevice physicalDevice)
    {
        VkPhysicalDeviceMemoryProperties memProperties;
        vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

        for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
        {
            if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
            {
                return i;
            }
        }

        throw std::runtime_error("failed to find suitable memory type!");
    }
}