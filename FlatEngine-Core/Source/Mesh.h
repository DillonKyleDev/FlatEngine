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
	class ViewportManager;

	class Mesh : public Component
	{
	public:
		Mesh(long myID = -1, long parentID = -1);
		~Mesh();
		std::string GetData();
		void CleanupTextures();
		void Cleanup();

		bool Initialized();		
		void SetModel(Model model);
		void SetModel(std::string modelPath);
		Model& GetModel();
		void CreateModelResources(VkCommandPool commandPool, PhysicalDevice& physicalDevice, LogicalDevice& logicalDevice);
		void SetMaterial(std::shared_ptr<Material> material);
		void SetMaterial(std::string materialName);
		std::shared_ptr<Material> GetMaterial();
		std::string GetMaterialName();
		void CreateResources();
		void AddTexture(std::string path, uint32_t index);
		void AddTexture(Texture texture, uint32_t index);
		std::vector<Texture>& GetTextures();
		void CreateTextureResources();
		std::vector<VkDescriptorSet>& GetDescriptorSets();

	private:
		Model m_model;
		std::string m_materialName;
		std::shared_ptr<Material> m_material;
		std::vector<Texture> m_textures;
		std::vector<VkDescriptorSet> m_descriptorSets;
		int m_allocationPoolIndex;
		bool m_b_initialized;
		// handles
		LogicalDevice* m_logicalDevice;
		ViewportManager* m_viewportManager;
	};
}