#include "managers/Assets.h"
#include "managers/Settings.h"
#include "render/DeviceManager.h"
#include "render/Helper.h"
#include "render/RenderWindow.h"
#include "render/SceneView.h"
#include "render/VulkanManager.h"
#include "tools/FileHelper.h"
#include "tools/JsonHelper.h"
#include "tools/Logger.h"

#include <imgui_impl_sdl2.h>
#include <imgui_impl_vulkan.h>
#include <fstream>
#include <list>
#include "SDL_vulkan.h"

namespace FL = FlatEngine;


namespace FlatEngine
{
    namespace VulkanManager
    {
        // TODO: Remove PipelineManager class and move RenderPasses into Materials alongside GraphicsPipeline to be controlled by each Material. 
        
        Vulkan vulkan = Vulkan();	
        std::string selectedMaterialName = "";
        ValidationLayers validationLayers = ValidationLayers();
        uint32_t currentFrame = 0;
        uint32_t imageCount = 0;
        int MAX_FRAMES_IN_FLIGHT = 2;
        // sprite size * 0.1f brings sprite down from 1 sprite pixel per 1 grid block to 10 sprite pixels per 1 grid block
        // then to make an 8x8 sprite fit properly into a 10px by 10px grid block
        // take the ratio of 10px / 8px and multiply it by our now scaled down sprite size to get:  0.1 * (10 / 8) = 0.125 = the scale multiplier
        // Now our 8x8, 16x16, 32x32, etc, pixel art fits nicely inside the grid space blocks
        float pixelsPerGridSpace = 8.0f;
        float spriteScaleMultiplier = 0.1f * (10.0f / pixelsPerGridSpace);
        int maxSpriteLayers = 55;


        void Vulkan::check_vk_result(VkResult err)
        {
            if (err == 0)
            {
                return;
            }
            fprintf(stderr, "[vulkan] Error: VkResult = %d\n", err);
            if (err < 0)
            {
                abort();
            }
        }

        Vulkan::Vulkan()
        {
            m_instance = VK_NULL_HANDLE;
            RenderWindow::window = RenderWindow::Window();
            DeviceManager::physicalDevice = DeviceManager::PhysicalDevice();
            DeviceManager::logicalDevice = DeviceManager::LogicalDevice();       

            m_renderToTextureSceneViewRenderPass = RenderPass();
            m_renderToTextureGameViewRenderPass = RenderPass();
            m_postProcessingRenderPass = RenderPass();
            m_imGuiRenderPass = RenderPass();

            m_postProcessingDescriptorSets = std::vector<VkDescriptorSet>();

            // gpu communication
            m_systemCommandPool = VK_NULL_HANDLE;
            m_imGuiCommandPool = VK_NULL_HANDLE;
            m_sceneViewCommandPool = VK_NULL_HANDLE;
            m_gameViewCommandPool = VK_NULL_HANDLE;
            m_postProcessingCommandPool = VK_NULL_HANDLE;
            m_imageAvailableSemaphores = std::vector<VkSemaphore>();
            m_renderFinishedSemaphores = std::vector<VkSemaphore>();
            m_inFlightFences = std::vector<VkFence>();
            m_b_framebufferResized = false; 

            m_imGuiMaterial = std::shared_ptr<Material>();
            m_sceneViewMaterials = std::map<std::string, std::shared_ptr<Material>>();
            m_gameViewMaterials = std::map<std::string, std::shared_ptr<Material>>();
            m_sceneViewMaterialMeshes = std::map<std::string, std::map<std::string, std::map<long, Mesh*>>>();
            m_gameViewMaterialMeshes = std::map<std::string, std::map<std::string, std::map<long, Mesh*>>>();
            m_sceneViewTexture = Texture();
            m_gameViewTexture = Texture();
            m_postProcessingTexture = Texture();
            m_models = std::map<std::string, std::shared_ptr<Model>>();

            m_bufferDeleteQueue= std::vector<VkBuffer>();
            m_deviceMemoryDeleteQueue = std::vector<VkDeviceMemory>();
            m_descriptorPoolDeleteQueue = std::vector<VkDescriptorPool>();

            m_maxSamples = VK_SAMPLE_COUNT_1_BIT;
        }

        Vulkan::~Vulkan()
        {
        }

        void Vulkan::Cleanup()
        {
            for (std::map<std::string, std::shared_ptr<Model>>::iterator model = m_models.begin(); model != m_models.end(); model++)
            {
                model->second->Cleanup();
            }

            QuitImGui();

            vkDeviceWaitIdle(DeviceManager::logicalDevice.GetDevice()); // This may need to be moved elsewhere potentially

            // Semaphores and Fences
            for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
            {
                vkDestroySemaphore(DeviceManager::logicalDevice.GetDevice(), m_imageAvailableSemaphores[i], nullptr);
                vkDestroySemaphore(DeviceManager::logicalDevice.GetDevice(), m_renderFinishedSemaphores[i], nullptr);
                vkDestroyFence(DeviceManager::logicalDevice.GetDevice(), m_inFlightFences[i], nullptr);
            }

            RenderWindow::window.CleanupDrawingResources();

            for (std::map<std::string, std::shared_ptr<Material>>::iterator material = m_sceneViewMaterials.begin(); material != m_sceneViewMaterials.end(); material++)
            {
                material->second->Cleanup();
            }
            for (std::map<std::string, std::shared_ptr<Material>>::iterator material = m_gameViewMaterials.begin(); material != m_gameViewMaterials.end(); material++)
            {
                material->second->Cleanup();
            }

            vkDestroyCommandPool(DeviceManager::logicalDevice.GetDevice(), m_systemCommandPool, nullptr);
            vkDestroyCommandPool(DeviceManager::logicalDevice.GetDevice(), m_imGuiCommandPool, nullptr);
            vkDestroyCommandPool(DeviceManager::logicalDevice.GetDevice(), m_sceneViewCommandPool, nullptr);
            vkDestroyCommandPool(DeviceManager::logicalDevice.GetDevice(), m_gameViewCommandPool, nullptr);

            DeviceManager::logicalDevice.Cleanup();
            DeviceManager::physicalDevice.Cleanup();
            validationLayers.Cleanup(m_instance);
            RenderWindow::window.CleanupSystem();

            m_renderToTextureSceneViewRenderPass.Cleanup();
            m_renderToTextureGameViewRenderPass.Cleanup();
            m_postProcessingRenderPass.Cleanup();
            m_imGuiRenderPass.Cleanup();

            // Destroy Vulkan instance
            vkDestroyInstance(m_instance, nullptr);
        }

        bool Vulkan::Init(int width, int height)
        {
            bool b_success = true;

            if (!RenderWindow::window.CreateSDLWindow("FlatEngine", width, height))
            {
                Logger::log.Critical("SDL window creation failed!\n");
                b_success = false;
            }
            else
            {
                if (!CreateVulkanInstance())
                {
                    Logger::log.Critical("Failed to create Vulkan instance!\n");
                    b_success = false;
                }
                else
                {
                    RenderWindow::window.SetHandles(&m_systemCommandPool);
                    RenderWindow::window.CreateSurface();
                    validationLayers.SetupDebugMessenger(m_instance);
                    DeviceManager::physicalDevice.Init(m_instance, RenderWindow::window.GetSurface());
                    DeviceManager::logicalDevice.Init(DeviceManager::physicalDevice, RenderWindow::window.GetSurface());
                    RenderWindow::window.CreateDrawingResources();
                    QueueFamilyIndices indices = Helper::FindQueueFamilies(DeviceManager::physicalDevice.GetDevice(), RenderWindow::window.GetSurface());
                    DeviceManager::logicalDevice.SetGraphicsIndex(indices.graphicsFamily.value());
                    CreateCommandPool(m_systemCommandPool, indices.graphicsFamily.value());
                    CreateCommandPool(m_imGuiCommandPool, indices.graphicsFamily.value());
                    CreateCommandPool(m_sceneViewCommandPool, indices.graphicsFamily.value());
                    CreateCommandPool(m_gameViewCommandPool, indices.graphicsFamily.value());   
                    CreateCommandPool(m_postProcessingCommandPool, indices.graphicsFamily.value());                
                    CreateSyncObjects();

                    m_sceneViewTexture.CreateRenderToTextureResources(m_sceneViewCommandPool);
                    m_gameViewTexture.CreateRenderToTextureResources(m_gameViewCommandPool);
                    m_postProcessingTexture.CreateRenderToTextureResources(m_postProcessingCommandPool);
                    
                    CreateRenderToTextureRenderPassResources(m_renderToTextureSceneViewRenderPass, m_sceneViewTexture, m_sceneViewCommandPool);
                    CreateRenderToTextureRenderPassResources(m_renderToTextureGameViewRenderPass, m_gameViewTexture, m_gameViewCommandPool);
                    CreatePostProcessingRenderPassResources();
                    CreateImGuiRendePassResources();

                    m_imGuiMaterial = LoadMaterial("../engine/materials/fl_imgui.mat", &m_imGuiRenderPass, false);
                    CreateImGuiResources();
                    m_imGuiMaterial->Init();

                    LoadEngineMaterials();                    

                    SceneView::CreateSceneViewGridObjects();
                }
            }

            return b_success;
        }

        void Vulkan::CreateTextureImage(VkImage& image, std::string path, uint32_t mipLevels, VkDeviceMemory& imageMemory)
        {
            image = RenderWindow::window.CreateTextureImage(path, mipLevels, imageMemory, m_systemCommandPool);
        }

        void Vulkan::CreateImageView(VkImageView& imageView, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels)
        {
            RenderWindow::window.CreateImageView(imageView, image, format, aspectFlags, mipLevels);
        }

        void Vulkan::CreateTextureSampler(VkSampler& textureSampler, uint32_t mipLevels)
        {
            RenderWindow::window.CreateTextureSampler(textureSampler, mipLevels);
        }

        bool Vulkan::CreateVulkanInstance()
        {
            bool b_success = true;

            // Validation layer setup for debugger
            if (b_ENABLE_VALIDATION_LAYERS && !validationLayers.CheckSupport())
            {
                Logger::log.Err("Error: Validation layers requested, but not available.\n");
                b_success = false;
            }

            VkApplicationInfo appInfo{};
            appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
            appInfo.pApplicationName = "FlatEngine";
            appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
            appInfo.pEngineName = "FlatEngine";
            appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
            appInfo.apiVersion = VK_API_VERSION_1_0;

            uint32_t sdlExtensionCount = 0;
            const char** sdlExtensionNames;
            SDL_Vulkan_GetInstanceExtensions(RenderWindow::window.GetWindow(), &sdlExtensionCount, nullptr);
            sdlExtensionNames = new const char* [sdlExtensionCount];
            SDL_Vulkan_GetInstanceExtensions(RenderWindow::window.GetWindow(), &sdlExtensionCount, sdlExtensionNames);

            VkInstanceCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
            createInfo.pApplicationInfo = &appInfo;

            VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{}; // DebugUtilsMessenger for CreateInstance and DestroyInstance functions (automatically destroyed by Vulkan when closed)

            // Get extensions for use with debug messenger
            auto validationExtensions = validationLayers.GetRequiredExtensions();
            uint32_t totalExtensionCount = (uint32_t)(sdlExtensionCount + validationExtensions.size());
            const char** extensionNames = new const char* [totalExtensionCount];
            for (uint32_t i = 0; i < sdlExtensionCount; i++)
            {
                extensionNames[i] = sdlExtensionNames[i];
            }
            for (uint32_t i = 0; i < validationExtensions.size(); i++)
            {
                extensionNames[sdlExtensionCount + i] = validationExtensions[i];
            }
            createInfo.enabledExtensionCount = totalExtensionCount;
            createInfo.ppEnabledExtensionNames = &extensionNames[0];

            if (b_ENABLE_VALIDATION_LAYERS)
            {
                createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.Size());
                createInfo.ppEnabledLayerNames = validationLayers.Data();
                validationLayers.PopulateDebugMessengerCreateInfo(debugCreateInfo);
                createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
            }
            else
            {
                createInfo.enabledLayerCount = 0;
                createInfo.pNext = nullptr;
            }

            if (vkCreateInstance(&createInfo, nullptr, &m_instance) != VK_SUCCESS)
            {
                Logger::log.Critical("Failed to create Vulkan instance...\n");
                b_success = false;
            }

            return b_success;
        }

        void Vulkan::CreateCommandPool(VkCommandPool& commandPool, uint32_t queueFamilyIndex, VkCommandPoolCreateFlags flags)
        {
            VkCommandPoolCreateInfo poolInfo{};
            poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
            poolInfo.flags = flags;
            poolInfo.queueFamilyIndex = queueFamilyIndex;

            if (vkCreateCommandPool(DeviceManager::logicalDevice.GetDevice(), &poolInfo, nullptr, &commandPool) != VK_SUCCESS)
            {
                throw std::runtime_error("failed to create command pool!");
            }
        }

        void Vulkan::CreateRenderToTextureRenderPassResources(RenderPass& renderPass, Texture& renderToTexture, VkCommandPool& commandPool)
        {
            renderPass.SetHandles(&commandPool);

            renderPass.EnableDepthBuffering();
            renderPass.EnableMsaa();
            VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_1_BIT; // F_Vulkan->GetMaxSamples();
            VkFormat colorFormat = VK_FORMAT_R32G32B32A32_SFLOAT;
            renderPass.SetImageColorFormat(colorFormat);
            renderPass.SetMSAASampleCount(msaaSamples);

            VkAttachmentDescription colorAttachment{};
            colorAttachment.format = colorFormat;
            colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
            colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
            colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
            colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            VkAttachmentReference colorAttachmentRef{};
            colorAttachmentRef.attachment = 0;
            colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            renderPass.AddRenderPassAttachment(colorAttachment, colorAttachmentRef);

            VkAttachmentDescription depthAttachment{};
            depthAttachment.format = Helper::FindDepthFormat(DeviceManager::physicalDevice.GetDevice());
            depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
            depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
            depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
            depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
            depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
            VkAttachmentReference depthAttachmentRef{};
            depthAttachmentRef.attachment = 1;
            depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
            renderPass.AddRenderPassAttachment(depthAttachment, depthAttachmentRef);

            VkAttachmentDescription colorAttachmentResolve{};
            colorAttachmentResolve.format = colorFormat;
            colorAttachmentResolve.samples = msaaSamples;
            colorAttachmentResolve.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
            colorAttachmentResolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
            colorAttachmentResolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            colorAttachmentResolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            colorAttachmentResolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            colorAttachmentResolve.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            VkAttachmentReference colorAttachmentResolveRef{};
            colorAttachmentResolveRef.attachment = 2;
            colorAttachmentResolveRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            renderPass.AddRenderPassAttachment(colorAttachmentResolve, colorAttachmentResolveRef);

            VkSubpassDescription subpass{};
            subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
            subpass.colorAttachmentCount = 1;
            subpass.pColorAttachments = &renderPass.GetAttachmentRefs()[0];
            subpass.pDepthStencilAttachment = &renderPass.GetAttachmentRefs()[1];
            subpass.pResolveAttachments = &renderPass.GetAttachmentRefs()[2];
            renderPass.AddSubpass(subpass);

            renderPass.ConfigureFrameBufferImageViews(renderToTexture.GetImageViews()); // Give m_renderPass the VkImageViews to write to their VkImages (to be used later by ImGui material)		

            renderPass.Init();
        }

        void Vulkan::CreatePostProcessingRenderPassResources()
        {
            m_postProcessingRenderPass.SetHandles(&m_postProcessingCommandPool);

            m_postProcessingRenderPass.EnableDepthBuffering();
            m_postProcessingRenderPass.EnableMsaa();
            VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_1_BIT; // F_Vulkan->GetMaxSamples();
            VkFormat colorFormat = VK_FORMAT_R32G32B32A32_SFLOAT;
            m_postProcessingRenderPass.SetImageColorFormat(colorFormat);
            m_postProcessingRenderPass.SetMSAASampleCount(msaaSamples);

            VkAttachmentDescription colorAttachment{};
            colorAttachment.format = colorFormat;
            colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
            colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
            colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
            colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            VkAttachmentReference colorAttachmentRef{};
            colorAttachmentRef.attachment = 0;
            colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            m_postProcessingRenderPass.AddRenderPassAttachment(colorAttachment, colorAttachmentRef);

            VkAttachmentDescription depthAttachment{};
            depthAttachment.format = Helper::FindDepthFormat(DeviceManager::physicalDevice.GetDevice());
            depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
            depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
            depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
            VkAttachmentReference depthAttachmentRef{};
            depthAttachmentRef.attachment = 1;
            depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
            m_postProcessingRenderPass.AddRenderPassAttachment(depthAttachment, depthAttachmentRef);

            VkAttachmentDescription colorAttachmentResolve{};
            colorAttachmentResolve.format = colorFormat;
            colorAttachmentResolve.samples = msaaSamples;
            colorAttachmentResolve.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
            colorAttachmentResolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
            colorAttachmentResolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            colorAttachmentResolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            colorAttachmentResolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            colorAttachmentResolve.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            VkAttachmentReference colorAttachmentResolveRef{};
            colorAttachmentResolveRef.attachment = 2;
            colorAttachmentResolveRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            m_postProcessingRenderPass.AddRenderPassAttachment(colorAttachmentResolve, colorAttachmentResolveRef);

            //VkSubpassDependency dependency{};
            //dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
            //dependency.dstSubpass = 0;
            //dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            //dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
            //dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
            //dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
            //m_postProcessingRenderPass.AddSubpassDependency(dependency);

            VkSubpassDependency dependency = {};
            dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
            dependency.dstSubpass = 0;
            dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            dependency.srcAccessMask = 0;
            dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            m_postProcessingRenderPass.AddSubpassDependency(dependency);

            VkSubpassDependency depthDependency = {};
            depthDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
            depthDependency.dstSubpass = 0;
            depthDependency.srcStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
            depthDependency.srcAccessMask = 0;
            depthDependency.dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
            depthDependency.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
            m_postProcessingRenderPass.AddSubpassDependency(depthDependency);

            VkSubpassDescription subpass{};
            subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
            subpass.colorAttachmentCount = 1;
            subpass.pColorAttachments = &m_postProcessingRenderPass.GetAttachmentRefs()[0];
            subpass.pDepthStencilAttachment = &m_postProcessingRenderPass.GetAttachmentRefs()[1];
            subpass.pResolveAttachments = &m_postProcessingRenderPass.GetAttachmentRefs()[2];
            m_postProcessingRenderPass.AddSubpass(subpass);

            m_postProcessingRenderPass.ConfigureFrameBufferImageViews(m_postProcessingTexture.GetImageViews()); // Give m_renderPass the VkImageViews to write to their VkImages (to be used later by ImGui material)		

            m_postProcessingRenderPass.Init();
        }

        void Vulkan::CreateImGuiRendePassResources()
        {
            m_imGuiRenderPass.SetHandles(&m_imGuiCommandPool);

            m_imGuiRenderPass.EnableMsaa();
            VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_1_BIT; // F_Vulkan->GetMaxSamples();
            VkFormat colorFormat = VK_FORMAT_B8G8R8A8_UNORM;
            m_imGuiRenderPass.SetImageColorFormat(colorFormat);
            m_imGuiRenderPass.SetMSAASampleCount(msaaSamples);

            VkAttachmentDescription colorAttachment = {};
            colorAttachment.format = RenderWindow::window.GetImageFormat();
            colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
            colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
            colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
            colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            VkAttachmentReference colorAttachmentRef = {};
            colorAttachmentRef.attachment = 0;
            colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            m_imGuiRenderPass.AddRenderPassAttachment(colorAttachment, colorAttachmentRef);

            VkAttachmentDescription colorAttachmentResolve{};
            colorAttachmentResolve.format = colorFormat;
            colorAttachmentResolve.samples = msaaSamples;
            colorAttachmentResolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            colorAttachmentResolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
            colorAttachmentResolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            colorAttachmentResolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            colorAttachmentResolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            colorAttachmentResolve.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
            VkAttachmentReference colorAttachmentResolveRef{};
            colorAttachmentResolveRef.attachment = 1;
            colorAttachmentResolveRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            m_imGuiRenderPass.AddRenderPassAttachment(colorAttachmentResolve, colorAttachmentResolveRef);
            
            VkSubpassDependency dependency{};
            dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
            dependency.dstSubpass = 0;
            dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
            dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
            dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
            m_imGuiRenderPass.AddSubpassDependency(dependency);

            VkSubpassDescription subpass = {};
            subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
            subpass.colorAttachmentCount = 1;
            subpass.pColorAttachments = &colorAttachmentRef;
            subpass.pResolveAttachments = &colorAttachmentResolveRef;
            m_imGuiRenderPass.AddSubpass(subpass);

            m_imGuiRenderPass.ConfigureFrameBufferImageViews(RenderWindow::window.GetSwapChainImageViews());

            m_imGuiRenderPass.Init();
        }

        void Vulkan::GetImGuiDescriptorSetLayoutInfo(std::vector<VkDescriptorSetLayoutBinding>& bindings, VkDescriptorSetLayoutCreateInfo& layoutInfo)
        {
            bindings.resize(1);
            bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            bindings[0].descriptorCount = 1;
            bindings[0].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
            bindings[0].binding = 0;
    
            //bindings[1].binding = 1;
            //bindings[1].descriptorCount = 1;
            //bindings[1].descriptorType = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
            //bindings[1].pImmutableSamplers = nullptr;
            //bindings[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

            layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
            layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
            layoutInfo.pBindings = bindings.data();
        }

        void Vulkan::GetImGuiDescriptorPoolInfo(std::vector<VkDescriptorPoolSize>& poolSizes, VkDescriptorPoolCreateInfo& poolInfo)
        {
            poolSizes =
            {
                { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
                { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
                { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
                { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
                { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
                { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
                { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
                { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
                { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
                { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
                { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
            };
            poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
            poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
            poolInfo.maxSets = 1000;
            poolInfo.poolSizeCount = (uint32_t)poolSizes.size();
            poolInfo.pPoolSizes = poolSizes.data();
        }

        void Vulkan::CreateImGuiResources()
        {
            // https://frguthmann.github.io/posts/vulkan_imgui/       

            IMGUI_CHECKVERSION();
            ImGui::CreateContext();
            ImGuiIO& io = ImGui::GetIO();
            io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;// | ImGuiConfigFlags_ViewportsEnable;
            //io.BackendFlags |= ImGuiBackendFlags_PlatformHasViewports | ImGuiBackendFlags_RendererHasViewports;

            Vulkan::CreateCommandPool(m_imGuiCommandPool, DeviceManager::logicalDevice.GetGraphicsIndex(), VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

            // Set up Descriptor Material Allocator
            std::vector<VkDescriptorSetLayoutBinding> bindings{};
            VkDescriptorSetLayoutCreateInfo layoutInfo{};
            GetImGuiDescriptorSetLayoutInfo(bindings, layoutInfo);
            std::vector<VkDescriptorPoolSize> poolSizes{};
            VkDescriptorPoolCreateInfo poolInfo{};
            GetImGuiDescriptorPoolInfo(poolSizes, poolInfo);

            m_imGuiMaterial->GetAllocator().ConfigureDescriptorSetLayout(bindings, layoutInfo);
            m_imGuiMaterial->GetAllocator().ConfigureDescriptorPools(poolSizes, poolInfo);
            m_imGuiMaterial->AddTexture(0, TexturePipelineData());
            m_imGuiMaterial->GetAllocator().Init(AllocatorType::DescriptorPool, m_imGuiMaterial->GetTexturesPipelineData());

            ImGui_ImplSDL2_InitForVulkan(RenderWindow::window.GetWindow());

            ImGui_ImplVulkan_InitInfo init_info = {};
            init_info.Instance = m_instance;
            init_info.PhysicalDevice = DeviceManager::physicalDevice.GetDevice();
            init_info.Device = DeviceManager::logicalDevice.GetDevice();
            init_info.QueueFamily = ImGui_ImplVulkanH_SelectQueueFamilyIndex(DeviceManager::physicalDevice.GetDevice());
            init_info.Queue = DeviceManager::logicalDevice.GetGraphicsQueue();
            init_info.PipelineCache = VK_NULL_HANDLE;
            init_info.DescriptorPool = m_imGuiMaterial->CreateDescriptorPool();
            init_info.RenderPass = m_imGuiRenderPass.GetRenderPass();
            init_info.Subpass = 0;
            init_info.MinImageCount = imageCount;
            init_info.ImageCount = imageCount;
            init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
            init_info.Allocator = nullptr;
            init_info.CheckVkResultFn = Vulkan::check_vk_result;

            if (!ImGui_ImplVulkan_Init(&init_info))
            {
                FlatEngine::Logger::log.Err("ImGui backends setup failed!");
            }
        }

        void Vulkan::CreateImGuiTexture(Texture& texture, std::vector<VkDescriptorSet>& descriptorSets)
        {
            texture.CreateTextureImage();        
            std::vector<VkBuffer> emptyUBO = std::vector<VkBuffer>();
            std::map<uint32_t, Texture> textures = std::map<uint32_t, Texture>();
            textures.emplace(0, texture);
            m_imGuiMaterial->CreateDescriptorSets(descriptorSets, emptyUBO, textures);
        }

        void Vulkan::FreeImGuiTexture(uint32_t allocatedFrom)
        {
            m_imGuiMaterial->GetAllocator().SetFreed(allocatedFrom);
        }

        void Vulkan::QuitImGui()
        {
            vkDestroyCommandPool(DeviceManager::logicalDevice.GetDevice(), m_imGuiCommandPool, nullptr);

            VkResult err = vkDeviceWaitIdle(DeviceManager::logicalDevice.GetDevice());
            Vulkan::check_vk_result(err);
            ImGui_ImplVulkan_Shutdown();
            ImGui_ImplSDL2_Shutdown();
            ImGui::DestroyContext();
        }

        void Vulkan::LoadEngineMaterials()
        {
            // TODO: Remove m_renderToTexture reference
            AddSceneViewMaterial(LoadMaterial("../engine/materials/fl_empty.mat", &m_renderToTextureSceneViewRenderPass));
            AddGameViewMaterial(LoadMaterial("../engine/materials/fl_empty.mat", &m_renderToTextureGameViewRenderPass));
            AddSceneViewMaterial(LoadMaterial("../engine/materials/fl_uv.mat", &m_renderToTextureSceneViewRenderPass));
            AddGameViewMaterial(LoadMaterial("../engine/materials/fl_uv.mat", &m_renderToTextureGameViewRenderPass));
            AddSceneViewMaterial(LoadMaterial("../engine/materials/fl_unlit.mat", &m_renderToTextureSceneViewRenderPass));
            AddGameViewMaterial(LoadMaterial("../engine/materials/fl_unlit.mat", &m_renderToTextureGameViewRenderPass));
            AddSceneViewMaterial(LoadMaterial("../engine/materials/fl_verticesOnly.mat", &m_renderToTextureSceneViewRenderPass));
            AddGameViewMaterial(LoadMaterial("../engine/materials/fl_verticesOnly.mat", &m_renderToTextureGameViewRenderPass));
            AddSceneViewMaterial(LoadMaterial("../engine/materials/fl_grid.mat", &m_renderToTextureSceneViewRenderPass));
            AddGameViewMaterial(LoadMaterial("../engine/materials/fl_grid.mat", &m_renderToTextureGameViewRenderPass));
            AddSceneViewMaterial(LoadMaterial("../engine/materials/fl_xAxis.mat", &m_renderToTextureSceneViewRenderPass));
            AddGameViewMaterial(LoadMaterial("../engine/materials/fl_xAxis.mat", &m_renderToTextureGameViewRenderPass));
            AddSceneViewMaterial(LoadMaterial("../engine/materials/fl_yAxis.mat", &m_renderToTextureSceneViewRenderPass));
            AddGameViewMaterial(LoadMaterial("../engine/materials/fl_yAxis.mat", &m_renderToTextureGameViewRenderPass));
            AddSceneViewMaterial(LoadMaterial("../engine/materials/fl_zAxis.mat", &m_renderToTextureSceneViewRenderPass));
            AddGameViewMaterial(LoadMaterial("../engine/materials/fl_zAxis.mat", &m_renderToTextureGameViewRenderPass));
            AddSceneViewMaterial(LoadMaterial("../engine/materials/fl_transformGizmo.mat", &m_renderToTextureSceneViewRenderPass));
            AddGameViewMaterial(LoadMaterial("../engine/materials/fl_transformGizmo.mat", &m_renderToTextureGameViewRenderPass));
        }

        void Vulkan::InitializeMaterials()
        {
            //m_sceneViewMaterials.clear();
            //m_gameViewMaterials.clear();

            LoadEngineMaterials();

            std::vector<std::string> materialFiles = std::vector<std::string>();
            materialFiles = FL::FileHelper::FindAllFilesWithExtension(Assets::assetManager.GetDir("projectDir"), ".mat");

            for (std::string path : materialFiles)
            {
                AddSceneViewMaterial(LoadMaterial(path, &m_renderToTextureSceneViewRenderPass));
                AddGameViewMaterial(LoadMaterial(path, &m_renderToTextureGameViewRenderPass));
            }
        }

        void Vulkan::SaveMaterial(std::shared_ptr<Material> material)
        {
            if (material != nullptr)
            {
                std::string filepath = material->GetPath();

                std::ofstream fileObject;
                std::ifstream ifstream(filepath);

                // Delete old contents of the file
                fileObject.open(filepath, std::ofstream::out | std::ofstream::trunc);
                fileObject.close();

                // Opening file in append mode
                fileObject.open(filepath, std::ios::app);

                std::string data = material->GetData();

                fileObject << data.c_str() << std::endl;
                fileObject.close();   

                if (m_sceneViewMaterialMeshes.count(material->GetName()))
                {
                    std::map<std::string, std::map<long, Mesh*>> meshesWithMaterial = m_sceneViewMaterialMeshes.at(material->GetName());
                    for (std::map<std::string, std::map<long, Mesh*>>::iterator iter = meshesWithMaterial.begin(); iter != meshesWithMaterial.end(); iter++)
                    {
                        for (std::map<long, Mesh*>::iterator meshIter = iter->second.begin(); meshIter != iter->second.end(); meshIter++)
                        {
                            meshIter->second->CreateResources();
                        }                    
                    }
                }
            }
        }

        std::shared_ptr<Material> Vulkan::LoadMaterial(std::string path, RenderPass* renderPass, bool b_init)
        {
            std::shared_ptr<Material> newMaterial = std::make_shared<Material>();        

            json materialData = FL::JsonHelper::LoadFileData(path);
            if (materialData != nullptr)
            {
                std::string name = JsonHelper::CheckJsonString(materialData, "name", "Material");
                newMaterial->SetName(name);
                newMaterial->SetPath(path);

                if (name == "")
                {
                    newMaterial->SetName(FL::FileHelper::GetFilenameFromPath(path));
                }
            
                std::string vertexShaderPath = JsonHelper::CheckJsonString(materialData, "vertexShaderPath", name);
                if (vertexShaderPath != "")
                {
                    newMaterial->SetVertexPath(vertexShaderPath);
                }
                std::string fragmentShaderPath = JsonHelper::CheckJsonString(materialData, "fragmentShaderPath", name);
                if (fragmentShaderPath != "")
                {
                    newMaterial->SetFragmentPath(fragmentShaderPath);
                }

                if (JsonHelper::JsonContains(materialData, "uboVec4Names", name))
                {
                    json uboVec4Data = materialData["uboVec4Names"];

                    if (uboVec4Data.size())
                    {
                        for (auto item = uboVec4Data.begin(); item != uboVec4Data.end(); ++item)
                        {
                            try
                            {
                                newMaterial->AddUBOVec4(item.value(), (uint32_t)std::stoi(item.key()));
                            }
                            catch (const json::out_of_range& e)
                            {
                                Logger::log.Err("{}", e.what());
                            }
                        }
                    }
                }

                if (JsonHelper::JsonContains(materialData, "texturePipelineData", name))
                {
                    json texturesPipelineData = materialData["texturePipelineData"];

                    if (texturesPipelineData.size())
                    {
                        for (auto item = texturesPipelineData.begin(); item != texturesPipelineData.end(); ++item)
                        {
                            try
                            {
                                json pipelineData = texturesPipelineData[item.key()];
                                TexturePipelineData textureData = TexturePipelineData();
                                int shaderStage = JsonHelper::CheckJsonInt(pipelineData, "shaderStage", name);
                                if (shaderStage != -1)
                                {
                                    textureData.shaderStage = (VkShaderStageFlags)shaderStage;
                                }
                                else
                                {
                                    break;
                                }
                                int descriptorType = JsonHelper::CheckJsonInt(pipelineData, "descriptorType", name);
                                if (descriptorType != -1)
                                {
                                    textureData.descriptorType = (VkDescriptorType)descriptorType;
                                }
                                else
                                {
                                    break;
                                }

                                newMaterial->AddTexture((uint32_t)std::stoi(item.key()), textureData);
                            }
                            catch (const json::out_of_range& e)
                            {
                                Logger::log.Err("{}", e.what());
                            }
                        }
                    }
                }

                // Graphics Pipeline configuration
                if (JsonHelper::JsonContains(materialData, "inputAssemblyData", name))
                {
                    json inputAssemblyData = materialData["inputAssemblyData"];

                    VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfos = newMaterial->GetInputAssemblyCreateInfos(); // Maybe just use initialized VkPipelineInputAssemblyStateCreateInfo instead
                    inputAssemblyInfos.topology = (VkPrimitiveTopology)JsonHelper::CheckJsonInt(inputAssemblyData, "topology", name);
                    inputAssemblyInfos.primitiveRestartEnable = (bool)JsonHelper::CheckJsonBool(inputAssemblyData, "_primitiveRestartEnable", name);
                    newMaterial->SetInputAssemblyCreateInfos(inputAssemblyInfos);
                }
                if (JsonHelper::JsonContains(materialData, "rasterizerData", name))
                {                
                    json rasterizerData = materialData["rasterizerData"];

                    VkPipelineRasterizationStateCreateInfo rasterizerInfos = newMaterial->GetRasterizerCreateInfos();
                    rasterizerInfos.polygonMode = (VkPolygonMode)JsonHelper::CheckJsonInt(rasterizerData, "polygonMode", name);
                    rasterizerInfos.cullMode = (VkCullModeFlagBits)JsonHelper::CheckJsonInt(rasterizerData, "cullMode", name);
                    rasterizerInfos.lineWidth = JsonHelper::CheckJsonFloat(rasterizerData, "lineWidth", name);
                    newMaterial->SetRasterizerCreateInfos(rasterizerInfos);
                }
                if (JsonHelper::JsonContains(materialData, "colorBlendAttachmentData", name))
                {
                    json colorBlendAttachmentData = materialData["colorBlendAttachmentData"];

                    VkPipelineColorBlendAttachmentState colorBlendAttachmentInfos = newMaterial->GetColorBlendAttachmentCreateInfos();
                    colorBlendAttachmentInfos.alphaBlendOp = (VkBlendOp)JsonHelper::CheckJsonInt(colorBlendAttachmentData, "alphaBlendOp", name);
                    newMaterial->SetColorBlendAttachmentCreateInfos(colorBlendAttachmentInfos);                
                }

                newMaterial->SetHandles(renderPass);

                if (b_init)
                {
                    newMaterial->Init();
                }            
            }

            return newMaterial;
        }

        std::shared_ptr<Material> Vulkan::CreateNewMaterialFile(std::string fileName, std::string path)
        {
            std::string filePath = "";
            std::shared_ptr<Material> newMaterial = std::make_shared<Material>();

            if (path == "")
            {
                filePath = Assets::assetManager.GetDir("projectDir") + "/materials/" + fileName + ".mat";
            }
            else
            {
                filePath = path + "/" + fileName + ".mat";
            }

            // TODO: SHOULD ADD THE MATERIAL TO BOTH SCENE VIEW AND GAME VIEW MATERIALS
            newMaterial->SetPath(filePath);
            newMaterial->SetName(fileName);
            newMaterial->SetHandles(&m_renderToTextureSceneViewRenderPass);
            SaveMaterial(newMaterial);        

            return newMaterial;
        }

        // Only call add material after all material members have been filled
        void Vulkan::AddSceneViewMaterial(std::shared_ptr<Material> material)
        {        
            if (m_sceneViewMaterials.count(material->GetName()))
            {
                m_sceneViewMaterials.at(material->GetName()) = material;
            }
            else
            {
                m_sceneViewMaterials.emplace(material->GetName(), material);
            }
        }

        void Vulkan::AddGameViewMaterial(std::shared_ptr<Material> material)
        {                
            if (m_gameViewMaterials.count(material->GetName()))
            {
                m_gameViewMaterials.at(material->GetName()) = material;
            }
            else
            {
                m_gameViewMaterials.emplace(material->GetName(), material);
            }
        }

        void Vulkan::AddTextureToMaterial(std::string materialName, uint32_t index, TexturePipelineData textureData)
        {
            if (m_sceneViewMaterials.count(materialName))
            {
                m_sceneViewMaterials.at(materialName)->AddTexture(index, textureData);
            }
            if (m_gameViewMaterials.count(materialName))
            {
                m_gameViewMaterials.at(materialName)->AddTexture(index, textureData);
            }
        }

        void Vulkan::RemoveTextureFromMaterial(std::string materialName, uint32_t index)
        {
            if (m_sceneViewMaterials.count(materialName))
            {
                m_sceneViewMaterials.at(materialName)->RemoveTexture(index);
            }
            if (m_gameViewMaterials.count(materialName))
            {
                m_gameViewMaterials.at(materialName)->RemoveTexture(index);
            }
        }

        void Vulkan::AddUBOVec4ToMaterial(std::string materialName, std::string uboVec4Name, int index)
        {
            if (m_sceneViewMaterials.count(materialName))
            {
                m_sceneViewMaterials.at(materialName)->AddUBOVec4(uboVec4Name, index);
            }
            if (m_gameViewMaterials.count(materialName))
            {
                m_gameViewMaterials.at(materialName)->AddUBOVec4(uboVec4Name, index);
            }
        }

        void Vulkan::RemoveUBOVec4FromMaterial(std::string materialName, int index)
        {
            if (m_sceneViewMaterials.count(materialName))
            {
                m_sceneViewMaterials.at(materialName)->RemoveUBOVec4(index);
            }
            if (m_gameViewMaterials.count(materialName))
            {
                m_gameViewMaterials.at(materialName)->RemoveUBOVec4(index);
            }
        }

        void Vulkan::SetMaterialVertexPath(std::string materialName, std::string vertexPath)
        {
            if (m_sceneViewMaterials.count(materialName))
            {
                m_sceneViewMaterials.at(materialName)->SetVertexPath(vertexPath);
            }
            if (m_gameViewMaterials.count(materialName))
            {
                m_gameViewMaterials.at(materialName)->SetVertexPath(vertexPath);
            }
        }

        void Vulkan::SetMaterialFragmentPath(std::string materialName, std::string fragmentPath)
        {
            if (m_sceneViewMaterials.count(materialName))
            {
                m_sceneViewMaterials.at(materialName)->SetFragmentPath(fragmentPath);
            }
            if (m_gameViewMaterials.count(materialName))
            {
                m_gameViewMaterials.at(materialName)->SetFragmentPath(fragmentPath);
            }
        }

        std::shared_ptr<Material> Vulkan::GetMaterial(std::string materialName, ViewportType viewportType)
        {                
            switch (viewportType)
            {
            case ViewportType::ViewportType_SceneView:
                if (m_sceneViewMaterials.count(materialName))
                {
                    return m_sceneViewMaterials.at(materialName);
                }
                break;
            case ViewportType::ViewportType_GameView:
                if (m_gameViewMaterials.count(materialName))
                {
                    return m_gameViewMaterials.at(materialName);
                }
                break;
            default:
                break;
            }

            if (materialName == "imgui")
            {
                return m_imGuiMaterial;
            }

            return nullptr;
        }

        // Assume Scene View Materials for now
        std::map<std::string, std::shared_ptr<Material>>& Vulkan::GetMaterials()
        {
            return m_sceneViewMaterials;
        }

        void Vulkan::ReloadShaders()
        {
            for (std::map<std::string, std::shared_ptr<Material>>::iterator material = m_sceneViewMaterials.begin(); material != m_sceneViewMaterials.end(); material++)
            {
                if (material->second->Initialized())
                {
                    material->second->RecreateGraphicsPipeline();
                }
            }
            for (std::map<std::string, std::shared_ptr<Material>>::iterator material = m_gameViewMaterials.begin(); material != m_gameViewMaterials.end(); material++)
            {
                if (material->second->Initialized())
                {
                    material->second->RecreateGraphicsPipeline();
                }
            }
        }

        void Vulkan::AddSceneViewMaterialMesh(std::string materialName, long ID, Mesh* mesh)
        {
            if (mesh->GetModel() != nullptr && materialName != "")
            {
                if (m_sceneViewMaterialMeshes.count(materialName)) // Material key exists
                {
                    if (m_sceneViewMaterialMeshes.at(materialName).count(mesh->GetModel()->GetModelPath())) // Model key exists
                    {
                        if (m_sceneViewMaterialMeshes.at(materialName).at(mesh->GetModel()->GetModelPath()).count(ID)) // Mesh exists
                        {
                            m_sceneViewMaterialMeshes.at(materialName).at(mesh->GetModel()->GetModelPath()).at(ID) = mesh;
                        }
                        else
                        {
                            m_sceneViewMaterialMeshes.at(materialName).at(mesh->GetModel()->GetModelPath()).emplace(ID, mesh);
                        }
                    }
                    else
                    {
                        std::map<long, Mesh*> newMap = std::map<long, Mesh*>();
                        newMap.emplace(ID, mesh);
                        m_sceneViewMaterialMeshes.at(materialName).emplace(mesh->GetModel()->GetModelPath(), newMap);
                    }
                }
                else
                {
                    std::map<std::string, std::map<long, Mesh*>> modelMap = std::map<std::string, std::map<long, Mesh*>>();
                    std::map<long, Mesh*> meshMap = std::map<long, Mesh*>();
                    meshMap.emplace(ID, mesh);
                    modelMap.emplace(mesh->GetModel()->GetModelPath(), meshMap);
                    m_sceneViewMaterialMeshes.emplace(materialName, modelMap);
                }
            }
        }

        void Vulkan::AddGameViewMaterialMesh(std::string materialName, long ID, Mesh* mesh)
        {
            if (mesh->GetModel() != nullptr && materialName != "")
            {
                if (m_gameViewMaterialMeshes.count(materialName)) // Material key exists
                {
                    if (m_gameViewMaterialMeshes.at(materialName).count(mesh->GetModel()->GetModelPath())) // Model key exists
                    {
                        if (m_gameViewMaterialMeshes.at(materialName).at(mesh->GetModel()->GetModelPath()).count(ID)) // Mesh exists
                        {
                            m_gameViewMaterialMeshes.at(materialName).at(mesh->GetModel()->GetModelPath()).at(ID) = mesh;
                        }
                        else
                        {
                            m_gameViewMaterialMeshes.at(materialName).at(mesh->GetModel()->GetModelPath()).emplace(ID, mesh);
                        }
                    }
                    else
                    {
                        std::map<long, Mesh*> newMap = std::map<long, Mesh*>();
                        newMap.emplace(ID, mesh);
                        m_gameViewMaterialMeshes.at(materialName).emplace(mesh->GetModel()->GetModelPath(), newMap);
                    }
                }
                else
                {
                    std::map<std::string, std::map<long, Mesh*>> modelMap = std::map<std::string, std::map<long, Mesh*>>();
                    std::map<long, Mesh*> meshMap = std::map<long, Mesh*>();
                    meshMap.emplace(ID, mesh);
                    modelMap.emplace(mesh->GetModel()->GetModelPath(), meshMap);
                    m_gameViewMaterialMeshes.emplace(materialName, modelMap);
                }
            }
        }

        void Vulkan::RemoveSceneViewMaterialMesh(std::string materialName, long ID, Mesh* mesh)
        {
            if (m_sceneViewMaterialMeshes.count(materialName) && m_sceneViewMaterialMeshes.at(materialName).count(mesh->GetModel()->GetModelPath()) && m_sceneViewMaterialMeshes.at(materialName).at(mesh->GetModel()->GetModelPath()).count(ID))
            {
                m_sceneViewMaterialMeshes.at(materialName).at(mesh->GetModel()->GetModelPath()).erase(ID);
            }
        }

        void Vulkan::RemoveGameViewMaterialMesh(std::string materialName, long ID, Mesh* mesh)
        {
            if (m_gameViewMaterialMeshes.count(materialName) && m_gameViewMaterialMeshes.at(materialName).count(mesh->GetModel()->GetModelPath()) && m_gameViewMaterialMeshes.at(materialName).at(mesh->GetModel()->GetModelPath()).count(ID))
            {
                m_gameViewMaterialMeshes.at(materialName).at(mesh->GetModel()->GetModelPath()).erase(ID);
            }
        }

        void Vulkan::ClearGroupedByMaterialMeshes()
        {
            m_sceneViewMaterialMeshes.clear();
            m_gameViewMaterialMeshes.clear();
        }

        std::shared_ptr<Model> Vulkan::GetModel(std::string modelPath)
        {
            if (m_models.count(modelPath))
            {
                return m_models.at(modelPath);
            }
            else
            {
                std::shared_ptr<Model> loadedModel = LoadModel(modelPath);
                m_models.emplace(modelPath, loadedModel);
                return loadedModel;
            }
        }

        std::shared_ptr<Model> Vulkan::LoadModel(std::string modelPath)
        {
            std::shared_ptr<Model> newModel = std::make_shared<Model>();
            newModel->Init(&m_systemCommandPool);
            newModel->SetModelPath(modelPath);
            newModel->LoadModel(modelPath);        
            m_models.emplace(modelPath, newModel);

            return newModel;
        }

        void Vulkan::QueueBufferDeletion(VkBuffer bufferToDelete)
        {
            m_bufferDeleteQueue.push_back(bufferToDelete);
        }

        void Vulkan::QueueDeviceMemoryDeletion(VkDeviceMemory deviceMemoryToDelete)
        {
            m_deviceMemoryDeleteQueue.push_back(deviceMemoryToDelete);
        }

        void Vulkan::QueueDescriptorPoolDeletion(VkDescriptorPool descriptorPoolToDelete)
        {
            m_descriptorPoolDeleteQueue.push_back(descriptorPoolToDelete);;
        }

        void Vulkan::DeleteQueuedVKObjects()
        {
            if (m_bufferDeleteQueue.size())
            {
                for (VkBuffer buffer : m_bufferDeleteQueue)
                {
                    vkDestroyBuffer(DeviceManager::logicalDevice.GetDevice(), buffer, nullptr);
                }
                m_bufferDeleteQueue.clear();
            }

            if (m_deviceMemoryDeleteQueue.size())
            {
                for (VkDeviceMemory deviceMemory : m_deviceMemoryDeleteQueue)
                {
                    vkFreeMemory(DeviceManager::logicalDevice.GetDevice(), deviceMemory, nullptr);
                }
                m_deviceMemoryDeleteQueue.clear();
            }

            if (m_descriptorPoolDeleteQueue.size())
            {
                for (VkDescriptorPool descriptorPool : m_descriptorPoolDeleteQueue)
                {
                    vkDestroyDescriptorPool(DeviceManager::logicalDevice.GetDevice(), descriptorPool, nullptr);
                }
                m_descriptorPoolDeleteQueue.clear();
            }
        }

        void Vulkan::CreateSyncObjects()
        {
            // More info here - https://vulkan-tutorial.com/en/Drawing_a_triangle/Drawing/Rendering_and_presentation

            VkSemaphoreCreateInfo semaphoreInfo{};
            semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
            VkFenceCreateInfo fenceInfo{};
            fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
            fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT; // Start it signaled so the very first frame doesn't block indefinitely

            m_imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
            m_renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
            m_inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

            for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
            {
                if (vkCreateSemaphore(DeviceManager::logicalDevice.GetDevice(), &semaphoreInfo, nullptr, &m_imageAvailableSemaphores[i]) != VK_SUCCESS ||
                    vkCreateSemaphore(DeviceManager::logicalDevice.GetDevice(), &semaphoreInfo, nullptr, &m_renderFinishedSemaphores[i]) != VK_SUCCESS ||
                    vkCreateFence(DeviceManager::logicalDevice.GetDevice(), &fenceInfo, nullptr, &m_inFlightFences[i]) != VK_SUCCESS)
                {
                    throw std::runtime_error("failed to create semaphores!");
                }
            }
        }

        std::vector<VkDescriptorSet>& Vulkan::GetSceneViewDescriptorSets()
        {
            return m_sceneViewTexture.GetDescriptorSets();
        }

        std::vector<VkDescriptorSet>& Vulkan::GetGameViewDescriptorSets()
        {
            return m_gameViewTexture.GetDescriptorSets();
        }

        void Vulkan::DrawFrame()
        {
            // More info here - https://vulkan-tutorial.com/en/Drawing_a_triangle/Drawing/Rendering_and_presentation

            if (RenderWindow::window.m_b_framebufferResized)
            {
                RecreateSwapChainAndFrameBuffers();
                RenderWindow::window.m_b_framebufferResized = false;
            }

            // At the start of the frame, we want to wait until the previous frame has finished, so that the command buffer and semaphores are available to use. To do that, we call vkWaitForFences:
            vkWaitForFences(DeviceManager::logicalDevice.GetDevice(), 1, &m_inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

            DeleteQueuedVKObjects();

            uint32_t imageIndex;        
            VkResult aquireImageResult = vkAcquireNextImageKHR(DeviceManager::logicalDevice.GetDevice(), RenderWindow::window.GetSwapChain(), UINT64_MAX, m_imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);
            if (!CheckSwapChainIntegrity(aquireImageResult, "Failed to acquire swap chain image."))
            {
                return;
            }

            // manually reset the fence to the unsignaled state with the vkResetFences call:
            vkResetFences(DeviceManager::logicalDevice.GetDevice(), 1, &m_inFlightFences[currentFrame]);
            


            std::list<VkCommandBuffer> commandBuffers = std::list<VkCommandBuffer>();       
            std::vector<std::thread> threads = std::vector<std::thread>();

            //threads.emplace_back([this, &commandBuffers, imageIndex]
            //{
                // Scene View
                if (m_renderToTextureSceneViewRenderPass.Initialized() && Settings::settings.b_showSceneView)
                {                                  
                    m_renderToTextureSceneViewRenderPass.BeginRenderPass(imageIndex);

                    if (SceneView::ShouldShowSceneViewGridObjects())
                    {
                        for (int i = 0; i < 7; i++)                        
                        {
                            Mesh& mesh = SceneView::sceneViewMeshes[i];
                            Transform& transform = SceneView::sceneViewTransforms[i];
                            std::shared_ptr<Material> material = mesh.GetSceneViewMaterial();
                            if (mesh.Initialized() && material != nullptr)
                            {                                                  
                                if (mesh.IsActive())
                                {
                                    m_renderToTextureSceneViewRenderPass.RecordCommandBuffer(material->GetGraphicsPipeline());
                                    mesh.UpdateUniformBuffer(ViewportType::ViewportType_SceneView, SceneView::IsOrthoGraphic(), &transform);
                                    m_renderToTextureSceneViewRenderPass.BindIndexed(mesh.GetModel()); // NOTE: Binding the indices can be broken out if we group Meshes by Model
                                    m_renderToTextureSceneViewRenderPass.BindDescriptorSets(mesh.GetSceneViewDescriptorSets()[currentFrame], material, ViewportType::ViewportType_SceneView);
                                    m_renderToTextureSceneViewRenderPass.DrawIndexed(mesh.GetModel()); // Create final VkImage on m_sceneViewTexture's m_images member variable                                                       
                                }
                            }
                        }
                    }          

                    std::vector<Mesh*> meshesMissingTextures = std::vector<Mesh*>();

                    std::vector<VkDrawIndirectCommand> drawCommands = std::vector<VkDrawIndirectCommand>();
                    uint32_t meshCount = 0;
                    for (std::map<std::string, std::map<std::string, std::map<long, Mesh*>>>::iterator materials = m_sceneViewMaterialMeshes.begin(); materials != m_sceneViewMaterialMeshes.end(); materials++)
                    {
                        if (m_sceneViewMaterials.count(materials->first))
                        {
                            std::shared_ptr<Material> material = m_sceneViewMaterials.at(materials->first);
                            m_renderToTextureSceneViewRenderPass.RecordCommandBuffer(material->GetGraphicsPipeline());      

                            for (std::map<std::string, std::map<long, Mesh*>>::iterator models = materials->second.begin(); models != materials->second.end(); models++)
                            {
                                m_renderToTextureSceneViewRenderPass.BindIndexed(GetModel(models->first));

                                for (std::map<long, Mesh*>::iterator mesh = models->second.begin(); mesh != models->second.end(); mesh++)
                                {
                                    //encode the draw data of each object into the indirect draw buffer

                                    //drawCommands[meshCount].vertexCount = GetModel(models->first)->GetVertices().size();
                                    //drawCommands[meshCount].instanceCount = 1;
                                    //drawCommands[meshCount].firstVertex = 0;
                                    //drawCommands[meshCount].firstInstance = meshCount; //used to access object matrix in the shader
                                
                                    //VkDeviceSize indirectOffset = draw.first * sizeof(VkDrawIndirectCommand);
                                    //uint32_t draw_stride = sizeof(VkDrawIndirectCommand);

                                    ////execute the draw command buffer on each section as defined by the array of draws
                                    //vkCmdDrawIndirect(m_renderToTextureSceneViewRenderPass.GetCommandBuffers()[currentFrame], get_current_frame().indirectBuffer, indirectOffset, draw.count, draw_stride);
                                    //

                                    if (mesh->second->Initialized() && material != nullptr && !mesh->second->MissingTextures())
                                    {
                                        mesh->second->UpdateUniformBuffer(ViewportType::ViewportType_SceneView, SceneView::IsOrthoGraphic());
                                        m_renderToTextureSceneViewRenderPass.BindDescriptorSets(mesh->second->GetSceneViewDescriptorSets()[currentFrame], material, ViewportType::ViewportType_SceneView);
                                        m_renderToTextureSceneViewRenderPass.DrawIndexed(mesh->second->GetModel()); // Create final VkImage on m_sceneViewTexture's m_images member variable                                       
                                    }
                                    else if (mesh->second->MissingTextures())
                                    {
                                        meshesMissingTextures.push_back(mesh->second);
                                    }

                                    meshCount++;
                                }
                            }                   
                        }
                    }

                    // Render the Mesh but using the fl_empty material
                    if (meshesMissingTextures.size())
                    {
                        m_renderToTextureSceneViewRenderPass.RecordCommandBuffer(GetMaterial("fl_empty")->GetGraphicsPipeline());

                        for (Mesh* mesh : meshesMissingTextures)
                        {
                            mesh->UpdateUniformBuffer(ViewportType::ViewportType_SceneView, SceneView::IsOrthoGraphic());
                            m_renderToTextureSceneViewRenderPass.BindIndexed(mesh->GetModel());
                            m_renderToTextureSceneViewRenderPass.BindDescriptorSets(mesh->GetEmptySceneViewDescriptorSets()[currentFrame], GetMaterial("fl_empty"), ViewportType::ViewportType_SceneView);
                            m_renderToTextureSceneViewRenderPass.DrawIndexed(mesh->GetModel()); // Create final VkImage on m_sceneViewTexture's m_images member variable   
                        }
                    }

                    m_renderToTextureSceneViewRenderPass.EndRenderPass();

                    commandBuffers.push_back(m_renderToTextureSceneViewRenderPass.GetCommandBuffers()[currentFrame]);  
                }
            //});


            //threads.emplace_back([this, &commandBuffers, imageIndex]
            //{
                // Game View
                if (m_renderToTextureGameViewRenderPass.Initialized() && Settings::settings.b_showGameView)
                {            
                m_renderToTextureGameViewRenderPass.BeginRenderPass(imageIndex);

                std::vector<Mesh*> meshesMissingTextures = std::vector<Mesh*>();

                for (std::map<std::string, std::map<std::string, std::map<long, Mesh*>>>::iterator materials = m_gameViewMaterialMeshes.begin(); materials != m_gameViewMaterialMeshes.end(); materials++)
                {
                    if (m_gameViewMaterials.count(materials->first))
                    {
                        std::shared_ptr<Material> material = m_gameViewMaterials.at(materials->first);

                        m_renderToTextureGameViewRenderPass.RecordCommandBuffer(material->GetGraphicsPipeline());                        

                        for (std::map<std::string, std::map<long, Mesh*>>::iterator models = materials->second.begin(); models != materials->second.end(); models++)
                            {
                                m_renderToTextureSceneViewRenderPass.BindIndexed(GetModel(models->first));

                                for (std::map<long, Mesh*>::iterator meshes = models->second.begin(); meshes != models->second.end(); meshes++)
                                {
                                    Mesh* mesh = meshes->second;

                                    if (mesh->Initialized() && material != nullptr && !mesh->MissingTextures())
                                    {
                                        mesh->UpdateUniformBuffer(ViewportType::ViewportType_GameView, SceneView::IsOrthoGraphic());
                                        m_renderToTextureGameViewRenderPass.BindIndexed(mesh->GetModel());
                                        m_renderToTextureGameViewRenderPass.BindDescriptorSets(mesh->GetGameViewDescriptorSets()[currentFrame], material, ViewportType::ViewportType_GameView);
                                        m_renderToTextureGameViewRenderPass.DrawIndexed(mesh->GetModel()); // Create final VkImage on m_sceneViewTexture's m_images member variable                                       
                                    }
                                    else if (mesh->MissingTextures())
                                    {
                                        meshesMissingTextures.push_back(mesh);
                                    }
                                }
                        }                        
                    }                    
                }

                // Render the Mesh but using the fl_empty material (empty meshes
                if (meshesMissingTextures.size())
                {
                    m_renderToTextureGameViewRenderPass.RecordCommandBuffer(GetMaterial("fl_empty")->GetGraphicsPipeline());

                    for (Mesh* mesh : meshesMissingTextures)
                    {
                        mesh->UpdateUniformBuffer(ViewportType::ViewportType_GameView, SceneView::IsOrthoGraphic());
                        m_renderToTextureGameViewRenderPass.BindIndexed(mesh->GetModel());
                        m_renderToTextureGameViewRenderPass.BindDescriptorSets(mesh->GetEmptyGameViewDescriptorSets()[currentFrame], GetMaterial("fl_empty"), ViewportType::ViewportType_GameView);
                        m_renderToTextureGameViewRenderPass.DrawIndexed(mesh->GetModel()); // Create final VkImage on m_gameViewTexture's m_images member variable   
                    }
                }

                m_renderToTextureGameViewRenderPass.EndRenderPass();

                commandBuffers.push_back(m_renderToTextureGameViewRenderPass.GetCommandBuffers()[currentFrame]);
                }
            //});
            
            //for (auto& thread : threads)
            //{
            //    if (thread.joinable())
            //    {
            //        thread.join();
            //    }
            //}

            //if (m_postProcessingRenderPass.Initialized())
            //{
            //    m_postProcessingRenderPass.BeginRenderPass(imageIndex);
            //    m_postProcessingRenderPass.RecordCommandBuffer(GetMaterial("fl_postProcessing", ViewportType::SceneView)->GetGraphicsPipeline());
            //    m_postProcessingRenderPass.BindDescriptorSets(m_postProcessingDescriptorSets[currentFrame], GetMaterial("fl_postProcessing", ViewportType::SceneView), ViewportType::SceneView);                
            //    m_postProcessingRenderPass.EndRenderPass();
            //}

            // Once VkImages have been written to in each viewport RenderPass, those VkImages can be used as textures and sampled by a different material and desired descriptorSets, (ie. ImGui!), so we need to create descriptor sets for them using the ImGui Material's configuration           
            // This process can't be done inside the threads because the same Allocator (VkDescriptorPool) is being used to allocate the VkDescriptorSets of each RenderToTexture, and accessing the same VkDescriptorPool from separate threads is prohibited, so we just wait for them to be joined back to the main thread.
            std::vector<VkBuffer> emptyUBO = std::vector<VkBuffer>();
            std::map<uint32_t, Texture> sceneViewTextures = std::map<uint32_t, Texture>(); 
            sceneViewTextures.emplace(0, m_sceneViewTexture); // m_sceneViewTexture was given to m_renderToTextureSceneViewRenderPass in each material and it was written to in m_renderToTextureSceneViewRenderPass.DrawIndexed().. Likewise with Game View but with Game View specific member variables
            m_imGuiMaterial->GetAllocator().AllocateDescriptorSets(m_sceneViewTexture.GetDescriptorSets(), emptyUBO, *m_imGuiMaterial->GetTexturesPipelineData(), sceneViewTextures);

            std::map<uint32_t, Texture> gameViewTextures = std::map<uint32_t, Texture>();
            gameViewTextures.emplace(0, m_gameViewTexture);
            m_imGuiMaterial->GetAllocator().AllocateDescriptorSets(m_gameViewTexture.GetDescriptorSets(), emptyUBO, *m_imGuiMaterial->GetTexturesPipelineData(), gameViewTextures);

            if (m_imGuiMaterial != nullptr)
            {
                m_imGuiRenderPass.BeginRenderPass(imageIndex);
                ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), m_imGuiRenderPass.GetCommandBuffers()[currentFrame]);
                m_imGuiRenderPass.EndRenderPass();
            }
            commandBuffers.push_back(m_imGuiRenderPass.GetCommandBuffers()[currentFrame]);
    
            std::vector<VkCommandBuffer> buffers = std::vector<VkCommandBuffer>();
            
            for (VkCommandBuffer& buffer : commandBuffers)
            {
                buffers.push_back(buffer);
            }

            VkSubmitInfo submitInfo{};
            submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

            VkSemaphore waitSemaphores[] = { m_imageAvailableSemaphores[currentFrame] };
            VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
            submitInfo.waitSemaphoreCount = 1;
            submitInfo.pWaitSemaphores = waitSemaphores;
            submitInfo.pWaitDstStageMask = waitStages;
            submitInfo.commandBufferCount = (uint32_t)buffers.size();
            submitInfo.pCommandBuffers = buffers.data();

            VkSemaphore signalSemaphores[] = { m_renderFinishedSemaphores[currentFrame] }; // { m_renderFinishedSemaphores[imageIndex] }; Seemed to fix the semaphore issue, but didn't fix the frame flickering
            submitInfo.signalSemaphoreCount = 1;
            submitInfo.pSignalSemaphores = signalSemaphores;

            VkResult submitResult = vkQueueSubmit(DeviceManager::logicalDevice.GetGraphicsQueue(), 1, &submitInfo, m_inFlightFences[currentFrame]);
            CheckSwapChainIntegrity(submitResult, "Failed to submit draw command buffer.");

            VkPresentInfoKHR presentInfo{};
            presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
            presentInfo.waitSemaphoreCount = 1;
            presentInfo.pWaitSemaphores = signalSemaphores;

            VkSwapchainKHR swapChains[] = { RenderWindow::window.GetSwapChain() };
            presentInfo.swapchainCount = 1;
            presentInfo.pSwapchains = swapChains;
            presentInfo.pImageIndices = &imageIndex;
            presentInfo.pResults = nullptr;

            // Present!
            VkResult presentResult = vkQueuePresentKHR(DeviceManager::logicalDevice.GetPresentQueue(), &presentInfo);

            currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
        }

        VkSampleCountFlagBits Vulkan::GetMaxSamples()
        {
            return m_maxSamples;
        }

        void Vulkan::SetMaxSamples(VkSampleCountFlagBits maxSamples)
        {
            m_maxSamples = maxSamples;
        }

        void Vulkan::RecreateSwapChainAndFrameBuffers()
        {       
            // int windowW, windowH;
            // SDL_GetWindowSize(RenderWindow::window.GetWindow(), &windowW, &windowH);

            // int drawableW, drawableH;
            // SDL_Vulkan_GetDrawableSize(RenderWindow::window.GetWindow(), &drawableW, &drawableH);

            // VkSurfaceCapabilitiesKHR capabilities;
            // vkGetPhysicalDeviceSurfaceCapabilitiesKHR(DeviceManager::physicalDevice.GetDevice(), RenderWindow::window.GetSurface(), &capabilities);

            // Logger::log.Debug("SDL window size: {}x{}", windowW, windowH);
            // Logger::log.Debug("SDL drawable size: {}x{}", drawableW, drawableH);
            // Logger::log.Debug("Surface currentExtent: {}x{}", capabilities.currentExtent.width, capabilities.currentExtent.height);
            // Logger::log.Debug("Chosen swapchain extent: {}x{}", RenderWindow::window.GetExtent().width, RenderWindow::window.GetExtent().height);
            RenderWindow::window.RecreateSwapChain();   

            m_sceneViewTexture.CreateRenderToTextureResources(m_sceneViewCommandPool);
            m_gameViewTexture.CreateRenderToTextureResources(m_gameViewCommandPool);

            m_renderToTextureSceneViewRenderPass.ConfigureFrameBufferImageViews(m_sceneViewTexture.GetImageViews());        
            m_renderToTextureSceneViewRenderPass.RecreateFrameBuffers();

            m_renderToTextureGameViewRenderPass.ConfigureFrameBufferImageViews(m_gameViewTexture.GetImageViews());
            m_renderToTextureGameViewRenderPass.RecreateFrameBuffers();

            m_imGuiRenderPass.ConfigureFrameBufferImageViews(RenderWindow::window.GetSwapChainImageViews());
            m_imGuiRenderPass.RecreateFrameBuffers();

            ImGui_ImplVulkan_SetMinImageCount(static_cast<uint32_t>(RenderWindow::window.GetSwapChainImageViews().size()));
        }

        bool Vulkan::CheckSwapChainIntegrity(VkResult result, std::string errorMessage)
        {
            // More details here - https://vulkan-tutorial.com/en/Drawing_a_triangle/Swap_chain_recreation
            // Check on swap chain integrity after image access and after present

            bool b_swapChainGood = true;

            if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
            {
                b_swapChainGood = false;
                RecreateSwapChainAndFrameBuffers();
            }
            else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
            {
                RecreateSwapChainAndFrameBuffers();
                throw std::runtime_error(errorMessage);
            }

            return b_swapChainGood;
        }

        VkInstance& Vulkan::GetInstance()
        {
            return m_instance;
        }

        VkQueue& Vulkan::GetGraphicsQueue()
        {
            return DeviceManager::logicalDevice.GetGraphicsQueue();
        }

        VkCommandPool& Vulkan::GetCommandPool()
        {
            return m_systemCommandPool;
        }

        Vector2 AddImageToDrawList(VkDescriptorSet texture, Vector2 positionInGrid, Vector2 relativeCenterPoint, float textureWidthPx, float textureHeightPx, Vector2 offsetPx, Vector2 scale, bool b_scalesWithZoom, float zoomMultiplier, ImDrawList* drawList, float rotation, ImU32 addColor, Vector2 uvStart, Vector2 uvEnd)
        {
            // Changing the scale here because sprites render too large
            Vector2 newScale = Vector2(scale.x * spriteScaleMultiplier, scale.y * spriteScaleMultiplier);

            float scalingXStart = relativeCenterPoint.x + (positionInGrid.x * zoomMultiplier) - (offsetPx.x * newScale.x * zoomMultiplier);
            float scalingYStart = relativeCenterPoint.y - (positionInGrid.y * zoomMultiplier) - (offsetPx.y * newScale.y * zoomMultiplier);
            float scalingXEnd = scalingXStart + (textureWidthPx * newScale.x * zoomMultiplier);
            float scalingYEnd = scalingYStart + (textureHeightPx * newScale.y * zoomMultiplier);

            float unscaledXStart = relativeCenterPoint.x + (positionInGrid.x * zoomMultiplier) - offsetPx.x * scale.x;
            float unscaledYStart = relativeCenterPoint.y + (-positionInGrid.y * zoomMultiplier) - offsetPx.y * scale.y;

            Vector2 renderStart;
            Vector2 renderEnd;

            if (b_scalesWithZoom)
            {
                renderStart = Vector2(scalingXStart, scalingYStart);
                renderEnd = Vector2(scalingXEnd, scalingYEnd);

                // FOR DEBUGGING - draw white box around where the texture should be
                //DrawRectangle(renderStart, renderEnd, Vector2(0,0), Vector2(ImGui::GetWindowWidth(), ImGui::GetWindowHeight()), F_whiteColor, 2, draw_list);
            }
            else
            {
                renderStart = Vector2(unscaledXStart, unscaledYStart);
                renderEnd = Vector2(renderStart.x + textureWidthPx * scale.x, renderStart.y + textureHeightPx * scale.y);
            }

            if (rotation != 0)
            {
                float x = (renderEnd.x - renderStart.x) / 2.0f;
                float y = (renderEnd.y - renderStart.y) / 2.0f;
                
                Vector2 topLeft =     Vector2::Rotate(Vector2(-x, -y), rotation);
                Vector2 topRight =    Vector2::Rotate(Vector2(+x, -y), rotation);
                Vector2 bottomRight = Vector2::Rotate(Vector2(+x, +y), rotation);
                Vector2 bottomLeft =  Vector2::Rotate(Vector2(-x, +y), rotation);

                Vector2 center = Vector2(renderStart.x + ((renderEnd.x - renderStart.x) / 2), renderStart.y + ((renderEnd.y - renderStart.y) / 2));
                Vector2 pos[4] =
                {
                    Vector2(center.x + topLeft.x, center.y + topLeft.y),
                    Vector2(center.x + topRight.x, center.y + topRight.y),
                    Vector2(center.x + bottomRight.x, center.y + bottomRight.y),
                    Vector2(center.x + bottomLeft.x, center.y + bottomLeft.y),
                };
                Vector2 uvs[4] =
                {
                    Vector2(0.0f, 0.0f),
                    Vector2(1.0f, 0.0f),
                    Vector2(1.0f, 1.0f),
                    Vector2(0.0f, 1.0f)
                };

                // Render sprite to viewport
                drawList->AddImageQuad(texture, pos[0], pos[1], pos[2], pos[3], uvs[0], uvs[1], uvs[2], uvs[3], addColor);
            }
            else
            {
                // Render sprite to viewport
                drawList->AddImage((void*)texture, renderStart, renderEnd, uvStart, uvEnd, addColor);
            }

            return renderStart;
        }
    }
}