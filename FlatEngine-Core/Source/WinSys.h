#pragma once
#include "PhysicalDevice.h"
#include "LogicalDevice.h"

#include "SDL.h"
#include <glm.hpp>

#include <vector>
#include <string>


namespace FlatEngine
{
	class WinSys
	{
	public:
		WinSys();
		~WinSys();
		void CleanupSystem();
		void CleanupDrawingResources();

		void SetHandles(VkInstance* instance, PhysicalDevice* physicalDevice, LogicalDevice* logicalDevice, VkCommandPool* commandPool);
		bool CreateSDLWindow(std::string windowTitle, int windowWidth, int windowHeight);
		SDL_Window* GetWindow();
		void DestroyWindow();
		void CreateSurface();
		void DestroySurface();
		void CreateDrawingResources();
		void CreateSwapChain();
		void DestroySwapChain();
		void RecreateSwapChain();
		void CreateImageViews();
		void DestroyImageViews();

		VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
		VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
		VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
		VkSurfaceKHR& GetSurface();
		VkSurfaceFormatKHR GetSurfaceFormat();
		VkPresentModeKHR GetPresentMode();
		std::vector<VkImageView>& GetSwapChainImageViews();
		VkFormat GetImageFormat();
		VkExtent2D GetExtent();
		VkSwapchainKHR& GetSwapChain();

		void CreateImage(uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
		void CreateImageView(VkImageView& imageView, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels);
		void CreateTextureSampler(VkSampler& textureSampler, uint32_t mipLevels);
		VkImage CreateTextureImage(std::string path, uint32_t mipLevels, VkDeviceMemory textureImageMemory);
		void GenerateMipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels);
		void TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels);
		void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
		void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
		void CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
		void InsertImageMemoryBarrier(VkCommandBuffer commandBuffer, VkImage image, VkAccessFlags srcAccessMask, VkAccessFlags dstAccessMask, VkImageLayout oldImageLayout, VkImageLayout newImageLayout, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, VkImageSubresourceRange subresourceRange);
		void ResizeWindow(int width, int height);
		void SetFullscreen(bool b_isFullscreen);

		bool m_b_framebufferResized;

	private:	
		SDL_Window* m_window;
		std::string m_title;
		int m_windowWidth;
		int m_windowHeight;
		bool m_b_isFullscreen;

		VkSurfaceKHR m_surface;
		VkSurfaceFormatKHR m_surfaceFormat;
		VkPresentModeKHR m_presentMode;
		VkSwapchainKHR m_swapChain;
		VkFormat m_swapChainImageFormat;
		VkExtent2D m_swapChainExtent;
		std::vector<VkImage> m_swapChainImages;
		std::vector<VkImageView> m_swapChainImageViews;
		// handles
		VkInstance* m_instance;
		PhysicalDevice* m_physicalDevice;
		LogicalDevice* m_logicalDevice;
		VkCommandPool* m_commandPool;
	};
}

