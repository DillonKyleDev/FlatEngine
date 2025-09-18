#pragma once
#include "Structs.h"
#include "PhysicalDevice.h"
#include "LogicalDevice.h"
#include "WinSys.h"

#include <vector>


namespace FlatEngine
{
	class Mesh;

	class Model
	{
	public:
		Model();
		~Model();
		void CleanupUniformBuffers(LogicalDevice& logicalDevice);
		void CleanupIndexBuffers(LogicalDevice& logicalDevice);
		void CleanupVertexBuffers(LogicalDevice& logicalDevice);
		void Cleanup(LogicalDevice& logicalDevice);

		void SetModelPath(std::string path);
		std::string GetModelPath();
		void LoadModel();
		void CreateVertexBuffer(VkCommandPool commandPool, PhysicalDevice& physicalDevice, LogicalDevice& logicalDevice);
		void CreateIndexBuffer(VkCommandPool commandPool, PhysicalDevice& physicalDevice, LogicalDevice& logicalDevice);
		void CreateUniformBuffers(PhysicalDevice& physicalDevice, LogicalDevice& logicalDevice);
		void UpdateUniformBuffer(WinSys& winSystem, Mesh* mesh, ViewportType viewport);
		std::vector<VkBuffer>& GetUniformBuffers();
		VkBuffer& GetVertexBuffer();
		VkBuffer& GetIndexBuffer();
		std::vector<uint32_t>& GetIndices();

	private:
		std::string m_modelPath;
		std::vector<Vertex> m_vertices;
		std::vector<uint32_t> m_indices;
		VkBuffer m_vertexBuffer;
		VkBuffer m_indexBuffer;
		VkDeviceMemory m_vertexBufferMemory;
		VkDeviceMemory m_indexBufferMemory;
		std::vector<VkBuffer> m_uniformBuffers;
		std::vector<VkDeviceMemory> m_uniformBuffersMemory;
		std::vector<void*> m_uniformBuffersMapped;
		WinSys* m_winSystem;
	};
}

