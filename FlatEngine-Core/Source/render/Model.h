#pragma once
#include "render/Structs.h"

#include <string>
#include <vector>


namespace FlatEngine
{
	class Model
	{
	public:
		Model();
		void CleanupIndexBuffers();
		void CleanupVertexBuffers();
		void Cleanup();

		void SetModelPath(std::string path);
		std::string GetModelPath();
		void Init(VkCommandPool* commandPool);
		void LoadModel(std::string path);
		void CreateResources();
		void CreateVertexBuffer();
		void CreateIndexBuffer();
		VkBuffer& GetVertexBuffer();
		VkBuffer& GetIndexBuffer();
		std::vector<Vertex> GetVertices();
		std::vector<uint32_t> GetIndices();

	private:
		std::string m_modelPath;
		std::vector<Vertex> m_vertices;
		std::vector<uint32_t> m_indices;
		VkBuffer m_vertexBuffer;
		VkBuffer m_indexBuffer;
		VkDeviceMemory m_vertexBufferMemory;
		VkDeviceMemory m_indexBufferMemory;
		VkCommandPool* m_commandPool;
	};
}

