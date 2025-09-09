#pragma once
#include "LogicalDevice.h"
#include "WinSys.h"
#include "RenderPass.h"

#include <vector>
#include <string>


namespace FlatEngine
{
	class GraphicsPipeline
	{
	public:
		GraphicsPipeline(std::string vertexPath = "", std::string fragmentPath = "");		
		~GraphicsPipeline();
		void Cleanup(LogicalDevice& logicalDevice);

		void SetVertexPath(std::string path);
		void SetFragmentPath(std::string path);
		std::string GetVertexPath();
		std::string GetFragmentPath();
		VkShaderModule CreateShaderModule(const std::vector<char>& code, LogicalDevice& logicalDevice);
		void CreateGraphicsPipeline(LogicalDevice& logicalDevice, WinSys& winSystem, RenderPass& renderPass, VkDescriptorSetLayout& descriptorSetLayout);
		VkPipeline& GetGraphicsPipeline();
		VkPipelineLayout& GetPipelineLayout();
		VkPipelineInputAssemblyStateCreateInfo& GetInputAssemblyInfos();
		void SetInputAssemblyInfos(VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfos);
		VkPipelineRasterizationStateCreateInfo& GetRasterizerCreateInfos();		
		void SetRasterizerCreateInfos(VkPipelineRasterizationStateCreateInfo rasterizerInfos);

	private:
		VkPipeline m_graphicsPipeline;
		VkPipelineLayout m_pipelineLayout;
		std::string m_vertexPath;
		std::string m_fragmentPath;

		// structs
		VkPipelineInputAssemblyStateCreateInfo m_inputAssembly{};
		VkViewport m_viewport{};
		VkRect2D m_scissor{};
		std::vector<VkDynamicState> m_dynamicStatesUsed{};
		VkPipelineDynamicStateCreateInfo m_dynamicState{};
		VkPipelineViewportStateCreateInfo m_viewportState{};
		VkPipelineRasterizationStateCreateInfo m_rasterizer{};
		VkPipelineMultisampleStateCreateInfo m_multisampling{};
		VkPipelineColorBlendAttachmentState m_colorBlendAttachment{};
		VkPipelineColorBlendStateCreateInfo m_colorBlending{};
		VkPipelineDepthStencilStateCreateInfo m_depthStencil{};
		std::vector<VkPushConstantRange> m_pushRanges{};
		VkPipelineLayoutCreateInfo m_pipelineLayoutInfo{};
		VkGraphicsPipelineCreateInfo m_pipelineInfo{};
	};
}