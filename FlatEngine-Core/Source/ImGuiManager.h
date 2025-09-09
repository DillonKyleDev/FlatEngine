#pragma once
#include "Structs.h"
#include "PipelineManager.h"

#include "imgui.h"
#include "imgui_impl_vulkan.h"
#include <glm.hpp>

#include <vector>
#include <memory>


namespace FlatEngine
{
    class Texture;
    class Model;
    class Material;

    class ImGuiManager : public PipelineManager
    {
    public:
        ImGuiManager();
        ~ImGuiManager();
        void Cleanup();

        void Setup(VkInstance& instance, WinSys& winSystem, PhysicalDevice& physicalDevice, LogicalDevice& logicalDevice, VkCommandPool& commandPool);
        void HandleRenderPass(uint32_t imageIndex);
        void CreateRenderPassResources();
        void CreateImageResources();
        void CreateDescriptorSets(std::shared_ptr<Material> material, std::vector<VkDescriptorSet>& descriptorSets, Model& model, std::vector<Texture>& textures);
        void OnWindowResized();

        void QuitImGui();        
      

    private:
        void CreateImGuiResources();
        void GetImGuiDescriptorSetLayoutInfo(std::vector<VkDescriptorSetLayoutBinding>& bindings, VkDescriptorSetLayoutCreateInfo& layoutInfo);
        void GetImGuiDescriptorPoolInfo(std::vector<VkDescriptorPoolSize>& poolSizes, VkDescriptorPoolCreateInfo& poolInfo);
    };
}

