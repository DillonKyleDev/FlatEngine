#pragma once
#include "PhysicalDevice.h"
#include "LogicalDevice.h"
#include "WinSys.h"

//#ifndef GLFW_INCLUDE_VULKAN
//#define GLFW_INCLUDE_VULKAN
//#endif
//#include <glfw3.h>

#include <vector>


namespace FlatEngine
{
	class Mesh;

	class RenderPass
	{
	public:
		RenderPass();
		~RenderPass();
		void Cleanup(LogicalDevice& logicalDevice);

		void SetHandles(VkInstance instance, WinSys& winSystem, PhysicalDevice& physicalDevice, LogicalDevice& logicalDevice);
		void Init(VkCommandPool commandPool);
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
		void CreateFrameBuffers();
		void DestroyFrameBuffers();
		void RecreateFrameBuffers();
		std::vector<VkFramebuffer>& GetFrameBuffers();
		void EnableMsaa();
		void EnableDepthBuffering();
		VkSampleCountFlagBits GetMsaa();
		void CreateCommandBuffers(VkCommandPool commandPool);
		void DestroyCommandBuffers();
		std::vector<VkCommandBuffer>& GetCommandBuffers();
		void RecordCommandBuffer(uint32_t imageIndex, Mesh& mesh);
		void DrawIndexed(Mesh& mesh);
		void BeginRenderPass(uint32_t imageIndex);
		void EndRenderPass();

		void SetImageColorFormat(VkFormat colorFormat);

	private:
		void CreateColorResources();
		void DestroyColorResources();
		void CreateDepthResources();
		void DestroyDepthResources();

		VkRenderPass m_renderPass;
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
		VkImage m_depthImage;
		VkDeviceMemory m_depthImageMemory;
		VkImageView m_depthImageView;
		bool m_b_depthBuffersEnabled;
		// handles
		VkInstance m_instanceHandle;
		WinSys* m_winSystem;
		PhysicalDevice* m_physicalDeviceHandle;
		LogicalDevice* m_deviceHandle;
	};
}


