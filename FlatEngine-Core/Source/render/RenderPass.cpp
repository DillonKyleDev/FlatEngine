#include "Types.h"
#include "components/Light.h"
#include "managers/Assets.h"
#include "managers/SceneManager.h"
#include "render/DeviceManager.h"
#include "render/Helper.h"
#include "render/RenderPass.h"
#include "render/RenderWindow.h"
#include "render/VulkanManager.h"
#include "tools/Vector3.h"
#include "tools/Vector4.h"
#include "Types.h"

#include <glm.hpp>


namespace FlatEngine
{
    RenderPass::RenderPass()
    {
        m_renderPass = VK_NULL_HANDLE;
        m_b_initialized = false;
        m_commandPool = nullptr;
        
        m_b_defaultRenderPassConfig = false;
        m_colorFormat = VK_FORMAT_R32G32B32A32_SFLOAT;
        // antialiasing
        m_msaaSamples = VK_SAMPLE_COUNT_1_BIT;
        m_colorImage = VK_NULL_HANDLE;
        m_colorImageMemory = VK_NULL_HANDLE;
        m_colorImageView = VK_NULL_HANDLE;
        m_b_msaaEnabled = false;
        // depth testing
        m_b_depthBuffersEnabled = false;
        
        m_imageUsageFlags = 0;

        m_beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        m_beginInfo.flags = 0;
        m_beginInfo.pInheritanceInfo = nullptr;

        m_renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        m_renderPassInfo.renderArea.offset = { 0, 0 };        

        m_viewport.x = 0.0f;
        m_viewport.y = 0.0f;
        m_viewport.minDepth = 0.0f;
        m_viewport.maxDepth = 1.0f;
        
        m_scissor.offset = { 0, 0 };
    }

    void RenderPass::Cleanup()
    {
        vkDestroyRenderPass(DeviceManager::logicalDevice.GetDevice(), m_renderPass, nullptr);
    }


    void RenderPass::SetHandles(VkCommandPool* commandPool)
    {
        m_commandPool = commandPool;
    }

    void RenderPass::Init()
    {
        if (m_b_msaaEnabled)
        {
            CreateColorResources();
        }
        if (m_b_depthBuffersEnabled)
        {
            CreateDepthResources();
        }
        CreateRenderPass();
        CreateFrameBuffers();
        CreateCommandBuffers();
        m_b_initialized = true;
    }

    bool RenderPass::Initialized()
    {
        return m_b_initialized;
    }

    void RenderPass::SetDefaultRenderPassConfig()
    {
        //m_b_defaultRenderPassConfig = true;
        //m_colorFormat = RenderWindow::window.GetImageFormat();
        //EnableDepthBuffering();
        //EnableMsaa();

        //// Attachments for fragment shader stage

        //VkAttachmentDescription colorAttachment{};
        //colorAttachment.format = m_colorFormat;
        //colorAttachment.samples = m_msaaSamples;
        //colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        //colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        //colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        //colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        //colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        //colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        //VkAttachmentReference colorAttachmentRef{};
        //colorAttachmentRef.attachment = 0; // (layout = 0) in shader
        //colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        //AddRenderPassAttachment(colorAttachment, colorAttachmentRef);

        //// Depth attachment
        //VkAttachmentDescription depthAttachment{};
        //depthAttachment.format = Helper::FindDepthFormat(DeviceManager::physicalDevice.GetDevice());
        //depthAttachment.samples = m_msaaSamples;
        //depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        //depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        //depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        //depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        //depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        //depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        //VkAttachmentReference depthAttachmentRef{};
        //depthAttachmentRef.attachment = 1; // (layout = 1) in shader
        //depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        //AddRenderPassAttachment(depthAttachment, depthAttachmentRef);

        //// Resolve attachment for MSAA
        //VkAttachmentDescription colorAttachmentResolve{};
        //colorAttachmentResolve.format = m_colorFormat;
        //colorAttachmentResolve.samples = m_msaaSamples;
        //colorAttachmentResolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        //colorAttachmentResolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        //colorAttachmentResolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        //colorAttachmentResolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        //colorAttachmentResolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        //colorAttachmentResolve.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        //VkAttachmentReference colorAttachmentResolveRef{};
        //colorAttachmentResolveRef.attachment = 2; // (layout = 2) in shader
        //colorAttachmentResolveRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        //AddRenderPassAttachment(colorAttachmentResolve, colorAttachmentResolveRef);

        //VkSubpassDescription subpass{};
        //subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        //// The index of the attachment in this array is directly referenced from the fragment shader with the layout(location = 0) out vec4 outColor directive!
        //subpass.colorAttachmentCount = 1;
        //subpass.pColorAttachments = &m_renderPassAttachmentRefs[0];
        //subpass.pDepthStencilAttachment = &m_renderPassAttachmentRefs[1];
        //subpass.pResolveAttachments = &m_renderPassAttachmentRefs[2];
        //AddSubpass(subpass);

        //// Create Dependency ( not used in default config )
        //VkSubpassDependency dependency{};
        //dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        //dependency.dstSubpass = 0;
        //dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        //dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        //dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        //dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        //// AddSubpassDependency(dependency);
    }

    void RenderPass::CreateSceneRenderPassResources()
    {
        //EnableDepthBuffering();
        //EnableMsaa();
        //SetImageColorFormat(VK_FORMAT_R32G32B32A32_SFLOAT);
        //m_msaaSamples = VK_SAMPLE_COUNT_1_BIT;

        //VkAttachmentDescription colorAttachment{};
        //colorAttachment.format = m_colorFormat;
        //colorAttachment.samples = m_msaaSamples;
        //colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        //colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        //colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        //colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        //colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        //colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        //VkAttachmentReference colorAttachmentRef{};
        //colorAttachmentRef.attachment = 0;
        //colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        //AddRenderPassAttachment(colorAttachment, colorAttachmentRef);

        //VkAttachmentDescription depthAttachment{};
        //depthAttachment.format = Helper::FindDepthFormat(DeviceManager::physicalDevice.GetDevice());
        //depthAttachment.samples = m_msaaSamples;
        //depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        //depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        //depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        //depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        //depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        //depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        //VkAttachmentReference depthAttachmentRef{};
        //depthAttachmentRef.attachment = 1;
        //depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        //AddRenderPassAttachment(depthAttachment, depthAttachmentRef);

        //VkAttachmentDescription colorAttachmentResolve{};
        //colorAttachmentResolve.format = m_colorFormat;
        //colorAttachmentResolve.samples = m_msaaSamples;
        //colorAttachmentResolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        //colorAttachmentResolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        //colorAttachmentResolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        //colorAttachmentResolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        //colorAttachmentResolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        //colorAttachmentResolve.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        //VkAttachmentReference colorAttachmentResolveRef{};
        //colorAttachmentResolveRef.attachment = 2;
        //colorAttachmentResolveRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        //AddRenderPassAttachment(colorAttachmentResolve, colorAttachmentResolveRef);

        //// Create Dependency
        //VkSubpassDependency dependency{};
        //dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        //dependency.dstSubpass = 0;
        //dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        //dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        //dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        //dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        //AddSubpassDependency(dependency);

        //VkSubpassDescription subpass{};
        //subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        //subpass.colorAttachmentCount = 1;
        //subpass.pColorAttachments = &GetAttachmentRefs()[0];
        //subpass.pDepthStencilAttachment = &GetAttachmentRefs()[1];
        //subpass.pResolveAttachments = &GetAttachmentRefs()[2];
        //AddSubpass(subpass);
    }


    void RenderPass::AddRenderPassAttachment(VkAttachmentDescription description, VkAttachmentReference reference)
    {
        m_renderPassAttachments.push_back(description);
        m_renderPassAttachmentRefs.push_back(reference);
    }

    std::vector<VkAttachmentReference>& RenderPass::GetAttachmentRefs()
    {
        return m_renderPassAttachmentRefs;
    }

    void RenderPass::AddSubpass(VkSubpassDescription subpass)
    {
        m_subpasses.push_back(subpass);
    }

    void RenderPass::AddSubpassDependency(VkSubpassDependency dependency)
    {
        m_subpassDependencies.push_back(dependency);
    }

    void RenderPass::CreateRenderPass()
    {
        // More info here - https://vulkan-tutorial.com/en/Drawing_a_triangle/Graphics_pipeline_basics/Render_passes

        VkRenderPassCreateInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = (uint32_t)m_renderPassAttachments.size();
        renderPassInfo.pAttachments = m_renderPassAttachments.data();
        renderPassInfo.subpassCount = (uint32_t)m_subpasses.size();
        renderPassInfo.pSubpasses = m_subpasses.data();
        renderPassInfo.dependencyCount = (uint32_t)m_subpassDependencies.size();
        renderPassInfo.pDependencies = m_subpassDependencies.data();

        if (vkCreateRenderPass(DeviceManager::logicalDevice.GetDevice(), &renderPassInfo, nullptr, &m_renderPass) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create render pass!");
        }
    }

    void RenderPass::DestroyRenderPass()
    {
        m_renderPassAttachments.clear();
        m_renderPassAttachmentRefs.clear();
        m_subpasses.clear();
        m_subpassDependencies.clear();
        vkDestroyRenderPass(DeviceManager::logicalDevice.GetDevice(), m_renderPass, nullptr);
    }

    VkRenderPass& RenderPass::GetRenderPass()
    {
        return m_renderPass;
    }

    void RenderPass::ConfigureFrameBufferImageViews(std::vector<VkImageView>& imageViews)
    {
        m_imageViews = imageViews;
    }

    Texture& RenderPass::GetDepthTexture()
    {
        return m_depthTexture;
    }

    void RenderPass::CreateFrameBuffers()
    {
        // More info here - https://vulkan-tutorial.com/en/Drawing_a_triangle/Drawing/Framebuffers

        m_framebuffers.resize(m_imageViews.size());     

        for (size_t i = 0; i < m_imageViews.size(); i++)
        {
            std::vector<VkImageView> attachments = {};
            if (m_b_msaaEnabled)
            {
                attachments.push_back(m_colorImageView);
            }
            if (m_b_depthBuffersEnabled)
            {
                attachments.push_back(m_depthTexture.GetImageViews()[i]);
            }
            attachments.push_back(m_imageViews[i]);

            VkFramebufferCreateInfo framebufferInfo{};
            framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferInfo.renderPass = m_renderPass;            
            framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
            framebufferInfo.pAttachments = attachments.data();
            framebufferInfo.width = RenderWindow::window.GetExtent().width;
            framebufferInfo.height = RenderWindow::window.GetExtent().height;
            framebufferInfo.layers = 1;

            if (vkCreateFramebuffer(DeviceManager::logicalDevice.GetDevice(), &framebufferInfo, nullptr, &m_framebuffers[i]) != VK_SUCCESS)
            {
                throw std::runtime_error("failed to create framebuffer!");
            }
        }
    }

    void RenderPass::DestroyFrameBuffers()
    {
        for (VkFramebuffer framebuffer : m_framebuffers)
        {
            vkDestroyFramebuffer(DeviceManager::logicalDevice.GetDevice(), framebuffer, nullptr);
        }
    }

    void RenderPass::RecreateFrameBuffers()
    {
        DestroyFrameBuffers();        

        if (m_b_msaaEnabled)
        {
            DestroyColorResources();
            CreateColorResources();
        }
        if (m_b_depthBuffersEnabled)
        {
            DestroyDepthResources();
            CreateDepthResources();
        }
        
        CreateFrameBuffers();
    }

    std::vector<VkFramebuffer>& RenderPass::GetFrameBuffers()
    {
        return m_framebuffers;
    }

    void RenderPass::EnableMsaa()
    {
        CreateColorResources();
        m_b_msaaEnabled = true;
    }

    void RenderPass::EnableDepthBuffering()
    {
        CreateDepthResources();
        m_b_depthBuffersEnabled = true;
    }

    void RenderPass::CreateColorResources()
    {
        // Refer to - https://vulkan-tutorial.com/Multisampling
        RenderWindow::window.CreateImage(RenderWindow::window.GetExtent().width, RenderWindow::window.GetExtent().height, 1, m_msaaSamples, m_colorFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_colorImage, m_colorImageMemory);
        RenderWindow::window.CreateImageView(m_colorImageView, m_colorImage, m_colorFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1);
    }

    void RenderPass::DestroyColorResources()
    {
        if (DeviceManager::logicalDevice.GetDevice() != nullptr)
        {
            vkDestroyImageView(DeviceManager::logicalDevice.GetDevice(), m_colorImageView, nullptr);
            vkDestroyImage(DeviceManager::logicalDevice.GetDevice(), m_colorImage, nullptr);
            vkFreeMemory(DeviceManager::logicalDevice.GetDevice(), m_colorImageMemory, nullptr);
        }
    }

    VkSampleCountFlagBits RenderPass::GetMsaa()
    {
        return m_msaaSamples;
    }

    void RenderPass::CreateDepthResources()
    {
        // Refer to - https://vulkan-tutorial.com/en/Depth_buffering
        // and for msaa - https://vulkan-tutorial.com/Multisampling

        VkFormat depthFormat = Helper::FindDepthFormat(DeviceManager::physicalDevice.GetDevice());
        uint32_t singleMipLevel = 1;
        m_depthTexture.SetDescriptorType(VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT);

        std::vector<VkImage>& depthImages = m_depthTexture.GetImages();
        std::vector<VkImageView>& depthImageViews = m_depthTexture.GetImageViews();
        std::vector<VkDeviceMemory>& depthImageMemory = m_depthTexture.GetImageMemory();

        depthImages.resize(VulkanManager::MAX_FRAMES_IN_FLIGHT);
        depthImageViews.resize(VulkanManager::MAX_FRAMES_IN_FLIGHT);
        depthImageMemory.resize(VulkanManager::MAX_FRAMES_IN_FLIGHT);

        for (int i = 0; i < VulkanManager::MAX_FRAMES_IN_FLIGHT; i++)
        {
            RenderWindow::window.CreateImage(RenderWindow::window.GetExtent().width, RenderWindow::window.GetExtent().height, 1, m_msaaSamples, depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, depthImages[i], depthImageMemory[i]);
            RenderWindow::window.CreateImageView(depthImageViews[i], depthImages[i], depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, singleMipLevel);
        }

        RenderWindow::window.CreateTextureSampler(m_depthTexture.GetSampler(), m_depthTexture.GetMipLevels());
    }

    void RenderPass::DestroyDepthResources()
    {
        if (DeviceManager::logicalDevice.GetDevice() != nullptr)
        {
            std::vector<VkImage>& depthImages = m_depthTexture.GetImages();
            std::vector<VkImageView>& depthImageViews = m_depthTexture.GetImageViews();
            std::vector<VkDeviceMemory>& depthImageMemory = m_depthTexture.GetImageMemory();

            depthImages.resize(VulkanManager::MAX_FRAMES_IN_FLIGHT);
            depthImageViews.resize(VulkanManager::MAX_FRAMES_IN_FLIGHT);
            depthImageMemory.resize(VulkanManager::MAX_FRAMES_IN_FLIGHT);

            for (int i = 0; i < VulkanManager::MAX_FRAMES_IN_FLIGHT; i++)
            {
                vkDestroyImageView(DeviceManager::logicalDevice.GetDevice(), depthImageViews[i], nullptr);
                vkDestroyImage(DeviceManager::logicalDevice.GetDevice(), depthImages[i], nullptr);
                vkFreeMemory(DeviceManager::logicalDevice.GetDevice(), depthImageMemory[i], nullptr);
            }
        }
    }

    void RenderPass::BeginRenderPass(uint32_t imageIndex)
    {
        // Reset to make sure it is able to be recorded
        vkResetCommandBuffer(m_commandBuffers[VulkanManager::currentFrame], 0);

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = 0;
        beginInfo.pInheritanceInfo = nullptr;

        if (vkBeginCommandBuffer(m_commandBuffers[VulkanManager::currentFrame], &m_beginInfo) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to begin recording command buffer!");
        }

        std::vector<VkClearValue> clearValues;
        VkClearValue clearColor;
        Vector4 clear = Assets::assetManager.GetColor("clearColor");
        clearColor.color = { {clear.x, clear.y, clear.z, clear.w} };
        clearValues.push_back(clearColor);
        if (m_b_depthBuffersEnabled)
        {
            VkClearValue depth;
            depth.depthStencil = { 1.0f, 0 };
            clearValues.push_back(depth);
        }
        if (m_b_msaaEnabled)
        {
            VkClearValue msaa;
            msaa.color = { {0.0f, 0.0f, 0.0f, 0.0f} };
            clearValues.push_back(msaa);
        }

        m_renderPassInfo.framebuffer = m_framebuffers[VulkanManager::currentFrame];
        m_renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        m_renderPassInfo.pClearValues = clearValues.data();
        m_renderPassInfo.renderArea.extent = RenderWindow::window.GetExtent();
        m_renderPassInfo.renderPass = m_renderPass;

        // we specified viewport and scissor state for this pipeline to be dynamic, so we need to set them in the command buffer before issuing our draw command:
        m_viewport.width = static_cast<float>(RenderWindow::window.GetExtent().width);
        m_viewport.height = static_cast<float>(RenderWindow::window.GetExtent().height);
        vkCmdSetViewport(m_commandBuffers[VulkanManager::currentFrame], 0, 1, &m_viewport);
        
        m_scissor.extent = RenderWindow::window.GetExtent();
        vkCmdSetScissor(m_commandBuffers[VulkanManager::currentFrame], 0, 1, &m_scissor);

        vkCmdBeginRenderPass(m_commandBuffers[VulkanManager::currentFrame], &m_renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    }

    void RenderPass::EndRenderPass()
    {
        // End render pass
        vkCmdEndRenderPass(m_commandBuffers[VulkanManager::currentFrame]);
     
        // Finish recording the command buffer
        if (vkEndCommandBuffer(m_commandBuffers[VulkanManager::currentFrame]) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to record command buffer!");
        }
    }

    void RenderPass::SetImageColorFormat(VkFormat colorFormat)
    {
        m_colorFormat = colorFormat;
    }

    void RenderPass::RecordCommandBuffer(GraphicsPipeline& graphicsPipeline)
    {        
        VkPipeline& pipeline = graphicsPipeline.GetGraphicsPipeline();
        VkPipelineLayout& pipelineLayout = graphicsPipeline.GetPipelineLayout();
        
        PushConstants pushConstants;
        int lightCounter = 0;

        for (Light& light : SceneManager::loadedScene.GetAll<Light>().GetAll())
        {
            Vector3 dir = light.GetDirection();
            Vector4 color = light.GetColor();

            if (lightCounter == 0)
            {
                pushConstants.light1Direction = glm::normalize(glm::vec4(dir.x, dir.y, dir.z, 1));
                pushConstants.light1Color = glm::vec4(color.x, color.y, color.z, color.w);
            }
            else if (lightCounter == 1)
            {
                pushConstants.light2Direction = glm::normalize(glm::vec4(dir.x, dir.y, dir.z, 1));
                pushConstants.light2Color = glm::vec4(color.x, color.y, color.z, color.w);
            }
            else if (lightCounter == 2)
            {
                pushConstants.light3Direction = glm::normalize(glm::vec4(dir.x, dir.y, dir.z, 1));
                pushConstants.light3Color = glm::vec4(color.x, color.y, color.z, color.w);
            }
            else if (lightCounter == 3)
            {
                pushConstants.light4Direction = glm::normalize(glm::vec4(dir.x, dir.y, dir.z, 1));
                pushConstants.light4Color = glm::vec4(color.x, color.y, color.z, color.w);
            }

            lightCounter++;
        }        

        uint32_t pushOffset = 0;
        uint32_t pushSize = sizeof(PushConstants);

        vkCmdPushConstants(m_commandBuffers[VulkanManager::currentFrame], pipelineLayout, VK_SHADER_STAGE_FRAGMENT_BIT, pushOffset, pushSize, &pushConstants);

        vkCmdBindPipeline(m_commandBuffers[VulkanManager::currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
    }

    // TODO: iron out dynamic push constants
    void RenderPass::RecordCommandBuffer(VkPipelineLayout pipelineLayout, std::vector<uint32_t>& pushConstOffsets, std::vector<uint32_t>& pushConstSizes, std::vector<const void*>& pushValues)
    {
        for (int i = 0; i < pushValues.size(); i++)
        {
            vkCmdPushConstants(m_commandBuffers[VulkanManager::currentFrame], pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, pushConstOffsets[i], pushConstSizes[i], &pushValues[i]);
        }
    }

    void RenderPass::BindIndexed(std::shared_ptr<Model> model)
    {
        VkBuffer& vertexBuffer = model->GetVertexBuffer();
        VkBuffer& indexBuffer = model->GetIndexBuffer();
        VkBuffer vertexBuffers[] = { vertexBuffer };
        VkDeviceSize offsets[] = { 0 };

        vkCmdBindVertexBuffers(m_commandBuffers[VulkanManager::currentFrame], 0, 1, vertexBuffers, offsets);
        vkCmdBindIndexBuffer(m_commandBuffers[VulkanManager::currentFrame], indexBuffer, 0, VK_INDEX_TYPE_UINT32);
    }

    void RenderPass::BindDescriptorSets(VkDescriptorSet& descriptorSet, std::shared_ptr<Material> material, ViewportType viewportType)
    {
        VkPipelineLayout& pipelineLayout = material->GetPipelineLayout();        

        vkCmdBindDescriptorSets(m_commandBuffers[VulkanManager::currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSet, 0, nullptr);
    }

    void RenderPass::DrawIndexed(std::shared_ptr<Model> model)
    {
        std::vector<uint32_t> indices = model->GetIndices();

        // Refer to - https://vulkan-tutorial.com/en/Vertex_buffers/Index_buffer
        vkCmdDrawIndexed(m_commandBuffers[VulkanManager::currentFrame], static_cast<uint32_t>(indices.size()), 1, 0, 0, 0); // reusing vertices with index buffers.
        // NOTE FROM THE WIKI: The previous chapter already mentioned that you should allocate multiple resources like buffers from a single memory allocation, but in fact you should go a step further. Driver developers recommend that you also store multiple buffers, like the vertex and index buffer, into a single VkBuffer and use offsets in commands like vkCmdBindVertexBuffers. The advantage is that your data is more cache friendly in that case, because it's closer together. It is even possible to reuse the same chunk of memory for multiple resources if they are not used during the same render operations, provided that their data is refreshed, of course. This is known as aliasing and some Vulkan functions have explicit flags to specify that you want to do this.
    }



    void RenderPass::CreateCommandBuffers()
    {
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = *m_commandPool;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = VulkanManager::MAX_FRAMES_IN_FLIGHT;

        m_commandBuffers.resize(VulkanManager::MAX_FRAMES_IN_FLIGHT);

        if (vkAllocateCommandBuffers(DeviceManager::logicalDevice.GetDevice(), &allocInfo, m_commandBuffers.data()) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to allocate command buffers!");
        }
    }

    void RenderPass::DestroyCommandBuffers()
    {

    }

    std::vector<VkCommandBuffer>& RenderPass::GetCommandBuffers()
    {
        return m_commandBuffers;
    }

    void RenderPass::SetMSAASampleCount(VkSampleCountFlagBits sampleCount)
    {
        m_msaaSamples = sampleCount;
    }
}
