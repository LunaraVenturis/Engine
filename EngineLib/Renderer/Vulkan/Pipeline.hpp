#include "vulkan/vulkan.h"
#include <vector>
#pragma once

namespace LunaraEngine
{

    class Pipeline
    {
    public:
        Pipeline() = default;
        Pipeline(VkDevice device);
        virtual ~Pipeline();

    public:
        VkPipeline GetPipeline() const;
        VkPipelineLayout GetLayout() const;

    protected:
        VkPipeline p_Pipeline = VK_NULL_HANDLE;
        VkPipelineLayout p_Layout = VK_NULL_HANDLE;
        VkDevice p_Device = VK_NULL_HANDLE;
        std::vector<VkDescriptorSetLayout> p_DescriptorLayouts;

    protected:
        void CreatePipelineLayout(const std::vector<VkDescriptorSetLayout>& descriptorSets);
        VkShaderModule CreateShaderModule(const std::vector<uint32_t>& spirvCode);
    };
}// namespace LunaraEngine