#pragma once
#include "render/Allocator.h"
#include "render/GraphicsPipeline.h"
#include "render/RenderPass.h"
#include "render/Texture.h"
#include "tools/JsonHelper.h"

#include <string>
#include <map>
#include <vector>


namespace FlatEngine
{
	class Material
	{
	public:
		Material(std::string name, std::string vertexPath, std::string fragmentPath);
		Material();
		json GetData();
		void Init();
		bool Initialized();	
		void Cleanup();
		void RecreateGraphicsPipeline();

		void SetHandles(RenderPass* renderPass);
		void SetName(std::string name);
		std::string GetName();		
		void SetPath(std::string path);
		std::string GetPath();
		void SetVertexPath(std::string path);
		void SetFragmentPath(std::string path);
		std::string GetVertexPath();
		std::string GetFragmentPath();
		GraphicsPipeline& GetGraphicsPipeline();
		VkPipelineLayout& GetPipelineLayout();
		VkDescriptorPool CreateDescriptorPool();
		void CreateDescriptorSets(std::vector<VkDescriptorSet>& descriptorSets, std::vector<VkBuffer>& uniformBuffers, std::map<uint32_t, Texture>& textures);
		Allocator& GetAllocator();
		std::map<uint32_t, TexturePipelineData>* GetTexturesPipelineData();
		uint32_t GetTextureCount();	
		void AddTexture(uint32_t index, TexturePipelineData textureData);
		void RemoveTexture(int index = -1);
		void OnWindowResized();

		// Configure GraphicsPipeline
		VkPipelineInputAssemblyStateCreateInfo& GetInputAssemblyCreateInfos();
		void SetInputAssemblyCreateInfos(VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfos);
		void SetRasterizerCreateInfos(VkPipelineRasterizationStateCreateInfo rasterizerInfos);
		VkPipelineRasterizationStateCreateInfo& GetRasterizerCreateInfos();
		void SetColorBlendAttachmentCreateInfos(VkPipelineColorBlendAttachmentState colorBlendAttachmentInfos);
		VkPipelineColorBlendAttachmentState& GetColorBlendAttachmentCreateInfos();
		
		RenderPass* GetRenderPass();

		std::map<uint32_t, std::string>& GetUBOVec4Names();
		bool AddUBOVec4(std::string name, int index = -1);
		void RemoveUBOVec4(int index = -1);

	private:
		void SetDefaultValues();

		std::string m_name;		
		std::string m_path;
		GraphicsPipeline m_graphicsPipeline;
		RenderPass* m_renderPass;
		Allocator m_allocator;
		std::map<uint32_t, TexturePipelineData> m_texturePipelineData;

		bool m_b_initialized;

		std::vector<uint32_t> m_pushConstOffsets;
		std::vector<uint32_t> m_pushConstSizes;
		std::vector<const void*> m_pushValues;

		// Uniform Buffer Data
		std::map<uint32_t, std::string> m_uboVec4Names;

		// GraphicsPipeline Configuration structs
		VkPipelineRasterizationStateCreateInfo m_rasterizer{};
		VkPipelineInputAssemblyStateCreateInfo m_inputAssembly{};
		VkPipelineColorBlendAttachmentState m_colorBlendAttachment{};
	};
}