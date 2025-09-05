#pragma once
#include "Component.h"
#include "Model.h"
#include "Material.h"
#include "PhysicalDevice.h"
#include "LogicalDevice.h"
#include "RenderPass.h"

#include <memory>
#include <string>


namespace FlatEngine
{
	class Mesh : public Component
	{
	public:
		Mesh(long myID = -1, long parentID = -1);
		~Mesh();
		std::string GetData();
		void CleanupTextures();
		void Cleanup();

		bool Initialized();
		void SetHandles(VkInstance instance, WinSys& winSystem, PhysicalDevice& physicalDevice, LogicalDevice& logicalDevice, RenderPass& renderPass, VkCommandPool& commandPool);
		void SetModel(Model model);
		Model& GetModel();
		void CreateModelResources(VkCommandPool commandPool, PhysicalDevice& physicalDevice, LogicalDevice& logicalDevice);
		void SetMaterial(std::shared_ptr<Material> material);
		void SetMaterial(std::string materialName);
		std::shared_ptr<Material> GetMaterial();
		void CreateResources();
		void AddTexture(std::string path);
		void AddTexture(Texture texture);
		std::vector<Texture>& GetTextures();
		void CreateTextureResources();
		std::vector<VkDescriptorSet>& GetDescriptorSets();

	private:
		Model m_model;
		std::shared_ptr<Material> m_material;
		std::vector<Texture> m_textures;
		std::vector<VkDescriptorSet> m_descriptorSets;
		int m_allocationPoolIndex;
		bool m_b_initialized;
		// handles
		VkInstance m_instanceHandle;
		WinSys* m_winSystem;
		PhysicalDevice* m_physicalDeviceHandle;
		LogicalDevice* m_deviceHandle;
		RenderPass* m_renderPass;
		VkCommandPool* m_commandPool;
	};
}