#include "Model.h"
#include "VulkanManager.h"
#include "Camera.h"
#include "Transform.h"
#include "FlatEngine.h"

#define TINYOBJLOADER_IMPLEMENTATION // Loading obj files
#include "tiny_obj_loader.h"

#include <stdexcept>
#include <chrono>


namespace FlatEngine
{
    Model::Model()
    {
        m_modelPath = "";
        m_vertices = std::vector<Vertex>();
        m_indices = std::vector<uint32_t>();
        m_vertexBuffer = VK_NULL_HANDLE;
        m_indexBuffer = VK_NULL_HANDLE;
        m_indexBufferMemory = VK_NULL_HANDLE;
        m_vertexBufferMemory = VK_NULL_HANDLE;
        m_uniformBuffers = std::vector<VkBuffer>();
        m_uniformBuffersMemory = std::vector<VkDeviceMemory>();
        m_uniformBuffersMapped = std::vector<void*>();
    }

    Model::~Model()
    {
    }

    void Model::CleanupUniformBuffers(LogicalDevice& logicalDevice)
    {
        for (size_t i = 0; i < VM_MAX_FRAMES_IN_FLIGHT; i++)
        {
            vkDestroyBuffer(logicalDevice.GetDevice(), m_uniformBuffers[i], nullptr);
            vkFreeMemory(logicalDevice.GetDevice(), m_uniformBuffersMemory[i], nullptr);
        }
    }

    void Model::CleanupIndexBuffers(LogicalDevice& logicalDevice)
    {
        vkDestroyBuffer(logicalDevice.GetDevice(), m_indexBuffer, nullptr);
        vkFreeMemory(logicalDevice.GetDevice(), m_indexBufferMemory, nullptr);
    }

    void Model::CleanupVertexBuffers(LogicalDevice& logicalDevice)
    {
        vkDestroyBuffer(logicalDevice.GetDevice(), m_vertexBuffer, nullptr);
        vkFreeMemory(logicalDevice.GetDevice(), m_vertexBufferMemory, nullptr);
    }

    void Model::Cleanup(LogicalDevice& logicalDevice)
    {
        CleanupUniformBuffers(logicalDevice);
        CleanupIndexBuffers(logicalDevice);
        CleanupVertexBuffers(logicalDevice);
    }


    void Model::SetModelPath(std::string path)
    {
        m_modelPath = path;
    }

    std::string Model::GetModelPath()
    {
        return m_modelPath;
    }

    void Model::LoadModel()
    {
        // Refer to - https://vulkan-tutorial.com/en/Loading_models

        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string warn, err;

        std::unordered_map<Vertex, uint32_t> uniqueVertices{};

        if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, m_modelPath.c_str()))
        {
            throw std::runtime_error(warn + err);
        }

        m_vertices.clear();
        m_indices.clear();

        for (const auto& shape : shapes)
        {
            // For mesh inputs (triangles)
            if (shape.mesh.indices.size())
            {
                for (const auto& index : shape.mesh.indices)
                {
                    Vertex vertex{};

                    vertex.pos =
                    {
                        attrib.vertices[3 * index.vertex_index + 0],
                        attrib.vertices[3 * index.vertex_index + 1],
                        attrib.vertices[3 * index.vertex_index + 2]
                    };

                    vertex.texCoord =
                    {
                        attrib.texcoords[2 * index.texcoord_index + 0],
                        1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
                    };

                    vertex.color = { 1.0f, 1.0f, 1.0f };

                    vertex.normal =
                    {
                        attrib.normals[3 * index.normal_index + 0],
                        attrib.normals[3 * index.normal_index + 1],
                        attrib.normals[3 * index.normal_index + 2]
                    };

                    // Keep only unique vertices
                    if (uniqueVertices.count(vertex) == 0)
                    {
                        uniqueVertices[vertex] = static_cast<uint32_t>(m_vertices.size());
                        m_vertices.push_back(vertex);
                    }

                    m_indices.push_back(uniqueVertices[vertex]);
                }
            }
            // For line inputs
            else
            {
                for (const auto& index : shape.lines.indices)
                {
                    Vertex vertex{};

                    vertex.pos =
                    {
                        attrib.vertices[3 * index.vertex_index + 0],
                        attrib.vertices[3 * index.vertex_index + 1],
                        attrib.vertices[3 * index.vertex_index + 2]
                    };

                    if (attrib.texcoords.size())
                    {
                        vertex.texCoord =
                        {
                            attrib.texcoords[2 * index.texcoord_index + 0],
                            1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
                        };
                    }

                    vertex.color = { 1.0f, 1.0f, 1.0f };

                    if (attrib.normals.size())
                    {
                        vertex.normal =
                        {
                            attrib.normals[3 * index.normal_index + 0],
                            attrib.normals[3 * index.normal_index + 1],
                            attrib.normals[3 * index.normal_index + 2]
                        };
                    }

                    // Keep only unique vertices
                    if (uniqueVertices.count(vertex) == 0)
                    {
                        uniqueVertices[vertex] = static_cast<uint32_t>(m_vertices.size());
                        m_vertices.push_back(vertex);
                    }

                    m_indices.push_back(uniqueVertices[vertex]);
                }
            }
        }
    }

    void Model::CreateVertexBuffer(VkCommandPool commandPool, PhysicalDevice& physicalDevice, LogicalDevice& logicalDevice)
    {
        // Create staging buffer for control from the cpu
        // Refer to - https://vulkan-tutorial.com/en/Vertex_buffers/Staging_buffer

        VkDeviceSize bufferSize = sizeof(m_vertices[0]) * m_vertices.size();

        VkBuffer stagingBuffer{};
        VkDeviceMemory stagingBufferMemory{};
        WinSys::CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory, physicalDevice, logicalDevice);

        void* data;
        vkMapMemory(logicalDevice.GetDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, m_vertices.data(), (size_t)bufferSize);
        vkUnmapMemory(logicalDevice.GetDevice(), stagingBufferMemory);

        // Create device local vertex buffer for actual buffer
        WinSys::CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_vertexBuffer, m_vertexBufferMemory, physicalDevice, logicalDevice);

        // We can now call copyBuffer function to move the vertex data to the device local buffer:
        WinSys::CopyBuffer(stagingBuffer, m_vertexBuffer, bufferSize, commandPool, logicalDevice);

        // After copying the data from the staging buffer to the device buffer, we should clean it up:
        vkDestroyBuffer(logicalDevice.GetDevice(), stagingBuffer, nullptr);
        vkFreeMemory(logicalDevice.GetDevice(), stagingBufferMemory, nullptr);
    }

    void Model::CreateIndexBuffer(VkCommandPool commandPool, PhysicalDevice& physicalDevice, LogicalDevice& logicalDevice)
    {
        // Refer to - https://vulkan-tutorial.com/en/Vertex_buffers/Index_buffer

        VkDeviceSize bufferSize = sizeof(m_indices[0]) * m_indices.size();

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;
        WinSys::CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory, physicalDevice, logicalDevice);

        void* data;
        vkMapMemory(logicalDevice.GetDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, m_indices.data(), (size_t)bufferSize);
        vkUnmapMemory(logicalDevice.GetDevice(), stagingBufferMemory);

        WinSys::CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_indexBuffer, m_indexBufferMemory, physicalDevice, logicalDevice);

        WinSys::CopyBuffer(stagingBuffer, m_indexBuffer, bufferSize, commandPool, logicalDevice);

        vkDestroyBuffer(logicalDevice.GetDevice(), stagingBuffer, nullptr);
        vkFreeMemory(logicalDevice.GetDevice(), stagingBufferMemory, nullptr);
    }

    void Model::CreateUniformBuffers(PhysicalDevice& physicalDevice, LogicalDevice& logicalDevice)
    {
        // Refer to - https://vulkan-tutorial.com/en/Uniform_buffers/Descriptor_layout_and_buffer

        VkDeviceSize bufferSize = sizeof(UniformBufferObject);

        m_uniformBuffers.resize(VM_MAX_FRAMES_IN_FLIGHT);
        m_uniformBuffersMemory.resize(VM_MAX_FRAMES_IN_FLIGHT);
        m_uniformBuffersMapped.resize(VM_MAX_FRAMES_IN_FLIGHT);

        for (size_t i = 0; i < VM_MAX_FRAMES_IN_FLIGHT; i++)
        {
            WinSys::CreateBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, m_uniformBuffers[i], m_uniformBuffersMemory[i], physicalDevice, logicalDevice);

            vkMapMemory(logicalDevice.GetDevice(), m_uniformBuffersMemory[i], 0, bufferSize, 0, &m_uniformBuffersMapped[i]);
        }
    }

    void Model::UpdateUniformBuffer(uint32_t currentImage, WinSys& winSystem, float multiplier)
    {
        // No longer using UBO for this data, switched to push constants
        //Camera* primaryCamera = FlatEngine::GetPrimaryCamera();
        //Vector3 cameraPosition = primaryCamera->GetParent()->GetTransform()->GetPosition();
        //glm::vec3 cameraPos = glm::vec3(cameraPosition.x, cameraPosition.y, cameraPosition.z);
        //glm::vec3 up = glm::vec3(0.0f, 0.0f, 1.0f);
        //glm::vec3 center = glm::vec3(0.0f, 0.0f, 0.0f);
        //Vector3 lookDir = Vector3(1.0f, 0.0f, 0.0f);

        //UniformBufferObject ubo{};
        ////ubo.model = glm::rotate(glm::mat4(1.0f), glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f)); // Creates a vec4 that is used to matrix multiply by the positions of each vertex.
        //ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        //ubo.proj = glm::perspective(glm::radians(90.0f), winSystem.GetExtent().width / (float)winSystem.GetExtent().height, 0.1f, 10.0f);
        //ubo.proj[1][1] *= -1;

        //memcpy(m_uniformBuffersMapped[currentImage], &ubo, sizeof(ubo));

        // This function will generate a new transformation every frame to make the geometry spin around.We need to include two new headers to implement this functionality:
        static auto startTime = std::chrono::high_resolution_clock::now();

        auto currentTime = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

        UniformBufferObject ubo{};
        ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f)); // Creates a vec4 that is used to matrix multiply by the positions of each vertex.
        ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        ubo.proj = glm::perspective(glm::radians(90.0f), winSystem.GetExtent().width / (float)winSystem.GetExtent().height, 0.1f, 10.0f);
        ubo.proj[1][1] *= -1;

        memcpy(m_uniformBuffersMapped[currentImage], &ubo, sizeof(ubo));
    }

    void Model::UpdateUniformBufferObject(glm::mat4 transformationMatrix, glm::vec3 cameraPos, glm::vec3 center, WinSys& winSystem, uint32_t currentImage)
    {        
        // This function will generate a new transformation every frame to make the geometry spin around.We need to include two new headers to implement this functionality:
        static auto startTime = std::chrono::high_resolution_clock::now();

        auto currentTime = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
        glm::vec3 up = glm::vec3(0.0f, 0.0f, 1.0f);

        UniformBufferObject ubo{};
        ubo.model = transformationMatrix;
        ubo.view = glm::lookAt(cameraPos, center, up);
        ubo.proj = glm::perspective(glm::radians(45.0f), winSystem.GetExtent().width / (float)winSystem.GetExtent().height, 0.1f, 10.0f);
        ubo.proj[1][1] *= -1;

        memcpy(m_uniformBuffersMapped[currentImage], &ubo, sizeof(ubo));
    }

    std::vector<VkBuffer>& Model::GetUniformBuffers()
    {
        return m_uniformBuffers;
    }

    VkBuffer& Model::GetVertexBuffer()
    {
        return m_vertexBuffer;
    }

    VkBuffer& Model::GetIndexBuffer()
    {
        return m_indexBuffer;
    }

    std::vector<uint32_t>& Model::GetIndices()
    {
        return m_indices;
    }
}