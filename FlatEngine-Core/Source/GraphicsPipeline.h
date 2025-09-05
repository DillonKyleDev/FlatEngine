#pragma once
#include "LogicalDevice.h"
#include "WinSys.h"
#include "RenderPass.h"

//#ifndef GLFW_INCLUDE_VULKAN
//#define GLFW_INCLUDE_VULKAN
//#endif
//#include <glfw3.h>

#include <vector>
#include <string>


namespace FlatEngine
{
	class GraphicsPipeline
	{
	public:
		GraphicsPipeline(std::string vertexPath, std::string fragmentPath);
		GraphicsPipeline();
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

	private:
		VkPipeline m_graphicsPipeline;
		VkPipelineLayout m_pipelineLayout;
		std::string m_vertexPath;
		std::string m_fragmentPath;
	};
}