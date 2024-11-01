#include "Shader.hpp"

namespace Engine
{
    std::expected<ShaderState, ErrorStatus> Shader::CreateFromString(std::string_view vertexSource,
                                                                     std::string_view fragmentSource)
    {
        if (vertexSource.length() <= 0 || fragmentSource.length() <= 0)
            return std::unexpected(ErrorStatus::StringLengthIsZero);

        m_ShaderSource[shaderc_shader_kind::shaderc_vertex_shader] = vertexSource;
        m_ShaderSource[shaderc_shader_kind::shaderc_fragment_shader] = fragmentSource;

        return ShaderState::Created;
    }

    std::expected<ShaderState, ErrorStatus> Shader::Compile(bool enableOptimization)
    {
        shaderc::Compiler compiler;
        shaderc::CompileOptions options;
        if (enableOptimization) options.SetOptimizationLevel(shaderc_optimization_level_performance);

        for (auto& [kind, source]: m_ShaderSource)
        {
            shaderc::SpvCompilationResult module =
                    compiler.CompileGlslToSpv(source, kind, "dummy source name", options);

            if (module.GetCompilationStatus() != shaderc_compilation_status_success)
            {
                LOG_ERROR(module.GetErrorMessage());
                return std::unexpected(ErrorStatus::CanNotCompileShader);
            }
            m_ShaderBinary[kind] = std::vector<uint32_t>(module.begin(), module.end());
            std::cout << "Compiled to an optimized binary module with " << m_ShaderBinary[kind].size() << " words."
                      << std::endl;
        }
        return ShaderState::Compiled;
    }
}// namespace Engine