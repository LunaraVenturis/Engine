#pragma once

/**
 * @file
 * @author Krusto Stoyanov ( k.stoianov2@gmail.com )
 * @brief 
 * @version 1.0
 * @date 
 * 
 * 
 * @section DESCRIPTION
 * 
 * GraphicsPipeline class definition
 */


#include <Core/Result.hpp>
#include "UniformBuffer.hpp"
#include <vulkan/vulkan.hpp>

namespace Engine
{
    enum class GraphicsPipelineStatus
    {
        Fail,
        Success
    };
}// namespace Engine

namespace Engine
{
    struct GraphicsPipelineSpec {
        VkFormat format;
        VkExtent2D extent;
    };
}// namespace Engine

namespace Engine
{

    class GraphicsPipeline
    {
    public:
        GraphicsPipeline() = default;
        ~GraphicsPipeline();

    public:
        ResultValueType<GraphicsPipelineStatus> Init(VkDevice device, GraphicsPipelineSpec graphicsPipelineSpec);
        ResultValueType<GraphicsPipelineStatus> Update(VkDevice device, UniformBuffer* uniformBuffer);

        ResultValue<GraphicsPipelineStatus, VkPipeline> GetGraphicsPipeline();
        ResultValue<GraphicsPipelineStatus, VkPipelineLayout> GetGraphicsPipelineLayout();
        ResultValue<GraphicsPipelineStatus, GraphicsPipelineSpec> GetGraphicsPipelineSpec();
        ResultValueType<GraphicsPipelineStatus> Destroy(VkDevice device);

    private:
        VkPipeline m_GraphicsPipeline;
        VkPipelineLayout m_GraphicsPipelineLayout;
        VkDescriptorSetLayout m_DescriptorSetLayout;
        VkDescriptorPool m_DescriptorPool;
        VkDescriptorSet m_DescriptorSet;
        VkDeviceMemory m_Memory;

        GraphicsPipelineSpec m_GraphicsPipelineSpec;
    };
}// namespace Engine