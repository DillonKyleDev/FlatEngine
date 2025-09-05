#include "Material.h"
#include "VulkanManager.h"

#include <stdexcept>

#include "json.hpp"
using json = nlohmann::json;
using namespace nlohmann::literals;


namespace FlatEngine
{
	Material::Material(std::string name, std::string vertexPath, std::string fragmentPath)
	{
		m_name = name;
		m_graphicsPipeline = GraphicsPipeline(vertexPath, fragmentPath);
		m_textureCount = 0;
		m_allocator = Allocator();
		// handles
		m_instanceHandle = VK_NULL_HANDLE;
		m_winSystem = nullptr;
		m_physicalDeviceHandle = nullptr;
		m_deviceHandle = nullptr;
		m_renderPass = nullptr;
		m_commandPool = nullptr;
		m_b_initialized = false;
	}

	Material::Material()
	{
		m_name = "";
		m_graphicsPipeline = GraphicsPipeline();
		m_textureCount = 0;
		m_allocator = Allocator();
		// handles
		m_instanceHandle = VK_NULL_HANDLE;
		m_winSystem = nullptr;
		m_physicalDeviceHandle = nullptr;
		m_deviceHandle = nullptr;
		m_renderPass = nullptr;
		m_commandPool = nullptr;
		m_b_initialized = false;
	}

	Material::~Material()
	{
	}

	std::string Material::GetData()
	{
		json jsonData = {
			{ "name", m_name },
			{ "vertexShaderPath", m_graphicsPipeline.GetVertexPath() },
			{ "fragmentShaderPath", m_graphicsPipeline.GetFragmentPath() },
			{ "textureCount", m_textureCount }		
		};

		std::string data = jsonData.dump(4);
		// Return dumped json object with required data for saving
		return data;
	}

	void Material::Init()
	{
		if (m_graphicsPipeline.GetVertexPath() != "" && m_graphicsPipeline.GetFragmentPath() != "")
		{
			CreateMaterialResources();
			m_b_initialized = true;
		}
	}

	bool Material::Initialized()
	{
		return m_b_initialized;
	}

	void Material::CleanupGraphicsPipeline()
	{
		m_graphicsPipeline.Cleanup(*m_deviceHandle);
	}

	void Material::Cleanup()
	{
		CleanupGraphicsPipeline();
	}

	void Material::RecreateGraphicsPipeline()
	{
		CleanupGraphicsPipeline();

		if (m_graphicsPipeline.GetVertexPath() != "" && m_graphicsPipeline.GetFragmentPath() != "")
		{
			CreateGraphicsPipeline();
		}
	}


	void Material::SetHandles(VkInstance instance, WinSys& winSystem, PhysicalDevice& physicalDevice, LogicalDevice& logicalDevice, RenderPass& renderPass, VkCommandPool& commandPool)
	{
		m_instanceHandle = instance;
		m_winSystem = &winSystem;
		m_physicalDeviceHandle = &physicalDevice;
		m_deviceHandle = &logicalDevice;
		m_renderPass = &renderPass;
		m_commandPool = &commandPool;
	}

	void Material::CreateMaterialResources()
	{
		m_allocator.Init(AllocatorType::DescriptorPool, m_textureCount, *m_deviceHandle);
		CreateGraphicsPipeline();
	}

	void Material::SetName(std::string name)
	{
		m_name = name;
	}

	std::string Material::GetName()
	{
		return m_name;
	}

	void Material::SetPath(std::string path)
	{
		m_path = path;
	}

	std::string Material::GetPath()
	{
		return m_path;
	}


	void Material::SetVertexPath(std::string path)
	{
		m_graphicsPipeline.SetVertexPath(path);
	}

	void Material::SetFragmentPath(std::string path)
	{
		m_graphicsPipeline.SetFragmentPath(path);
	}

	std::string Material::GetVertexPath()
	{
		return m_graphicsPipeline.GetVertexPath();
	}

	std::string Material::GetFragmentPath()
	{
		return m_graphicsPipeline.GetFragmentPath();
	}


	void Material::CreateGraphicsPipeline()
	{
		m_graphicsPipeline.CreateGraphicsPipeline(*m_deviceHandle, *m_winSystem, *m_renderPass, m_allocator.GetDescriptorSetLayout());
	}

	VkPipeline& Material::GetGraphicsPipeline()
	{
		return m_graphicsPipeline.GetGraphicsPipeline();
	}

	VkPipelineLayout& Material::GetPipelineLayout()
	{
		return m_graphicsPipeline.GetPipelineLayout();
	}

	VkDescriptorPool Material::CreateDescriptorPool()
	{
		return m_allocator.CreateDescriptorPool();
	}

	void Material::CreateDescriptorSets(std::vector<VkDescriptorSet>& descriptorSets, Model& model, std::vector<Texture>& textures)
	{
		m_allocator.AllocateDescriptorSets(descriptorSets, model, textures);
	}

	Allocator& Material::GetAllocator()
	{
		return m_allocator;
	}

	void Material::SetTextureCount(uint32_t textureCount)
	{
		m_textureCount = textureCount;
	}

	uint32_t Material::GetTextureCount()
	{
		return m_textureCount;
	}
}