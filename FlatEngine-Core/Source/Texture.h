#pragma once
#include "PhysicalDevice.h"
#include "LogicalDevice.h"
#include "WinSys.h"

#include <SDL_image.h>
#include <SDL_ttf.h>
#include <string>


namespace FlatEngine
{
	enum Pivot {
		PivotCenter,
		PivotLeft,
		PivotRight,
		PivotTop,
		PivotBottom,
		PivotTopLeft,
		PivotTopRight,
		PivotBottomRight,
		PivotBottomLeft
	};
	const std::string F_PivotStrings[9] = {
		"PivotCenter",
		"PivotLeft",
		"PivotRight",
		"PivotTop",
		"PivotBottom",
		"PivotTopLeft",
		"PivotTopRight",
		"PivotBottomRight",
		"PivotBottomLeft"
	};

	class Texture
	{
	public:
		Texture(std::string path = "");
		~Texture();
		void Cleanup(LogicalDevice& logicalDevice);

		bool LoadFromFile(std::string path);
		int& GetAllocationIndex();
		void SetAllocationIndex(int index);
		bool LoadFromRenderedText(std::string textureText, SDL_Color textColor, TTF_Font* font);
		void FreeTexture();		
		VkDescriptorSet GetTexture();
		int GetWidth();
		int GetHeight();
		void SetDimensions(int width, int height);

		void SetTexturePath(std::string path);
		std::string GetTexturePath();
		VkImageView& GetImageView();
		VkImage& GetImage();
		VkDeviceMemory& GetTextureImageMemory();
		VkSampler& GetTextureSampler();
		uint32_t GetMipLevels();

		void CreateTextureImage(WinSys& winSystem, VkCommandPool commandPool, PhysicalDevice& physicalDevice, LogicalDevice& logicalDevice);
		void ConfigureImageResources(VkImage& image, VkImageView& imageView, VkDeviceMemory& textureImageMemory, VkSampler& textureSampler);

	private:
		std::string m_path;
		int m_textureWidth;
		int m_textureHeight;
		std::vector<VkDescriptorSet> m_descriptorSets;
		int m_allocationIndex;
		VkImage m_image;
		VkImageView m_imageView;
		VkDeviceMemory m_textureImageMemory;
		VkSampler m_textureSampler;
		uint32_t m_mipLevels;
	};
}