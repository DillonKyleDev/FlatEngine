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
		std::vector<VkImage>& GetImages();
		std::vector<VkImageView>& GetImageViews();
		void SetImageViews(std::vector<VkImageView>& imageViews);
		std::vector<VkDeviceMemory>& GetImageMemory();
		VkSampler& GetSampler();
		uint32_t GetMipLevels();
		VkFormat GetImageFormat();
		std::vector<VkDescriptorSet>& GetDescriptorSets();

		void CreateTextureImage();
		void CreateRenderToTextureResources();		

	private:
		std::string m_path;
		int m_textureWidth;
		int m_textureHeight;
		std::vector<VkDescriptorSet> m_descriptorSets; // For ImGui to use for it's textures (icons, etc.)
		int m_allocationIndex;
		std::vector<VkImage> m_images;
		std::vector<VkImageView> m_imageViews;
		std::vector<VkDeviceMemory> m_imageMemory;
		VkSampler m_sampler;
		uint32_t m_mipLevels;
		VkFormat m_imageFormat;
		VkImageAspectFlagBits m_aspectFlags;
	};
}