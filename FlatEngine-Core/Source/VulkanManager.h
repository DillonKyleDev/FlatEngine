#pragma once
#include "ImGuiManager.h"
#include "ViewportManager.h"
#include "Structs.h"
#include "ValidationLayers.h"
#include "PhysicalDevice.h"
#include "LogicalDevice.h"
#include "WinSys.h"
#include "RenderPass.h"
#include "Mesh.h"
#include "Material.h"
#include "Vector2.h"

#include <memory>
#include <vector>
#include <map>


namespace FlatEngine
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

    extern ValidationLayers VM_validationLayers;
    const int VM_MAX_FRAMES_IN_FLIGHT = 2; // Max number of frames that are not currently being presented, but are being prepared
    extern uint32_t VM_currentFrame;
    extern uint32_t VM_imageCount;

    class Scene;

    class VulkanManager
    {
        friend class Mesh;
        friend class Application;        

    public:
        VulkanManager();
        ~VulkanManager();
        void Cleanup();

        bool Init(int windowWidth, int windowHeight);
        WinSys& GetWinSystem();
        VkInstance& GetInstance();
        VkQueue& GetGraphicsQueue();
        void DrawFrame();
        VkSampleCountFlagBits GetMaxSamples();
        void SetMaxSamples(VkSampleCountFlagBits maxSamples);

        VkCommandPool& GetCommandPool();
        PhysicalDevice& GetPhysicalDevice();
        LogicalDevice& GetLogicalDevice();        

        static void check_vk_result(VkResult err);
        static void CreateCommandPool(VkCommandPool& commandPool, LogicalDevice& logicalDevice, uint32_t queueFamilyIndices, VkCommandPoolCreateFlags flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

        // Renderpass
        void CreateRenderToTextureRenderPassResources();
        void CreateImGuiRendePassResources();
        void GetImGuiDescriptorSetLayoutInfo(std::vector<VkDescriptorSetLayoutBinding>& bindings, VkDescriptorSetLayoutCreateInfo& layoutInfo);
        void GetImGuiDescriptorPoolInfo(std::vector<VkDescriptorPoolSize>& poolSizes, VkDescriptorPoolCreateInfo& poolInfo);
        void CreateImGuiResources();
        void QuitImGui();

        // Materials
        void LoadEngineMaterials();
        void InitializeMaterials();        
        void SaveMaterial(std::shared_ptr<Material> material);
        std::shared_ptr<Material> LoadMaterial(std::string path, Texture* renderToTexture = nullptr);
        std::shared_ptr<Material> CreateNewMaterialFile(std::string fileName, std::string path = "");        
        void AddSceneViewMaterial(std::shared_ptr<Material> material);
        void AddGameViewMaterial(std::shared_ptr<Material> material);
        std::shared_ptr<Material> GetMaterial(std::string materialName);
        std::map<std::string, std::shared_ptr<Material>>& GetMaterials();
        void ReloadShaders();

        // ImGui
        void CreateImGuiTexture(Texture& texture, std::vector<VkDescriptorSet>& descriptorSets);
        void FreeImGuiTexture(uint32_t allocatedFrom);
        // Scene View
        void CreateSceneViewGridObjects();
        std::vector<VkDescriptorSet>& GetSceneViewDescriptorSets();        
        // Game View
        std::vector<VkDescriptorSet>& GetGameViewDescriptorSets();

        // WinSystem wrappers
        void CreateTextureImage(VkImage& image, std::string path, uint32_t mipLevels, VkDeviceMemory& imageMemory);
        void CreateImageView(VkImageView& imageView, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels);
        void CreateTextureSampler(VkSampler& textureSampler, uint32_t mipLevels);

    private:
        bool InitVulkan(int windowWidth, int windowHeight);
        bool CheckSwapChainIntegrity(VkResult result, std::string errorMessage);
        void RecreateSwapChainAndFrameBuffers();
        bool CreateVulkanInstance();
        void CreateSyncObjects();        
        
        RenderPass m_renderToTextureRenderPass;
        RenderPass m_imGuiRenderPass;
        std::shared_ptr<Material> m_imGuiMaterial;
        std::map<std::string, std::shared_ptr<Material>> m_engineMaterials;
        std::map<std::string, std::shared_ptr<Material>> m_sceneViewMaterials;
        std::map<std::string, std::shared_ptr<Material>> m_gameViewMaterials;
        Texture m_sceneViewTexture;
        Texture m_gameViewTexture;        

        VkInstance m_instance;
        WinSys m_winSystem;
        PhysicalDevice m_physicalDevice;
        VkSampleCountFlagBits m_maxSamples;
        LogicalDevice m_logicalDevice;
        bool m_b_framebufferResized;
        VkCommandPool m_commandPool;
        std::vector<VkSemaphore> m_imageAvailableSemaphores;
        std::vector<VkSemaphore> m_renderFinishedSemaphores;
        std::vector<VkFence> m_inFlightFences;        
    };
}