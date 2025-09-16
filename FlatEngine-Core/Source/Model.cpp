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
        m_winSystem = &F_VulkanManager->GetWinSystem();
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
            // For line inputs.. TODO: Should query topology member in PipelineManager and switch
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
        m_winSystem->CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

        void* data;
        vkMapMemory(logicalDevice.GetDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, m_vertices.data(), (size_t)bufferSize);
        vkUnmapMemory(logicalDevice.GetDevice(), stagingBufferMemory);

        // Create device local vertex buffer for actual buffer
        m_winSystem->CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_vertexBuffer, m_vertexBufferMemory);

        // We can now call copyBuffer function to move the vertex data to the device local buffer:
        m_winSystem->CopyBuffer(stagingBuffer, m_vertexBuffer, bufferSize);

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
        m_winSystem->CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

        void* data;
        vkMapMemory(logicalDevice.GetDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, m_indices.data(), (size_t)bufferSize);
        vkUnmapMemory(logicalDevice.GetDevice(), stagingBufferMemory);

        m_winSystem->CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_indexBuffer, m_indexBufferMemory);

        m_winSystem->CopyBuffer(stagingBuffer, m_indexBuffer, bufferSize);

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
            m_winSystem->CreateBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, m_uniformBuffers[i], m_uniformBuffersMemory[i]);

            vkMapMemory(logicalDevice.GetDevice(), m_uniformBuffersMemory[i], 0, bufferSize, 0, &m_uniformBuffersMapped[i]);
        }
    }

    void Model::UpdateUniformBuffer(uint32_t currentImage, WinSys& winSystem, Mesh* mesh, ViewportType viewport)
    {        
        Transform* transform = mesh->GetParent()->GetTransform();
        Vector3 meshPosition = transform->GetPosition();
        glm::mat4 meshScale = transform->GetScaleMatrix();
        glm::mat4 meshRotation = transform->GetRotationMatrix();
        Camera* primaryCamera = nullptr;
        Vector3 cameraPosition = Vector3();

        switch (viewport)
        {
        case ViewportType::SceneView:
            primaryCamera = F_sceneViewCameraObject.GetCamera();
            cameraPosition = F_sceneViewCameraObject.GetTransform()->GetPosition();
            break;
        case ViewportType::GameView:
            primaryCamera = GetPrimaryCamera();
            if (primaryCamera != nullptr)
            {
                cameraPosition = primaryCamera->GetParent()->GetTransform()->GetPosition();
            }
        default:
            break;
        }
                 
        if (primaryCamera != nullptr)
        {
            Vector3 lookDir = primaryCamera->GetLookDirection();
            float nearClip = primaryCamera->GetNearClippingDistance();
            float farClip = primaryCamera->GetFarClippingDistance();
            float perspectiveAngle = primaryCamera->GetPerspectiveAngle();
            glm::vec3 up = glm::vec3(0.0f, 0.0f, 1.0f);

            glm::vec4 meshPos = glm::vec4(meshPosition.x, meshPosition.y, meshPosition.z, 0);
            glm::vec4 viewportCameraPos = glm::vec4(cameraPosition.x, cameraPosition.y, cameraPosition.z, 0);
            glm::mat4 model = meshRotation * meshScale;
            glm::vec4 cameraLookDir = glm::vec4(lookDir.x, lookDir.y, lookDir.z, 0);
            glm::mat4 view = glm::lookAt(cameraPosition.GetGLMVec3(), glm::vec3(cameraPosition.x + cameraLookDir.x, cameraPosition.y + cameraLookDir.y, cameraPosition.z + cameraLookDir.z), up); // Look at camera direction not working right...
            float aspectRatio = (float)(winSystem.GetExtent().width / winSystem.GetExtent().height);
            glm::mat4 projection = glm::perspective(glm::radians(perspectiveAngle), aspectRatio, nearClip, farClip);
            projection[1][1] *= -1;

            UniformBufferObject ubo{};
            ubo.meshPosition = meshPos;
            ubo.cameraPosition = viewportCameraPos;
            ubo.model = model;
            ubo.viewAndProjection = projection * view;
            ubo.time = (glm::float32)((glm::float32)GetEngineTime() / 1000.0f);
            memcpy(m_uniformBuffersMapped[currentImage], &ubo, sizeof(ubo));
        }
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