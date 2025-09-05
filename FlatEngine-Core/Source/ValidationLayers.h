#pragma once

#include "vulkan/vulkan_core.h"

#include <vector>


namespace FlatEngine
{
    class ValidationLayers
    {
    public:
        ValidationLayers();
        ~ValidationLayers();

        void SetupDebugMessenger(VkInstance instance);
        bool CheckSupport();
        std::vector<const char*> GetRequiredExtensions(); // Get extensions for debug validation layers
        // Create info for debug messenger so we don't have to repeat code for debug messenger for vkCreateInstance and vkDestroyInstance calls
        void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
        std::vector<const char*>::size_type Size();
        const char* const* Data();
        void Cleanup(VkInstance instance);

        static VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
        static void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);
        static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);


    private:
        VkDebugUtilsMessengerEXT m_debugMessenger;
        const std::vector<const char*> m_layers = {
            "VK_LAYER_KHRONOS_validation"
        };
    };
}