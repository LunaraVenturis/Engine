
#include <unordered_map>
#include <mutex>
#include "Ref.hpp"

void RefCounted::IncRefCount() const
{
    m_RefCount++;
    RefCounted::IncTotalRefCount();
}

void RefCounted::DecRefCount() const
{
    m_RefCount--;
    RefCounted::DecTotalRefCount();
}

uint32_t RefCounted::GetRefCount() const { return m_RefCount.load(); }

void RefCounted::IncTotalRefCount()
{
    RefCounted::s_TotalRefCount++;
    LOG_MEMORY_ALLOC("Total RefCount: %i\n", RefCounted::s_TotalRefCount);
}

void RefCounted::DecTotalRefCount()
{
    RefCounted::s_TotalRefCount--;
    LOG_MEMORY_ALLOC("Total RefCount: %i\n", RefCounted::s_TotalRefCount);
}