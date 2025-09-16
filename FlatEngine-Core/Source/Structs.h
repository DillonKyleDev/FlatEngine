#pragma once

#include "vulkan/vulkan_core.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm.hpp>
#include <gtx/hash.hpp>

#include <array>
#include <optional>
#include <vector>


namespace FlatEngine
{
    enum ViewportType {
        None,
        SceneView,
        GameView,
    };

    struct QueueFamilyIndices
    {
        std::optional<uint32_t> graphicsFamily; // Make sure graphics can be rendered
        std::optional<uint32_t> presentFamily; // Make sure device can present images to the surface we created

        bool isComplete()
        {
            return graphicsFamily.has_value() && presentFamily.has_value();
        }
    };

    struct SwapChainSupportDetails
    {
        // Creating a swap chain involves a lot more settings than instance and device creation, so we need to query for some more details from our device before we're able to proceed.
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };

    struct Vertex
    {
        glm::vec3 pos;
        glm::vec3 color;
        glm::vec2 texCoord;
        glm::vec3 normal;

        bool operator==(const Vertex& other) const
        {
            return pos == other.pos && color == other.color && texCoord == other.texCoord && normal == other.normal;
        }

        // Bindings: spacing between data and whether the data is per - vertex or per - instance(see instancing)
        static VkVertexInputBindingDescription getBindingDescription()
        {
            VkVertexInputBindingDescription bindingDescription{};
            bindingDescription.binding = 0;
            bindingDescription.stride = sizeof(Vertex);
            bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

            return bindingDescription;
        }
        // Attribute descriptions : type of the attributes passed to the vertex shader, which binding to load them from and at which offset
        static std::array<VkVertexInputAttributeDescription, 4> getAttributeDescriptions()
        {
            std::array<VkVertexInputAttributeDescription, 4> attributeDescriptions{};

            // 
            // float: VK_FORMAT_R32_SFLOAT
            // vec2 : VK_FORMAT_R32G32_SFLOAT
            // vec3 : VK_FORMAT_R32G32B32_SFLOAT
            // vec4 : VK_FORMAT_R32G32B32A32_SFLOAT

            attributeDescriptions[0].binding = 0;
            attributeDescriptions[0].location = 0;
            attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
            attributeDescriptions[0].offset = offsetof(Vertex, pos);

            attributeDescriptions[1].binding = 0;
            attributeDescriptions[1].location = 1;
            attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
            attributeDescriptions[1].offset = offsetof(Vertex, color);

            attributeDescriptions[2].binding = 0;
            attributeDescriptions[2].location = 2;
            attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
            attributeDescriptions[2].offset = offsetof(Vertex, texCoord);

            attributeDescriptions[3].binding = 0;
            attributeDescriptions[3].location = 3;
            attributeDescriptions[3].format = VK_FORMAT_R32G32B32_SFLOAT;
            attributeDescriptions[3].offset = offsetof(Vertex, normal);

            return attributeDescriptions;
        }
    };

    struct UniformBufferObject {
        glm::vec4 meshPosition;
        glm::vec4 cameraPosition;
        glm::mat4 model;
        glm::mat4 viewAndProjection;             
        glm::float32 time;
    };

    struct PushConstants {
        glm::vec4 lightDirection;
    };

    struct SamplerProps {
        uint32_t mipLevels;
        VkFilter filter;
    };
}

// Refer to - https://vulkan-tutorial.com/en/Loading_models
// and https://vulkan-tutorial.com/Loading_models#page_Vertex-deduplication at the bottom
template<> struct std::hash<FlatEngine::Vertex>
{
    size_t operator()(FlatEngine::Vertex const& vertex) const
    {
        return ((std::hash<glm::vec3>()(vertex.pos) ^
            (std::hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^
            (std::hash<glm::vec2>()(vertex.texCoord) << 1) ^
            (std::hash<glm::vec3>()(vertex.normal) << 1);
    }
};


// Shader ubo alignment cheatsheet
// 
// Vulkan expects the data in your structure to be aligned in memory in a specific way, for example:

// Scalars have to be aligned by N(= 4 bytes given 32 bit floats).
// A vec2 must be aligned by 2N(= 8 bytes)
// A vec3 or vec4 must be aligned by 4N(= 16 bytes)
// A nested structure must be aligned by the base alignment of its members rounded up to a multiple of 16.
// A mat4 matrix must have the same alignment as a vec4.

// You can find the full list of alignment requirements in the specification.

// Our original shader with just three mat4 fields already met the alignment requirements.As each mat4 is 4 x 4 x 4 = 64 bytes in size, model has an offset of 0, view has an offset of 64 and proj has an offset of 128. All of these are multiples of 16 and that's why it worked fine.

// The new structure starts with a vec2 which is only 8 bytes in size and therefore throws off all of the offsets.Now model has an offset of 8, view an offset of 72 and proj an offset of 136, none of which are multiples of 16. To fix this problem we can use the alignas specifier introduced in C++11:

// struct UniformBufferObject {
//     glm::vec2 foo;
//     alignas(16) glm::mat4 model;
//     glm::mat4 view;
//     glm::mat4 proj;
// };

// If you now compile and run your program again you should see that the shader correctly receives its matrix values once again.