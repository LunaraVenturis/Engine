#pragma once
#include <Core/Allocator.hpp>
#include <Core/Log.hpp>

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
    bool Allocator::IsLive(T* instance)
    {
        return s_AllocatedMemory.find(instance) != s_AllocatedMemory.end();
    }


}// namespace Engine