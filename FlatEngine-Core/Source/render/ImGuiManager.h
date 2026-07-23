#pragma once
#include "render/PipelineManager.h"


namespace FlatEngine
{
    class ImGuiManager : public PipelineManager
    {
    public:
        ImGuiManager();
        void Cleanup();

        void Setup(VkCommandPool& commandPool);
        void HandleRenderPass(uint32_t imageIndex);
        //void CreateRenderPassResources();
        //void CreateImageResources();
        //void CreateDescriptorSets(std::shared_ptr<Material> material, std::vector<VkDescriptorSet>& descriptorSets, Model& model, std::vector<Texture>& textures);
        //void OnWindowResized();
        //void RecordCommandBuffer(uint32_t imageIndex, Mesh& mesh);

        void QuitImGui();        
      

    private:
        //void CreateImGuiResources();
        //void GetImGuiDescriptorSetLayoutInfo(std::vector<VkDescriptorSetLayoutBinding>& bindings, VkDescriptorSetLayoutCreateInfo& layoutInfo);
        //void GetImGuiDescriptorPoolInfo(std::vector<VkDescriptorPoolSize>& poolSizes, VkDescriptorPoolCreateInfo& poolInfo);
    };
}

