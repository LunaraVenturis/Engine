#pragma once

/**
 * @file
 * @author Krusto Stoyanov ( k.stoianov2@gmail.com )
 * @brief 
 * @version 1.0
 * @date 
 * 
 * @section DESCRIPTION
 * 
 * Shader class definition
 */


#include <expected>
#include <unordered_map>

#include <Core/Core.hpp>
#include <Core/Error.hpp>
#include <shaderc/shaderc.hpp>
#include <vulkan/vulkan.h>

namespace Engine
{
    enum class ShaderState
    {
        Created,
        Destroyed,
        Compiling,
        Compiled
    };

}// namespace Engine

namespace Engine
{

    class Shader
    {
    public:
        Shader() = default;
        ~Shader() = default;

    public:
        std::expected<ShaderState, ErrorStatus> CreateFromString(std::string_view vertexSource,
                                                                 std::string_view fragmentSource);

        std::expected<ShaderState, ErrorStatus> Compile(bool enableOptimization = true);

    public:
        std::unordered_map<shaderc_shader_kind, std::string> m_ShaderSource;
        std::unordered_map<shaderc_shader_kind, std::vector<uint32_t>> m_ShaderBinary;
    };
}// namespace Engine