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
    const int VM_MAX_FRAMES_IN_FLIGHT = 2;
    extern uint32_t VM_currentFrame;
    extern uint32_t VM_imageCount;

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

        static void check_vk_result(VkResult err);
        static void CreateCommandPool(VkCommandPool& commandPool, LogicalDevice& logicalDevice, uint32_t queueFamilyIndices, VkCommandPoolCreateFlags flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

        void InitializeMaterials();        
        void SaveMaterial(std::shared_ptr<Material> material);
        void LoadMaterial(std::string path, bool b_init = true);
        std::shared_ptr<Material> CreateNewMaterialFile(std::string fileName, std::string path = "");        
        void AddMaterial(std::shared_ptr<Material> material);
        std::shared_ptr<Material> GetMaterial(std::string materialName);
        std::map<std::string, std::shared_ptr<Material>>& GetMaterials();

        // ImGui
        void CreateImGuiTexture(Texture& texture, std::vector<VkDescriptorSet>& descriptorSets);
        void FreeImGuiTexture(uint32_t allocatedFrom);
        // Scene View
        std::vector<VkDescriptorSet> GetSceneDescriptorSets();        

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

        VkCommandPool& GetCommandPool();
        PhysicalDevice& GetPhysicalDevice();
        LogicalDevice& GetLogicalDevice();
        
        std::map<std::string, std::shared_ptr<Material>> m_materials;

        ImGuiManager m_imguiManager;
        ViewportManager m_sceneViewManager;

        std::vector<PipelineManager*> m_pipelineManagers;

        VkInstance m_instance;
        WinSys m_winSystem;
        PhysicalDevice m_physicalDevice;
        LogicalDevice m_logicalDevice;
        bool m_b_framebufferResized;
        VkCommandPool m_commandPool;
        std::vector<VkSemaphore> m_imageAvailableSemaphores;
        std::vector<VkSemaphore> m_renderFinishedSemaphores;
        std::vector<VkFence> m_inFlightFences;        
    };
}