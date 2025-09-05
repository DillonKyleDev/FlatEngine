#pragma once
#include "Structs.h"
#include "ValidationLayers.h"
#include "PhysicalDevice.h"
#include "LogicalDevice.h"
#include "WinSys.h"
#include "RenderPass.h"
#include "Mesh.h"
#include "Material.h"

#include "imgui.h"
//#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"

//#ifndef GLFW_INCLUDE_VULKAN
//#define GLFW_INCLUDE_VULKAN
//#endif
//#include <glfw3.h>
#include <glm.hpp>


namespace FlatEngine
{
    class ImGuiManager
    {
    public:
        ImGuiManager();
        ~ImGuiManager();
        void Cleanup();

        bool SetupImGui(VkInstance instance, WinSys& winSystem, PhysicalDevice& physicalDevice, LogicalDevice& logicalDevice);
        void QuitImGui();
        void HandleRenderPass(uint32_t imageIndex);
        void CreateDescriptorSets(Texture& texture, std::vector<VkDescriptorSet>& descriptorSets);
        void FreeDescriptorSet(uint32_t allocationIndex);
        RenderPass& GetRenderPass();
        std::shared_ptr<Material> GetMaterial();

    private:
        bool CreateImGuiResources();
        void GetImGuiDescriptorSetLayoutInfo(std::vector<VkDescriptorSetLayoutBinding>& bindings, VkDescriptorSetLayoutCreateInfo& layoutInfo);
        void GetImGuiDescriptorPoolInfo(std::vector<VkDescriptorPoolSize>& poolSizes, VkDescriptorPoolCreateInfo& poolInfo);

        RenderPass m_renderPass;
        VkCommandPool m_commandPool;
        std::shared_ptr<Material> m_material;
        // handles
        VkInstance m_instanceHandle;
        WinSys* m_winSystem;
        PhysicalDevice* m_physicalDeviceHandle;
        LogicalDevice* m_deviceHandle;
    };
}

