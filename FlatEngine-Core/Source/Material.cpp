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
		//m_renderPass = RenderPass();
		m_renderPass = VK_NULL_HANDLE;
		SetDefaultValues();
	}

	Material::Material()
	{
		m_name = "";
		m_graphicsPipeline = GraphicsPipeline();
		SetDefaultValues();
	}

	Material::~Material()
	{
	}

	void Material::SetDefaultValues()
	{
		m_textureCount = 0;
		m_allocator = Allocator();
		// handles
		m_winSystem = nullptr;
		m_logicalDevice = nullptr;
		m_b_initialized = false;

		// Set default RenderPass configuration
		// TODO
		
		// Default Graphics Pipeline configuration (Filled in with saved values when LoadMaterial() is called
		m_inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		m_inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		m_inputAssembly.primitiveRestartEnable = VK_FALSE;

		m_rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		m_rasterizer.depthClampEnable = VK_FALSE;
		m_rasterizer.rasterizerDiscardEnable = VK_FALSE;
		m_rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
		m_rasterizer.lineWidth = 1.0f;
		m_rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
		m_rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
		m_rasterizer.depthBiasEnable = VK_FALSE;
		m_rasterizer.depthBiasConstantFactor = 0.0f;
		m_rasterizer.depthBiasClamp = 0.0f;
		m_rasterizer.depthBiasSlopeFactor = 0.0f;

		m_colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT | 0;
		m_colorBlendAttachment.blendEnable = VK_TRUE;
		m_colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
		m_colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		m_colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
		m_colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
		m_colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		m_colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
		
		m_graphicsPipeline.SetInputAssemblyInfos(m_inputAssembly);
		m_graphicsPipeline.SetRasterizerCreateInfos(m_rasterizer);
		m_graphicsPipeline.SetColorBlendAttachmentCreateInfos(m_colorBlendAttachment);
	}

	std::string Material::GetData()
	{
		json inputAssemblyData = {
			{ "topology", (int)m_inputAssembly.topology },
			{ "_primitiveRestartEnable", (bool)m_inputAssembly.primitiveRestartEnable }
		};

		json rasterizerData = {
			{ "polygonMode", (int)m_rasterizer.polygonMode },
			{ "cullMode", (int)m_rasterizer.cullMode },
			{ "lineWidth", m_rasterizer.lineWidth }
		};

		json colorBlendAttachmentData = {
			{ "alphaBlendOp", (int)m_colorBlendAttachment.alphaBlendOp }
		};

		json jsonData = {
			{ "name", m_name },
			{ "vertexShaderPath", m_graphicsPipeline.GetVertexPath() },
			{ "fragmentShaderPath", m_graphicsPipeline.GetFragmentPath() },
			{ "textureCount", m_textureCount },
			{ "rasterizerData", rasterizerData },
			{ "inputAssemblyData", inputAssemblyData },
			{ "colorBlendAttachmentData", colorBlendAttachmentData }
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
		m_graphicsPipeline.Cleanup(*m_logicalDevice);
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


	void Material::SetHandles(WinSys& winSystem, LogicalDevice& logicalDevice, RenderPass& renderPass)
	{		
		m_winSystem = &winSystem;
		m_logicalDevice = &logicalDevice;
		m_renderPass = &renderPass;
	}

	void Material::CreateMaterialResources()
	{
		m_allocator.Init(AllocatorType::DescriptorPool, m_textureCount, *m_logicalDevice);
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
		m_graphicsPipeline.CreateGraphicsPipeline(*m_logicalDevice, *m_winSystem, *m_renderPass, m_allocator.GetDescriptorSetLayout());
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

	VkPipelineInputAssemblyStateCreateInfo& Material::GetInputAssemblyCreateInfos()
	{
		return m_inputAssembly;
	}

	void Material::SetInputAssemblyCreateInfos(VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfos)
	{
		m_inputAssembly = inputAssemblyInfos;
		m_graphicsPipeline.SetInputAssemblyInfos(m_inputAssembly);
	}

	VkPipelineRasterizationStateCreateInfo& Material::GetRasterizerCreateInfos()
	{
		return m_rasterizer;
	}

	void Material::SetRasterizerCreateInfos(VkPipelineRasterizationStateCreateInfo rasterizerInfos)
	{
		m_rasterizer = rasterizerInfos;
		m_graphicsPipeline.SetRasterizerCreateInfos(m_rasterizer);
	}

	void Material::SetColorBlendAttachmentCreateInfos(VkPipelineColorBlendAttachmentState colorBlendAttachmentInfos)
	{
		m_colorBlendAttachment = colorBlendAttachmentInfos;
	}

	VkPipelineColorBlendAttachmentState& Material::GetColorBlendAttachmentCreateInfos()
	{
		return m_colorBlendAttachment;
	}
}