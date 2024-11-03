#pragma once
#include <unordered_map>
#include <cstdint>

namespace Engine
{
    class Allocator
    {
    public:
        Allocator() = default;
        ~Allocator() = default;

    public:
        template <typename T, typename... Args>
        static T* Allocate(Args&&... args);

        template <typename T>
        static T* AllocateArray(const size_t size);

        template <typename T, size_t size = sizeof(T)>
        static void AddToAllocatedMemory(T* instance = nullptr);

        template <typename T>
        static void Deallocate(T* instance);

        template <typename T>
        static void DeallocateArray(T* instance);

        template <typename T>
        static bool IsLive(T* instance);

        template <typename T>
        static size_t Copy(T* destination, T* source, size_t size);

    private:
        inline static size_t s_AllocatedMemorySize{};
        inline static std::unordered_map<void*, size_t> s_AllocatedMemory{};
    };

}// namespace Engine

#include <Core/Allocator.impl.hpp>