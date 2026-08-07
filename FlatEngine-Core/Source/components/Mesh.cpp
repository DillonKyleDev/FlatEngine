#include "components/Camera.h"
#include "components/Transform.h"
#include "components/Mesh.h"
#include "GameObject.h"
#include "managers/Assets.h"
#include "managers/SceneManager.h"
#include "render/DeviceManager.h"
#include "render/RenderWindow.h"
#include "render/SceneView.h"
#include "render/VulkanManager.h"
#include "tools/FileHelper.h"
#include "tools/Logger.h"
#include "tools/Vector3.h"

#include <glm/gtx/string_cast.hpp>


namespace FlatEngine
{
	Mesh::Mesh(long ownerID)
	{
		SetOwnerID(ownerID);
		SetType(ComponentType_Mesh);

		m_model = nullptr;
		m_sceneViewMaterial = VulkanManager::vulkan.GetMaterial("fl_empty");
		m_gameViewMaterial = VulkanManager::vulkan.GetMaterial("fl_empty");
		m_sceneViewDescriptorSets = std::vector<VkDescriptorSet>(VulkanManager::MAX_FRAMES_IN_FLIGHT, {});
		m_gameViewDescriptorSets = std::vector<VkDescriptorSet>(VulkanManager::MAX_FRAMES_IN_FLIGHT, {});
		m_emptySceneViewDescriptorSets = std::vector<VkDescriptorSet>(VulkanManager::MAX_FRAMES_IN_FLIGHT, {});
		m_emptyGameViewDescriptorSets = std::vector<VkDescriptorSet>(VulkanManager::MAX_FRAMES_IN_FLIGHT, {});
		m_texturesByIndex = std::map<uint32_t, Texture>();
		m_allocationPoolIndex = -1;
		m_b_initialized = false;
		m_b_missingTextures = false;
		
		m_uboVec4s = std::map<std::string, glm::vec4>();
	}

	json Mesh::GetData(bool b_IDOverride)
	{
		json texturesData = json::object();
		for (std::map<uint32_t, Texture>::iterator textureData = m_texturesByIndex.begin(); textureData != m_texturesByIndex.end(); textureData++)
		{
			texturesData.emplace(std::to_string(textureData->first), textureData->second.GetTexturePath());
		}

		std::string materialName = "";
		json uboVec4s = json::object();

		if (m_sceneViewMaterial != nullptr)
		{	
			std::map<uint32_t, std::string> vec4Names = m_sceneViewMaterial->GetUBOVec4Names();
			for (std::map<uint32_t, std::string>::iterator iter = vec4Names.begin(); iter != vec4Names.end(); iter++)
			{
				if (m_uboVec4s.count(iter->second))
				{
					glm::vec4 data = m_uboVec4s.at(iter->second);

					json vec4Data = {
						{ "x", data.x },
						{ "y", data.y },
						{ "z", data.z },
						{ "w", data.w }
					};

					uboVec4s.emplace(iter->second, vec4Data);
				}
			}
		}

		std::string modelPath = "";
		if (m_model != nullptr)
		{
			modelPath = m_model->GetModelPath();
		}

		json jsonData = {
			{ "type", (int)GetType() },
			{ "b_isCollapsed", IsCollapsed() },
			{ "b_isActive", IsActive() },
			{ "textures", texturesData },
			{ "materialName", m_materialName },
			{ "modelPath", modelPath },
			{ "uboVec4s", uboVec4s }
		};

		return jsonData;
	}

	void Mesh::PutData(json componentJson, std::string objectName)
	{
        Component::PutData(componentJson, objectName);

		std::string materialName = JsonHelper::CheckJsonString(componentJson, "materialName", objectName);
		std::string modelPath = JsonHelper::CheckJsonString(componentJson, "modelPath", objectName);

		CreateUniformBuffers();

		if (materialName != "")
		{
			SetMaterial(materialName);
		}
		if (FileHelper::DoesFileExist(modelPath))
		{
			SetModel(modelPath);
		}

		if (JsonHelper::JsonContains(componentJson, "textures", objectName))
		{
			json texturesShaderData = componentJson["textures"];

			if (texturesShaderData.size())
			{
				for (auto item = texturesShaderData.begin(); item != texturesShaderData.end(); ++item)
				{
					try
					{
						AddTexture(item.value(), (uint32_t)std::stoi(item.key()));
					}
					catch (const json::out_of_range& e)
					{
						Logger::log.Err("{}", e.what());
					}
				}
			}
		}

		std::shared_ptr<Material> material = VulkanManager::vulkan.GetMaterial(materialName);

		if (material != nullptr)
		{
			if (JsonHelper::JsonContains(componentJson, "uboVec4s", objectName))
			{
				std::map<uint32_t, std::string> uboVec4Names = material->GetUBOVec4Names();
				for (std::map<uint32_t, std::string>::iterator uboVec4Name = uboVec4Names.begin(); uboVec4Name != uboVec4Names.end(); uboVec4Name++)
				{
					try
					{
						json uboVec4Data = componentJson["uboVec4s"][uboVec4Name->second];
						Vector4 uboVec4 = Vector4(JsonHelper::CheckJsonFloat(uboVec4Data, "x", objectName), JsonHelper::CheckJsonFloat(uboVec4Data, "y", objectName), JsonHelper::CheckJsonFloat(uboVec4Data, "z", objectName), JsonHelper::CheckJsonFloat(uboVec4Data, "w", objectName));
						SetUBOVec4(uboVec4Name->second, uboVec4);
					}
					catch (const json::out_of_range& e)
					{
						Logger::log.Err("{}", e.what());
					}
				}										
			}
		}

		CreateResources();
    }

	void Mesh::CleanupTextures()
	{
		for (std::map<uint32_t, Texture>::iterator texture = m_texturesByIndex.begin(); texture != m_texturesByIndex.end(); texture++)
		{
			texture->second.Cleanup();
			int allocationIndex = texture->second.GetAllocationIndex();

			if (allocationIndex >= 0)
			{
				m_sceneViewMaterial->GetAllocator().SetFreed(allocationIndex);
				m_gameViewMaterial->GetAllocator().SetFreed(allocationIndex);
			}
		}
	}

	void Mesh::CleanupUniformBuffers()
	{
		for (size_t i = 0; i < VulkanManager::MAX_FRAMES_IN_FLIGHT; i++)
		{
			VulkanManager::vulkan.QueueBufferDeletion(m_sceneViewUniformBuffers[i]);
			VulkanManager::vulkan.QueueDeviceMemoryDeletion(m_sceneViewUniformBuffersMemory[i]);
			VulkanManager::vulkan.QueueBufferDeletion(m_gameViewUniformBuffers[i]);
			VulkanManager::vulkan.QueueDeviceMemoryDeletion(m_gameViewUniformBuffersMemory[i]);
		}
	}

	void Mesh::Cleanup()
	{				
		CleanupTextures();	
		CleanupUniformBuffers();
	}
	
	bool Mesh::Initialized()
	{
		return m_b_initialized;
	}

	bool Mesh::MissingTextures()
	{
		return m_b_missingTextures;
	}

	void Mesh::SetModel(std::string modelPath, bool b_addMaterialMesh)
	{
		std::shared_ptr<Model> loadedModel = VulkanManager::vulkan.GetModel(modelPath);

		if (loadedModel == nullptr)
		{
			loadedModel = VulkanManager::vulkan.LoadModel(modelPath);
		}

		m_model = loadedModel;

		// Already done by SetMaterial().
		// if (b_addMaterialMesh)
		// {
		// 	VulkanManager::vulkan.AddSceneViewMaterialMesh(m_materialName, GetOwnerID());
		// 	VulkanManager::vulkan.AddGameViewMaterialMesh(m_materialName, GetOwnerID());
		// }
	}

	std::shared_ptr<Model> Mesh::GetModel()
	{
		return m_model;
	}

	void Mesh::SetMaterial(std::string materialName)
	{
		m_sceneViewMaterial = VulkanManager::vulkan.GetMaterial(materialName, ViewportType::ViewportType_SceneView);
		m_gameViewMaterial = VulkanManager::vulkan.GetMaterial(materialName, ViewportType::ViewportType_GameView);

		if (m_materialName != "")
		{			
			VulkanManager::vulkan.RemoveSceneViewMaterialMesh(m_materialName, GetOwnerID());
			VulkanManager::vulkan.RemoveGameViewMaterialMesh(m_materialName, GetOwnerID());
		}
		m_materialName = materialName;

		if (GetOwnerID() != -1)
		{
			VulkanManager::vulkan.AddSceneViewMaterialMesh(m_materialName, GetOwnerID());
			VulkanManager::vulkan.AddGameViewMaterialMesh(m_materialName, GetOwnerID());
		}

		if (m_sceneViewMaterial != nullptr)
		{
			std::map<uint32_t, std::string> uboVec4Names = m_sceneViewMaterial->GetUBOVec4Names();

			for (std::map<uint32_t, std::string>::iterator iter = uboVec4Names.begin(); iter != uboVec4Names.end(); iter++)
			{
				SetUBOVec4(iter->second, Vector4(1));
			}
		}
		else
		{
			Logger::log.Err("Material not found: {}", materialName);
		}
	}

	std::shared_ptr<Material> Mesh::GetSceneViewMaterial()
	{
		return m_sceneViewMaterial;
	}

	std::shared_ptr<Material> Mesh::GetGameViewMaterial()
	{
		return m_gameViewMaterial;
	}

	std::string Mesh::GetMaterialName()
	{
		return m_materialName;
	}

	void Mesh::CreateResources()
	{
		m_b_missingTextures = false;
		m_b_initialized = true;

		if (m_sceneViewMaterial != nullptr && m_model != nullptr)
		{
			for (std::map<uint32_t, TexturePipelineData>::iterator iter = m_sceneViewMaterial->GetTexturesPipelineData()->begin(); iter != m_sceneViewMaterial->GetTexturesPipelineData()->end(); iter++)
			{
				if (iter->second.descriptorType != VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT)
				{
					if (!m_texturesByIndex.count(iter->first) || (m_texturesByIndex.count(iter->first) && (m_texturesByIndex.at(iter->first).GetTexturePath() == "" || m_texturesByIndex.at(iter->first).GetTexturePath() == Assets::assetManager.GetTextureObject("resourceNotPresent")->GetTexturePath())))
					{
						Texture emptyTexture = Texture();
						m_texturesByIndex.emplace(iter->first, emptyTexture);
						m_texturesByIndex.at(iter->first).LoadFromFile(Assets::assetManager.GetTextureObject("resourceNotPresent")->GetTexturePath());
						m_b_missingTextures = true;
					}
				}
			}

			CreateTextureResources();

			if (m_sceneViewMaterial != nullptr)
			{
				m_sceneViewMaterial->CreateDescriptorSets(m_sceneViewDescriptorSets, m_sceneViewUniformBuffers, m_texturesByIndex);
			}	
			if (m_gameViewMaterial != nullptr)
			{
				m_gameViewMaterial->CreateDescriptorSets(m_gameViewDescriptorSets, m_gameViewUniformBuffers, m_texturesByIndex);
			}

			// Create empty material descriptor sets for Scene View and Game View
			std::map<uint32_t, Texture> emptyTextures = std::map<uint32_t, Texture>();
			VulkanManager::vulkan.GetMaterial("fl_empty", ViewportType::ViewportType_SceneView)->CreateDescriptorSets(m_emptySceneViewDescriptorSets, m_sceneViewUniformBuffers, emptyTextures);
			VulkanManager::vulkan.GetMaterial("fl_empty", ViewportType::ViewportType_GameView)->CreateDescriptorSets(m_emptyGameViewDescriptorSets, m_gameViewUniformBuffers, emptyTextures);
		}
		else
		{
			m_b_initialized = false;
		}
	}

	void Mesh::AddTexture(std::string path, uint32_t index)
	{
		Texture newTexture = Texture();
		newTexture.SetTexturePath(path);

		if (m_texturesByIndex.count(index))
		{
			m_texturesByIndex.at(index) = newTexture;
		}
		else
		{
			m_texturesByIndex.emplace(index, newTexture);			
		}

		CreateTextureResources();
	}

	void Mesh::AddTexture(Texture texture, uint32_t index)
	{
		if (m_texturesByIndex.count(index))
		{
			m_texturesByIndex.at(index) = texture;			
		}
		else
		{
			m_texturesByIndex.emplace(index, texture);
		}

		CreateTextureResources();
	}

	void Mesh::AddTextureLua(std::string path, int index)
	{
		if (index >= 0)
		{
			AddTexture(path, index);
		}
		else
		{
			Logger::log.Err("Texture index must be positive.");
		}
	}

	std::map<uint32_t, Texture>& Mesh::GetTextures()
	{
		return m_texturesByIndex;
	}

	void Mesh::CreateTextureResources() // To be called once actual Textures with paths have been assigned to the Mesh
	{
		for (std::map<uint32_t, Texture>::iterator iter = m_texturesByIndex.begin(); iter != m_texturesByIndex.end(); iter++)
		{
			if (iter->second.GetTexturePath() == "" || !FileHelper::DoesFileExist(iter->second.GetTexturePath()))
			{				
				iter->second.LoadFromFile(Assets::assetManager.GetTextureObject("resourceNotPresent")->GetTexturePath());
			}
			
			iter->second.CreateTextureImage();
		}
	}

	std::vector<VkDescriptorSet>& Mesh::GetSceneViewDescriptorSets()
	{
		return m_sceneViewDescriptorSets;
	}

	std::vector<VkDescriptorSet>& Mesh::GetGameViewDescriptorSets()
	{
		return m_gameViewDescriptorSets;
	}

	std::vector<VkDescriptorSet>& Mesh::GetEmptySceneViewDescriptorSets()
	{
		return m_emptySceneViewDescriptorSets;
	}

	std::vector<VkDescriptorSet>& Mesh::GetEmptyGameViewDescriptorSets()
	{
		return m_emptyGameViewDescriptorSets;
	}

	void Mesh::CreateUniformBuffers()
	{
		// Refer to - https://vulkan-tutorial.com/en/Uniform_buffers/Descriptor_layout_and_buffer

		VkDeviceSize bufferSize = sizeof(CustomUBO);

		m_sceneViewUniformBuffers.resize(VulkanManager::MAX_FRAMES_IN_FLIGHT);
		m_sceneViewUniformBuffersMemory.resize(VulkanManager::MAX_FRAMES_IN_FLIGHT);
		m_sceneViewUniformBuffersMapped.resize(VulkanManager::MAX_FRAMES_IN_FLIGHT);

		m_gameViewUniformBuffers.resize(VulkanManager::MAX_FRAMES_IN_FLIGHT);
		m_gameViewUniformBuffersMemory.resize(VulkanManager::MAX_FRAMES_IN_FLIGHT);
		m_gameViewUniformBuffersMapped.resize(VulkanManager::MAX_FRAMES_IN_FLIGHT);

		for (size_t i = 0; i < VulkanManager::MAX_FRAMES_IN_FLIGHT; i++)
		{
			RenderWindow::window.CreateBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, m_sceneViewUniformBuffers[i], m_sceneViewUniformBuffersMemory[i]);
			vkMapMemory(DeviceManager::logicalDevice.GetDevice(), m_sceneViewUniformBuffersMemory[i], 0, bufferSize, 0, &m_sceneViewUniformBuffersMapped[i]);

			RenderWindow::window.CreateBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, m_gameViewUniformBuffers[i], m_gameViewUniformBuffersMemory[i]);
			vkMapMemory(DeviceManager::logicalDevice.GetDevice(), m_gameViewUniformBuffersMemory[i], 0, bufferSize, 0, &m_gameViewUniformBuffersMapped[i]);
		}
	}

	void Mesh::UpdateUniformBuffer(ViewportType viewportType, bool b_orthographic, Transform* transform)
	{
		GameObject* parent = SceneManager::loadedScene.GetObjectByID(GetOwnerID());		
		Camera* primaryCamera = &SceneView::sceneViewCamera;                        
		Vector3 cameraPosition = SceneView::sceneViewCameraTransform.GetPosition();
		std::map<uint32_t, std::string> materialVec4s;
		Transform empty;

		if (transform == nullptr && parent != nullptr)
			transform = parent->Get<Transform>();
		else if (transform == nullptr)
		 	transform = &empty; // Temporary fix for larger problem

		switch (viewportType)
		{
		case ViewportType::ViewportType_SceneView:
		{
			materialVec4s = m_sceneViewMaterial->GetUBOVec4Names();
			break;
		}
		case ViewportType::ViewportType_GameView:
		{
			primaryCamera = SceneManager::loadedScene.GetPrimaryCamera();
			if (primaryCamera == nullptr)
			{
				primaryCamera = &SceneView::sceneViewCamera;
				cameraPosition = Vector3();
			}
			else if (primaryCamera->GetOwningObject() != nullptr)
				cameraPosition = primaryCamera->GetOwningObject()->Get<Transform>()->GetPosition();
			
			materialVec4s = m_gameViewMaterial->GetUBOVec4Names();
			break;
		}
		default:
			break;
		}

		Vector3 meshPosition = transform->GetPosition();
		glm::mat4 meshScale = transform->GetScaleMatrix();
		glm::mat4 meshRotation = transform->GetRotationMatrix();
		bool b_forceZUp = primaryCamera->ForceZUp();
		glm::vec4 lookDir = viewportType == ViewportType::ViewportType_SceneView || !primaryCamera->IsPrimary() ? primaryCamera->GetLookDirectionNoRoll() : primaryCamera->GetLookDirection();
		glm::vec4 up = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);

		glm::vec4 meshPos = glm::vec4(meshPosition.x, meshPosition.y, meshPosition.z, 0);
		glm::vec4 viewportCameraPos = glm::vec4(cameraPosition.x, cameraPosition.y, cameraPosition.z, 0);
		glm::mat4 model = meshRotation * meshScale;
		glm::vec4 cameraLookDir = glm::vec4(lookDir.x, lookDir.y, lookDir.z, 0);
		glm::mat4 view = glm::lookAt(cameraPosition.GetGLMVec3(), glm::vec3(cameraPosition.x + cameraLookDir.x, cameraPosition.y + cameraLookDir.y, cameraPosition.z + cameraLookDir.z), glm::vec3(up));

		glm::mat4 projection;			
		float aspectRatio = 16.0f / 9.0f; // (float)RenderWindow::window.GetExtent().width / (float)RenderWindow::window.GetExtent().height;
		float nearClip = primaryCamera->GetNearClippingDistance();
		float farClip = primaryCamera->GetFarClippingDistance();


		if (b_orthographic)
		{    		
			float halfWidth  = (float)primaryCamera->m_orthoSize * aspectRatio;
			float halfHeight = (float)primaryCamera->m_orthoSize;
			projection = glm::ortho(-halfWidth, halfWidth, -halfHeight, halfHeight, -500.0f, 500.0f);			
			projection[1][1] *= -1;
		}
		else
		{
			float perspectiveAngle = primaryCamera->GetPerspectiveAngle();
			projection = glm::perspective(glm::radians(perspectiveAngle), aspectRatio, nearClip, farClip);
			projection[1][1] *= -1;
		}

		CustomUBO ubo{};

		BaseUBO base{};
		base.meshPosition = meshPos;
		base.cameraPosition = viewportCameraPos;
		base.model = model;
		base.view = view;
		base.projection = projection;
		ubo.baseUBO = base;
		
		int vec4Counter = 0;
		for (std::map<uint32_t, std::string>::iterator materialVec4 = materialVec4s.begin(); materialVec4 != materialVec4s.end(); materialVec4++)
		{
			if (materialVec4->first <= 32 && m_uboVec4s.count(materialVec4->second)) // FIX ME: 32 is the size of the m_uboVec4s array passed to the shaders
			{
				ubo.vec4s[materialVec4->first] = m_uboVec4s.at(materialVec4->second);
				vec4Counter++;
			}
		}

		switch (viewportType)
		{
		case ViewportType::ViewportType_SceneView:
		{
			memcpy(m_sceneViewUniformBuffersMapped[VulkanManager::currentFrame], &ubo, sizeof(ubo));
			break;
		}
		case ViewportType::ViewportType_GameView:
		{
			memcpy(m_gameViewUniformBuffersMapped[VulkanManager::currentFrame], &ubo, sizeof(ubo));
			break;
		}
		default:
			break;
		}
	}

	std::vector<VkBuffer>& Mesh::GetSceneViewUniformBuffers()
	{
		return m_sceneViewUniformBuffers;
	}

	std::vector<VkBuffer>& Mesh::GetGameViewUniformBuffers()
	{
		return m_gameViewUniformBuffers;
	}

	std::map<std::string, glm::vec4>& Mesh::GetUBOVec4s()
	{
		return m_uboVec4s;
	}

	void Mesh::SetUBOVec4(std::string name, Vector4 value)
	{
		if (m_uboVec4s.count(name))
		{
			m_uboVec4s.at(name) = glm::vec4(value.x, value.y, value.z, value.w);
		}
		else
		{
			m_uboVec4s.emplace(name, glm::vec4(value.x, value.y, value.z, value.w));
		}
	}
}