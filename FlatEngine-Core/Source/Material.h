#pragma once
#include "GraphicsPipeline.h"
#include "RenderPass.h"
#include "PhysicalDevice.h"
#include "LogicalDevice.h"
#include "WinSys.h"
#include "Model.h"
#include "Texture.h"
#include "Allocator.h"
#include "Structs.h"

#include <string>
#include <vector>
#include <memory>
#include <map>


namespace FlatEngine
{
	class Material
	{
	public:
		Material(std::string name, std::string vertexPath, std::string fragmentPath);
		Material();
		~Material();
		std::string GetData();
		void Init();
		bool Initialized();	
		void Cleanup();
		void RecreateGraphicsPipeline();

		void SetHandles(VkInstance* instance, WinSys* winSystem, PhysicalDevice* physicalDevice, LogicalDevice* logicalDevice, VkCommandPool* commandPool, RenderPass* renderPass);
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
		void CreateDescriptorSets(std::vector<VkDescriptorSet>& descriptorSets, Model& model, std::vector<Texture>& textures);
		Allocator& GetAllocator();
		void SetTextureCount(uint32_t textureCount);
		uint32_t GetTextureCount();		
		void OnWindowResized();

		// Configure GraphicsPipeline
		VkPipelineInputAssemblyStateCreateInfo& GetInputAssemblyCreateInfos();
		void SetInputAssemblyCreateInfos(VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfos);
		void SetRasterizerCreateInfos(VkPipelineRasterizationStateCreateInfo rasterizerInfos);
		VkPipelineRasterizationStateCreateInfo& GetRasterizerCreateInfos();
		void SetColorBlendAttachmentCreateInfos(VkPipelineColorBlendAttachmentState colorBlendAttachmentInfos);
		VkPipelineColorBlendAttachmentState& GetColorBlendAttachmentCreateInfos();

		void RecordDefaultCommandBuffer(uint32_t imageIndex);
		RenderPass* GetRenderPass();

	private:
		void SetDefaultValues();

		std::string m_name;		
		std::string m_path;
		GraphicsPipeline m_graphicsPipeline;
		RenderPass* m_renderPass;
		Allocator m_allocator;
		uint32_t m_textureCount;

		// handles
		VkInstance* m_instance;
		WinSys* m_winSystem;
		PhysicalDevice* m_physicalDevice;
		LogicalDevice* m_logicalDevice;	
		VkCommandPool* m_commandPool;
		bool m_b_initialized;

		std::vector<uint32_t> m_pushConstOffsets;
		std::vector<uint32_t> m_pushConstSizes;
		std::vector<const void*> m_pushValues;

		// GraphicsPipeline Configuration structs
		VkPipelineRasterizationStateCreateInfo m_rasterizer{};
		VkPipelineInputAssemblyStateCreateInfo m_inputAssembly{};
		VkPipelineColorBlendAttachmentState m_colorBlendAttachment{};
	};
}