#include <Renderer/Shader.hpp>
#include <Renderer/VertexBuffer.hpp>
#include <Renderer/Vulkan/ComputePipeline.hpp>
#include <Renderer/Vulkan/Shader.hpp>
#include <Renderer/Vulkan/SwapChain.hpp>
#include <Renderer/Vulkan/VertexBuffer.hpp>
#include <Renderer/Vulkan/VulkanDataTypes.hpp>
#include <array>
#include <stdexcept>
#include "ComputePipeline.hpp"

namespace LunaraEngine
{
    ComputePipeline::ComputePipeline(RendererDataType* rendererData, const ShaderInfo* info,
                                     const std::map<size_t, std::vector<uint32_t>>& shaderSources)
        : Pipeline(rendererData->device)
    {

        std::array<VkShaderModule, 1> shaderModules = {};
        std::array<VkShaderStageFlagBits, 1> shaderStageFlags = {VK_SHADER_STAGE_COMPUTE_BIT};
        std::array<VkPipelineShaderStageCreateInfo, 1> shaderStages = {};

        CreateDescriptorLayout(rendererData, *info);
        std::vector<VkDescriptorSetLayout> descriptorSetLayouts = {m_DescriptorLayout};
        auto pushConstantRanges = CreatePushConstantRanges(*info);

        CreatePipelineLayout(descriptorSetLayouts, pushConstantRanges);

        for (size_t i = 0; i < shaderStages.size(); ++i)
        {
            const std::vector<uint32_t>& shaderSource = shaderSources.at(shaderStageFlags[i]);
            shaderModules[i] = CreateShaderModule(shaderSource);
            shaderStages[i].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            shaderStages[i].stage = shaderStageFlags[i];
            shaderStages[i].module = shaderModules[i];
            shaderStages[i].pName = "main";
        }

        std::vector<VkVertexInputAttributeDescription> attributeDescriptions;

        VkPipelineColorBlendAttachmentState colorBlendAttachment{};
        colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                                              VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        colorBlendAttachment.blendEnable = VK_TRUE;
        colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;          // Multiply by fragment alpha
        colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;// Inverse of it
        colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;           // FinalColor = src + dst * (1 - src.a)
        colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;// For alpha channel itself
        colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

        VkComputePipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
        pipelineInfo.stage = shaderStages[0];
        pipelineInfo.layout = p_Layout;
        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;// Optional
        pipelineInfo.basePipelineIndex = -1;             // Optional

        if (vkCreateComputePipelines(p_Device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &p_Pipeline) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create graphics pipeline!");
        }

        for (const auto shaderModule: shaderModules) { vkDestroyShaderModule(p_Device, shaderModule, nullptr); }
    }

    ComputePipeline::~ComputePipeline()
    {

        if (m_DescriptorLayout != VK_NULL_HANDLE)
        {
            vkDestroyDescriptorSetLayout(p_Device, m_DescriptorLayout, nullptr);
        }
        if (p_Layout != VK_NULL_HANDLE) { vkDestroyPipelineLayout(p_Device, p_Layout, nullptr); }
        if (p_Pipeline != VK_NULL_HANDLE) { vkDestroyPipeline(p_Device, p_Pipeline, nullptr); }
    }

    VkDescriptorType ComputePipeline::GetDescriptorType(ShaderResourceType type)
    {
        switch (type)
        {
            case ShaderResourceType::Texture:
                return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            case ShaderResourceType::Sampler:
                return VK_DESCRIPTOR_TYPE_SAMPLER;
            case ShaderResourceType::UniformBuffer:
                return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            case ShaderResourceType::StorageBuffer:
                return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
            default:
                return VK_DESCRIPTOR_TYPE_MAX_ENUM;
        }
    }

    void ComputePipeline::CreateDescriptorLayout(RendererDataType* rendererData, const ShaderInfo& info)
    {

        std::vector<VkDescriptorSetLayoutBinding> descriptorSetLayoutBindings;
        for (const auto& resource: info.resources.bufferResources)
        {
            if (resource.type == ShaderResourceType::PushConstant) { continue; }

            const auto& type = resource.type;
            const auto& name = resource.name;
            ShaderBindingT binding = resource.layout.binding;
            ShaderResourceMemoryLayout layout = resource.layout.layoutType;

            //If memory model layout is missing put default
            if (resource.layout.layoutType == ShaderResourceMemoryLayout::None)
            {
                layout = ShaderResourceMemoryLayout::STD140;
            }


            VkDescriptorSetLayoutBinding descriptorSetLayoutBinding{};
            descriptorSetLayoutBinding.binding = binding;
            descriptorSetLayoutBinding.descriptorType = ComputePipeline::GetDescriptorType(type);

            //For now resources are availeble in all stages
            descriptorSetLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

            //For now only one resource per binding
            descriptorSetLayoutBinding.descriptorCount = 1;

            //For now no immutable samplers
            descriptorSetLayoutBinding.pImmutableSamplers = nullptr;

            descriptorSetLayoutBindings.push_back(descriptorSetLayoutBinding);

            (void) name;
            (void) layout;
        }

        VkDescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = static_cast<uint32_t>(descriptorSetLayoutBindings.size());
        layoutInfo.pBindings = descriptorSetLayoutBindings.data();

        //if there are no bindings exit
        if (layoutInfo.bindingCount == 0) return;

        if (vkCreateDescriptorSetLayout(rendererData->device, &layoutInfo, nullptr, &m_DescriptorLayout) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create descriptor set layout!");
        }
    }

    std::vector<VkPushConstantRange> ComputePipeline::CreatePushConstantRanges(const ShaderInfo& info) const
    {
        std::vector<VkPushConstantRange> ranges{};

        for (auto& resource: info.resources.bufferResources)
        {
            if (resource.type == ShaderResourceType::PushConstant)
            {
                VkPushConstantRange range{};
                range.offset = 0;
                range.size = (uint32_t) resource.length * (uint32_t) resource.stride;
                range.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
                ranges.push_back(range);
            }
        }
        return ranges;
    }
}// namespace LunaraEngine