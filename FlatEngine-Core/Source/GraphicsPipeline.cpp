#include "GraphicsPipeline.h"
#include "Helper.h"
#include "VulkanManager.h"
#include "FlatEngine.h"

#include <stdexcept>


namespace FlatEngine
{
    GraphicsPipeline::GraphicsPipeline(std::string vertexPath, std::string fragmentPath)
    {
        m_graphicsPipeline = VK_NULL_HANDLE;
        m_pipelineLayout = VK_NULL_HANDLE;
        SetVertexPath(vertexPath);
        SetFragmentPath(fragmentPath);

        // Describes what kind of geometry will be drawn from the vertices and if primitive restart should be enabled        
        // VK_PRIMITIVE_TOPOLOGY_POINT_LIST: points from vertices
        // VK_PRIMITIVE_TOPOLOGY_LINE_LIST : line from every 2 vertices without reuse
        // VK_PRIMITIVE_TOPOLOGY_LINE_STRIP : the end vertex of every line is used as start vertex for the next line
        // VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST : triangle from every 3 vertices without reuse
        // VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP : the second and third vertex of every triangle are used as first two vertices of the next triangle  
        m_inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        m_inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_LINE_STRIP; // VK_PRIMITIVE_TOPOLOGY_LINE_LIST
        m_inputAssembly.primitiveRestartEnable = VK_FALSE;        
        m_viewport.x = 0.0f;
        m_viewport.y = 0.0f;
        m_viewport.minDepth = 0.0f;
        m_viewport.maxDepth = 1.0f;
        m_scissor.offset = { 0, 0 };

        // For creating dynamic pipeline that doesn't need to be fully recreated for certain values (ie. viewport size, line width, and blend constants)
        // Configuration of these values will be ignored and you will be able (and required) to specify the data at drawing time.
        m_dynamicStatesUsed =
        {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR
        };
        m_dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        m_dynamicState.flags = 0;

        m_viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        m_viewportState.viewportCount = 1;
        m_viewportState.scissorCount = 1;        
                
        // Rasterizer - more info found here: https://vulkan-tutorial.com/en/Drawing_a_triangle/Graphics_pipeline_basics/Fixed_functions        
        m_rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        m_rasterizer.depthClampEnable = VK_FALSE;
        m_rasterizer.rasterizerDiscardEnable = VK_FALSE;
        m_rasterizer.polygonMode = VK_POLYGON_MODE_FILL; // VK_POLYGON_MODE_LINE;
        m_rasterizer.lineWidth = 1.0f;
        m_rasterizer.cullMode = VK_CULL_MODE_BACK_BIT; // VK_CULL_MODE_NONE;
        m_rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        m_rasterizer.depthBiasEnable = VK_FALSE;
        m_rasterizer.depthBiasConstantFactor = 0.0f;
        m_rasterizer.depthBiasClamp = 0.0f;
        m_rasterizer.depthBiasSlopeFactor = 0.0f;

        // Multisampling - one of the ways to perform anti-aliasing - Enabling it requires enabling a GPU feature.        
        m_multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        m_multisampling.sampleShadingEnable = VK_TRUE;
        m_multisampling.minSampleShading = 0.2f;
        m_multisampling.pSampleMask = nullptr;
        m_multisampling.alphaToCoverageEnable = VK_FALSE;
        m_multisampling.alphaToOneEnable = VK_FALSE;
    
        // Color blending - After a fragment shader has returned a color, it needs to be combined with the color that is already in the framebuffer.
        m_colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT | 0;
        m_colorBlendAttachment.blendEnable = VK_TRUE;
        m_colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
        m_colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        m_colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
        m_colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
        m_colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        m_colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
        
        m_colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        m_colorBlending.logicOpEnable = VK_FALSE;
        m_colorBlending.logicOp = VK_LOGIC_OP_COPY;
        m_colorBlending.attachmentCount = 1;
        m_colorBlending.blendConstants[0] = 0.0f;
        m_colorBlending.blendConstants[1] = 0.0f;
        m_colorBlending.blendConstants[2] = 0.0f;
        m_colorBlending.blendConstants[3] = 0.0f;
        
        m_depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        m_depthStencil.depthTestEnable = VK_TRUE;
        m_depthStencil.depthWriteEnable = VK_TRUE;
        m_depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
        m_depthStencil.depthBoundsTestEnable = VK_FALSE;
        m_depthStencil.minDepthBounds = 0.0f;
        m_depthStencil.maxDepthBounds = 1.0f;
        m_depthStencil.stencilTestEnable = VK_FALSE;
        m_depthStencil.front = {};
        m_depthStencil.back = {};
                 
        VkPushConstantRange translateRange = {};
        translateRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
        translateRange.offset = (uint32_t)0;
        translateRange.size = (uint32_t)16;
        VkPushConstantRange timeRange = {};
        timeRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
        timeRange.offset = (uint32_t)16;
        timeRange.size = (uint32_t)16;
        VkPushConstantRange modelRange = {};
        modelRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
        modelRange.offset = (uint32_t)64;
        modelRange.size = (uint32_t)64;
        VkPushConstantRange viewRange = {};
        viewRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
        viewRange.offset = (uint32_t)128;
        viewRange.size = (uint32_t)64;
        VkPushConstantRange projectionRange = {};
        projectionRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
        projectionRange.offset = (uint32_t)192;
        projectionRange.size = (uint32_t)64;

        m_pushRanges.push_back(translateRange);
        m_pushRanges.push_back(timeRange);
        m_pushRanges.push_back(modelRange);
        m_pushRanges.push_back(viewRange);
        m_pushRanges.push_back(projectionRange);
                
        m_pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        m_pipelineLayoutInfo.setLayoutCount = VM_MAX_FRAMES_IN_FLIGHT;
                                
        m_pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;        
        // Vulkan allows you to create a new graphics pipeline by deriving from an existing pipeline (not using this)
        m_pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
        m_pipelineInfo.basePipelineIndex = -1;        
    }

    GraphicsPipeline::~GraphicsPipeline()
    {
    }

    void GraphicsPipeline::Cleanup(LogicalDevice& logicalDevice)
    {
        vkDestroyPipeline(logicalDevice.GetDevice(), m_graphicsPipeline, nullptr);
        vkDestroyPipelineLayout(logicalDevice.GetDevice(), m_pipelineLayout, nullptr);
    }

    void GraphicsPipeline::SetVertexPath(std::string path)
    {
        m_vertexPath = path;
    }

    void GraphicsPipeline::SetFragmentPath(std::string path)
    {
        m_fragmentPath = path;
    }

    std::string GraphicsPipeline::GetVertexPath()
    {
        return m_vertexPath;
    }

    std::string GraphicsPipeline::GetFragmentPath()
    {
        return m_fragmentPath;
    }

    VkShaderModule GraphicsPipeline::CreateShaderModule(const std::vector<char>& code, LogicalDevice& logicalDevice)
    {
        // Before we can pass the code to the pipeline, we have to wrap it in a VkShaderModule object.
        // We need to specify a pointer to the buffer with the bytecode and the length of it.
        VkShaderModuleCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = code.size();
        createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

        VkShaderModule shaderModule = VK_NULL_HANDLE;
        if (vkCreateShaderModule(logicalDevice.GetDevice(), &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create shader module.");
        }

        return shaderModule;
    }

    void GraphicsPipeline::CreateGraphicsPipeline(LogicalDevice& logicalDevice, WinSys& winSystem, RenderPass& renderPass, VkDescriptorSetLayout& descriptorSetLayout)
    {
        // More info here - https://vulkan-tutorial.com/en/Drawing_a_triangle/Graphics_pipeline_basics/Fixed_functions
        // Load the bytecode of the shaders
        auto vertShaderCode = Helper::ReadFile(m_vertexPath);
        auto fragShaderCode = Helper::ReadFile(m_fragmentPath);

        // The compilation and linking of the SPIR-V bytecode to machine code for execution by the GPU doesn't happen until the graphics pipeline is created, so we make them local and destroy them immediately after pipeline creation is finished
        VkShaderModule vertShaderModule = CreateShaderModule(vertShaderCode, logicalDevice);
        VkShaderModule fragShaderModule = CreateShaderModule(fragShaderCode, logicalDevice);

        // Assign shaders to whichever pipeline stage we want through this struct used to create the pipeline
        // Vertex shader
        VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
        vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT; // Stage to inject shader code
        vertShaderStageInfo.module = vertShaderModule; // Shader to invoke
        vertShaderStageInfo.pName = "main"; // Entry point
        vertShaderStageInfo.pSpecializationInfo = nullptr; // Allows you to specify values for shader constants to be plugged in at shader runtime
        // Fragment shader
        VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
        fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        fragShaderStageInfo.module = fragShaderModule;
        fragShaderStageInfo.pName = "main";
        
        VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };
       
        auto bindingDescription = Vertex::getBindingDescription();
        auto attributeDescription = Vertex::getAttributeDescriptions();

        // VkPipelineVertexInputStateCreateInfo Describes the format of the vertex data that will be passed to the vertex shader in these two ways:
        VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo.vertexBindingDescriptionCount = 1;
        vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
        vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescription.size());
        vertexInputInfo.pVertexAttributeDescriptions = attributeDescription.data();
          
        m_dynamicState.dynamicStateCount = static_cast<uint32_t>(m_dynamicStatesUsed.size());
        m_dynamicState.pDynamicStates = m_dynamicStatesUsed.data();
        m_viewport.width = (float)winSystem.GetExtent().width;
        m_viewport.height = (float)winSystem.GetExtent().height;
        m_scissor.extent = winSystem.GetExtent();           
        m_viewportState.pScissors = &m_scissor;
        m_multisampling.rasterizationSamples = renderPass.GetMsaa();
        m_colorBlending.pAttachments = &m_colorBlendAttachment;

        std::vector<VkDescriptorSetLayout> layouts(VM_MAX_FRAMES_IN_FLIGHT, descriptorSetLayout);
        m_pipelineLayoutInfo.pSetLayouts = layouts.data();
        m_pipelineLayoutInfo.pushConstantRangeCount = (uint32_t)m_pushRanges.size();
        m_pipelineLayoutInfo.pPushConstantRanges = m_pushRanges.data();

        if (vkCreatePipelineLayout(logicalDevice.GetDevice(), &m_pipelineLayoutInfo, nullptr, &m_pipelineLayout) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create pipeline layout!");
        }
        
        // More info here - https://vulkan-tutorial.com/en/Drawing_a_triangle/Graphics_pipeline_basics/Conclusion                     
        m_pipelineInfo.stageCount = 2;
        m_pipelineInfo.pStages = shaderStages;        
        m_pipelineInfo.pVertexInputState = &vertexInputInfo;                    
        m_pipelineInfo.renderPass = renderPass.GetRenderPass();
        m_pipelineInfo.subpass = 0;
        m_pipelineInfo.pInputAssemblyState = &m_inputAssembly;
        m_pipelineInfo.pViewportState = &m_viewportState;
        m_pipelineInfo.pRasterizationState = &m_rasterizer;
        m_pipelineInfo.pMultisampleState = &m_multisampling;
        m_pipelineInfo.pColorBlendState = &m_colorBlending;
        m_pipelineInfo.pDynamicState = &m_dynamicState;
        m_pipelineInfo.layout = m_pipelineLayout;
        m_pipelineInfo.pDepthStencilState = &m_depthStencil;

        if (vkCreateGraphicsPipelines(logicalDevice.GetDevice(), VK_NULL_HANDLE, 1, &m_pipelineInfo, nullptr, &m_graphicsPipeline) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create graphics pipeline.");
        }

        // Cleanup when pipeline is finished being created
        vkDestroyShaderModule(logicalDevice.GetDevice(), vertShaderModule, nullptr);
        vkDestroyShaderModule(logicalDevice.GetDevice(), fragShaderModule, nullptr);
    }

    VkPipeline& GraphicsPipeline::GetGraphicsPipeline()
    {
        return m_graphicsPipeline;
    }

    VkPipelineLayout& GraphicsPipeline::GetPipelineLayout()
    {
        return m_pipelineLayout;
    }

    VkPipelineInputAssemblyStateCreateInfo& GraphicsPipeline::GetInputAssemblyInfos()
    {
        return m_inputAssembly;
    }

    void GraphicsPipeline::SetInputAssemblyInfos(VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfos)
    {
        m_inputAssembly = inputAssemblyInfos;
    }

    VkPipelineRasterizationStateCreateInfo& GraphicsPipeline::GetRasterizerCreateInfos()
    {
        return m_rasterizer;
    }

    void GraphicsPipeline::SetRasterizerCreateInfos(VkPipelineRasterizationStateCreateInfo rasterizerInfos)
    {
        m_rasterizer = rasterizerInfos;
    }

}