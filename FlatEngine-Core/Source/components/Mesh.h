#pragma once
#include "components/Component.h"
#include "render/Model.h"
#include "render/Material.h"
#include "tools/JsonHelper.h"
#include "tools/Vector4.h"

#include <map>
#include <memory>
#include <string>


namespace FlatEngine
{	
	class Transform;

	class Mesh : public Component
	{
	public:
		Mesh(long myID = -1, long parentObjectID = -1);		
		std::string GetData();
		void PutData(json componentJson);
		void CleanupTextures();
		void CleanupUniformBuffers();
		void Cleanup();

		void Init();		
		bool Initialized();
		bool MissingTextures();		
		void SetModel(std::string modelPath);
		std::shared_ptr<Model> GetModel();
		void SetMaterial(std::string materialName);
		std::shared_ptr<Material> GetSceneViewMaterial();
		std::shared_ptr<Material> GetGameViewMaterial();
		std::string GetMaterialName();
		void CreateResources();
		void AddTexture(std::string path, uint32_t index);
		void AddTexture(Texture texture, uint32_t index);
		void AddTextureLua(std::string path, int index);		
		std::map<uint32_t, Texture>& GetTextures();
		void CreateTextureResources();
		std::vector<VkDescriptorSet>& GetSceneViewDescriptorSets();
		std::vector<VkDescriptorSet>& GetGameViewDescriptorSets();
		std::vector<VkDescriptorSet>& GetEmptySceneViewDescriptorSets();
		std::vector<VkDescriptorSet>& GetEmptyGameViewDescriptorSets();
		void UpdateUniformBuffer(ViewportType viewportType, bool b_orthographic, Transform* transform = nullptr);
		std::vector<VkBuffer>& GetSceneViewUniformBuffers();
		std::vector<VkBuffer>& GetGameViewUniformBuffers();		
		std::map<std::string, glm::vec4>& GetUBOVec4s();
		void SetUBOVec4(std::string name, Vector4 value);
		void CreateUniformBuffers();

	private:
		

		std::shared_ptr<Model> m_model;
		std::string m_materialName;
		std::shared_ptr<Material> m_sceneViewMaterial;		
		std::shared_ptr<Material> m_gameViewMaterial;
		std::map<uint32_t, Texture> m_texturesByIndex;
		std::vector<VkDescriptorSet> m_sceneViewDescriptorSets;
		std::vector<VkDescriptorSet> m_gameViewDescriptorSets;
		std::vector<VkDescriptorSet> m_emptySceneViewDescriptorSets;
		std::vector<VkDescriptorSet> m_emptyGameViewDescriptorSets;
		std::vector<VkBuffer> m_sceneViewUniformBuffers;
		std::vector<VkDeviceMemory> m_sceneViewUniformBuffersMemory;
		std::vector<void*> m_sceneViewUniformBuffersMapped;
		std::vector<VkBuffer> m_gameViewUniformBuffers;
		std::vector<VkDeviceMemory> m_gameViewUniformBuffersMemory;
		std::vector<void*> m_gameViewUniformBuffersMapped;
		int m_allocationPoolIndex;
		bool m_b_initialized;
		bool m_b_missingTextures;

		// Uniform Buffer Data
		std::map<std::string, glm::vec4> m_uboVec4s;
	};
}