#include "ParticleSystem.hpp"
#include <Renderer/IndexBuffer.hpp>
#include <Renderer/Shader.hpp>
#include <Renderer/StorageBuffer.hpp>
#include <Renderer/RendererCommands.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/random.hpp>
#include <numeric>

#include <span>

namespace LunaraEngine
{
    ParticleSystem::ParticleSystem()
    {
        m_Capacity = MAX_PARTICLES;
        m_Offset = 0;
        m_ParticleCount = m_Capacity;
        m_Positions.resize(m_Capacity);
        m_Velocities.resize(m_Capacity);
        m_Lifes.resize(m_Capacity);
        m_LifeIndices.resize(m_Capacity);
        std::fill(m_Lifes.begin(), m_Lifes.end(), 0.0f);
        std::fill(m_LifeIndices.begin(), m_LifeIndices.end(), 0);
        std::fill(m_Positions.begin(), m_Positions.end(), glm::vec2(0.0f));
        std::fill(m_Velocities.begin(), m_Velocities.end(), glm::vec2(0.0f));
    }

    void ParticleSystem::Create(std::filesystem::path shaderPath)
    {
        s_ParticleSystem = new ParticleSystem();

        s_ParticleSystem->m_Shader = Shader::Create(
                ShaderInfoBuilder("ParticleShader", shaderPath)
                        .AddResources({ShaderResourceBuilder("UniformBuffer", ShaderResourceType::UniformBuffer)
                                               .AddAttribute("model", ShaderResourceAttributeType::Mat4)
                                               .AddAttribute("view", ShaderResourceAttributeType::Mat4)
                                               .AddAttribute("projection", ShaderResourceAttributeType::Mat4)
                                               .AddAttribute("zoom", ShaderResourceAttributeType::Float)
                                               .Build(),
                                       ShaderResourceBuilder("ParticlePositions", ShaderResourceType::StorageBuffer,
                                                             MAX_PARTICLES)
                                               .AddAttribute("Position", ShaderResourceAttributeType::Vec2)
                                               .Build(),
                                       ShaderResourceBuilder("ParticleVelocities", ShaderResourceType::StorageBuffer,
                                                             MAX_PARTICLES)
                                               .AddAttribute("Velocity", ShaderResourceAttributeType::Vec2)
                                               .Build(),
                                       ShaderResourceBuilder("ParticleLifes", ShaderResourceType::StorageBuffer,
                                                             MAX_PARTICLES)
                                               .AddAttribute("Life", ShaderResourceAttributeType::Float)
                                               .Build(),
                                       ShaderResourceBuilder("AliveParticles", ShaderResourceType::StorageBuffer,
                                                             MAX_PARTICLES)
                                               .AddAttribute("Index", ShaderResourceAttributeType::UInt)
                                               .Build()})
                        .Build());
    }

    void ParticleSystem::Destroy() { delete s_ParticleSystem; }

    void ParticleSystem::Update(float dt)
    {
        constexpr float emitPerSecond = 150.0f;
        constexpr float emitInterval = 1.0f / emitPerSecond;
        constexpr float speed = 300.0f;
        constexpr float windSpeed = 5.0f;
        constexpr float windResistance = 0.001f;
        constexpr glm::vec2 windDir = {0.2f, -0.08f};

        for (size_t id = 0; id < m_Capacity; ++id)
        {
            auto& particleLife = m_Lifes[id];
            if (particleLife <= 0.0f)
            {
                particleLife = 0.0f;
                continue;
            }

            m_Positions[id] += m_Velocities[id] * speed * dt;
            m_Velocities[id] += windDir * windSpeed * dt;
            m_Velocities[id] *= 1.0f - windResistance;
            particleLife -= dt;
        }

        m_PassedTime += dt;

        auto emitCount = static_cast<size_t>(m_PassedTime / emitInterval);

        if (emitCount > 0)
        {
            m_PassedTime -= (float) emitCount * emitInterval;
            for (size_t i = 0; i < emitCount; i++)
            {
                // float t = (1.0f + (float) i) / (1.0f + (float) emitCount);
                glm::vec2 origin = {500.0f, 750.0f};
                glm::vec2 velocity = {glm::linearRand(-0.04f, 0.04f), -1.0f};
                glm::vec2 position = origin + glm::vec2{glm::linearRand(-0.5f, 0.5f), 0.0f} * speed;

                Emit(1, position, velocity, 2.0f, 1.0f);
            }
        }
    }

    void ParticleSystem::Emit(size_t count, glm::vec2 position, glm::vec2 velocity, float life, float mass)
    {
        for (size_t i = 0; i < count; i++)
        {
            auto slot = std::ranges::find_if(m_Lifes, [](auto& life) { return life <= 0.0f; });
            if (slot == m_Lifes.end()) return;
            auto index = (size_t) std::distance(m_Lifes.begin(), slot);
            m_Positions[index] = position;
            m_Velocities[index] = velocity;
            m_Lifes[index] = life;
        }
        (void) mass;
    }

    ParticleSystem* ParticleSystem::GetInstance() { return s_ParticleSystem; }

    RendererCommandDrawBatch* ParticleSystem::CreateDrawCommand()
    {
        auto instance = GetInstance();
        auto shader = instance->m_Shader;

        size_t aliveParticleCount = 0;

        std::ranges::for_each(instance->m_Lifes, [&](const auto& life) {
            if (life <= 0.0f) return;
            instance->m_LifeIndices[aliveParticleCount++] = (uint32_t) (&life - &instance->m_Lifes[0]);
        });

        RendererCommandDrawBatch::BufferUploadList uploadList{
                {(StorageBuffer<uint8_t>*) shader->GetBuffer(ShaderBinding::_1),
                 std::span<uint8_t>{(uint8_t*) (instance->m_Positions.data()), instance->m_Positions.size()}},
                {(StorageBuffer<uint8_t>*) shader->GetBuffer(ShaderBinding::_2),
                 std::span<uint8_t>{(uint8_t*) (instance->m_Velocities.data()), instance->m_Velocities.size()}},
                {(StorageBuffer<uint8_t>*) shader->GetBuffer(ShaderBinding::_3),
                 std::span<uint8_t>{(uint8_t*) (instance->m_Lifes.data()), instance->m_Lifes.size()}},
                {(StorageBuffer<uint8_t>*) shader->GetBuffer(ShaderBinding::_4),
                 std::span<uint8_t>{(uint8_t*) (instance->m_LifeIndices.data()), aliveParticleCount}},
        };

        RendererCommandDrawBatch* command = new RendererCommandDrawBatch(uploadList, aliveParticleCount, 0);
        return command;
    }

    void ParticleSystem::Flush()
    {
        auto instance = ParticleSystem::GetInstance();
        instance->m_Offset = 0;
        instance->m_ParticleCount = 0;
    }

    std::weak_ptr<Shader> ParticleSystem::GetShader() { return s_ParticleSystem->m_Shader; }

}// namespace LunaraEngine
