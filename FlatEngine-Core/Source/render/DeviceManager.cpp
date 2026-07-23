#include "render/DeviceManager.h"
#include "render/Helper.h"
#include "render/Structs.h"
#include "render/VulkanManager.h"

#include <set>


namespace FlatEngine
{
    namespace DeviceManager
    {
        LogicalDevice logicalDevice = LogicalDevice();
        PhysicalDevice physicalDevice = PhysicalDevice();

        
        PhysicalDevice::PhysicalDevice()
        {
            m_physicalDevice = VK_NULL_HANDLE;
            m_physicalDeviceProperties = VkPhysicalDeviceProperties();
            m_b_fillModeNonSolid = VK_FALSE;
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

            VkPhysicalDevice backupDevice;
            bool b_nonIntegratedDeviceFound = false;
            // Check if any of the devices are suitable for our needs
            for (const auto& device : devices)
            {
                // Here we are just taking the very first suitable device and going with it
                if (IsDeviceSuitable(device, surface))
                {
                    vkGetPhysicalDeviceProperties(device, &m_physicalDeviceProperties);
                    if (m_physicalDeviceProperties.deviceType != VkPhysicalDeviceType::VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU)
                    {
                        m_physicalDevice = device;
                        VulkanManager::vulkan.SetMaxSamples(Helper::GetMaxUsableSampleCount(m_physicalDevice));
                        b_nonIntegratedDeviceFound = true;
                        break;
                    }
                    else
                    {
                        backupDevice = device;
                    }
                }
            }

            if (!b_nonIntegratedDeviceFound)
            {
                m_physicalDevice = backupDevice;
                VulkanManager::vulkan.SetMaxSamples(Helper::GetMaxUsableSampleCount(m_physicalDevice));
                printf("No non-integrated phyiscal device found, using integrated graphics. This may lead to problems.");
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

            m_b_fillModeNonSolid = supportedFeatures.fillModeNonSolid;

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

            if (deviceFeatures.fillModeNonSolid) {
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

        VkBool32 PhysicalDevice::FillModeNonSolidEnabled()
        {
            return m_b_fillModeNonSolid;
        }

        void PhysicalDevice::Cleanup()
        {
            // TODO - I don't think there is anything to cleanup here
        }

        // LOGICAL

        LogicalDevice::LogicalDevice()
        {
            m_device = VK_NULL_HANDLE;
            m_graphicsQueue = VK_NULL_HANDLE;
            m_presentQueue = VK_NULL_HANDLE;
            m_graphicsPipelineCache = VK_NULL_HANDLE;
            m_graphicsQueueFamilyIndex = 0;
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
            createInfo.enabledExtensionCount = static_cast<uint32_t>(VulkanManager::DEVICE_EXTENSIONS.size()); // give number of enabled extensions
            createInfo.ppEnabledExtensionNames = VulkanManager::DEVICE_EXTENSIONS.data(); // give names of extensions enabled (ie. VK_KHR_swapchain)

            if (VulkanManager::b_ENABLE_VALIDATION_LAYERS)
            {
                createInfo.enabledLayerCount = static_cast<uint32_t>(VulkanManager::validationLayers.Size());
                createInfo.ppEnabledLayerNames = VulkanManager::validationLayers.Data();
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
}