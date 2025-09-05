#include "ImGuiManager.h"
#include "VulkanManager.h"
#include "FlatEngine.h"

#include <stdexcept>


namespace FlatEngine
{
    ImGuiManager::ImGuiManager()
    {
        m_renderPass = RenderPass();
        m_commandPool = VK_NULL_HANDLE;
        m_material = std::make_shared<Material>("imgui", "../engine/shaders/imguiVert.spv", "../engine/shaders/imguiFrag.spv");
        // handles
        m_instanceHandle = VK_NULL_HANDLE;
        m_winSystem = nullptr;
        m_physicalDeviceHandle = nullptr;
        m_deviceHandle = nullptr;
    }

    ImGuiManager::~ImGuiManager()
    {
    }

    void ImGuiManager::Cleanup()
    {
        ImGui_ImplVulkan_Shutdown();
        ImGui_ImplSDL2_Shutdown();
    }


    bool ImGuiManager::SetupImGui(VkInstance instance, WinSys& winSystem, PhysicalDevice& physicalDevice, LogicalDevice& logicalDevice)
    {
        // https://frguthmann.github.io/posts/vulkan_imgui/

        bool b_success = true;

        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; // | ImGuiConfigFlags_ViewportsEnable;
        //io.BackendFlags |= ImGuiBackendFlags_PlatformHasViewports | ImGuiBackendFlags_RendererHasViewports;

        m_instanceHandle = instance;
        m_winSystem = &winSystem;
        m_physicalDeviceHandle = &physicalDevice;
        m_deviceHandle = &logicalDevice;
        m_renderPass.SetHandles(instance, winSystem, physicalDevice, logicalDevice);
        m_material->SetHandles(instance, winSystem, physicalDevice, logicalDevice, m_renderPass, m_commandPool);        

        if (!CreateImGuiResources())
        {
            b_success = false;
        }

        return b_success;
    }

    void ImGuiManager::GetImGuiDescriptorSetLayoutInfo(std::vector<VkDescriptorSetLayoutBinding>& bindings, VkDescriptorSetLayoutCreateInfo& layoutInfo)
    {
        bindings.resize(1);
        bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        bindings[0].descriptorCount = 1;
        bindings[0].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
        bindings[0].binding = 0;
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
        layoutInfo.pBindings = bindings.data();
    }

    void ImGuiManager::GetImGuiDescriptorPoolInfo(std::vector<VkDescriptorPoolSize>& poolSizes, VkDescriptorPoolCreateInfo& poolInfo)
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

    bool ImGuiManager::CreateImGuiResources()
    {
        bool b_success = true;

        // Create ImGui Command Pool
        VulkanManager::CreateCommandPool(m_commandPool, *m_deviceHandle, m_deviceHandle->GetGraphicsIndex(), VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

        // Configure ImGui Render Pass
        m_renderPass.ConfigureFrameBufferImageViews(m_winSystem->GetSwapChainImageViews());

        VkAttachmentDescription colorAttachment = {};
        colorAttachment.format = m_winSystem->GetImageFormat();
        colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        VkAttachmentReference colorAttachmentRef = {};
        colorAttachmentRef.attachment = 0;
        colorAttachmentRef.layout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        m_renderPass.AddRenderPassAttachment(colorAttachment, colorAttachmentRef);

        // Create Dependency
        VkSubpassDependency dependency{};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        m_renderPass.AddSubpassDependency(dependency);

        VkSubpassDescription subpass = {};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorAttachmentRef;
        m_renderPass.AddSubpass(subpass);

        m_renderPass.Init(m_commandPool);

        // Set up Descriptor Allocator
        std::vector<VkDescriptorSetLayoutBinding> bindings{};
        VkDescriptorSetLayoutCreateInfo layoutInfo{};
        GetImGuiDescriptorSetLayoutInfo(bindings, layoutInfo);
        std::vector<VkDescriptorPoolSize> poolSizes{};
        VkDescriptorPoolCreateInfo poolInfo{};
        GetImGuiDescriptorPoolInfo(poolSizes, poolInfo);

        // Material
        m_material->GetAllocator().ConfigureDescriptorSetLayout(bindings, layoutInfo);
        m_material->GetAllocator().ConfigureDescriptorPools(poolSizes, poolInfo);
        m_material->SetTextureCount(1);
        m_material->Init();

        // Setup ImGui Platform/Renderer backends        
        ImGui_ImplSDL2_InitForVulkan(m_winSystem->GetWindow());

        ImGui_ImplVulkan_InitInfo init_info = {};
        init_info.Instance = m_instanceHandle;
        init_info.PhysicalDevice = m_physicalDeviceHandle->GetDevice();
        init_info.Device = m_deviceHandle->GetDevice();
        init_info.QueueFamily = ImGui_ImplVulkanH_SelectQueueFamilyIndex(m_physicalDeviceHandle->GetDevice());
        init_info.Queue = m_deviceHandle->GetGraphicsQueue();
        init_info.PipelineCache = VK_NULL_HANDLE;
        init_info.DescriptorPool = m_material->CreateDescriptorPool();
        init_info.RenderPass = m_renderPass.GetRenderPass();
        init_info.Subpass = 0;
        init_info.MinImageCount = VM_MAX_FRAMES_IN_FLIGHT;
        init_info.ImageCount = VM_MAX_FRAMES_IN_FLIGHT;
        init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
        init_info.Allocator = nullptr;
        init_info.CheckVkResultFn = VulkanManager::check_vk_result;

        if (!ImGui_ImplVulkan_Init(&init_info))
        {
            b_success = false;
            FlatEngine::LogError("ImGui backends setup failed!");
        }

        return b_success;
    }

    void ImGuiManager::QuitImGui()
    {
        vkDestroyRenderPass(m_deviceHandle->GetDevice(), m_renderPass.GetRenderPass(), nullptr);
        vkDestroyCommandPool(m_deviceHandle->GetDevice(), m_commandPool, nullptr);

        VkResult err = vkDeviceWaitIdle(m_deviceHandle->GetDevice());
        VulkanManager::check_vk_result(err);
        ImGui_ImplVulkan_Shutdown();
        ImGui_ImplSDL2_Shutdown();
        ImGui::DestroyContext();
    }

    void ImGuiManager::HandleRenderPass(uint32_t imageIndex)
    {
        m_renderPass.BeginRenderPass(imageIndex);
        ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), m_renderPass.GetCommandBuffers()[VM_currentFrame]);
        m_renderPass.EndRenderPass();
    }

    void ImGuiManager::CreateDescriptorSets(Texture& texture, std::vector<VkDescriptorSet>& descriptorSets)
    {
        Model emptyModel = Model();
        std::vector<Texture> textures = std::vector<Texture>(1, texture);
        m_material->GetAllocator().AllocateDescriptorSets(descriptorSets, emptyModel, textures);
    }

    void ImGuiManager::FreeDescriptorSet(uint32_t allocationIndex)
    {
        m_material->GetAllocator().SetFreed(allocationIndex);
    }

    RenderPass& ImGuiManager::GetRenderPass()
    {
        return m_renderPass;
    }

    std::shared_ptr<Material> ImGuiManager::GetMaterial()
    {
        return m_material;
    }
}