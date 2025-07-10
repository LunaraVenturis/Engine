#pragma once
#include <cstdint>
#include <vector>
#include <memory>
#include <filesystem>

#include <glm/glm.hpp>

namespace LunaraEngine
{

    struct ParticleBufferElement {
        glm::vec2 Position;
        glm::vec2 Velocity;
        float Life;
    };

    using std::size_t;
    template <typename T>
    class IndexBuffer;
    template <typename T>
    class StorageBuffer;
    class RendererCommandDrawBatch;
    class Shader;

    class ParticleSystem
    {
    public:
        ParticleSystem();
        ~ParticleSystem() = default;

    public:
        static ParticleSystem* GetInstance();
        static void Create(std::filesystem::path shaderPath, std::filesystem::path texturesPath);
        static void Destroy();

    public:
        void Update(float dt);
        void Emit(size_t count, glm::vec2 position, glm::vec2 velocity, float life, float mass);

    public:
        static RendererCommandDrawBatch* CreateDrawCommand();
        static void Flush();
        static std::weak_ptr<Shader> GetShader();

    private:
        inline static ParticleSystem* s_ParticleSystem = nullptr;

        static constexpr size_t MAX_PARTICLES = 1000;

    private:
        size_t m_Offset{};
        size_t m_ParticleCount{};
        size_t m_Capacity{};
        float m_PassedTime{};

        std::vector<glm::vec2> m_Positions;
        std::vector<glm::vec2> m_Velocities;
        std::vector<float> m_Lifes;
        std::vector<uint32_t> m_LifeIndices;
        std::shared_ptr<Shader> m_Shader;
        std::shared_ptr<Shader> m_ComputeShader;
    };
}// namespace LunaraEngine