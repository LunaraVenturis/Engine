#include <Renderer/CommonTypes.hpp>
#include <Renderer/Shader.hpp>
#include <Renderer/Vulkan/VulkanDataTypes.hpp>
#include <Renderer/Vulkan/Shader.hpp>
#include <Renderer/Vulkan/GraphicsPipeline.hpp>
#include <Renderer/Vulkan/UniformBuffer.hpp>
#include <Core/Log.h>
#include "Shader.hpp"

namespace LunaraEngine
{

    void VulkanShader::Create(RendererDataType* rendererData, const ShaderInfo& info)
    {
        if (info.isComputeShader) { throw std::runtime_error("Compute shaders are not supported"); }

        PrintShaderResource(info);

        m_RendererData = rendererData;

        std::map<size_t, std::vector<uint32_t>> shaderSource;
        ReadShaderSource(info, shaderSource);

        m_Pipeline = new GraphicsPipeline(rendererData, &info, shaderSource);

        CreateUniformBuffers(info);
    }

    void VulkanShader::ReadShaderSource(const ShaderInfo& info, std::map<size_t, std::vector<uint32_t>>& shaderSource)
    {
        shaderSource[VK_SHADER_STAGE_VERTEX_BIT] =
                ReadFile(info.path / std::filesystem::path(info.name + L".vert.spv"));
        shaderSource[VK_SHADER_STAGE_FRAGMENT_BIT] =
                ReadFile(info.path / std::filesystem::path(info.name + L".frag.spv"));
    }

    VulkanShader::~VulkanShader() { Destroy(); }

    void VulkanShader::Destroy()
    {
        if (m_Pipeline != nullptr)
        {
            vkDeviceWaitIdle(m_RendererData->device);
            delete (GraphicsPipeline*) m_Pipeline;
            m_Pipeline = nullptr;

            for (auto uniformBuffer: m_UniformBuffers) { delete uniformBuffer; }
            m_UniformBuffers.clear();
        }
    }

    VkPipeline VulkanShader::GetPipeline() const { return m_Pipeline->GetPipeline(); }

    void VulkanShader::CreateUniformBuffers(const ShaderInfo& info)
    {
        for (const auto& resource: info.resources.bufferResources)
        {
            if (resource.type == ShaderResourceType::UniformBuffer)
            {
                auto size = resource.size;
                for (size_t i = 0; i < m_RendererData->maxFramesInFlight; i++)
                {
                    m_UniformBuffers.push_back(new VulkanUniformBuffer(m_RendererData, nullptr, 1, size));
                }
            }
        }
    }

    std::vector<uint32_t> VulkanShader::ReadFile(std::filesystem::path name)
    {

        std::ifstream file(name, std::ios::ate | std::ios::binary);
        if (!file.is_open())
        {
            std::string path = name.string();
            LOG_ERROR("Failed to open file: %s", path.c_str());
            throw std::runtime_error("failed to open file!");
        }

        std::streamsize fileSize = file.tellg();
        if (fileSize < 0) { throw std::runtime_error("failed to determine file size!"); }
        std::vector<uint32_t> buffer(static_cast<size_t>(fileSize));
        file.seekg(0);
        file.read(reinterpret_cast<char*>(buffer.data()), fileSize);
        file.close();

        return buffer;
    }

    void VulkanShader::PrintShaderResource(const ShaderInfo& info)
    {
        LOG_INFO("Shader path: %ls", info.path.wstring().c_str());
        LOG_INFO("Shader name: %ls", info.name.c_str());
        LOG_INFO("Shader resources:");
        LOG_INFO("INPUT RESOURCES");
        for (const auto& resource: info.resources.inputResources)
        {
            LOG_INFO("%s", resource.name.data());
            LOG_INFO("\tType: %s", Shader::GetShaderResourceType(resource.format, resource.type).c_str());
            LOG_INFO("\tBinding: %d", resource.binding);
            LOG_INFO("\tLocation: %d", resource.location);
            LOG_INFO("\tFormat: %s", Shader::GetShaderResourceFormat(resource.format, resource.type).c_str());
        }
        LOG_INFO("BUFFER RESOURCES");

        for (const auto& resource: info.resources.bufferResources)
        {
            LOG_INFO("%s", resource.name.data());
            LOG_INFO("\tType: %s", GetShaderResourceType(resource.type).c_str());
            LOG_INFO("\tSize: %zu", resource.size);
            LOG_INFO("\tBinding: %d", resource.layout.binding);
            LOG_INFO("\tLayout: %s", GetShaderResourceLayoutType(resource.layout.layoutType).c_str());
        }
    }

    std::string VulkanShader::GetShaderResourceType(ShaderResourceType type)
    {
        switch (type)
        {
            case ShaderResourceType::Texture:
                return "Texture";
            case ShaderResourceType::Sampler:
                return "Sampler";
            case ShaderResourceType::PushConstant:
                return "PushConstant";
            case ShaderResourceType::UniformBuffer:
                return "UniformBuffer";
            case ShaderResourceType::StorageBuffer:
                return "StorageBuffer";
            default:
                return "Unknown";
        }
    }

    std::string VulkanShader::GetShaderResourceLayoutType(ShaderResourceMemoryLayout type)
    {
        switch (type)
        {
            case ShaderResourceMemoryLayout::STD140:
                return "STD140";
            case ShaderResourceMemoryLayout::STD430:
                return "STD430";
            default:
                return "Unknown";
        }
    }

    VkFormat VulkanShader::GetShaderResourceFormat(ShaderResourceFormatT format, ShaderResourceDataTypeT type)
    {
        switch (format)
        {
            case ShaderResourceFormatT::R8:
                if (type == ShaderResourceDataTypeT::SFloat) return VK_FORMAT_R8_SRGB;
                else
                    return VK_FORMAT_R8_UNORM;
            case ShaderResourceFormatT::R16:
                if (type == ShaderResourceDataTypeT::SFloat) return VK_FORMAT_R16_SFLOAT;
                else
                    return VK_FORMAT_R16_UNORM;
            case ShaderResourceFormatT::R32:
                if (type == ShaderResourceDataTypeT::SFloat) return VK_FORMAT_R32_SFLOAT;
                else
                    return VK_FORMAT_R32_UINT;
            case ShaderResourceFormatT::R8G8:
                if (type == ShaderResourceDataTypeT::SFloat) return VK_FORMAT_R8G8_SRGB;
                else
                    return VK_FORMAT_R8G8_UNORM;
            case ShaderResourceFormatT::R16G16:
                if (type == ShaderResourceDataTypeT::SFloat) return VK_FORMAT_R16G16_SFLOAT;
                else
                    return VK_FORMAT_R16G16_UNORM;
            case ShaderResourceFormatT::R32G32:
                if (type == ShaderResourceDataTypeT::SFloat) return VK_FORMAT_R32G32_SFLOAT;
                else
                    return VK_FORMAT_R32G32_UINT;
            case ShaderResourceFormatT::R8G8B8:
                if (type == ShaderResourceDataTypeT::SFloat) return VK_FORMAT_R8G8B8_SRGB;
                else
                    return VK_FORMAT_R8G8B8_UNORM;
            case ShaderResourceFormatT::R16G16B16:
                if (type == ShaderResourceDataTypeT::SFloat) return VK_FORMAT_R16G16B16_SFLOAT;
                else
                    return VK_FORMAT_R16G16B16_UNORM;
            case ShaderResourceFormatT::R32G32B32:
                if (type == ShaderResourceDataTypeT::SFloat) return VK_FORMAT_R32G32B32_SFLOAT;
                else
                    return VK_FORMAT_R32G32B32_UINT;
            case ShaderResourceFormatT::R8G8B8A8:
                if (type == ShaderResourceDataTypeT::SFloat) return VK_FORMAT_R8G8B8A8_SRGB;
                else
                    return VK_FORMAT_R8G8B8A8_UNORM;
            case ShaderResourceFormatT::R16G16B16A16:
                if (type == ShaderResourceDataTypeT::SFloat) return VK_FORMAT_R16G16B16A16_SFLOAT;
                else
                    return VK_FORMAT_R16G16B16A16_UNORM;
            case ShaderResourceFormatT::R32G32B32A32:
                if (type == ShaderResourceDataTypeT::SFloat) return VK_FORMAT_R32G32B32A32_SFLOAT;
                else
                    return VK_FORMAT_R32G32B32A32_UINT;

            case ShaderResourceFormatT::None:
                return VK_FORMAT_UNDEFINED;

            default:
                return VK_FORMAT_UNDEFINED;
        }
    }
}// namespace LunaraEngine
