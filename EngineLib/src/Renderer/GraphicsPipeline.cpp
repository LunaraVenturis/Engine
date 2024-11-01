/**
 * @file
 * @author Krusto Stoyanov ( k.stoianov2@gmail.com )
 * @brief 
 * @version 1.0
 * @date 
 * 
 * @section DESCRIPTION
 * 
 * GraphicsPipeline class implementation
 */


#include "GraphicsPipeline.hpp"
#include <Core/Log.hpp>
#include <vulkan/vulkan.hpp>

namespace Engine
{

    GraphicsPipeline::~GraphicsPipeline() {}

    ResultValueType<GraphicsPipelineStatus> GraphicsPipeline::Init(VkDevice device,
                                                                   GraphicsPipelineSpec graphicsPipelineSpec)
    {
        m_GraphicsPipelineSpec = graphicsPipelineSpec;

        VkDescriptorSetLayoutBinding descriptorSetLayoutBinding = {};
        descriptorSetLayoutBinding.binding = 0;
        descriptorSetLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorSetLayoutBinding.descriptorCount = 1;
        descriptorSetLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

        VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {};
        descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        descriptorSetLayoutCreateInfo.bindingCount = 1;
        descriptorSetLayoutCreateInfo.pBindings = &descriptorSetLayoutBinding;

        auto descriptorSetLayoutStatus =
                vkCreateDescriptorSetLayout(device, &descriptorSetLayoutCreateInfo, nullptr, &m_DescriptorSetLayout);

        if (VK_SUCCESS != descriptorSetLayoutStatus)
        {
            LOG_ERROR("Can Not Create Descriptor Set Layout!\n");
            return ResultValueType{GraphicsPipelineStatus::Fail};
        }

        VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
        pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutCreateInfo.setLayoutCount = 1;

        auto pipelineLayoutStatus =
                vkCreatePipelineLayout(device, &pipelineLayoutCreateInfo, nullptr, &m_GraphicsPipelineLayout);

        if (VK_SUCCESS != pipelineLayoutStatus)
        {
            LOG_ERROR("Can Not Create Graphics Pipeline Layout Layout!\n");
            return ResultValueType{GraphicsPipelineStatus::Fail};
        }

        VkDescriptorPoolSize descriptorPoolSize = {};
        descriptorPoolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorPoolSize.descriptorCount = 1;

        VkDescriptorPoolCreateInfo poolInfo = {};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = 1;
        poolInfo.pPoolSizes = &descriptorPoolSize;
        poolInfo.maxSets = 1;
        poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;

        auto descriptorPoolStatus = vkCreateDescriptorPool(device, &poolInfo, nullptr, &m_DescriptorPool);
        if (VK_SUCCESS != descriptorPoolStatus)
        {
            LOG_ERROR("Can Not Create Descriptor Pool!\n");
            return ResultValueType{GraphicsPipelineStatus::Fail};
        }

        VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {};
        descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        descriptorSetAllocateInfo.descriptorSetCount = 1;
        descriptorSetAllocateInfo.pSetLayouts = &m_DescriptorSetLayout;
        descriptorSetAllocateInfo.descriptorPool = m_DescriptorPool;

        auto descriptorSetStatus = vkAllocateDescriptorSets(device, &descriptorSetAllocateInfo, &m_DescriptorSet);
        if (VK_SUCCESS != descriptorSetStatus)
        {
            LOG_ERROR("Can Not Allocate Descriptor Sets!\n");
            return ResultValueType{GraphicsPipelineStatus::Fail};
        }

        return ResultValueType{GraphicsPipelineStatus::Success};
    }

    ResultValueType<GraphicsPipelineStatus> GraphicsPipeline::Update(VkDevice device, UniformBuffer* uniformBuffer)
    {
        auto bufferStatus = uniformBuffer->GetBufferHandle();
        if (BufferStatus::Success != bufferStatus)
        {
            LOG_ERROR("Can Not Get UniformBuffer handle!\n");
            return ResultValueType{GraphicsPipelineStatus::Fail};
        }
        if (VK_NULL_HANDLE == m_DescriptorSet)
        {
            LOG_ERROR("Invalid Discriptor Set!\n");
            return ResultValueType{GraphicsPipelineStatus::Fail};
        }

        VkDescriptorBufferInfo descriptorBufferInfo = {};
        descriptorBufferInfo.buffer = bufferStatus.value;
        descriptorBufferInfo.offset = 0;
        descriptorBufferInfo.range = uniformBuffer->GetSize();
        VkWriteDescriptorSet writeDescriptorSet = {};
        writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writeDescriptorSet.dstSet = m_DescriptorSet;
        writeDescriptorSet.dstBinding = 0;
        writeDescriptorSet.dstArrayElement = 0;
        writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        writeDescriptorSet.pBufferInfo = &descriptorBufferInfo;

        vkUpdateDescriptorSets(device, 1, &writeDescriptorSet, 0, nullptr);
        return ResultValueType{GraphicsPipelineStatus::Success};
    }

    ResultValue<GraphicsPipelineStatus, GraphicsPipelineSpec> GraphicsPipeline::GetGraphicsPipelineSpec()
    {
        return {GraphicsPipelineStatus::Success, m_GraphicsPipelineSpec};
    }

    ResultValue<GraphicsPipelineStatus, VkPipeline> GraphicsPipeline::GetGraphicsPipeline()
    {
        if (VK_NULL_HANDLE != m_GraphicsPipeline)
        {
            return ResultValue<GraphicsPipelineStatus, VkPipeline>{GraphicsPipelineStatus::Fail};
        }
        return ResultValue{GraphicsPipelineStatus::Success, m_GraphicsPipeline};
    }

    ResultValue<GraphicsPipelineStatus, VkPipelineLayout> GraphicsPipeline::GetGraphicsPipelineLayout()
    {
        if (VK_NULL_HANDLE != m_GraphicsPipelineLayout)
        {
            return ResultValue<GraphicsPipelineStatus, VkPipelineLayout>{GraphicsPipelineStatus::Fail};
        }
        return ResultValue<GraphicsPipelineStatus, VkPipelineLayout>{GraphicsPipelineStatus::Success,
                                                                     m_GraphicsPipelineLayout};
    }

    ResultValueType<GraphicsPipelineStatus> GraphicsPipeline::Destroy(VkDevice device)
    {
        if (VK_NULL_HANDLE != m_DescriptorPool) { return ResultValueType{GraphicsPipelineStatus::Fail}; }
        if (VK_NULL_HANDLE != m_DescriptorSet) { return ResultValueType{GraphicsPipelineStatus::Fail}; }

        vkFreeDescriptorSets(device, m_DescriptorPool, 1, &m_DescriptorSet);
        vkDestroyDescriptorPool(device, m_DescriptorPool, nullptr);

        if (VK_NULL_HANDLE != m_GraphicsPipelineLayout) { return ResultValueType{GraphicsPipelineStatus::Fail}; }
        vkDestroyPipelineLayout(device, m_GraphicsPipelineLayout, nullptr);

        if (VK_NULL_HANDLE != m_DescriptorSetLayout) { return ResultValueType{GraphicsPipelineStatus::Fail}; }
        vkDestroyDescriptorSetLayout(device, m_DescriptorSetLayout, nullptr);
        return ResultValueType{GraphicsPipelineStatus::Success};
    }

}// namespace Engine