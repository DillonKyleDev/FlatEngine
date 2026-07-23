#pragma once
#include "render/Structs.h"
#include "render/Texture.h"

#include <vector>
#include <memory>


namespace FlatEngine
{
	class Model;
	class GraphicsPipeline;
	class Material;	

	class RenderPass
	{
	public:
		RenderPass();		
		void Cleanup();

		void SetHandles(VkCommandPool* commandPool);
		void Init();
		bool Initialized();
		void SetDefaultRenderPassConfig();
		void CreateSceneRenderPassResources();
		void AddRenderPassAttachment(VkAttachmentDescription description, VkAttachmentReference reference);
		std::vector<VkAttachmentReference>& GetAttachmentRefs();
		void AddSubpass(VkSubpassDescription subpass);
		void AddSubpassDependency(VkSubpassDependency dependency);
		void CreateRenderPass();
		void DestroyRenderPass();
		VkRenderPass& GetRenderPass();
		void ConfigureFrameBufferImageViews(std::vector<VkImageView>& imageViews);
		Texture& GetDepthTexture();		
		void CreateFrameBuffers();
		void DestroyFrameBuffers();
		void RecreateFrameBuffers();
		std::vector<VkFramebuffer>& GetFrameBuffers();
		void EnableMsaa();
		void EnableDepthBuffering();
		VkSampleCountFlagBits GetMsaa();
		void CreateCommandBuffers();
		void DestroyCommandBuffers();
		std::vector<VkCommandBuffer>& GetCommandBuffers();
		void RecordCommandBuffer(GraphicsPipeline& graphicsPipeline);
		void RecordCommandBuffer(VkPipelineLayout pipelineLayout, std::vector<uint32_t>& m_pushConstOffsets, std::vector<uint32_t>& m_pushConstSizes, std::vector<const void*>& m_pushValues);
		void BindIndexed(std::shared_ptr<Model> model);
		void BindDescriptorSets(VkDescriptorSet& descriptorSet, std::shared_ptr<Material> material, ViewportType viewportType);
		void DrawIndexed(std::shared_ptr<Model> model);
		void BeginRenderPass(uint32_t imageIndex);
		void EndRenderPass();

		void SetImageColorFormat(VkFormat colorFormat);		
		void SetMSAASampleCount(VkSampleCountFlagBits sampleCount);

	private:
		void CreateColorResources();
		void DestroyColorResources();
		void CreateDepthResources();
		void DestroyDepthResources();

		VkRenderPass m_renderPass;
		bool m_b_initialized;
		std::vector<VkFramebuffer> m_framebuffers;
		std::vector<VkCommandBuffer> m_commandBuffers;
		std::vector<VkImageView> m_imageViews;
		std::vector<VkAttachmentDescription> m_renderPassAttachments;
		std::vector<VkAttachmentReference> m_renderPassAttachmentRefs;
		std::vector<VkSubpassDescription> m_subpasses;
		std::vector<VkSubpassDependency> m_subpassDependencies;
		bool m_b_defaultRenderPassConfig;
		VkFormat m_colorFormat;
		VkImageUsageFlags m_imageUsageFlags;
		// antialiasing
		VkSampleCountFlagBits m_msaaSamples;
		VkImage m_colorImage;
		VkDeviceMemory m_colorImageMemory;
		VkImageView m_colorImageView;
		bool m_b_msaaEnabled;
		// depth buffers
		Texture m_depthTexture;
		bool m_b_depthBuffersEnabled;
		// handles
		VkCommandPool* m_commandPool;
		
		VkCommandBufferBeginInfo m_beginInfo{};
		VkRenderPassBeginInfo m_renderPassInfo{};
		VkViewport m_viewport{};
		VkRect2D m_scissor{};
	};
}


