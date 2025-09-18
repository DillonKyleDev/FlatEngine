#include "LogicalDevice.h"
#include "VulkanManager.h"
#include "Structs.h"
#include "Helper.h"

#include <set>
#include <stdexcept>


namespace FlatEngine
{
    LogicalDevice::LogicalDevice()
    {
        m_device = VK_NULL_HANDLE;
        m_graphicsQueue = VK_NULL_HANDLE;
        m_presentQueue = VK_NULL_HANDLE;
        m_graphicsPipelineCache = VK_NULL_HANDLE;
        m_graphicsQueueFamilyIndex = 0;
    }

    LogicalDevice::~LogicalDevice()
    {
    }

    VkDevice& LogicalDevice::GetDevice()
    {
        return m_device;
    }

    VkQueue& LogicalDevice::GetGraphicsQueue()
    {
        return m_graphicsQueue;
    }

    VkQueue& LogicalDevice::GetPresentQueue()
    {
        return m_presentQueue;
    }

    void LogicalDevice::SetGraphicsIndex(uint32_t index)
    {
        m_graphicsQueueFamilyIndex = index;
    }

    uint32_t LogicalDevice::GetGraphicsIndex()
    {
        return m_graphicsQueueFamilyIndex;
    }

    void LogicalDevice::SetGraphicsPipelineCache(VkPipelineCache& cache)
    {
        m_graphicsPipelineCache = cache;
    }

    VkPipelineCache& LogicalDevice::GetGraphicsPipelineCache()
    {
        return m_graphicsPipelineCache;
    }

    void LogicalDevice::Init(PhysicalDevice& physicalDevice, VkSurfaceKHR& surface)
    {
        // Create logical device to interface with the m_phyiscalDevice and queues for the device

        // Setup just requires filling a struct with information about the specific physical device we want to control
        QueueFamilyIndices indices = Helper::FindQueueFamilies(physicalDevice.GetDevice(), surface);

        // For each queue family, (graphics, presenting, etc..), we need to create a queue and get a queue handle for it. Create collection of queueFamilies to do this programatically
        // We create unique createInfos for each of these queueFamilies
        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };


        float f_queuePriority = 1.0f; // Vulkan lets you assign priorities to queues to influence the scheduling of command buffer execution using floating point numbers between 0.0 and 1.0
        for (uint32_t ui_queueFamily : uniqueQueueFamilies)
        {
            // Create info struct with info to pass to the logical device later
            VkDeviceQueueCreateInfo queueCreateInfo{};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = ui_queueFamily;
            queueCreateInfo.queueCount = 1; // Number of queues, will only ever really need 1 because of multi threaded command buffers
            queueCreateInfo.pQueuePriorities = &f_queuePriority;
            queueCreateInfos.push_back(queueCreateInfo);
        }

        // Create features struct with info to pass to the logical device later
        VkPhysicalDeviceFeatures deviceFeatures{};
        deviceFeatures.samplerAnisotropy = VK_TRUE; // Anisotropic texture filtering
        deviceFeatures.sampleRateShading = VK_TRUE; // enable sample shading feature for the device (at a potential performance cost)
        deviceFeatures.fillModeNonSolid = physicalDevice.FillModeNonSolidEnabled();  // allows for wireframe rendering if enabled
        // ^^ Need to also change multisampling.sampleShadingEnable and multisampling.minSampleShading to correctly switch this on and off in createGraphicsPipeline() function


        // Now we can start filling out the VkDeviceCreateInfo structure for our logical device with the structs created above
        VkDeviceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.pQueueCreateInfos = queueCreateInfos.data();
        createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
        createInfo.pEnabledFeatures = &deviceFeatures;


        // NOTE FROM WIKI: The remainder of the information bears a resemblance to the VkInstanceCreateInfo struct and requires you to specify extensions and validation layers. The difference is that these are device specific this time.
        createInfo.enabledExtensionCount = static_cast<uint32_t>(DEVICE_EXTENSIONS.size()); // give number of enabled extensions
        createInfo.ppEnabledExtensionNames = DEVICE_EXTENSIONS.data(); // give names of extensions enabled (ie. VK_KHR_swapchain)

        if (b_ENABLE_VALIDATION_LAYERS)
        {
            createInfo.enabledLayerCount = static_cast<uint32_t>(VM_validationLayers.Size());
            createInfo.ppEnabledLayerNames = VM_validationLayers.Data();
        }
        else
        {
            createInfo.enabledLayerCount = 0;
        }

        // Actually create the logical device with queue and feature usage info stucts created above
        if (vkCreateDevice(physicalDevice.GetDevice(), &createInfo, nullptr, &m_device) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create logical device.");
        }
        else
        {
            // Get queuehandles for our newly created device queues to be able to use them to send commands to
            vkGetDeviceQueue(m_device, indices.graphicsFamily.value(), 0, &m_graphicsQueue); // Graphics queue handle
            // ^^ NOTE FROM WIKI: The parameters are the logical device, queue family, queue index and a pointer to the variable to store the queue handle in. Because we're only creating a single queue from this family, we'll simply use index 0.
            vkGetDeviceQueue(m_device, indices.presentFamily.value(), 0, &m_presentQueue); // Present queue handle
            // ^^ NOTE FROM WIKI: In case the queue families are the same, the two handles will most likely have the same value now. 
        }
    }

    void LogicalDevice::Cleanup()
    {
        vkDestroyDevice(m_device, nullptr);
    }
}