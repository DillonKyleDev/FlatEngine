#include "render/Material.h"
#include "tools/JsonHelper.h"
#include "tools/Logger.h"


namespace FlatEngine
{
	Material::Material(std::string name, std::string vertexPath, std::string fragmentPath)
	{
		m_name = name;
		m_graphicsPipeline = GraphicsPipeline(vertexPath, fragmentPath);	
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
		m_texturePipelineData = std::map<uint32_t, TexturePipelineData>();		
		m_allocator = Allocator();	
		m_renderPass = nullptr;

		m_uboVec4Names = std::map<uint32_t, std::string>();
		
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
			{ "b_primitiveRestartEnable", (bool)m_inputAssembly.primitiveRestartEnable }
		};

		json rasterizerData = {
			{ "polygonMode", (int)m_rasterizer.polygonMode },
			{ "cullMode", (int)m_rasterizer.cullMode },
			{ "lineWidth", m_rasterizer.lineWidth }
		};

		json colorBlendAttachmentData = {
			{ "alphaBlendOp", (int)m_colorBlendAttachment.alphaBlendOp }
		};

		json uboVec4Names = json::object();		
		for (std::map<uint32_t, std::string>::iterator iter = m_uboVec4Names.begin(); iter != m_uboVec4Names.end(); iter++)
		{
			uboVec4Names.emplace(std::to_string(iter->first), iter->second);
		}

		json texturesShaderData = json::object();
		for (std::map<uint32_t, TexturePipelineData>::iterator textureData = m_texturePipelineData.begin(); textureData != m_texturePipelineData.end(); textureData++)
		{
			json texture = json::object();

			texture.emplace("shaderStage", textureData->second.shaderStage);
			texture.emplace("descriptorType", textureData->second.descriptorType);

			texturesShaderData.emplace(std::to_string(textureData->first), texture);
		}

		json jsonData = {
			{ "name", m_name },
			{ "vertexShaderPath", m_graphicsPipeline.GetVertexPath() },
			{ "fragmentShaderPath", m_graphicsPipeline.GetFragmentPath() },			
			{ "rasterizerData", rasterizerData },
			{ "inputAssemblyData", inputAssemblyData },
			{ "colorBlendAttachmentData", colorBlendAttachmentData },
			{ "uboVec4Names", uboVec4Names },
			{ "texturePipelineData", texturesShaderData }
		};

		std::string data = jsonData.dump(4);
		// Return dumped json object with required data for saving
		return data;
	}

	void Material::Init()
	{
		if (m_graphicsPipeline.GetVertexPath() != "" && m_graphicsPipeline.GetFragmentPath() != "")
		{			
			if (m_name != "imgui")
			{
				m_allocator.Init(AllocatorType::DescriptorPool, &m_texturePipelineData);
				m_graphicsPipeline.CreatePushConstantRanges();
			}			
			m_graphicsPipeline.CreateGraphicsPipeline(*m_renderPass, m_allocator.GetDescriptorSetLayout());
			m_b_initialized = true;
		}
	}

	bool Material::Initialized()
	{
		return m_b_initialized;
	}

	void Material::Cleanup()
	{
		m_graphicsPipeline.Cleanup();
	}

	void Material::SetHandles(RenderPass* renderPass)
	{		
		m_renderPass = renderPass;
	}

	void Material::RecreateGraphicsPipeline()
	{
		m_graphicsPipeline.Cleanup();

		if (m_graphicsPipeline.GetVertexPath() != "" && m_graphicsPipeline.GetFragmentPath() != "")
		{
			m_graphicsPipeline.CreateGraphicsPipeline(*m_renderPass, m_allocator.GetDescriptorSetLayout());
		}
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

	GraphicsPipeline& Material::GetGraphicsPipeline()
	{
		return m_graphicsPipeline;
	}

	VkPipelineLayout& Material::GetPipelineLayout()
	{
		return m_graphicsPipeline.GetPipelineLayout();
	}

	VkDescriptorPool Material::CreateDescriptorPool()
	{
		return m_allocator.CreateDescriptorPool();
	}

	void Material::CreateDescriptorSets(std::vector<VkDescriptorSet>& descriptorSets, std::vector<VkBuffer>& uniformBuffers, std::map<uint32_t, Texture>& meshTextures)
	{
		m_allocator.AllocateDescriptorSets(descriptorSets, uniformBuffers, m_texturePipelineData, meshTextures);
	}

	Allocator& Material::GetAllocator()
	{
		return m_allocator;
	}

	std::map<uint32_t, TexturePipelineData>* Material::GetTexturesPipelineData()
	{
		return &m_texturePipelineData;
	}

	uint32_t Material::GetTextureCount()
	{
		return (uint32_t)m_texturePipelineData.size();
	}

	void Material::AddTexture(uint32_t index, TexturePipelineData textureData)
	{
		if (m_texturePipelineData.count(index))
		{
			m_texturePipelineData.at(index) = textureData;
		}
		else
		{
			m_texturePipelineData.emplace(index, textureData);
		}

		m_allocator.Init(AllocatorType::DescriptorPool, &m_texturePipelineData);
	}

	void Material::RemoveTexture(int index)
	{
		if (index == -1 && m_texturePipelineData.size())
		{			
			std::map<uint32_t, TexturePipelineData>::iterator last = m_texturePipelineData.end();
			last--;
			m_texturePipelineData.erase(last);
		}
		else if (m_texturePipelineData.count((uint32_t)index))
		{
			m_texturePipelineData.erase((uint32_t)index);
		}

		m_allocator.Init(AllocatorType::DescriptorPool, &m_texturePipelineData);
	}

	void Material::OnWindowResized()
	{
		RecreateGraphicsPipeline();
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

	RenderPass* Material::GetRenderPass()
	{
		return m_renderPass;
	}

	std::map<uint32_t, std::string>& Material::GetUBOVec4Names()
	{
		return m_uboVec4Names;
	}

	bool CompareStrings(std::string a, std::string b) 
	{ 
		return a < b; 
	}

	bool Material::AddUBOVec4(std::string name, int index)
	{
		for (std::map<uint32_t, std::string>::iterator iter = m_uboVec4Names.begin(); iter != m_uboVec4Names.end(); iter++)
		{
			if (iter->second == name)
			{
				Logger::log.Err("Vec4 name already taken.");
				return false;
			}
		}

		if (index != -1)
		{
			m_uboVec4Names.emplace((uint32_t)index, name);
		}
		else
		{
			m_uboVec4Names.emplace((uint32_t)m_uboVec4Names.size(), name);
		}

		return true;
	}

	void Material::RemoveUBOVec4(int index)
	{
		if (index != -1 && m_uboVec4Names.size() >= index)
		{
			m_uboVec4Names.erase(index);
		}
		else if (m_uboVec4Names.size())
		{
			m_uboVec4Names.erase(--m_uboVec4Names.end());
		}
	}
}