#pragma once
#include <algorithm>
#include <Core/Allocator.hpp>
#include <Core/Log.hpp>
#include "Allocator.hpp"

namespace Engine
{
    template <typename T, typename... Args>
    T* Allocator::Allocate(Args&&... args)
    {
        auto ptr = new T(std::forward<Args>(args)...);
        LOG_MEMORY_ALLOC("Allocated %ziB\n", sizeof(T));

        s_AllocatedMemorySize += sizeof(T);
        s_AllocatedMemory[ptr] = sizeof(T);
        return ptr;
    }

    template <typename T>
    inline T* Allocator::AllocateArray(size_t size)
    {
        auto ptr = new T[size];
        LOG_MEMORY_ALLOC("Allocated %ziB\n", sizeof(T));

        s_AllocatedMemorySize += sizeof(T);
        s_AllocatedMemory[ptr] = sizeof(T);
        return ptr;
    }

    template <typename T, size_t size>
    void Allocator::AddToAllocatedMemory(T* instance)
    {
        if (instance)
        {
            auto [ignore, inserted] = s_AllocatedMemory.try_emplace(instance, size);
            if (inserted) { s_AllocatedMemorySize += size; }
        }
    }

    template <typename T>
    void Allocator::Deallocate(T* instance)
    {
        if (s_AllocatedMemory.find(instance) != s_AllocatedMemory.end())
        {
            s_AllocatedMemorySize -= s_AllocatedMemory[instance];
            delete instance;
            LOG_MEMORY_ALLOC("Deallocated %ziB\n", s_AllocatedMemory[instance]);

            s_AllocatedMemory.erase(instance);
        }
    }

    template <typename T>
    void Allocator::DeallocateArray(T* instance)
    {
        if (s_AllocatedMemory.find(instance) != s_AllocatedMemory.end())
        {
            s_AllocatedMemorySize -= s_AllocatedMemory[instance];
            delete[] instance;
            LOG_MEMORY_ALLOC("Deallocated %ziB\n", s_AllocatedMemory[instance]);

            s_AllocatedMemory.erase(instance);
        }
    }

    template <typename T>
    bool Allocator::IsLive(T* instance)
    {
        return s_AllocatedMemory.find(instance) != s_AllocatedMemory.end();
    }

    template <typename T>
    size_t Allocator::Copy(T* destination, T* source, size_t size)
    {
        if (nullptr == destination) { return 0; }
        if (nullptr == source) { return 0; }

        std::for_each_n(destination, size, [source](T& value) mutable {
            value = *source;
            ++source;
        });

        return size;
    }


}// namespace Engine