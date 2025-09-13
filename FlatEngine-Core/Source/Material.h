#pragma once
#include "GraphicsPipeline.h"
#include "RenderPass.h"
#include "PhysicalDevice.h"
#include "LogicalDevice.h"
#include "WinSys.h"
#include "Model.h"
#include "Texture.h"
#include "Allocator.h"

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

		void SetHandles(VkInstance* instance, WinSys* winSystem, PhysicalDevice* physicalDevice, LogicalDevice* logicalDevice, VkCommandPool* commandPool);
		void SetName(std::string name);
		std::string GetName();		
		void SetPath(std::string path);
		std::string GetPath();
		void SetVertexPath(std::string path);
		void SetFragmentPath(std::string path);
		std::string GetVertexPath();
		std::string GetFragmentPath();
		VkPipeline& GetGraphicsPipeline();
		VkPipelineLayout& GetPipelineLayout();
		VkDescriptorPool CreateDescriptorPool();
		void CreateDescriptorSets(std::vector<VkDescriptorSet>& descriptorSets, Model& model, std::vector<Texture>& textures);
		Allocator& GetAllocator();
		void SetTextureCount(uint32_t textureCount);
		uint32_t GetTextureCount();
		void CreateImageResources();
		void OnWindowResized();

		// Configure RenderPass
		// TODO
		// Configure GraphicsPipeline
		VkPipelineInputAssemblyStateCreateInfo& GetInputAssemblyCreateInfos();
		void SetInputAssemblyCreateInfos(VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfos);
		void SetRasterizerCreateInfos(VkPipelineRasterizationStateCreateInfo rasterizerInfos);
		VkPipelineRasterizationStateCreateInfo& GetRasterizerCreateInfos();
		void SetColorBlendAttachmentCreateInfos(VkPipelineColorBlendAttachmentState colorBlendAttachmentInfos);
		VkPipelineColorBlendAttachmentState& GetColorBlendAttachmentCreateInfos();

		void CreateRenderPassResources();
		void HandleRenderPass(uint32_t imageIndex);
		void RecordDefaultCommandBuffer(uint32_t imageIndex, Mesh& mesh);
		RenderPass& GetRenderPass();

		// For rendering to texture
		void AddMaterialDescriptorSetToRenderTo(std::string materialName, std::vector<VkDescriptorSet>* descriptorSetsToWriteTo);

	private:
		void QuitImGui();
		void SetDefaultValues();
		void CreateImGuiResources();
		void GetImGuiDescriptorSetLayoutInfo(std::vector<VkDescriptorSetLayoutBinding>& bindings, VkDescriptorSetLayoutCreateInfo& layoutInfo);
		void GetImGuiDescriptorPoolInfo(std::vector<VkDescriptorPoolSize>& poolSizes, VkDescriptorPoolCreateInfo& poolInfo);

		std::string m_name;		
		std::string m_path;
		GraphicsPipeline m_graphicsPipeline;
		RenderPass m_renderPass;
		Allocator m_allocator;
		uint32_t m_textureCount;

		// For rendering to texture instead of to the screen
		Texture m_renderTexture;
		std::map<std::string, std::vector<VkDescriptorSet>*> m_renderTextureMaterialDescriptorSets; // std::map<materialNameToUse, std::vector<descriptorSets to allocate to>> -- Configure to meet the demands of where the Texture will be used.

		VkFormat m_imageFormat;
		uint32_t m_mipLevels;

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