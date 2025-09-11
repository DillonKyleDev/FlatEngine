#include "ImGuiManager.h"
#include "VulkanManager.h"
#include "FlatEngine.h"
#include "Vector3.h"
#include "Transform.h"
#include "Camera.h"

#include <stdexcept>


namespace FlatEngine
{
    ImGuiManager::ImGuiManager()
    {        
    }

    ImGuiManager::~ImGuiManager()
    {
    }

    void ImGuiManager::Cleanup()
    {
        PipelineManager::Cleanup();

        QuitImGui();
        ImGui_ImplVulkan_Shutdown();
        ImGui_ImplSDL2_Shutdown();
    }


    void ImGuiManager::Setup(VkInstance& instance, WinSys& winSystem, PhysicalDevice& physicalDevice, LogicalDevice& logicalDevice, VkCommandPool& commandPool)
    {
        PipelineManager::Setup(instance, winSystem, physicalDevice, logicalDevice, commandPool);

        // https://frguthmann.github.io/posts/vulkan_imgui/        

        CreateImageResources();
        CreateRenderPassResources();
        CreateImGuiResources();
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

    void ImGuiManager::CreateRenderPassResources()
    {
        // Configure ImGui Render Pass
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

        m_renderPass.ConfigureFrameBufferImageViews(m_renderTexture.GetImageViews());
        //m_renderPass.ConfigureFrameBufferImageViews(m_winSystem->GetSwapChainImageViews());
        m_renderPass.Init(*m_commandPool);
    }

    void ImGuiManager::CreateImGuiResources()
    {        
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;// | ImGuiConfigFlags_ViewportsEnable;
        //io.BackendFlags |= ImGuiBackendFlags_PlatformHasViewports | ImGuiBackendFlags_RendererHasViewports;
        
        VulkanManager::CreateCommandPool(*m_commandPool, *m_logicalDevice, m_logicalDevice->GetGraphicsIndex(), VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

        // Set up Descriptor Material Allocator
        std::vector<VkDescriptorSetLayoutBinding> bindings{};
        VkDescriptorSetLayoutCreateInfo layoutInfo{};
        GetImGuiDescriptorSetLayoutInfo(bindings, layoutInfo);
        std::vector<VkDescriptorPoolSize> poolSizes{};
        VkDescriptorPoolCreateInfo poolInfo{};
        GetImGuiDescriptorPoolInfo(poolSizes, poolInfo);

        std::shared_ptr<Material> imGuiMaterial = F_VulkanManager->GetMaterial("imgui");
        imGuiMaterial->GetAllocator().ConfigureDescriptorSetLayout(bindings, layoutInfo);
        imGuiMaterial->GetAllocator().ConfigureDescriptorPools(poolSizes, poolInfo);                
        imGuiMaterial->Init();
              
        ImGui_ImplSDL2_InitForVulkan(m_winSystem->GetWindow());

        ImGui_ImplVulkan_InitInfo init_info = {};
        init_info.Instance = *m_instance;
        init_info.PhysicalDevice = m_physicalDevice->GetDevice();
        init_info.Device = m_logicalDevice->GetDevice();
        init_info.QueueFamily = ImGui_ImplVulkanH_SelectQueueFamilyIndex(m_physicalDevice->GetDevice());
        init_info.Queue = m_logicalDevice->GetGraphicsQueue();
        init_info.PipelineCache = VK_NULL_HANDLE;
        init_info.DescriptorPool = imGuiMaterial->CreateDescriptorPool();
        init_info.RenderPass = m_renderPass.GetRenderPass();
        init_info.Subpass = 0;
        init_info.MinImageCount = VM_MAX_FRAMES_IN_FLIGHT;
        init_info.ImageCount = VM_MAX_FRAMES_IN_FLIGHT;
        init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
        init_info.Allocator = nullptr;
        init_info.CheckVkResultFn = VulkanManager::check_vk_result;

        if (!ImGui_ImplVulkan_Init(&init_info))
        {
            FlatEngine::LogError("ImGui backends setup failed!");
        }
    }

    void ImGuiManager::QuitImGui()
    {        
        vkDestroyCommandPool(m_logicalDevice->GetDevice(), *m_commandPool, nullptr);

        VkResult err = vkDeviceWaitIdle(m_logicalDevice->GetDevice());
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

    void ImGuiManager::CreateImageResources()
    {        
        m_renderTexture.GetImageViews() = m_winSystem->GetSwapChainImageViews();
        m_renderPass.ConfigureFrameBufferImageViews(m_renderTexture.GetImageViews()); // Give m_renderPass the new imageViews
    }

    void ImGuiManager::CreateDescriptorSets(std::shared_ptr<Material> material, std::vector<VkDescriptorSet>& descriptorSets, Model& model, std::vector<Texture>& textures)
    {        
        material->CreateDescriptorSets(descriptorSets, model, textures);
    }

    void ImGuiManager::OnWindowResized()
    {
        CreateImageResources();
        m_renderPass.RecreateFrameBuffers();
    }

    void ImGuiManager::RecordCommandBuffer(uint32_t imageIndex, Mesh& mesh)
    {
        std::vector<VkCommandBuffer>& commandBuffers = m_renderPass.GetCommandBuffers();
        VkPipeline& graphicsPipeline = mesh.GetMaterial()->GetGraphicsPipeline();
        VkPipelineLayout& pipelineLayout = mesh.GetMaterial()->GetPipelineLayout();
        Transform* transform = mesh.GetParent()->GetTransform();
        Vector3 meshPosition = transform->GetPosition();
        glm::mat4 meshScale = transform->GetScaleMatrix();
        glm::mat4 meshRotation = transform->GetRotationMatrix();
        Camera* primaryCamera = FlatEngine::GetPrimaryCamera();
        Vector3 cameraPosition = primaryCamera->GetParent()->GetTransform()->GetPosition();
        Vector3 lookDir = primaryCamera->GetLookDirection();
        float nearClip = primaryCamera->GetNearClippingDistance();
        float farClip = primaryCamera->GetFarClippingDistance();
        float perspectiveAngle = primaryCamera->GetPerspectiveAngle();
        glm::vec3 up = glm::vec3(0.0f, 0.0f, 1.0f);

        static auto startTime = std::chrono::high_resolution_clock::now();
        auto currentTime = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

        float aspectRatio = (float)(m_winSystem->GetExtent().width / m_winSystem->GetExtent().height);

        glm::vec4 cameraLookDir = glm::vec4(lookDir.x, lookDir.y, lookDir.z, 0);
        glm::vec4 meshPos = glm::vec4(meshPosition.x, meshPosition.y, meshPosition.z, 0);
        glm::mat4 model = meshRotation * meshScale;
        glm::mat4 view = glm::lookAt(cameraPosition.GetGLMVec3(), glm::vec3(cameraPosition.x + cameraLookDir.x, cameraPosition.y + cameraLookDir.y, cameraPosition.z + cameraLookDir.z), up); // Look at camera direction not working right...
        glm::mat4 projection = glm::perspective(glm::radians(perspectiveAngle), aspectRatio, nearClip, farClip);
        projection[1][1] *= -1;

        glm::vec4 viewportCameraPos = glm::vec4(cameraPosition.x, cameraPosition.y, cameraPosition.z, 0);

        uint32_t posOffset = 0;
        uint32_t posSize = sizeof(glm::vec4);
        uint32_t cameraPosOffset = sizeof(glm::vec4);
        uint32_t cameraPosSize = sizeof(glm::vec4);
        uint32_t timeOffset = sizeof(glm::vec4) * 2;
        uint32_t timeSize = sizeof(glm::vec4);
        uint32_t modelOffset = sizeof(glm::vec4) * 2;
        uint32_t modelSize = sizeof(glm::mat4);
        uint32_t viewProjectionOffset = sizeof(glm::mat4) * 2;
        uint32_t viewProjectionSize = sizeof(glm::mat4);

        //vkCmdPushConstants(commandBuffers[VM_currentFrame], pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, posOffset, posSize, &meshPos);
        //vkCmdPushConstants(commandBuffers[VM_currentFrame], pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, cameraPosOffset, cameraPosSize, &viewportCameraPos);
        //vkCmdPushConstants(commandBuffers[VM_currentFrame], pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, timeOffset, timeSize, &time);
        //vkCmdPushConstants(commandBuffers[VM_currentFrame], pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, modelOffset, modelSize, &model);
        //vkCmdPushConstants(commandBuffers[VM_currentFrame], pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, viewProjectionOffset, viewProjectionSize, &view);

        vkCmdBindPipeline(commandBuffers[VM_currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);
    }
}