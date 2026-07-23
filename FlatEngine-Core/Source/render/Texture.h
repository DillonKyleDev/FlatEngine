#pragma once

#include <SDL_image.h>
#include <SDL_ttf.h>
#include <string>
#include <vector>
#include <vulkan_core.h>


namespace FlatEngine
{
	struct TexturePipelineData {
		VkShaderStageFlags shaderStage = VK_SHADER_STAGE_FRAGMENT_BIT;
		VkDescriptorType descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	};

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
		void Cleanup();

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
		void SetImageFormat(VkFormat imageFormat);
		std::vector<VkDescriptorSet>& GetDescriptorSets();
		void SetDescriptorType(VkDescriptorType descriptorType);
		VkDescriptorType GetDescriptorType();

		void CreateTextureImage();
		void CreateRenderToTextureResources(VkCommandPool& commandPool);

	private:
		std::string m_path;
		int m_textureWidth;
		int m_textureHeight;
		std::vector<VkDescriptorSet> m_descriptorSets; // For ImGui to use for it's textures (icons, etc.)
		VkDescriptorType m_descriptorType;
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