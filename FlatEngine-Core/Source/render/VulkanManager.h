#pragma once
#include "Types.h"
#include "render/Material.h"
#include "render/RenderPass.h"
#include "render/ValidationLayers.h"

#include <map>
#include <memory>
#include <vector>


namespace FlatEngine
{
    namespace VulkanManager
    {
        // For device extensions required to present images to the window system (swap chain usage)
        const std::vector<const char*> DEVICE_EXTENSIONS =
        {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
        };

        #ifdef NDEBUG
            const bool b_ENABLE_VALIDATION_LAYERS = false;
        #else
            const bool b_ENABLE_VALIDATION_LAYERS = true;
        #endif

        class Vulkan
        {      
        public:
            Vulkan();
            void Cleanup();

            bool Init(int windowWidth, int windowHeight);
            VkInstance& GetInstance();
            VkQueue& GetGraphicsQueue();
            void DrawFrame();
            VkSampleCountFlagBits GetMaxSamples();
            void SetMaxSamples(VkSampleCountFlagBits maxSamples);
            VkCommandPool& GetCommandPool();

            static void check_vk_result(VkResult err);
            static void CreateCommandPool(VkCommandPool& commandPool, uint32_t queueFamilyIndices, VkCommandPoolCreateFlags flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);       

            // Materials
            void LoadEngineMaterials();
            void InitializeMaterials();        
            void SaveMaterial(std::shared_ptr<Material> material);
            std::shared_ptr<Material> LoadMaterial(std::string path, RenderPass* renderPass, bool b_init = true);
            std::shared_ptr<Material> CreateNewMaterialFile(std::string fileName, std::string path = "");        
            void AddSceneViewMaterial(std::shared_ptr<Material> material);
            void AddGameViewMaterial(std::shared_ptr<Material> material);
            void AddTextureToMaterial(std::string materialName, uint32_t index, TexturePipelineData textureData);
            void RemoveTextureFromMaterial(std::string materialName, uint32_t index = -1);
            void AddUBOVec4ToMaterial(std::string materialName, std::string uboVec4Name, int index = -1);
            void RemoveUBOVec4FromMaterial(std::string materialName, int index = -1);
            void SetMaterialVertexPath(std::string materialName, std::string vertexPath);
            void SetMaterialFragmentPath(std::string materialName, std::string fragmentPath);
            std::shared_ptr<Material> GetMaterial(std::string materialName, ViewportType viewportType = ViewportType::ViewportType_SceneView);
            std::map<std::string, std::shared_ptr<Material>>& GetMaterials();
            void ReloadShaders();
            void AddSceneViewMaterialMesh(std::string materialName, long objectID);
            void AddGameViewMaterialMesh(std::string materialName, long objectID);
            void RemoveSceneViewMaterialMesh(std::string materialName, long ID);
            void RemoveGameViewMaterialMesh(std::string materialName, long ID);
            void ClearGroupedByMaterialMeshes();
            std::shared_ptr<Model> GetModel(std::string modelPath);
            std::shared_ptr<Model> LoadModel(std::string modelPath);

            // Memory management
            void QueueBufferDeletion(VkBuffer bufferToDelete);
            void QueueDeviceMemoryDeletion(VkDeviceMemory deviceMemoryToDelete);
            void QueueDescriptorPoolDeletion(VkDescriptorPool descriptorPoolToDelete);
            void DeleteQueuedVKObjects();

            // ImGui
            void CreateImGuiRendePassResources();
            void GetImGuiDescriptorSetLayoutInfo(std::vector<VkDescriptorSetLayoutBinding>& bindings, VkDescriptorSetLayoutCreateInfo& layoutInfo);
            void GetImGuiDescriptorPoolInfo(std::vector<VkDescriptorPoolSize>& poolSizes, VkDescriptorPoolCreateInfo& poolInfo);
            void CreateImGuiResources();
            void QuitImGui();
            void CreateImGuiTexture(Texture& texture, std::vector<VkDescriptorSet>& descriptorSets);
            void FreeImGuiTexture(uint32_t allocatedFrom);        
            // Scene View
            void CreateRenderToTextureRenderPassResources(RenderPass& renderPass, Texture& renderToTexture, VkCommandPool& commandPool);
            std::vector<VkDescriptorSet>& GetSceneViewDescriptorSets();  
            // Game View
            std::vector<VkDescriptorSet>& GetGameViewDescriptorSets();
            // Post Processing
            void CreatePostProcessingRenderPassResources();

            // RenderWindow::Windowtem wrappers
            void CreateTextureImage(VkImage& image, std::string path, uint32_t mipLevels, VkDeviceMemory& imageMemory);
            void CreateImageView(VkImageView& imageView, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels);
            void CreateTextureSampler(VkSampler& textureSampler, uint32_t mipLevels);

            void RecreateSwapChainAndFrameBuffers();

        private:            
            bool CheckSwapChainIntegrity(VkResult result, std::string errorMessage);
            bool CreateVulkanInstance();
            void CreateSyncObjects();        
            
            RenderPass m_renderToTextureSceneViewRenderPass;
            RenderPass m_renderToTextureGameViewRenderPass;
            RenderPass m_postProcessingRenderPass;
            RenderPass m_imGuiRenderPass;
            std::shared_ptr<Material> m_imGuiMaterial;
            std::map<std::string, std::shared_ptr<Material>> m_sceneViewMaterials;
            std::map<std::string, std::shared_ptr<Material>> m_gameViewMaterials;
            std::map<std::string, UMapVector<long>> m_sceneViewMaterialMeshes;
            std::map<std::string, UMapVector<long>> m_gameViewMaterialMeshes;
            std::map<std::string, std::shared_ptr<Model>> m_models;
            Texture m_sceneViewTexture;
            Texture m_gameViewTexture;      
            Texture m_postProcessingTexture;
            std::vector<VkDescriptorSet> m_postProcessingDescriptorSets;

            VkInstance m_instance;
            VkSampleCountFlagBits m_maxSamples;
            bool m_b_framebufferResized;
            VkCommandPool m_systemCommandPool;
            VkCommandPool m_imGuiCommandPool;
            VkCommandPool m_sceneViewCommandPool;
            VkCommandPool m_gameViewCommandPool;
            VkCommandPool m_postProcessingCommandPool;
            std::vector<VkSemaphore> m_imageAvailableSemaphores;
            std::vector<VkSemaphore> m_renderFinishedSemaphores;
            std::vector<VkFence> m_inFlightFences;        

            std::vector<VkBuffer> m_bufferDeleteQueue;
            std::vector<VkDeviceMemory> m_deviceMemoryDeleteQueue;
            std::vector<VkDescriptorPool> m_descriptorPoolDeleteQueue;
        };

        extern Vulkan vulkan;	
        extern std::string selectedMaterialName;
        extern ValidationLayers validationLayers;
        extern uint32_t currentFrame;
        extern uint32_t imageCount;
        extern int MAX_FRAMES_IN_FLIGHT; // Max number of frames that are not currently being presented, but are being prepared
    }
}