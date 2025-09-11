#include "WinSys.h"
#include "VulkanManager.h"
#include "Helper.h"
#include "Structs.h"
#include "FlatEngine.h"

#include "SDL_vulkan.h"

#define STB_IMAGE_IMPLEMENTATION // Image loading
#include "stb_image.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE // Depth buffer helper - might not need??

#include <array>
#include <algorithm> // Necessary for std::clamp
#include <stdexcept>


namespace FlatEngine
{
    WinSys::WinSys()
    {
        m_window = nullptr;
        m_title = "";
        m_windowWidth = 900;
        m_windowHeight = 600;
        m_b_isFullscreen = false;
        m_surface = VK_NULL_HANDLE;
        m_surfaceFormat = {};
        m_presentMode = {};
        m_swapChain = VK_NULL_HANDLE;
        m_swapChainImageFormat = VkFormat();
        m_swapChainExtent = VkExtent2D();
        m_swapChainImages = std::vector<VkImage>();
        m_swapChainImageViews = std::vector<VkImageView>();
        m_b_framebufferResized = false;
        // handles
        m_instanceHandle = VK_NULL_HANDLE;
        m_physicalDeviceHandle = VK_NULL_HANDLE;
        m_deviceHandle = VK_NULL_HANDLE;
    }

    WinSys::~WinSys()
    {
        //CleanupDrawingResources();
        //CleanupSystem();
    }

    void WinSys::CleanupSystem()
    {
        //Destroy window
        DestroyWindow();
        //DestroySurface();        
    }

    void WinSys::CleanupDrawingResources()
    {
        DestroyImageViews(); // Not required but explicit
        DestroySwapChain();
    }


    void WinSys::SetHandles(VkInstance instance, PhysicalDevice& physicalDevice, LogicalDevice& logicalDevice)
    {
        m_instanceHandle = instance;
        m_physicalDeviceHandle = &physicalDevice;
        m_deviceHandle = &logicalDevice;
    }

    bool WinSys::CreateSDLWindow(std::string windowTitle, int windowWidth, int windowHeight)
    {
        bool b_success = true;

        m_windowWidth = windowWidth;
        m_windowHeight = windowHeight;

        if (m_window != NULL)
        {
            SDL_DestroyWindow(m_window);
        }

        // Create window with SDL_Renderer graphics context
        SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_VULKAN);
        m_window = SDL_CreateWindow("FlatEngine", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, m_windowWidth, m_windowHeight, window_flags);

        if (m_window == NULL)
        {
            printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
            b_success = false;
        }

        if (m_b_isFullscreen)
        {
            SDL_SetWindowFullscreen(m_window, SDL_WINDOW_FULLSCREEN_DESKTOP);
        }
        else
        {
            SDL_SetWindowFullscreen(m_window, 0);
        }

        return b_success;
    }

    SDL_Window* WinSys::GetWindow()
    {
        return m_window;
    }

    void WinSys::DestroyWindow()
    {
        SDL_DestroyWindow(m_window);
        m_window = NULL;
    }

    void WinSys::CreateSurface()
    {
        SDL_Vulkan_CreateSurface(GetWindow(), m_instanceHandle, &m_surface);
    }

    void WinSys::DestroySurface()
    {
        if (m_deviceHandle != VK_NULL_HANDLE)
        {
            vkDestroySurfaceKHR(m_instanceHandle, m_surface, nullptr);
        }
    }

    void WinSys::SetFullscreen(bool b_isFullscreen)
    {
        m_b_isFullscreen = b_isFullscreen;

        if (m_b_isFullscreen)
        {
            SDL_SetWindowFullscreen(m_window, SDL_WINDOW_FULLSCREEN_DESKTOP);
        }
        else
        {
            SDL_SetWindowFullscreen(m_window, 0);
        }
        //SDL_ShowCursor(true);
    }

    void WinSys::ResizeWindow(int width, int height)
    {
        m_windowWidth = width;
        m_windowHeight = height;
        SDL_SetWindowSize(m_window, m_windowWidth, m_windowHeight);
        SDL_SetWindowPosition(m_window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    }

    void WinSys::CreateDrawingResources()
    {
        CreateSwapChain();
        CreateImageViews();
    }

    void WinSys::CreateSwapChain()
    {
        SwapChainSupportDetails swapChainSupport = Helper::QuerySwapChainSupport(m_physicalDeviceHandle->GetDevice(), m_surface);

        m_surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupport.formats);
        m_presentMode = ChooseSwapPresentMode(swapChainSupport.presentModes);
        VkExtent2D extent = ChooseSwapExtent(swapChainSupport.capabilities);

        VM_imageCount = swapChainSupport.capabilities.minImageCount;
        if (VM_imageCount + 1 <= swapChainSupport.capabilities.maxImageCount || swapChainSupport.capabilities.maxImageCount == 0)
        {
            //VM_imageCount += 1;
        }

        if (VM_MAX_FRAMES_IN_FLIGHT <= swapChainSupport.capabilities.maxImageCount || swapChainSupport.capabilities.maxImageCount == 0)
        {
            VM_imageCount = VM_MAX_FRAMES_IN_FLIGHT;
        }

        VkSwapchainCreateInfoKHR createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = m_surface;
        createInfo.minImageCount = VM_imageCount;
        createInfo.imageFormat = m_surfaceFormat.format;
        createInfo.imageColorSpace = m_surfaceFormat.colorSpace;
        createInfo.imageExtent = extent;
        createInfo.imageArrayLayers = 1; // Specifies the amount of layers each image consists of
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT; // VK_IMAGE_USAGE_TRANSFER_DST_BIT // Specifies what kind of operations we'll use the images in the swap chain for.
        // ^^ NOTE FROM WIKI FOR ABOVE: It is also possible that you'll render images to a separate image first to perform operations like post-processing. In that case you may use a value like VK_IMAGE_USAGE_TRANSFER_DST_BIT instead and use a memory operation to transfer the rendered image to a swap chain image.

        QueueFamilyIndices indices = Helper::FindQueueFamilies(m_physicalDeviceHandle->GetDevice(), m_surface);
        uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

        if (indices.graphicsFamily != indices.presentFamily) // If the queue families are not the same, use concurrent sharing mode (more lenient, less performant)
        {
            createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices = queueFamilyIndices;
        }
        else  // If the queue families are the same, use exclusive mode (best performance)
        {
            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            createInfo.queueFamilyIndexCount = 0;
            createInfo.pQueueFamilyIndices = nullptr;
        }

        createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        createInfo.presentMode = m_presentMode;
        createInfo.clipped = VK_TRUE;
        createInfo.oldSwapchain = VK_NULL_HANDLE;

        if (vkCreateSwapchainKHR(m_deviceHandle->GetDevice(), &createInfo, nullptr, &m_swapChain) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create swapchain.");
        }
        else
        {
            m_swapChainImageFormat = m_surfaceFormat.format;
            m_swapChainExtent = extent;

            vkGetSwapchainImagesKHR(m_deviceHandle->GetDevice(), m_swapChain, &VM_imageCount, nullptr);
            m_swapChainImages.resize(VM_imageCount);
            vkGetSwapchainImagesKHR(m_deviceHandle->GetDevice(), m_swapChain, &VM_imageCount, m_swapChainImages.data());
        }
    }

    void WinSys::DestroySwapChain()
    {
        if (m_deviceHandle != VK_NULL_HANDLE)
        {
            vkDestroySwapchainKHR(m_deviceHandle->GetDevice(), m_swapChain, nullptr);
        }
    }

    void WinSys::RecreateSwapChain()
    {
        int width = 0, height = 0;
        while (width == 0 || height == 0)
        {
            SDL_GetWindowSize(m_window, &width, &height);
        }

        vkDeviceWaitIdle(m_deviceHandle->GetDevice());

        CleanupDrawingResources();
        CreateDrawingResources();
    }

    void WinSys::CreateImageViews()
    {
        // Get number of swapChainImages and use it to set the swapChainImageViews
        m_swapChainImageViews.resize(m_swapChainImages.size());

        for (size_t i = 0; i < m_swapChainImages.size(); i++)
        {
            uint32_t singleMipLevel = 1;
            CreateImageView(m_swapChainImageViews[i], m_swapChainImages[i], m_swapChainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT, singleMipLevel, *m_deviceHandle);
        }
    }

    void WinSys::DestroyImageViews()
    {
        for (auto imageView : m_swapChainImageViews)
        {
            vkDestroyImageView(m_deviceHandle->GetDevice(), imageView, nullptr);
        }
    }

    VkSurfaceFormatKHR WinSys::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
    {
        // Refer to https://vulkan-tutorial.com/en/Drawing_a_triangle/Presentation/Swap_chain for colorSpace info
        for (const auto& availableFormat : availableFormats)
        {
            // These color formats are pretty much the standard for image rendering
            if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            {
                return availableFormat;
            }
        }

        // If the one we want isn't available, just go with the first available colorSpace (easy)
        return availableFormats[0];
    }

    VkPresentModeKHR WinSys::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
    {
        // Refer to https://vulkan-tutorial.com/en/Drawing_a_triangle/Presentation/Swap_chain for colorSpace info
        for (const auto& availablePresentMode : availablePresentModes)
        {
            if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
            {
                return availablePresentMode;
            }
        }

        // If the one we want isn't available, this mode is guaranteed to be available so we'll choose to send that one instead
        return VK_PRESENT_MODE_FIFO_KHR;
    }

    VkExtent2D WinSys::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
    {
        if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
        {
            return capabilities.currentExtent;
        }
        else
        {
            // We must calculate width and height ourselves
            int width;
            int height;                    
            SDL_GetWindowSize(m_window, &width, &height);

            VkExtent2D actualExtent = {
                static_cast<uint32_t>(width),
                static_cast<uint32_t>(height)
            };

            // NOTE FROM WIKI: The clamp function is used here to bound the values of width and height between the allowed minimum and maximum extents that are supported by the implementation.
            actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
            actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

            return actualExtent;
        }
    }

    VkSurfaceKHR& WinSys::GetSurface()
    {
        return m_surface;
    }

    VkSurfaceFormatKHR WinSys::GetSurfaceFormat()
    {
        return m_surfaceFormat;
    }

    VkPresentModeKHR WinSys::GetPresentMode()
    {
        return m_presentMode;
    }

    std::vector<VkImageView>& WinSys::GetSwapChainImageViews()
    {
        return m_swapChainImageViews;
    }

    VkFormat WinSys::GetImageFormat()
    {
        return m_swapChainImageFormat;
    }

    VkExtent2D WinSys::GetExtent()
    {
        return m_swapChainExtent;
    }

    VkSwapchainKHR& WinSys::GetSwapChain()
    {
        return m_swapChain;
    }


    // statics
    void WinSys::CreateImage(uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory, PhysicalDevice& physicalDevice, LogicalDevice& logicalDevice)
    {
        VkImageCreateInfo imageInfo{};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.extent.width = width;
        imageInfo.extent.height = height;
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = mipLevels;
        imageInfo.arrayLayers = 1;
        imageInfo.format = format;
        imageInfo.tiling = tiling;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.usage = usage;
        imageInfo.samples = numSamples;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        if (vkCreateImage(logicalDevice.GetDevice(), &imageInfo, nullptr, &image) != VK_SUCCESS) {
            throw std::runtime_error("failed to create image!");
        }

        VkMemoryRequirements memRequirements{};
        vkGetImageMemoryRequirements(logicalDevice.GetDevice(), image, &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = physicalDevice.FindMemoryType(memRequirements.memoryTypeBits, properties);

        if (vkAllocateMemory(logicalDevice.GetDevice(), &allocInfo, nullptr, &imageMemory) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to allocate image memory!");
        }

        vkBindImageMemory(logicalDevice.GetDevice(), image, imageMemory, 0);
    }

    void WinSys::CreateImageView(VkImageView& imageView, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels, LogicalDevice& logicalDevice)
    {
        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = image;
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = format;
        viewInfo.subresourceRange.aspectMask = aspectFlags;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = mipLevels;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;

        if (vkCreateImageView(logicalDevice.GetDevice(), &viewInfo, nullptr, &imageView) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create texture image view!");
        }
    }

    void WinSys::CreateTextureSampler(VkSampler& textureSampler, uint32_t mipLevels, PhysicalDevice& physicalDevice, LogicalDevice& logicalDevice)
    {
        // Refer to - https://vulkan-tutorial.com/en/Texture_mapping/Image_view_and_sampler

        VkSamplerCreateInfo samplerInfo{};
        samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerInfo.magFilter = VK_FILTER_NEAREST; // Change this to bilinear for non pixel art
        samplerInfo.minFilter = VK_FILTER_NEAREST;
        samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

        VkPhysicalDeviceProperties properties{};
        vkGetPhysicalDeviceProperties(physicalDevice.GetDevice(), &properties);

        samplerInfo.anisotropyEnable = VK_TRUE;
        samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
        samplerInfo.borderColor = VK_BORDER_COLOR_INT_TRANSPARENT_BLACK; // VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        samplerInfo.unnormalizedCoordinates = VK_FALSE;
        samplerInfo.compareEnable = VK_FALSE;
        samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
        samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        samplerInfo.minLod = 0.0f; // Optional
        samplerInfo.maxLod = static_cast<float>(mipLevels);
        samplerInfo.mipLodBias = 0.0f; // Optional        

        if (vkCreateSampler(logicalDevice.GetDevice(), &samplerInfo, nullptr, &textureSampler) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create texture sampler!");
        }
    }

    VkImage WinSys::CreateTextureImage(std::string path, uint32_t mipLevels, VkCommandPool commandPool, PhysicalDevice& physicalDevice, LogicalDevice& logicalDevice, VkDeviceMemory textureImageMemory)
    {
        // Refer to - https://vulkan-tutorial.com/en/Texture_mapping/Images
        // And refer to - https://vulkan-tutorial.com/en/Generating_Mipmaps

        VkImage newImage = VK_NULL_HANDLE;

        int texWidth, texHeight, texChannels;
        stbi_uc* pixels = stbi_load(path.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
        VkDeviceSize imageSize = texWidth * texHeight * 4;

        if (!pixels)
        {
            throw std::runtime_error("failed to load texture image: " + path);
        }

        mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(texWidth, texHeight)))) + 1;

        VkBuffer stagingBuffer{};
        VkDeviceMemory stagingBufferMemory{};
        CreateBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory, physicalDevice, logicalDevice);

        void* data{};
        vkMapMemory(logicalDevice.GetDevice(), stagingBufferMemory, 0, imageSize, 0, &data);
        memcpy(data, pixels, static_cast<size_t>(imageSize));
        vkUnmapMemory(logicalDevice.GetDevice(), stagingBufferMemory);

        // Cleanup pixel array
        stbi_image_free(pixels);

        CreateImage(texWidth, texHeight, mipLevels, VK_SAMPLE_COUNT_1_BIT, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, newImage, textureImageMemory, physicalDevice, logicalDevice);

        TransitionImageLayout(newImage, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, mipLevels, commandPool, logicalDevice);
        CopyBufferToImage(stagingBuffer, newImage, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight), commandPool, logicalDevice);
        GenerateMipmaps(newImage, VK_FORMAT_R8G8B8A8_UNORM, texWidth, texHeight, mipLevels, commandPool, physicalDevice, logicalDevice);

        vkDestroyBuffer(logicalDevice.GetDevice(), stagingBuffer, nullptr);
        vkFreeMemory(logicalDevice.GetDevice(), stagingBufferMemory, nullptr);

        return newImage;
    }

    void WinSys::GenerateMipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels, VkCommandPool commandPool, PhysicalDevice& physicalDevice, LogicalDevice& logicalDevice)
    {
        // Refer to - https://vulkan-tutorial.com/en/Generating_Mipmaps

        // Check if image format supports linear blitting
        VkFormatProperties formatProperties;
        vkGetPhysicalDeviceFormatProperties(physicalDevice.GetDevice(), imageFormat, &formatProperties);
        if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT))
        {
            throw std::runtime_error("texture image format does not support linear blitting!");
        }

        VkCommandBuffer commandBuffer = Helper::BeginSingleTimeCommands(commandPool, logicalDevice);

        VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.image = image;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;
        barrier.subresourceRange.levelCount = 1;

        int32_t mipWidth = texWidth;
        int32_t mipHeight = texHeight;

        for (uint32_t i = 1; i < mipLevels; i++)
        {
            barrier.subresourceRange.baseMipLevel = i - 1;
            barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

            vkCmdPipelineBarrier(commandBuffer,
                VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
                0, nullptr,
                0, nullptr,
                1, &barrier);

            VkImageBlit blit{};
            blit.srcOffsets[0] = { 0, 0, 0 };
            blit.srcOffsets[1] = { mipWidth, mipHeight, 1 };
            blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            blit.srcSubresource.mipLevel = i - 1;
            blit.srcSubresource.baseArrayLayer = 0;
            blit.srcSubresource.layerCount = 1;
            blit.dstOffsets[0] = { 0, 0, 0 };
            blit.dstOffsets[1] = { mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1 };
            blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            blit.dstSubresource.mipLevel = i;
            blit.dstSubresource.baseArrayLayer = 0;
            blit.dstSubresource.layerCount = 1;

            vkCmdBlitImage(commandBuffer,
                image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                1, &blit,
                VK_FILTER_LINEAR);

            barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            vkCmdPipelineBarrier(commandBuffer,
                VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
                0, nullptr,
                0, nullptr,
                1, &barrier);

            if (mipWidth > 1) mipWidth /= 2;
            if (mipHeight > 1) mipHeight /= 2;
        }

        barrier.subresourceRange.baseMipLevel = mipLevels - 1;
        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        // Because the last mip level was never transitioned by the above loop
        vkCmdPipelineBarrier(commandBuffer,
            VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
            0, nullptr,
            0, nullptr,
            1, &barrier);

        Helper::EndSingleTimeCommands(commandPool, commandBuffer, logicalDevice);
    }

    void WinSys::TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels, VkCommandPool commandPool, LogicalDevice& logicalDevice)
    {
        // Refer to - https://vulkan-tutorial.com/en/Texture_mapping/Images

        VkCommandBuffer commandBuffer = Helper::BeginSingleTimeCommands(commandPool, logicalDevice);

        VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.pNext = 0;
        barrier.oldLayout = oldLayout;
        barrier.newLayout = newLayout;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = image;
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = mipLevels;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;

        VkPipelineStageFlags sourceStage{};
        VkPipelineStageFlags destinationStage{};

        if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) 
        {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        }
        else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) 
        {
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
            sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        }
        else 
        {
            throw std::invalid_argument("unsupported layout transition!");
        }

        vkCmdPipelineBarrier(commandBuffer, sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);

        Helper::EndSingleTimeCommands(commandPool, commandBuffer, logicalDevice);
    }

    void WinSys::CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory, PhysicalDevice& physicalDevice, LogicalDevice& logicalDevice)
    {
        // Refer to - https://vulkan-tutorial.com/en/Vertex_buffers/Vertex_buffer_creation
        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = size;
        bufferInfo.usage = usage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        if (vkCreateBuffer(logicalDevice.GetDevice(), &bufferInfo, nullptr, &buffer) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create vertex buffer!");
        }

        VkMemoryRequirements memRequirements{};
        vkGetBufferMemoryRequirements(logicalDevice.GetDevice(), buffer, &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = physicalDevice.FindMemoryType(memRequirements.memoryTypeBits, properties);

        // NOTE FROM THE WIKI: It should be noted that in a real world application, you're not supposed to actually call vkAllocateMemory for every individual buffer. The maximum number of simultaneous memory allocations is limited by the maxMemoryAllocationCount physical device limit
        if (vkAllocateMemory(logicalDevice.GetDevice(), &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to allocate vertex buffer memory!");
        }

        vkBindBufferMemory(logicalDevice.GetDevice(), buffer, bufferMemory, 0);
    }

    void WinSys::CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, VkCommandPool commandPool, LogicalDevice& logicalDevice)
    {
        VkCommandBuffer commandBuffer = Helper::BeginSingleTimeCommands(commandPool, logicalDevice);

        // We're only going to use the command buffer once and wait with returning from the function until the copy operation has finished executing. It's good practice to tell the driver about our intent using 
        VkBufferCopy copyRegion{};
        copyRegion.size = size;
        vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

        Helper::EndSingleTimeCommands(commandPool, commandBuffer, logicalDevice);
    }

    void WinSys::CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, VkCommandPool commandPool, LogicalDevice& logicalDevice)
    {
        VkCommandBuffer commandBuffer = Helper::BeginSingleTimeCommands(commandPool, logicalDevice);

        VkBufferImageCopy region{};
        region.bufferOffset = 0;
        region.bufferRowLength = 0;
        region.bufferImageHeight = 0;

        region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.imageSubresource.mipLevel = 0;
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount = 1;

        region.imageOffset = { 0, 0, 0 };
        region.imageExtent = {
            width,
            height,
            1
        };

        vkCmdCopyBufferToImage(
            commandBuffer,
            buffer,
            image,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            1,
            &region
        );

        Helper::EndSingleTimeCommands(commandPool, commandBuffer, logicalDevice);
    }

    void WinSys::InsertImageMemoryBarrier(VkCommandBuffer commandBuffer, VkImage image, VkAccessFlags srcAccessMask, VkAccessFlags dstAccessMask, VkImageLayout oldImageLayout, VkImageLayout newImageLayout, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, VkImageSubresourceRange subresourceRange)
    {
        VkImageMemoryBarrier imageMemoryBarrier{};
        imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        imageMemoryBarrier.srcAccessMask = srcAccessMask;
        imageMemoryBarrier.dstAccessMask = dstAccessMask;
        imageMemoryBarrier.oldLayout = oldImageLayout;
        imageMemoryBarrier.newLayout = newImageLayout;
        imageMemoryBarrier.image = image;
        imageMemoryBarrier.subresourceRange = subresourceRange;

        vkCmdPipelineBarrier(
            commandBuffer,
            srcStageMask,
            dstStageMask,
            0,
            0, nullptr,
            0, nullptr,
            1, &imageMemoryBarrier);
    }
}