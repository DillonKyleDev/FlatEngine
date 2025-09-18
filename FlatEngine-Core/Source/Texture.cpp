#include "Texture.h"
#include "FlatEngine.h"
#include "VulkanManager.h"
#include "Helper.h"

#include "stb_image.h"

#include "imgui_impl_vulkan.h"
#include <stdexcept>


namespace FlatEngine
{
	Texture::Texture(std::string path)
	{
		m_path = path;
		m_textureWidth = 0;
		m_textureHeight = 0;
		m_allocationIndex = -1;
		m_descriptorSets = std::vector<VkDescriptorSet>(VM_MAX_FRAMES_IN_FLIGHT, {});
		m_images = std::vector<VkImage>(VM_MAX_FRAMES_IN_FLIGHT, {});
		m_imageViews = std::vector<VkImageView>(VM_MAX_FRAMES_IN_FLIGHT, {});
		m_imageMemory = std::vector<VkDeviceMemory>(VM_MAX_FRAMES_IN_FLIGHT, {});
		m_sampler = VK_NULL_HANDLE;		
		m_mipLevels = 1;
		m_imageFormat = VK_FORMAT_R8G8B8A8_UNORM; // ImGui format
		m_aspectFlags = VK_IMAGE_ASPECT_COLOR_BIT;

		if (path != "")
		{
			LoadFromFile(path);
		}
	}

	Texture::~Texture()
	{		
	}

	void Texture::Cleanup(LogicalDevice& logicalDevice)
	{
		FreeTexture();		
		
		for (int i = 0; i < VM_MAX_FRAMES_IN_FLIGHT; i++)
		{
			vkFreeMemory(logicalDevice.GetDevice(), m_imageMemory[i], nullptr);
			vkDestroyImage(logicalDevice.GetDevice(), m_images[i], nullptr);
			vkDestroyImageView(logicalDevice.GetDevice(), m_imageViews[i], nullptr);
		}

		vkDestroySampler(logicalDevice.GetDevice(), m_sampler, nullptr);
	}

	bool Texture::LoadFromFile(std::string path)
	{
		m_path = path;

		if (path != "")
		{
			FreeTexture();

			int texChannels;
			stbi_uc* pixels = stbi_load(path.c_str(), &m_textureWidth, &m_textureHeight, &texChannels, STBI_rgb_alpha);			

			F_VulkanManager->CreateImGuiTexture(*this, m_descriptorSets);			
			//return m_allocationIndex != -1;  TODO: Look into this, allocation index not saving
			return true;
		}
		else
		{
			return false;
		}
	}

	int& Texture::GetAllocationIndex()
	{
		return m_allocationIndex;
	}

	void Texture::SetAllocationIndex(int index)
	{
		m_allocationIndex = index;
	}

	bool Texture::LoadFromRenderedText(std::string textureText, SDL_Color textColor, TTF_Font* font)
	{
		//FreeSurface();		
		//SDL_Surface* textSurface = TTF_RenderText_Solid(font, textureText.c_str(), textColor);

		//if (textSurface == NULL)
		//{
		//	printf("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
		//}
		//else
		//{
		//	if (m_texture == NULL)
		//	{
		//		printf("Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError());
		//	}
		//	else
		//	{
		//		m_textureWidth = textSurface->w;
		//		m_textureHeight = textSurface->h;
		//	}

		//	SDL_FreeSurface(textSurface);
		//}

		return true;
	}

	void Texture::FreeTexture()
	{			
		if (m_allocationIndex != -1)
		{
			F_VulkanManager->FreeImGuiTexture(m_allocationIndex);
			m_descriptorSets.clear();
			m_descriptorSets.resize(VM_MAX_FRAMES_IN_FLIGHT);
			m_textureWidth = 0;
			m_textureHeight = 0;
		}
	}

	VkDescriptorSet Texture::GetTexture()
	{
		return m_descriptorSets[VM_currentFrame];
	}

	int Texture::GetWidth()
	{
		return m_textureWidth;
	}

	int Texture::GetHeight()
	{
		return m_textureHeight;
	}

	void Texture::SetDimensions(int width, int height)
	{
		m_textureWidth = width;
		m_textureHeight = height;
	}

	void Texture::SetTexturePath(std::string path)
	{
		m_path = path;
	}

	std::string Texture::GetTexturePath()
	{
		return m_path;
	}

	std::vector<VkImage>& Texture::GetImages()
	{
		return m_images;
	}

	std::vector<VkImageView>& Texture::GetImageViews()
	{
		return m_imageViews;
	}

	void Texture::SetImageViews(std::vector<VkImageView>& imageViews)
	{
		m_imageViews = imageViews;
	}

	std::vector<VkDeviceMemory>& Texture::GetImageMemory()
	{
		return m_imageMemory;
	}

	VkSampler& Texture::GetSampler()
	{
		return m_sampler;
	}

	uint32_t Texture::GetMipLevels()
	{
		return m_mipLevels;
	}

	VkFormat Texture::GetImageFormat()
	{
		return m_imageFormat;
	}

	void Texture::SetImageFormat(VkFormat imageFormat)
	{
		m_imageFormat = imageFormat;
	}

	std::vector<VkDescriptorSet>& Texture::GetDescriptorSets()
	{
		return m_descriptorSets;
	}

	void Texture::CreateTextureImage()
	{
		m_images.resize(VM_MAX_FRAMES_IN_FLIGHT);
		m_imageViews.resize(VM_MAX_FRAMES_IN_FLIGHT);
		m_imageMemory.resize(VM_MAX_FRAMES_IN_FLIGHT);

		for (int i = 0; i < VM_MAX_FRAMES_IN_FLIGHT; i++)
		{
			F_VulkanManager->CreateTextureImage(m_images[i], m_path, m_mipLevels, m_imageMemory[i]);			
			F_VulkanManager->CreateImageView(m_imageViews[i], m_images[i], m_imageFormat, m_aspectFlags, m_mipLevels);			
		}
		
		F_VulkanManager->CreateTextureSampler(m_sampler, m_mipLevels);
	}

	void Texture::CreateRenderToTextureResources()
	{
		WinSys& windowSystem = F_VulkanManager->GetWinSystem();
		LogicalDevice& logicalDevice = F_VulkanManager->GetLogicalDevice();
		VkDevice& device = logicalDevice.GetDevice();
		m_imageFormat = VK_FORMAT_R32G32B32A32_SFLOAT;
		//Cleanup(logicalDevice);

		m_images.resize(VM_MAX_FRAMES_IN_FLIGHT);
		m_imageViews.resize(VM_MAX_FRAMES_IN_FLIGHT);
		m_imageMemory.resize(VM_MAX_FRAMES_IN_FLIGHT);
		VkExtent2D extent = F_VulkanManager->GetWinSystem().GetExtent();

		for (size_t i = 0; i < VM_MAX_FRAMES_IN_FLIGHT; i++)
		{
			uint32_t mipLevels = 1;
			int texWidth = extent.width;
			int texHeight = extent.height;
			VkDeviceSize imageSize = texWidth * texHeight * 16;
			VkBuffer stagingBuffer{};
			VkDeviceMemory stagingBufferMemory{};
			windowSystem.CreateBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);
			windowSystem.CreateImage(texWidth, texHeight, 1, VK_SAMPLE_COUNT_1_BIT, m_imageFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_images[i], m_imageMemory[i]);

			VkCommandBuffer copyCmd = Helper::BeginSingleTimeCommands();
			windowSystem.InsertImageMemoryBarrier(
				copyCmd,
				m_images[i],
				VK_ACCESS_TRANSFER_READ_BIT,
				VK_ACCESS_MEMORY_READ_BIT,
				VK_IMAGE_LAYOUT_UNDEFINED,
				VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
				VK_PIPELINE_STAGE_TRANSFER_BIT,
				VK_PIPELINE_STAGE_TRANSFER_BIT,
				VkImageSubresourceRange{ VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 });
			Helper::EndSingleTimeCommands(copyCmd);

			windowSystem.TransitionImageLayout(m_images[i], m_imageFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1);
			windowSystem.CopyBufferToImage(stagingBuffer, m_images[i], static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));
			windowSystem.TransitionImageLayout(m_images[i], m_imageFormat, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 1);

			vkDestroyBuffer(device, stagingBuffer, nullptr);
			vkFreeMemory(device, stagingBufferMemory, nullptr);

			windowSystem.CreateImageView(m_imageViews[i], m_images[i], m_imageFormat, VK_IMAGE_ASPECT_COLOR_BIT, mipLevels);
			windowSystem.CreateTextureSampler(m_sampler, mipLevels);
		}		
	}
}