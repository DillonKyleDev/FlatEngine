#pragma once

#include "PhysicalDevice.h"
#include "Structs.h"
#include "Helper.h"

#include <stdexcept>
#include <vector>


namespace FlatEngine
{
    PhysicalDevice::PhysicalDevice()
    {
        m_physicalDevice = VK_NULL_HANDLE;
        m_physicalDeviceProperties = VkPhysicalDeviceProperties();
    }

    PhysicalDevice::~PhysicalDevice()
    {

    }

    VkPhysicalDevice PhysicalDevice::GetDevice()
    {
        return m_physicalDevice;
    }

    void PhysicalDevice::Init(VkInstance instance, VkSurfaceKHR surface)
    {
        PickPhysicalDevice(instance, surface);
    }

    void PhysicalDevice::PickPhysicalDevice(VkInstance instance, VkSurfaceKHR surface)
    {
        // Get number of physical devices (graphics cards available)
        uint32_t ui_deviceCount = 0;
        vkEnumeratePhysicalDevices(instance, &ui_deviceCount, nullptr);

        if (ui_deviceCount == 0)
        {
            throw std::runtime_error("failed to find GPUs with Vulkan support.");
        }

        // Collect all available physical devices into devices vector
        std::vector<VkPhysicalDevice> devices(ui_deviceCount);
        vkEnumeratePhysicalDevices(instance, &ui_deviceCount, devices.data());

        // Check if any of the devices are suitable for our needs
        for (const auto& device : devices)
        {
            // Here we are just taking the very first suitable device and going with it
            if (IsDeviceSuitable(device, surface))
            {
                vkGetPhysicalDeviceProperties(device, &m_physicalDeviceProperties);
                m_physicalDevice = device;
                break;
            }
        }

        if (m_physicalDevice == VK_NULL_HANDLE)
        {
            throw std::runtime_error("failed to find a suitable GPU.");
        }
    }

    bool PhysicalDevice::IsDeviceSuitable(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface)
    {
        QueueFamilyIndices indices = Helper::FindQueueFamilies(physicalDevice, surface); // Get QueueFamilies

        bool b_extensionsSupported = Helper::CheckDeviceExtensionSupport(physicalDevice); // Check for extension support

        bool b_swapChainAdequate = false;
        if (b_extensionsSupported)
        {
            SwapChainSupportDetails swapChainSupport = Helper::QuerySwapChainSupport(physicalDevice, surface);
            b_swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
        }

        // Get supported features list on GPU device
        VkPhysicalDeviceFeatures supportedFeatures;
        vkGetPhysicalDeviceFeatures(physicalDevice, &supportedFeatures);

        return indices.isComplete() && b_extensionsSupported && b_swapChainAdequate && supportedFeatures.samplerAnisotropy;
    }

    int PhysicalDevice::RateDeviceSuitability(VkPhysicalDevice device)
    {
        // For use later when picking desirable features
        int score = 0;

        VkPhysicalDeviceProperties deviceProperties; // type, name, etc...
        vkGetPhysicalDeviceProperties(device, &deviceProperties);
        VkPhysicalDeviceFeatures deviceFeatures; // texture compression, 64 bit floats, multi viewport rendering, etc...
        vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

        // Discrete GPUs have a significant performance advantage
        if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
            score += 1000;
        }

        // Maximum possible size of textures affects graphics quality
        score += deviceProperties.limits.maxImageDimension2D;

        // Application can't function without geometry shaders
        if (!deviceFeatures.geometryShader) {
            return 0;
        }

        return score;
    }

    uint32_t PhysicalDevice::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
    {
        // Refer to - https://vulkan-tutorial.com/en/Vertex_buffers/Vertex_buffer_creation
        VkPhysicalDeviceMemoryProperties memProperties{};
        vkGetPhysicalDeviceMemoryProperties(m_physicalDevice, &memProperties);

        for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
        {
            if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
            {
                return i;
            }
        }

        throw std::runtime_error("failed to find suitable memory type!");
    }

    void PhysicalDevice::Cleanup()
    {
        // TODO - I don't think there is anything to cleanup here
    }
}