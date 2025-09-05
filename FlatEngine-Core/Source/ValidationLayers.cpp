#include "ValidationLayers.h"
#include "VulkanManager.h"

#include <stdexcept>
#include <iostream>


namespace FlatEngine
{
    ValidationLayers::ValidationLayers()
    {
        m_debugMessenger = VK_NULL_HANDLE;
    }

    ValidationLayers::~ValidationLayers()
    {
    }

    bool ValidationLayers::CheckSupport()
    {
        // Get list of available layers
        uint32_t ui_layerCount;
        vkEnumerateInstanceLayerProperties(&ui_layerCount, nullptr);
        std::vector<VkLayerProperties> availableLayers(ui_layerCount);
        vkEnumerateInstanceLayerProperties(&ui_layerCount, availableLayers.data());

        // Check if all requested layers exist within those layers
        for (const char* layerName : m_layers)
        {
            bool b_layerFound = false;

            for (const auto& layerProperties : availableLayers)
            {
                if (strcmp(layerName, layerProperties.layerName) == 0)
                {
                    b_layerFound = true;
                    break;
                }
            }

            if (!b_layerFound)
            {
                return false;
            }
        }

        return true;
    }

    void ValidationLayers::SetupDebugMessenger(VkInstance instance)
    {
        if (!b_ENABLE_VALIDATION_LAYERS)
            return;

        VkDebugUtilsMessengerCreateInfoEXT createInfo{};
        PopulateDebugMessengerCreateInfo(createInfo);

        // Create instance of debugMessenger
        if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &m_debugMessenger) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to set up debug messenger.");
        }
    }

    // Get extensions for debug validation layers
    std::vector<const char*> ValidationLayers::GetRequiredExtensions()
    {
        uint32_t ui_glfwExtensionCount = 0;
        //const char** glfwExtensions;        
        //glfwExtensions = glfwGetRequiredInstanceExtensions(&ui_glfwExtensionCount);

        std::vector<const char*> extensions; //(glfwExtensions, glfwExtensions + ui_glfwExtensionCount);

        if (b_ENABLE_VALIDATION_LAYERS) {
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }

        return extensions;
    }

    // Create info for debug messenger so we don't have to repeat code for debug messenger for vkCreateInstance and vkDestroyInstance calls
    void ValidationLayers::PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
    {
        createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        createInfo.pfnUserCallback = DebugCallback;
    }

    std::vector<const char*>::size_type ValidationLayers::Size()
    {
        return m_layers.size();
    }

    const char* const* ValidationLayers::Data()
    {
        return m_layers.data();
    }

    void ValidationLayers::Cleanup(VkInstance instance)
    {
        // Destroy validation messenger with specific vkDestroyDebugUtilsMessengerEXT looked up by following function (system specific)
        if (b_ENABLE_VALIDATION_LAYERS)
        {
            DestroyDebugUtilsMessengerEXT(instance, m_debugMessenger, nullptr);
        }
    }

    // Because the function vkCreateDebugUtilsMessengerEXT for creating debug messenger object is an extention function (specific to each environment),
    // it must be looked up manually per system - This function does that
    VkResult ValidationLayers::CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger)
    {
        auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
        if (func != nullptr)
        {
            return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
        }
        else
        {
            return VK_ERROR_EXTENSION_NOT_PRESENT;
        }
    }

    // Because the function DestroyDebugUtilsMessengerEXT for creating debug messenger object is an extention function (specific to each environment),
    // it must be looked up manually per system - This function does that
    void ValidationLayers::DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator)
    {
        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
        if (func != nullptr) {
            func(instance, debugMessenger, pAllocator);
        }
    }

    // NOTE FROM WIKI: If you want to see which call triggered a message, you can add a breakpoint to the message callback and look at the stack trace.
    // Custom function to be called by the validation layer system when an error occurs
    VKAPI_ATTR VkBool32 VKAPI_CALL ValidationLayers::DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
    {

        std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

        if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
            // Message is important enough to show
        }

        return VK_FALSE;
    }

}