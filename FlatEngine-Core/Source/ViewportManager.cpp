#include "ViewportManager.h"
#include "Helper.h"
#include "FlatEngine.h"
#include "ImGuiManager.h"


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
                m_renderPass.RecordCommandBuffer(imageIndex, mesh.second);
                m_renderPass.DrawIndexed(mesh.second);

                mesh.second.GetModel().UpdateUniformBuffer(VM_currentFrame, *m_winSystem, 0.5f);
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
        m_renderPass.SetImageColorFormat(VK_FORMAT_R32G32B32A32_SFLOAT);
        m_renderPass.SetMSAASampleCount(VK_SAMPLE_COUNT_1_BIT);

        VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_1_BIT;
        VkFormat colorFormat = VK_FORMAT_R32G32B32A32_SFLOAT;

        VkAttachmentDescription colorAttachment{};
        colorAttachment.format = colorFormat;
        colorAttachment.samples = msaaSamples;
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
        depthAttachment.samples = msaaSamples;
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
}
