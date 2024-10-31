#pragma once
#include <unordered_map>

namespace Engine
{
    class Allocator
    {
    public:
        Allocator() = default;
        ~Allocator() = default;

    public:
        Allocator* GetInstance() { return this; }

        template <typename T, typename... Args>
        static T* Allocate(Args&&... args);

        template <typename T, size_t size = sizeof(T)>
        static void AddToAllocatedMemory(T* instance = nullptr);

        template <typename T>
        static void Deallocate(T* instance);

        template <typename T>
        static bool IsLive(T* instance);

    private:
        inline static size_t s_AllocatedMemorySize{};
        inline static std::unordered_map<void*, size_t> s_AllocatedMemory{};
    };

}// namespace Engine

#include <Core/Allocator.impl.hpp>