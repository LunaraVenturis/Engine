#include "Pipeline.hpp"
#include <stdexcept>

namespace LunaraEngine
{
    Pipeline::Pipeline(VkDevice device) : p_Device(device) {}

    VkPipeline Pipeline::GetPipeline() const { return p_Pipeline; }

    VkPipelineLayout Pipeline::GetLayout() const { return p_Layout; }

    void Pipeline::CreatePipelineLayout(const std::vector<VkDescriptorSetLayout>& descriptorSets,
                                        const std::vector<VkPushConstantRange>& pushConstants)
    {
        p_DescriptorLayouts = descriptorSets;
        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(p_DescriptorLayouts.size());
        pipelineLayoutInfo.pSetLayouts = p_DescriptorLayouts.data();

        if (pushConstants.empty())
        {
            pipelineLayoutInfo.pushConstantRangeCount = 0;
            pipelineLayoutInfo.pPushConstantRanges = nullptr;
        }
        else
        {
            pipelineLayoutInfo.pushConstantRangeCount = static_cast<uint32_t>(pushConstants.size());
            pipelineLayoutInfo.pPushConstantRanges = pushConstants.data();
        }
        if (vkCreatePipelineLayout(p_Device, &pipelineLayoutInfo, nullptr, &p_Layout) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create pipeline layout!");
        }
    }

    VkShaderModule Pipeline::CreateShaderModule(const std::vector<uint32_t>& spirvCode)
    {
        VkShaderModuleCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = spirvCode.size();
        createInfo.pCode = spirvCode.data();
        VkShaderModule shaderModule;
        if (vkCreateShaderModule(p_Device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create shader module!");
        }
        return shaderModule;
    }

    VkDescriptorType Pipeline::GetDescriptorType(ShaderResourceType type)
    {
        switch (type)
        {
            case ShaderResourceType::Texture:
            case ShaderResourceType::Texture2D:
            case ShaderResourceType::Texture2DArray:
            case ShaderResourceType::Texture3D:
                return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;

            case ShaderResourceType::UniformBuffer:
                return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;

            case ShaderResourceType::StorageBuffer:
            case ShaderResourceType::Buffer:
                return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;

            case ShaderResourceType::PushConstant:
            case ShaderResourceType::None:
            default:
                throw std::runtime_error("Invalid or unsupported ShaderResourceType");
        }
    }

}// namespace LunaraEngine
