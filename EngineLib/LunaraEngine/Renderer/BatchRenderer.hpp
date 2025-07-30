#pragma once
#include <cstdint>
#include <vector>
#include <memory>
#include <filesystem>

#include <glm/glm.hpp>
#include <LunaraEngine/Core/CommonTypes.hpp>

namespace LunaraEngine
{

    using std::size_t;
    template <typename T>
    class IndexBuffer;
    template <typename T>
    class StorageBuffer;
    class RendererCommandDrawBatch;
    class Shader;

    class BatchRenderer
    {
    public:
        BatchRenderer();
        BatchRenderer(const ApplicationConfig& config, std::vector<std::wstring_view> textureNames);
        ~BatchRenderer() = default;

    public:
        void Create(const ApplicationConfig& config, std::vector<std::wstring_view> textureNames);
        void Destroy();

    public:
        void AddQuad(const glm::vec3&& position, const glm::vec2&& size, const uint32_t textureIndex = 0);
        RendererCommandDrawBatch* CreateDrawCommand();
        void Flush();
        std::weak_ptr<Shader> GetShader();

    private:
        static constexpr size_t MAX_QUADS = 10000;

    private:
        size_t m_Offset{};
        size_t m_QuadCount{};
        size_t m_Capacity{};

        std::vector<glm::vec4> m_Positions;
        std::vector<glm::vec2> m_Sizes;
        std::vector<uint32_t> m_TextureIndices;

        std::shared_ptr<Shader> m_Shader;
    };
}// namespace LunaraEngine