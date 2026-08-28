#pragma once
#include "structs/CanvasPlacement.h"
#include "components/Component.h"
#include "render/Model.h"
#include "render/Material.h"
#include "tools/Vector4.h"

#include <map>
#include <memory>
#include <string>


namespace FlatEngine
{	
	class Transform;
	class Camera;

	class Mesh : public Component
	{
	public:
		Mesh(long ownerID = -1);		
		json GetData(bool b_IDOverride = false);
		void PutData(json componentJson, std::string objectName);
		void CleanupTextures();
		void CleanupUniformBuffers();
		void Cleanup();

		void Init();		
		bool Initialized();
		bool MissingTextures();		
		void SetModel(std::string modelPath, bool b_addMaterialMesh = true);
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
		void UpdateUniformBuffer(ViewportType viewportType, Transform* transform, Camera* camera, Transform* cameraTransform);
		std::vector<VkBuffer>& GetSceneViewUniformBuffers();
		std::vector<VkBuffer>& GetGameViewUniformBuffers();		
		std::map<std::string, glm::vec4>& GetUBOVec4s();
		void SetUBOVec4(std::string name, Vector4 value);
		void CreateUniformBuffers();
		Vector2 GetRenderScale();
		void SetTextureDimensions(Vector2 dimensions);

		CanvasPlacement canvasPlacement;

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
		Vector2 m_textureDimensions;

		// Uniform Buffer Data
		std::map<std::string, glm::vec4> m_uboVec4s;
	};
}