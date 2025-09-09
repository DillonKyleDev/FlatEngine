#include "Texture.h"
#include "FlatEngine.h"
#include "VulkanManager.h"

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
		m_imageFormat = VK_FORMAT_R8G8B8A8_UNORM;
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

			// JUST FOR GETTING TEXTURE DIMENSIONS, todo: figure out how to get them without using SDL surface
			SDL_Surface* loadedSurface = IMG_Load(path.c_str());
			if (loadedSurface != nullptr)
			{
				m_textureWidth = loadedSurface->w;
				m_textureHeight = loadedSurface->h;
			}

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

	void Texture::ConfigureImageResources(std::vector<VkImage>& images, std::vector<VkImageView>& imageViews, std::vector<VkDeviceMemory>& imageMemory, VkSampler& sampler)
	{
		m_images = images;
		m_imageViews = imageViews;
		m_imageMemory = imageMemory;
		m_sampler = sampler;
	}
}