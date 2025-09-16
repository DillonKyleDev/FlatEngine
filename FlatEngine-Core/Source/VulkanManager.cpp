#pragma once
#include "VulkanManager.h"
#include "Helper.h"
#include "Material.h"
#include "Project.h"
#include "FlatEngine.h"

#include "Scene.h" // delete later

#include "SDL_vulkan.h"
#include <glm.hpp>

// Refer to - https://vulkan-tutorial.com/en/Uniform_buffers/Descriptor_layout_and_buffer
#include <gtc/matrix_transform.hpp> // Not used currently but might need it later

#include <chrono> // Time keeping
#include <memory>
#include <array>
#include <fstream>

// TODO: Remove PipelineManager class and move RenderPasses into Materials alongside GraphicsPipeline to be controlled by each Material. 

namespace FlatEngine
{
    std::string F_selectedMaterialName = "";
    ValidationLayers VM_validationLayers = ValidationLayers();
    uint32_t VM_currentFrame = 0;
    uint32_t VM_imageCount = 0;

    void VulkanManager::check_vk_result(VkResult err)
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

    VulkanManager::VulkanManager()
    {
        m_instance = VK_NULL_HANDLE;
        m_winSystem = WinSys();
        m_physicalDevice = PhysicalDevice();
        m_logicalDevice = LogicalDevice();       

        m_renderToTextureRenderPass = RenderPass();
        m_imGuiRenderPass = RenderPass();

        // gpu communication
        m_commandPool = VK_NULL_HANDLE;
        m_imageAvailableSemaphores = std::vector<VkSemaphore>();
        m_renderFinishedSemaphores = std::vector<VkSemaphore>();
        m_inFlightFences = std::vector<VkFence>();
        m_b_framebufferResized = false; 

        m_imGuiMaterial = std::shared_ptr<Material>();
        m_sceneViewMaterials = std::vector<std::shared_ptr<Material>>();
        m_gameViewMaterials = std::vector<std::shared_ptr<Material>>();
        m_sceneViewTexture = Texture();
        m_gameViewTexture = Texture();
    }

    VulkanManager::~VulkanManager()
    {
    }

    void VulkanManager::Cleanup()
    {
        vkDeviceWaitIdle(m_logicalDevice.GetDevice()); // This may need to be moved elsewhere potentially

        // Semaphores and Fences
        for (size_t i = 0; i < VM_MAX_FRAMES_IN_FLIGHT; i++)
        {
            vkDestroySemaphore(m_logicalDevice.GetDevice(), m_imageAvailableSemaphores[i], nullptr);
            vkDestroySemaphore(m_logicalDevice.GetDevice(), m_renderFinishedSemaphores[i], nullptr);
            vkDestroyFence(m_logicalDevice.GetDevice(), m_inFlightFences[i], nullptr);
        }

        m_winSystem.CleanupDrawingResources();

        for (std::shared_ptr<Material> material : m_sceneViewMaterials)
        {
            material->Cleanup();
        }
        for (std::shared_ptr<Material> material : m_gameViewMaterials)
        {
            material->Cleanup();
        }

        vkDestroyCommandPool(m_logicalDevice.GetDevice(), m_commandPool, nullptr);

        m_logicalDevice.Cleanup();
        m_physicalDevice.Cleanup();
        VM_validationLayers.Cleanup(m_instance);
        m_winSystem.CleanupSystem();

        m_renderToTextureRenderPass.Cleanup(m_logicalDevice);
        m_imGuiRenderPass.Cleanup(m_logicalDevice);

        // Destroy Vulkan instance
        vkDestroyInstance(m_instance, nullptr);
    }

    bool VulkanManager::Init(int width, int height)
    {
        bool b_success = true;

        if (!InitVulkan(width, height))
        {
            printf("Vulkan initialization failed!\n");
            b_success = false;
        }
        else
        {
            printf("Vulkan initialized...\n");
        }

        return b_success;
    }

    void VulkanManager::CreateTextureImage(VkImage& image, std::string path, uint32_t mipLevels, VkDeviceMemory& imageMemory)
    {
        image = m_winSystem.CreateTextureImage(path, mipLevels, imageMemory);
    }

    void VulkanManager::CreateImageView(VkImageView& imageView, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels)
    {
        m_winSystem.CreateImageView(imageView, image, format, aspectFlags, mipLevels);
    }

    void VulkanManager::CreateTextureSampler(VkSampler& textureSampler, uint32_t mipLevels)
    {
        m_winSystem.CreateTextureSampler(textureSampler, mipLevels);
    }

    bool VulkanManager::InitVulkan(int windowWidth, int windowHeight)
    {
        bool b_success = true;

        if (!m_winSystem.CreateSDLWindow("FlatEngine", windowWidth, windowHeight))
        {
            printf("SDL window creation failed!\n");
            b_success = false;
        }
        else
        {
            if (!CreateVulkanInstance())
            {
                printf("Failed to create Vulkan instance!\n");
                b_success = false;
            }
            else
            {
                m_winSystem.SetHandles(&m_instance, &m_physicalDevice, &m_logicalDevice, &m_commandPool);
                m_winSystem.CreateSurface();
                VM_validationLayers.SetupDebugMessenger(m_instance);
                m_physicalDevice.Init(m_instance, m_winSystem.GetSurface());
                m_logicalDevice.Init(m_physicalDevice, m_winSystem.GetSurface());
                m_winSystem.CreateDrawingResources();
                QueueFamilyIndices indices = Helper::FindQueueFamilies(m_physicalDevice.GetDevice(), m_winSystem.GetSurface());
                m_logicalDevice.SetGraphicsIndex(indices.graphicsFamily.value());
                CreateCommandPool(m_commandPool, m_logicalDevice, indices.graphicsFamily.value());
                CreateSyncObjects();

                m_imGuiMaterial = LoadMaterial("../engine/materials/imgui.mat");

                m_sceneViewTexture.CreateRenderToTextureResources();
                m_gameViewTexture.CreateRenderToTextureResources();
            }
        }

        return b_success;
    }

    bool VulkanManager::CreateVulkanInstance()
    {
        bool b_success = true;

        // Validation layer setup for debugger
        if (b_ENABLE_VALIDATION_LAYERS && !VM_validationLayers.CheckSupport())
        {
            printf("Error: Validation layers requested, but not available.\n");
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
        SDL_Vulkan_GetInstanceExtensions(m_winSystem.GetWindow(), &sdlExtensionCount, nullptr);
        sdlExtensionNames = new const char* [sdlExtensionCount];
        SDL_Vulkan_GetInstanceExtensions(m_winSystem.GetWindow(), &sdlExtensionCount, sdlExtensionNames);

        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;

        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{}; // DebugUtilsMessenger for CreateInstance and DestroyInstance functions (automatically destroyed by Vulkan when closed)

        // Get extensions for use with debug messenger
        auto validationExtensions = VM_validationLayers.GetRequiredExtensions();
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
            createInfo.enabledLayerCount = static_cast<uint32_t>(VM_validationLayers.Size());
            createInfo.ppEnabledLayerNames = VM_validationLayers.Data();
            VM_validationLayers.PopulateDebugMessengerCreateInfo(debugCreateInfo);
            createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
        }
        else
        {
            createInfo.enabledLayerCount = 0;
            createInfo.pNext = nullptr;
        }

        if (vkCreateInstance(&createInfo, nullptr, &m_instance) != VK_SUCCESS)
        {
            printf("Failed to create Vulkan instance...\n");
            b_success = false;
        }

        return b_success;
    }

    void VulkanManager::CreateCommandPool(VkCommandPool& commandPool, LogicalDevice& logicalDevice, uint32_t queueFamilyIndex, VkCommandPoolCreateFlags flags)
    {
        VkCommandPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.flags = flags;
        poolInfo.queueFamilyIndex = queueFamilyIndex;

        if (vkCreateCommandPool(logicalDevice.GetDevice(), &poolInfo, nullptr, &commandPool) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create command pool!");
        }
    }

    void VulkanManager::InitializeMaterials()
    {
        m_sceneViewMaterials.clear();
        m_gameViewMaterials.clear();

        std::vector<std::string> materialFiles = std::vector<std::string>();
        materialFiles = FindAllFilesWithExtension(GetDir("projectDir"), ".mat");

        for (std::string path : materialFiles)
        {
            AddSceneViewMaterial(LoadMaterial(path, &m_sceneViewTexture));
            AddGameViewMaterial(LoadMaterial(path, &m_gameViewTexture));
        }
    }

    void VulkanManager::SaveMaterial(std::shared_ptr<Material> material)
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
    }

    std::shared_ptr<Material> VulkanManager::LoadMaterial(std::string path, Texture* renderToTexture)
    {
        std::shared_ptr<Material> newMaterial = std::make_shared<Material>();

        json materialData = LoadFileData(path);
        if (materialData != nullptr)
        {
            std::string name = CheckJsonString(materialData, "name", "Material");
            newMaterial->SetName(name);
            newMaterial->SetPath(path);

            if (name == "")
            {
                newMaterial->SetName(GetFilenameFromPath(path));
            }
         
            std::string vertexShaderPath = CheckJsonString(materialData, "vertexShaderPath", name);
            if (vertexShaderPath != "")
            {
                newMaterial->SetVertexPath(vertexShaderPath);
            }
            std::string fragmentShaderPath = CheckJsonString(materialData, "fragmentShaderPath", name);
            if (fragmentShaderPath != "")
            {
                newMaterial->SetFragmentPath(fragmentShaderPath);
            }
            int textureCount = CheckJsonInt(materialData, "textureCount", name);
            if (textureCount >= 0)
            {
                newMaterial->SetTextureCount((uint32_t)textureCount);
            }

            // Graphics Pipeline configuration
            if (JsonContains(materialData, "inputAssemblyData", name))
            {
                json inputAssemblyData = materialData["inputAssemblyData"];

                VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfos = newMaterial->GetInputAssemblyCreateInfos(); // Maybe just use initialized VkPipelineInputAssemblyStateCreateInfo instead
                inputAssemblyInfos.topology = (VkPrimitiveTopology)CheckJsonInt(inputAssemblyData, "topology", name);
                inputAssemblyInfos.primitiveRestartEnable = (bool)CheckJsonBool(inputAssemblyData, "_primitiveRestartEnable", name);
                newMaterial->SetInputAssemblyCreateInfos(inputAssemblyInfos);
            }
            if (JsonContains(materialData, "rasterizerData", name))
            {                
                json rasterizerData = materialData["rasterizerData"];

                VkPipelineRasterizationStateCreateInfo rasterizerInfos = newMaterial->GetRasterizerCreateInfos();
                rasterizerInfos.polygonMode = (VkPolygonMode)CheckJsonInt(rasterizerData, "polygonMode", name);
                rasterizerInfos.cullMode = (VkCullModeFlagBits)CheckJsonInt(rasterizerData, "cullMode", name);
                rasterizerInfos.lineWidth = CheckJsonFloat(rasterizerData, "lineWidth", name);
                newMaterial->SetRasterizerCreateInfos(rasterizerInfos);
            }
            if (JsonContains(materialData, "colorBlendAttachmentData", name))
            {
                json colorBlendAttachmentData = materialData["colorBlendAttachmentData"];

                VkPipelineColorBlendAttachmentState colorBlendAttachmentInfos = newMaterial->GetColorBlendAttachmentCreateInfos();
                colorBlendAttachmentInfos.alphaBlendOp = (VkBlendOp)CheckJsonInt(colorBlendAttachmentData, "alphaBlendOp", name);
                newMaterial->SetColorBlendAttachmentCreateInfos(colorBlendAttachmentInfos);
            }

            if (renderToTexture != nullptr)
            {
                newMaterial->SetRenderToTexture(renderToTexture);
                newMaterial->SetHandles(&m_instance, &m_winSystem, &m_physicalDevice, &m_logicalDevice, &m_commandPool, &m_renderToTextureRenderPass);
            }
            else
            {
                newMaterial->SetHandles(&m_instance, &m_winSystem, &m_physicalDevice, &m_logicalDevice, &m_commandPool, &m_imGuiRenderPass);
            }

            newMaterial->Init();
        }

        return newMaterial;
    }

    std::shared_ptr<Material> VulkanManager::CreateNewMaterialFile(std::string fileName, std::string path)
    {
        std::string filePath = "";
        std::shared_ptr<Material> newMaterial = std::make_shared<Material>();

        if (path == "")
        {
            filePath = GetDir("projectDir") + "/materials/" + fileName + ".mat";
        }
        else
        {
            filePath = path + "/" + fileName + ".mat";
        }

        newMaterial->SetPath(filePath);
        newMaterial->SetName(fileName);
        newMaterial->SetHandles(&m_instance, &m_winSystem, &m_physicalDevice, &m_logicalDevice, &m_commandPool, &m_renderToTextureRenderPass);
        SaveMaterial(newMaterial);        

        return newMaterial;
    }

    // Only call add material after all material members have been filled
    void VulkanManager::AddSceneViewMaterial(std::shared_ptr<Material> material)
    {
        material->SetViewport(ViewportType::SceneView);
        m_sceneViewMaterials.push_back(material);
    }
    void VulkanManager::AddGameViewMaterial(std::shared_ptr<Material> material)
    {
        material->SetViewport(ViewportType::GameView);
        m_gameViewMaterials.push_back(material);
    }

    // Assume Scene View Materials for now
    std::shared_ptr<Material> VulkanManager::GetMaterial(std::string materialName)
    {                
        for (std::shared_ptr<Material> material : m_sceneViewMaterials)        
        {
            if (material->GetName() == materialName)
            {
                return material;
            }
        }

        return nullptr;
    }

    // Assume Scene View Materials for now
    std::vector<std::shared_ptr<Material>> VulkanManager::GetMaterials()
    {
        return m_sceneViewMaterials;
    }

    void VulkanManager::ReloadShaders()
    {
        for (std::shared_ptr<Material> material : m_sceneViewMaterials)
        {
            if (material->Initialized())
            {
                material->RecreateGraphicsPipeline();
            }
        }
        for (std::shared_ptr<Material> material : m_gameViewMaterials)
        {
            if (material->Initialized())
            {
                material->RecreateGraphicsPipeline();
            }
        }
    }

    void VulkanManager::CreateSyncObjects()
    {
        // More info here - https://vulkan-tutorial.com/en/Drawing_a_triangle/Drawing/Rendering_and_presentation

        VkSemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT; // Start it signaled so the very first frame doesn't block indefinitely

        m_imageAvailableSemaphores.resize(VM_MAX_FRAMES_IN_FLIGHT);
        m_renderFinishedSemaphores.resize(VM_MAX_FRAMES_IN_FLIGHT);
        m_inFlightFences.resize(VM_MAX_FRAMES_IN_FLIGHT);

        for (size_t i = 0; i < VM_MAX_FRAMES_IN_FLIGHT; i++)
        {
            if (vkCreateSemaphore(m_logicalDevice.GetDevice(), &semaphoreInfo, nullptr, &m_imageAvailableSemaphores[i]) != VK_SUCCESS ||
                vkCreateSemaphore(m_logicalDevice.GetDevice(), &semaphoreInfo, nullptr, &m_renderFinishedSemaphores[i]) != VK_SUCCESS ||
                vkCreateFence(m_logicalDevice.GetDevice(), &fenceInfo, nullptr, &m_inFlightFences[i]) != VK_SUCCESS)
            {
                throw std::runtime_error("failed to create semaphores!");
            }
        }
    }

    void VulkanManager::CreateImGuiTexture(Texture& texture, std::vector<VkDescriptorSet>& descriptorSets)
    {
        texture.CreateTextureImage();
        Model emptyModel = Model();
        std::vector<Texture> textures = std::vector<Texture>(1, texture);
        m_imGuiMaterial->CreateDescriptorSets(descriptorSets, emptyModel, textures);
    }

    void VulkanManager::FreeImGuiTexture(uint32_t allocatedFrom)
    {
        m_imGuiMaterial->GetAllocator().SetFreed(allocatedFrom);
    }

    std::vector<VkDescriptorSet>& VulkanManager::GetSceneViewDescriptorSets()
    {
        return m_sceneViewTexture.GetDescriptorSets();
    }

    std::vector<VkDescriptorSet>& VulkanManager::GetGameViewDescriptorSets()
    {
        return m_gameViewTexture.GetDescriptorSets();
    }

    void VulkanManager::DrawFrame()
    {
        // More info here - https://vulkan-tutorial.com/en/Drawing_a_triangle/Drawing/Rendering_and_presentation

        if (m_winSystem.m_b_framebufferResized)
        {
            RecreateSwapChainAndFrameBuffers();
            m_winSystem.m_b_framebufferResized = false;
        }

        // At the start of the frame, we want to wait until the previous frame has finished, so that the command buffer and semaphores are available to use. To do that, we call vkWaitForFences:
        vkWaitForFences(m_logicalDevice.GetDevice(), 1, &m_inFlightFences[VM_currentFrame], VK_TRUE, UINT64_MAX);

        uint32_t imageIndex;
        VkResult aquireImageResult = vkAcquireNextImageKHR(m_logicalDevice.GetDevice(), m_winSystem.GetSwapChain(), UINT64_MAX, m_imageAvailableSemaphores[VM_currentFrame], VK_NULL_HANDLE, &imageIndex);
        if (!CheckSwapChainIntegrity(aquireImageResult, "Failed to acquire swap chain image."))
        {
            return;
        }

        // manually reset the fence to the unsignaled state with the vkResetFences call:
        vkResetFences(m_logicalDevice.GetDevice(), 1, &m_inFlightFences[VM_currentFrame]);
        
        Model emptyModel = Model();
        std::vector<VkCommandBuffer> commandBuffers;
        
        bool b_sceneViewTextureDrawnTo = false;
        //bool b_gameViewTextureDrawnTo = false;

        if (m_renderToTextureRenderPass.Initialized())
        {
            m_renderToTextureRenderPass.BeginRenderPass(imageIndex);
            // Scene View
            for (std::shared_ptr<Material> material : m_sceneViewMaterials)
            {
                if (material->Initialized())
                {
                    if (material->HandleRenderPass(imageIndex, ViewportType::SceneView))
                    {
                        b_sceneViewTextureDrawnTo = true;
                    }
                }
            }
            // Game View
            //for (std::shared_ptr<Material> material : m_gameViewMaterials)
            //{
            //    if (material->Initialized())
            //    {
            //        material->HandleRenderPass(imageIndex, ViewportType::GameView);                     
            //    }
            //}
            m_renderToTextureRenderPass.EndRenderPass();

            if (b_sceneViewTextureDrawnTo) // Once VkImage has been written to from m_renderPass, that image can be used as a texture to render to using a different material and desired descriptorSets, so we'd need to create descriptor sets for it using that material's configuration
            {
                std::vector<Texture> textures = std::vector<Texture>(1, m_sceneViewTexture); // m_sceneViewTexture was given to m_renderPass in each material and if m_name != imgui, it was written to in m_renderPass.DrawIndexed()
                m_imGuiMaterial->GetAllocator().AllocateDescriptorSets(m_sceneViewTexture.GetDescriptorSets(), emptyModel, textures);

                commandBuffers.push_back(m_renderToTextureRenderPass.GetCommandBuffers()[VM_currentFrame]);
            }
            //if (b_gameViewTextureDrawnTo) // Once VkImage has been written to from m_renderPass, that image can be used as a texture to render to using a different material and desired descriptorSets, so we'd need to create descriptor sets for it using that material's configuration
            //{
            //    std::vector<Texture> textures = std::vector<Texture>(1, m_gameViewTexture); // m_sceneViewTexture was given to m_renderPass in each material and if m_name != imgui, it was written to in m_renderPass.DrawIndexed()
            //    m_imGuiMaterial->GetAllocator().AllocateDescriptorSets(m_gameViewTexture.GetDescriptorSets(), emptyModel, textures);
            //}
        }
 
        if (m_imGuiMaterial != nullptr)
        {
            m_imGuiMaterial->HandleRenderPass(imageIndex);
        }
        commandBuffers.push_back(m_imGuiRenderPass.GetCommandBuffers()[VM_currentFrame]);
   



        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        VkSemaphore waitSemaphores[] = { m_imageAvailableSemaphores[VM_currentFrame] };
        VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;
        submitInfo.commandBufferCount = (uint32_t)commandBuffers.size();
        submitInfo.pCommandBuffers = commandBuffers.data();

        VkSemaphore signalSemaphores[] = { m_renderFinishedSemaphores[VM_currentFrame] };
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        VkResult submitResult = vkQueueSubmit(m_logicalDevice.GetGraphicsQueue(), 1, &submitInfo, m_inFlightFences[VM_currentFrame]);
        CheckSwapChainIntegrity(submitResult, "Failed to submit draw command buffer.");

        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;

        VkSwapchainKHR swapChains[] = { m_winSystem.GetSwapChain() };
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapChains;
        presentInfo.pImageIndices = &imageIndex;
        presentInfo.pResults = nullptr;

        // Present!
        VkResult presentResult = vkQueuePresentKHR(m_logicalDevice.GetPresentQueue(), &presentInfo);

        VM_currentFrame = (VM_currentFrame + 1) % VM_MAX_FRAMES_IN_FLIGHT;
    }

    VkSampleCountFlagBits VulkanManager::GetMaxSamples()
    {
        return m_maxSamples;
    }

    void VulkanManager::SetMaxSamples(VkSampleCountFlagBits maxSamples)
    {
        m_maxSamples = maxSamples;
    }

    void VulkanManager::RecreateSwapChainAndFrameBuffers()
    {
        m_winSystem.RecreateSwapChain();   

        m_renderToTextureRenderPass.RecreateFrameBuffers();
        m_imGuiRenderPass.RecreateFrameBuffers();

        ImGui_ImplVulkan_SetMinImageCount(static_cast<uint32_t>(m_winSystem.GetSwapChainImageViews().size()));
    }

    bool VulkanManager::CheckSwapChainIntegrity(VkResult result, std::string errorMessage)
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

    WinSys& VulkanManager::GetWinSystem()
    {
        return m_winSystem;
    }

    VkInstance& VulkanManager::GetInstance()
    {
        return m_instance;
    }

    VkQueue& VulkanManager::GetGraphicsQueue()
    {
        return m_logicalDevice.GetGraphicsQueue();
    }

    PhysicalDevice& VulkanManager::GetPhysicalDevice()
    {
        return m_physicalDevice;
    }

    LogicalDevice& VulkanManager::GetLogicalDevice()
    {
        return m_logicalDevice;
    }

    VkCommandPool& VulkanManager::GetCommandPool()
    {
        return m_commandPool;
    }
}