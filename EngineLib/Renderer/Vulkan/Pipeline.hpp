#pragma once
#include <vulkan/vulkan.h>
#include <vector>

namespace LunaraEngine
{

    class Pipeline
    {
    public:
        Pipeline(VkDevice device);
        virtual ~Pipeline() = default;

    public:
        VkPipeline GetPipeline() const;
        VkPipelineLayout GetLayout() const;

    protected:
        VkPipeline p_Pipeline = VK_NULL_HANDLE;
        VkPipelineLayout p_Layout = VK_NULL_HANDLE;
        VkDevice p_Device = VK_NULL_HANDLE;
        std::vector<VkDescriptorSetLayout> p_DescriptorLayouts;

    protected:
        void CreatePipelineLayout(const std::vector<VkDescriptorSetLayout>& descriptorSets,
                                  const std::vector<VkPushConstantRange>& pushConstants);
        VkShaderModule CreateShaderModule(const std::vector<uint32_t>& spirvCode);
    };
}// namespace LunaraEngine