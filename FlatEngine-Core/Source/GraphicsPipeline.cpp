#include "GraphicsPipeline.h"
#include "Helper.h"
#include "VulkanManager.h"

#include <stdexcept>


namespace FlatEngine
{
    GraphicsPipeline::GraphicsPipeline(std::string vertexPath, std::string fragmentPath)
    {
        m_graphicsPipeline = VK_NULL_HANDLE;
        m_pipelineLayout = VK_NULL_HANDLE;
        SetVertexPath(vertexPath);
        SetFragmentPath(fragmentPath);
    }

    GraphicsPipeline::GraphicsPipeline()
    {
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

        // Need to assign shaders to whichever pipeline stage we want through this struct used to create the pipeline
        // VERT SHADER CODE
        VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
        vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT; // Stage to inject shader code
        vertShaderStageInfo.module = vertShaderModule; // Shader to invoke
        vertShaderStageInfo.pName = "main"; // Entry point
        vertShaderStageInfo.pSpecializationInfo = nullptr; // Allows you to specify values for shader constants to be plugged in at shader runtime
        // FRAG SHADER CODE
        VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
        fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        fragShaderStageInfo.module = fragShaderModule;
        fragShaderStageInfo.pName = "main";

        // Finish by defining an array that contains these two structs which we will use later when defining the pipeline
        VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

        // VkPipelineVertexInputStateCreateInfo Describes the format of the vertex data that will be passed to the vertex shader in these two ways:
        // 1. Bindings: spacing between data and whether the data is per - vertex or per - instance(see instancing)
        // 2. Attribute descriptions : type of the attributes passed to the vertex shader, which binding to load them from and at which offset

        auto bindingDescription = Vertex::getBindingDescription();
        auto attributeDescription = Vertex::getAttributeDescriptions();

        VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo.vertexBindingDescriptionCount = 1;
        vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
        vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescription.size());
        vertexInputInfo.pVertexAttributeDescriptions = attributeDescription.data();

        // Describes what kind of geometry will be drawn from the vertices and if primitive restart should be enabled
        // Options:
        // VK_PRIMITIVE_TOPOLOGY_POINT_LIST: points from vertices
        // VK_PRIMITIVE_TOPOLOGY_LINE_LIST : line from every 2 vertices without reuse
        // VK_PRIMITIVE_TOPOLOGY_LINE_STRIP : the end vertex of every line is used as start vertex for the next line
        // VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST : triangle from every 3 vertices without reuse
        // VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP : the second and third vertex of every triangle are used as first two vertices of the next triangle
        VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
        inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        inputAssembly.primitiveRestartEnable = VK_FALSE;

        // Describe viewport dimensions
        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = (float)winSystem.GetExtent().width;
        viewport.height = (float)winSystem.GetExtent().height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        // Any pixels outside the scissor rectangles will be discarded by the rasterizer. 
        // If we want to draw to the entire framebuffer, we specify a scissor rectangle that covers it entirely:
        VkRect2D scissor{};
        scissor.offset = { 0, 0 };
        scissor.extent = winSystem.GetExtent();

        // For creating dynamic pipeline that doesn't need to be fully recreated for certain values (ie. viewport size, line width, and blend constants)
        // Configuration of these values will be ignored and you will be able (and required) to specify the data at drawing time.
        std::vector<VkDynamicState> dynamicStates =
        {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR
        };
        VkPipelineDynamicStateCreateInfo dynamicState{};
        dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
        dynamicState.pDynamicStates = dynamicStates.data();

        VkPipelineViewportStateCreateInfo viewportState{};
        viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.viewportCount = 1;
        viewportState.scissorCount = 1;

        // Rasterizer - more info found here: https://vulkan-tutorial.com/en/Drawing_a_triangle/Graphics_pipeline_basics/Fixed_functions
        // NOTE FROM WIKI: The rasterizer takes the geometry that is shaped by the vertices from the vertex shader and turns it into fragments to be colored by the fragment shader. It also performs depth testing, face culling and the scissor test, and it can be configured to output fragments that fill entire polygons or just the edges (wireframe rendering).
        VkPipelineRasterizationStateCreateInfo rasterizer{};
        rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizer.depthClampEnable = VK_FALSE;
        rasterizer.rasterizerDiscardEnable = VK_FALSE;
        rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
        rasterizer.lineWidth = 1.0f;
        rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
        rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        rasterizer.depthBiasEnable = VK_FALSE;
        rasterizer.depthBiasConstantFactor = 0.0f; // Optional
        rasterizer.depthBiasClamp = 0.0f; // Optional
        rasterizer.depthBiasSlopeFactor = 0.0f; // Optional


        // Multisampling - one of the ways to perform anti-aliasing - Enabling it requires enabling a GPU feature.
        VkPipelineMultisampleStateCreateInfo multisampling{};
        multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.sampleShadingEnable = VK_TRUE; // VK_FALSE;
        multisampling.rasterizationSamples = renderPass.GetMsaa();
        multisampling.minSampleShading = 0.2f; // 1.0f; // Optional
        multisampling.pSampleMask = nullptr; // Optional
        multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
        multisampling.alphaToOneEnable = VK_FALSE; // Optional


        // Color blending - After a fragment shader has returned a color, it needs to be combined with the color that is already in the framebuffer.
        VkPipelineColorBlendAttachmentState colorBlendAttachment{};
        // Alpha blending: where we want the new color to be blended with the old color based on its opacity
        // finalColor.rgb = newAlpha * newColor + (1 - newAlpha) * oldColor;
        // finalColor.a = newAlpha.a;
        colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        colorBlendAttachment.blendEnable = VK_TRUE;
        colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
        colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
        colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

        VkPipelineColorBlendStateCreateInfo colorBlending{};
        colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlending.logicOpEnable = VK_FALSE;
        colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
        colorBlending.attachmentCount = 1;
        colorBlending.pAttachments = &colorBlendAttachment;
        colorBlending.blendConstants[0] = 0.0f; // Optional
        colorBlending.blendConstants[1] = 0.0f; // Optional
        colorBlending.blendConstants[2] = 0.0f; // Optional
        colorBlending.blendConstants[3] = 0.0f; // Optional


        // PUSH CONSTANTS
        // Pipeline Layout config
        VkPushConstantRange translateRange = {};
        translateRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
        translateRange.offset = 0;
        translateRange.size = (uint32_t)16;
        VkPushConstantRange modelRange = {};
        modelRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
        modelRange.offset = 64;
        modelRange.size = (uint32_t)64;
        VkPushConstantRange viewRange = {};
        viewRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
        viewRange.offset = 128;
        viewRange.size = (uint32_t)64;
        VkPushConstantRange projectionRange = {};
        projectionRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
        projectionRange.offset = 192;
        projectionRange.size = (uint32_t)64;

        std::vector<VkPushConstantRange> pushConstantsRanges = { translateRange, modelRange, viewRange, projectionRange };
        std::vector<VkDescriptorSetLayout> layouts(VM_MAX_FRAMES_IN_FLIGHT, descriptorSetLayout);
        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = VM_MAX_FRAMES_IN_FLIGHT;
        pipelineLayoutInfo.pSetLayouts = layouts.data();
        pipelineLayoutInfo.pushConstantRangeCount = pushConstantsRanges.size();
        pipelineLayoutInfo.pPushConstantRanges = pushConstantsRanges.data();


        if (vkCreatePipelineLayout(logicalDevice.GetDevice(), &pipelineLayoutInfo, nullptr, &m_pipelineLayout) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create pipeline layout!");
        }


        // CREATE GRAPHICS PIPELINE
        // More info here - https://vulkan-tutorial.com/en/Drawing_a_triangle/Graphics_pipeline_basics/Conclusion

        // Depth testing enable
        VkPipelineDepthStencilStateCreateInfo depthStencil{};
        depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        depthStencil.depthTestEnable = VK_TRUE;
        depthStencil.depthWriteEnable = VK_TRUE;
        depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
        depthStencil.depthBoundsTestEnable = VK_FALSE;
        depthStencil.minDepthBounds = 0.0f; // Optional
        depthStencil.maxDepthBounds = 1.0f; // Optional
        depthStencil.stencilTestEnable = VK_FALSE;
        depthStencil.front = {}; // Optional
        depthStencil.back = {}; // Optional

        // We start by referencing the array of VkPipelineShaderStageCreateInfo structs from way above.
        VkGraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.stageCount = 2;
        pipelineInfo.pStages = shaderStages;
        // Then we reference all of the structures describing the fixed-function stage.
        pipelineInfo.pVertexInputState = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &inputAssembly;
        pipelineInfo.pViewportState = &viewportState;
        pipelineInfo.pRasterizationState = &rasterizer;
        pipelineInfo.pMultisampleState = &multisampling;
        pipelineInfo.pDepthStencilState = nullptr; // Optional
        pipelineInfo.pColorBlendState = &colorBlending;
        pipelineInfo.pDynamicState = &dynamicState;
        // After that comes the pipeline layout, which is a Vulkan handle rather than a struct pointer.
        pipelineInfo.layout = m_pipelineLayout;
        // And finally we have the reference to the render pass and the index of the sub pass where this graphics pipeline will be used
        pipelineInfo.renderPass = renderPass.GetRenderPass();
        pipelineInfo.subpass = 0;
        // Vulkan allows you to create a new graphics pipeline by deriving from an existing pipeline (not using this)
        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
        pipelineInfo.basePipelineIndex = -1; // Optional
        // For Depth testing
        pipelineInfo.pDepthStencilState = &depthStencil;

        if (vkCreateGraphicsPipelines(logicalDevice.GetDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_graphicsPipeline) != VK_SUCCESS)
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
}