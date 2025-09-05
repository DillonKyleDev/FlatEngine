#include "Mesh.h"
#include "FlatEngine.h"

#include "json.hpp"
using json = nlohmann::json;
using namespace nlohmann::literals;


namespace FlatEngine
{
	Mesh::Mesh(long myID, long parentID)
	{
		SetID(myID);
		SetParentID(parentID);
		SetType(T_Mesh);

		m_model = Model();
		m_material = nullptr;
		m_descriptorSets = std::vector<VkDescriptorSet>();
		m_textures = std::vector<Texture>();
		m_allocationPoolIndex = -1;
		m_b_initialized = false;

		// handles
		m_instanceHandle = VK_NULL_HANDLE;
		m_winSystem = nullptr;
		m_physicalDeviceHandle = nullptr;
		m_deviceHandle = nullptr;
		m_renderPass = nullptr;
		m_commandPool = nullptr;		

		//GetModel().SetModelPath("../projects/Astrodome/models/quad.obj");
		//SetMaterial(F_VulkanManager->GetMaterial("Spaceboy"));
		//m_textures.resize(m_material->GetTextureCount());

		//// Assign image views used for sceneTextureRenderer
		//for (Texture& texture : m_textures)
		//{
		//	texture.ConfigureImageResources(F_VulkanManager->m_viewportImages[VM_currentFrame], F_VulkanManager->m_viewportImageViews[VM_currentFrame], F_VulkanManager->m_viewportImageMemory[VM_currentFrame], F_VulkanManager->m_viewportSampler);
		//}

		//CreateResources();		
	}

	Mesh::~Mesh()
	{
	}

	std::string Mesh::GetData()
	{
		json texturesArray = json::array();

		for (Texture texture : m_textures)
		{
			json textureData = {
				{ "path", texture.GetTexturePath() }
			};

			texturesArray.push_back(textureData);
		}

		json jsonData = {
			{ "type", "Mesh"},
			{ "id", GetID() },
			{ "_isCollapsed", IsCollapsed() },
			{ "_isActive", IsActive() },
			{ "textures", texturesArray }			
		};

		std::string data = jsonData.dump();
		// Return dumped json object with required data for saving
		return data;
	}

	void Mesh::CleanupTextures()
	{
		for (Texture& texture : m_textures)
		{
			texture.Cleanup(*m_deviceHandle);
			m_material->GetAllocator().SetFreed(texture.GetAllocationIndex());
		}
	}

	void Mesh::Cleanup()
	{		
		CleanupTextures();
		m_material->Cleanup();
		m_model.Cleanup(*m_deviceHandle);
	}


	bool Mesh::Initialized()
	{
		return m_b_initialized;
	}

	void Mesh::SetHandles(VkInstance instance, WinSys& winSystem, PhysicalDevice& physicalDevice, LogicalDevice& logicalDevice, RenderPass& renderPass, VkCommandPool& commandPool)
	{
		m_instanceHandle = instance;
		m_winSystem = &winSystem;
		m_physicalDeviceHandle = &physicalDevice;
		m_deviceHandle = &logicalDevice;
		m_renderPass = &renderPass;
		m_commandPool = &commandPool;
	}

	void Mesh::SetModel(Model model)
	{
		if (m_model.GetModelPath() != "")
		{
			m_model.Cleanup(*m_deviceHandle);
		}
		m_model = model;

		if (m_model.GetModelPath() != "")
		{
			CreateModelResources(FlatEngine::F_VulkanManager->GetCommandPool(), FlatEngine::F_VulkanManager->GetPhysicalDevice(), FlatEngine::F_VulkanManager->GetLogicalDevice());
		}
	}

	Model& Mesh::GetModel()
	{
		return m_model;
	}

	void Mesh::CreateModelResources(VkCommandPool commandPool, PhysicalDevice& physicalDevice, LogicalDevice& logicalDevice)
	{
		m_model.LoadModel();
		m_model.CreateVertexBuffer(commandPool, physicalDevice, logicalDevice);
		m_model.CreateIndexBuffer(commandPool, physicalDevice, logicalDevice);
		m_model.CreateUniformBuffers(physicalDevice, logicalDevice);
	}

	void Mesh::SetMaterial(std::shared_ptr<Material> material)
	{		
		if (m_material != nullptr)
		{
			m_material->Cleanup();
		}
		m_material = material;

		if (m_material != nullptr)
		{
			m_textures.resize(m_material->GetTextureCount());

			// Assign image views used for sceneTextureRenderer
			for (Texture& texture : m_textures)
			{
				texture.ConfigureImageResources(F_VulkanManager->m_viewportImages[VM_currentFrame], F_VulkanManager->m_viewportImageViews[VM_currentFrame], F_VulkanManager->m_viewportImageMemory[VM_currentFrame], F_VulkanManager->m_viewportSampler);
			}
		}
	}

	void Mesh::SetMaterial(std::string materialName)
	{
		if (m_material != nullptr)
		{
			m_material->Cleanup();
		}
		m_material = FlatEngine::F_VulkanManager->GetMaterial(materialName);

		if (m_material != nullptr)
		{
			m_textures.resize(m_material->GetTextureCount());

			// Assign image views used for sceneTextureRenderer
			for (Texture& texture : m_textures)
			{
				texture.ConfigureImageResources(F_VulkanManager->m_viewportImages[VM_currentFrame], F_VulkanManager->m_viewportImageViews[VM_currentFrame], F_VulkanManager->m_viewportImageMemory[VM_currentFrame], F_VulkanManager->m_viewportSampler);
			}
		}
	}

	std::shared_ptr<Material> Mesh::GetMaterial()
	{
		return m_material;
	}

	void Mesh::CreateResources()
	{
		//// make sure there are the necessary number of actual textures assigned to Mesh before creating resources
		//bool b_texturesAssigned = true;
		//if (m_textures.size() != m_material->GetTextureCount())
		//{
		//	b_texturesAssigned = false;
		//}
		//else
		//{
		//	for (int i = 0; i < m_material->GetTextureCount(); i++)
		//	{
		//		if (m_textures[i].GetTexturePath() == "")
		//		{
		//			b_texturesAssigned = false;
		//		}
		//	}
		//}

		//if (b_texturesAssigned)
		//{
		if (m_model.GetModelPath() != "" && m_material != nullptr)
		{
			CreateModelResources(FlatEngine::F_VulkanManager->GetCommandPool(), FlatEngine::F_VulkanManager->GetPhysicalDevice(), FlatEngine::F_VulkanManager->GetLogicalDevice());
			m_material->CreateDescriptorSets(m_descriptorSets, m_model, m_textures);
			m_b_initialized = true;
		}


		//}
	}

	void Mesh::AddTexture(std::string path)
	{
		Texture newTexture = Texture();
		newTexture.SetTexturePath(path);
		m_textures.push_back(newTexture);
		CreateTextureResources();
	}

	void Mesh::AddTexture(Texture texture)
	{
		m_textures.push_back(texture);
		CreateTextureResources();
	}

	std::vector<Texture>& Mesh::GetTextures()
	{
		return m_textures;
	}

	void Mesh::CreateTextureResources() // To be called once actual Textures with paths have been assigned to the Mesh
	{
		for (Texture& texture : m_textures)
		{
			if (texture.GetTexturePath() != "")
			{
				texture.CreateTextureImage(*m_winSystem, *m_commandPool, *m_physicalDeviceHandle, *m_deviceHandle);
			}
		}

		if (m_material != nullptr)
		{
			m_material->CreateDescriptorSets(m_descriptorSets, m_model, m_textures);
		}
	}

	std::vector<VkDescriptorSet>& Mesh::GetDescriptorSets()
	{
		return m_descriptorSets;
	}
}