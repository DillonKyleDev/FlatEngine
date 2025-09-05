#pragma once
#include "ImGuiManager.h"
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

    class VulkanManager
    {
        friend class Mesh;

    public:
        VulkanManager();
        ~VulkanManager();
        void Cleanup();

        bool Init(int windowWidth, int windowHeight);
        WinSys& GetWinSystem();
        VkInstance& GetInstance();
        VkQueue& GetGraphicsQueue();
        void DrawFrame(ImDrawData* draw_data);

        static void check_vk_result(VkResult err);
        static void CreateCommandPool(VkCommandPool& commandPool, LogicalDevice& logicalDevice, uint32_t queueFamilyIndices, VkCommandPoolCreateFlags flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

        void InitializeMaterials();        
        void SaveMaterial(std::shared_ptr<Material> material);
        void LoadMaterial(std::string path);
        std::shared_ptr<Material> CreateNewMaterialFile(std::string fileName, std::string path = "");        
        void AddMaterial(std::shared_ptr<Material> material);
        std::shared_ptr<Material>& GetMaterial(std::string materialName);
        std::map<std::string, std::shared_ptr<Material>>& GetMaterials();

        std::vector<VkImage> m_viewportImages;
        std::vector<VkImageView> m_viewportImageViews;
        std::vector<VkDeviceMemory> m_viewportImageMemory;
        VkSampler m_viewportSampler;
        std::vector<VkDescriptorSet> viewportDescriptorSets;
        VkFormat viewportImageFormat;

        // ImGui
        void CreateImGuiTexture(Texture& texture, std::vector<VkDescriptorSet>& descriptorSets); // TEMPORARY WORK AROUND BEFORE FULL IMPLIMENATION OF 2D TEXTURES
        void FreeImGuiTexture(uint32_t allocatedFrom);

    private:
        bool InitVulkan(int windowWidth, int windowHeight);
        bool CheckSwapChainIntegrity(VkResult result, std::string errorMessage);
        void RecreateSwapChainAndFrameBuffers();
        bool CreateVulkanInstance();
        void CreateSyncObjects();
        void CreateWriteToImageResources(std::vector<VkImage>& images, std::vector<VkImageView>& imageViews, std::vector<VkDeviceMemory>& imageMemory);

        VkCommandPool& GetCommandPool();
        PhysicalDevice& GetPhysicalDevice();
        LogicalDevice& GetLogicalDevice();
        
        std::map<std::string, std::shared_ptr<Material>> m_materials;

        RenderPass m_sceneTextureRenderPass;
        ImGuiManager m_imguiManager;

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