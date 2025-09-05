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
		m_descriptorSets = std::vector<VkDescriptorSet>(2, {});
		m_imageView = VK_NULL_HANDLE;
		m_image = VK_NULL_HANDLE;
		m_textureImageMemory = VK_NULL_HANDLE;
		m_textureSampler = VK_NULL_HANDLE;
		m_mipLevels = 1;

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
		vkDestroySampler(logicalDevice.GetDevice(), m_textureSampler, nullptr);
		vkFreeMemory(logicalDevice.GetDevice(), m_textureImageMemory, nullptr);
		vkDestroyImage(logicalDevice.GetDevice(), m_image, nullptr);
		vkDestroyImageView(logicalDevice.GetDevice(), m_imageView, nullptr);
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
			return m_allocationIndex != -1;
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

	VkImageView& Texture::GetImageView()
	{
		return m_imageView;
	}

	VkImage& Texture::GetImage()
	{
		return m_image;
	}

	VkDeviceMemory& Texture::GetTextureImageMemory()
	{
		return m_textureImageMemory;
	}

	VkSampler& Texture::GetTextureSampler()
	{
		return m_textureSampler;
	}

	uint32_t Texture::GetMipLevels()
	{
		return m_mipLevels;
	}

	void Texture::CreateTextureImage(WinSys& winSystem, VkCommandPool commandPool, PhysicalDevice& physicalDevice, LogicalDevice& logicalDevice)
	{
		VkImage newImage = winSystem.CreateTextureImage(m_path, m_mipLevels, commandPool, physicalDevice, logicalDevice, m_textureImageMemory);
		WinSys::CreateImageView(m_imageView, newImage, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT, m_mipLevels, logicalDevice);
		WinSys::CreateTextureSampler(m_textureSampler, m_mipLevels, physicalDevice, logicalDevice);
	}

	void Texture::ConfigureImageResources(VkImage& image, VkImageView& imageView, VkDeviceMemory& textureImageMemory, VkSampler& textureSampler)
	{
		m_image = image;
		m_imageView = imageView;
		m_textureImageMemory = textureImageMemory;
		m_textureSampler = textureSampler;
	}
}