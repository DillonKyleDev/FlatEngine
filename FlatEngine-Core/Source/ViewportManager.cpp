#include "ViewportManager.h"
#include "Helper.h"
#include "FlatEngine.h"
#include "ImGuiManager.h"
#include "Vector3.h"
#include "Transform.h"
#include "Camera.h"


namespace FlatEngine
{
	ViewportManager::ViewportManager()
	{      
        m_viewportDescriptorSets.resize(VM_MAX_FRAMES_IN_FLIGHT);
	}

    ViewportManager::~ViewportManager()
    {
    }

    void ViewportManager::Cleanup()
    {
    }

    void ViewportManager::Setup(VkInstance& instance, WinSys& winSystem, PhysicalDevice& physicalDevice, LogicalDevice& logicalDevice, VkCommandPool& commandPool)
    {
        PipelineManager::Setup(instance, winSystem, physicalDevice, logicalDevice, commandPool);
        
        CreateImageResources();
        CreateRenderPassResources();
    }

    void ViewportManager::HandleRenderPass(uint32_t imageIndex)
    {
        m_renderPass.BeginRenderPass(imageIndex);
        for (std::pair<long, Mesh> mesh : FlatEngine::GetMeshes())
        {
            if (mesh.second.Initialized())
            {
                RecordCommandBuffer(imageIndex, mesh.second);
                m_renderPass.DrawIndexed(mesh.second);
                mesh.second.GetModel().UpdateUniformBuffer(imageIndex, *m_winSystem, &mesh.second);
            }
        }
        m_renderPass.EndRenderPass();

        // Once VkImage has been written to from ViewportManager, that image needs to be used as a texture in the ImGuiManager Pipeline, so we need to create descriptor sets for it there
        std::shared_ptr<Material> imguiMaterial = F_VulkanManager->GetMaterial("imgui");
        Model emptyModel = Model();
        std::vector<Texture> textures = std::vector<Texture>(1, m_renderTexture);
        imguiMaterial->GetAllocator().AllocateDescriptorSets(m_viewportDescriptorSets, emptyModel, textures);        
    }

    void ViewportManager::CreateRenderPassResources()
    {
        m_renderPass.EnableDepthBuffering();
        m_renderPass.EnableMsaa();
        VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_1_BIT;// F_VulkanManager->GetMaxSamples();
        VkFormat colorFormat = VK_FORMAT_R32G32B32A32_SFLOAT;
        m_renderPass.SetImageColorFormat(colorFormat);
        m_renderPass.SetMSAASampleCount(msaaSamples);

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
        m_renderPass.AddRenderPassAttachment(colorAttachment, colorAttachmentRef);

        VkAttachmentDescription depthAttachment{};
        depthAttachment.format = Helper::FindDepthFormat(m_physicalDevice->GetDevice());
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
        m_renderPass.AddRenderPassAttachment(depthAttachment, depthAttachmentRef);

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
        colorAttachmentResolveRef.attachment = 2;
        colorAttachmentResolveRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        m_renderPass.AddRenderPassAttachment(colorAttachmentResolve, colorAttachmentResolveRef);

        // Create Dependency
        VkSubpassDependency dependency{};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        m_renderPass.AddSubpassDependency(dependency);

        VkSubpassDescription subpass{};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &m_renderPass.GetAttachmentRefs()[0];
        subpass.pDepthStencilAttachment = &m_renderPass.GetAttachmentRefs()[1];
        subpass.pResolveAttachments = &m_renderPass.GetAttachmentRefs()[2];
        m_renderPass.AddSubpass(subpass);

        m_renderPass.ConfigureFrameBufferImageViews(m_renderTexture.GetImageViews());
        m_renderPass.Init(*m_commandPool);
    }

    // Configure images used to render to texture
    void ViewportManager::CreateImageResources()
    {
        std::vector<VkImage>& images = m_renderTexture.GetImages();
        std::vector<VkImageView>& imageViews = m_renderTexture.GetImageViews();
        std::vector<VkDeviceMemory>& imageMemory = m_renderTexture.GetImageMemory();
        VkSampler& sampler = m_renderTexture.GetSampler();
        images.resize(VM_MAX_FRAMES_IN_FLIGHT);
        imageViews.resize(VM_MAX_FRAMES_IN_FLIGHT);
        imageMemory.resize(VM_MAX_FRAMES_IN_FLIGHT);
        VkExtent2D extent = m_winSystem->GetExtent();

        for (size_t i = 0; i < VM_MAX_FRAMES_IN_FLIGHT; i++)
        {
            uint32_t mipLevels = 1;
            int texWidth = extent.width;
            int texHeight = extent.height;
            VkDeviceSize imageSize = texWidth * texHeight * 16;
            VkBuffer stagingBuffer{};
            VkDeviceMemory stagingBufferMemory{};
            WinSys::CreateBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory, *m_physicalDevice, *m_logicalDevice);
            WinSys::CreateImage(texWidth, texHeight, 1, VK_SAMPLE_COUNT_1_BIT, m_imageFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, images[i], imageMemory[i], *m_physicalDevice, *m_logicalDevice);

            VkCommandBuffer copyCmd = Helper::BeginSingleTimeCommands(*m_commandPool, *m_logicalDevice);
            WinSys::InsertImageMemoryBarrier(
                copyCmd,
                images[i],
                VK_ACCESS_TRANSFER_READ_BIT,
                VK_ACCESS_MEMORY_READ_BIT,
                VK_IMAGE_LAYOUT_UNDEFINED,
                VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                VK_PIPELINE_STAGE_TRANSFER_BIT,
                VK_PIPELINE_STAGE_TRANSFER_BIT,
                VkImageSubresourceRange{ VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 });
            Helper::EndSingleTimeCommands(*m_commandPool, copyCmd, *m_logicalDevice);

            WinSys::TransitionImageLayout(images[i], m_imageFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, *m_commandPool, *m_logicalDevice);
            WinSys::CopyBufferToImage(stagingBuffer, images[i], static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight), *m_commandPool, *m_logicalDevice);
            WinSys::TransitionImageLayout(images[i], m_imageFormat, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 1, *m_commandPool, *m_logicalDevice);

            vkDestroyBuffer(m_logicalDevice->GetDevice(), stagingBuffer, nullptr);
            vkFreeMemory(m_logicalDevice->GetDevice(), stagingBufferMemory, nullptr);

            WinSys::CreateImageView(imageViews[i], images[i], m_imageFormat, VK_IMAGE_ASPECT_COLOR_BIT, mipLevels, *m_logicalDevice);
            WinSys::CreateTextureSampler(sampler, mipLevels, *m_physicalDevice, *m_logicalDevice);            
        }

        m_renderPass.ConfigureFrameBufferImageViews(m_renderTexture.GetImageViews()); // Update m_imageViews in m_renderPass
    }

    std::vector<VkDescriptorSet> ViewportManager::GetDescriptorSets()
    {
        return m_viewportDescriptorSets;
    }

    void ViewportManager::OnWindowResized()
    {
        CreateImageResources();
        m_renderPass.RecreateFrameBuffers();
    }

    void ViewportManager::CreateDescriptorSets(std::shared_ptr<Material> material, std::vector<VkDescriptorSet>& descriptorSets, Model& model, std::vector<Texture>& textures)
    {
        material->CreateDescriptorSets(descriptorSets, model, textures);
    }
    
    void ViewportManager::RecordCommandBuffer(uint32_t imageIndex, Mesh& mesh)
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
        uint32_t modelOffset = sizeof(glm::mat4);
        uint32_t modelSize = sizeof(glm::mat4);
        uint32_t viewOffset = sizeof(glm::mat4) * 2;
        uint32_t viewSize = sizeof(glm::mat4);
        uint32_t projectionOffset = sizeof(glm::mat4) * 3;
        uint32_t projectionSize = sizeof(glm::mat4);

        vkCmdPushConstants(commandBuffers[imageIndex], pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, posOffset, posSize, &meshPos);
        vkCmdPushConstants(commandBuffers[imageIndex], pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, cameraPosOffset, cameraPosSize, &viewportCameraPos);
        vkCmdPushConstants(commandBuffers[imageIndex], pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, timeOffset, timeSize, &time);
        vkCmdPushConstants(commandBuffers[imageIndex], pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, modelOffset, modelSize, &model);
        vkCmdPushConstants(commandBuffers[imageIndex], pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, viewOffset, viewSize, &view);
        vkCmdPushConstants(commandBuffers[imageIndex], pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, projectionOffset, projectionSize, &projection);

        vkCmdBindPipeline(commandBuffers[imageIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);
    }
}
