#pragma once
#include "GraphicsPipeline.h"
#include "PhysicalDevice.h"
#include "LogicalDevice.h"
#include "WinSys.h"
#include "RenderPass.h"
#include "Model.h"
#include "Texture.h"
#include "Allocator.h"

#include <string>
#include <vector>
#include <memory>


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
		void CleanupGraphicsPipeline();		
		void Cleanup();
		void RecreateGraphicsPipeline();

		void SetHandles(VkInstance instance, WinSys& winSystem, PhysicalDevice& physicalDevice, LogicalDevice& logicalDevice, RenderPass& renderPass, VkCommandPool& commandPool);
		void CreateMaterialResources();
		void SetName(std::string name);
		std::string GetName();		
		void SetPath(std::string path);
		std::string GetPath();
		void SetVertexPath(std::string path);
		void SetFragmentPath(std::string path);
		std::string GetVertexPath();
		std::string GetFragmentPath();
		void CreateGraphicsPipeline();
		VkPipeline& GetGraphicsPipeline();
		VkPipelineLayout& GetPipelineLayout();
		VkDescriptorPool CreateDescriptorPool();
		void CreateDescriptorSets(std::vector<VkDescriptorSet>& descriptorSets, Model& model, std::vector<Texture>& textures);
		Allocator& GetAllocator();
		void SetTextureCount(uint32_t textureCount);
		uint32_t GetTextureCount();

	private:
		std::string m_name;		
		std::string m_path;
		GraphicsPipeline m_graphicsPipeline;
		Allocator m_allocator;
		uint32_t m_textureCount;
		// handles
		VkInstance m_instanceHandle;
		WinSys* m_winSystem;
		PhysicalDevice* m_physicalDeviceHandle;
		LogicalDevice* m_deviceHandle;
		RenderPass* m_renderPass;
		VkCommandPool* m_commandPool;
		bool m_b_initialized;
	};
}