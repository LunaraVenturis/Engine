#pragma once
#include <Core/Ref.hpp>

template <typename T>
Ref<T>::Ref(T* instance) : m_Instance(instance)
{
    if (m_Instance)
    {
        static_assert(std::is_base_of<RefCounted, T>::value, "Class is not RefCounted!");
        std::lock_guard<std::mutex> lock(m);
        Engine::Allocator::AddToAllocatedMemory(m_Instance);
        IncRef();
    }
}

template <typename T>
template <typename T2>
Ref<T>::Ref(const Ref<T2>& other)
{
    m_Instance = (T*) other.m_Instance;
    IncRef();
}

template <typename T>
template <typename T2>
Ref<T>::Ref(Ref<T2>&& other)
{
    m_Instance = (T*) other.m_Instance;
    other.m_Instance = nullptr;
}

template <typename T>
Ref<T>::Ref(const Ref<T>& other) : m_Instance(other.m_Instance)
{
    IncRef();
}

template <typename T>
Ref<T>::~Ref()
{
    DecRef();
}

template <typename T>
Ref<T>& Ref<T>::operator=(std::nullptr_t)
{
    DecRef();
    m_Instance = nullptr;
    return *this;
}

template <typename T>
Ref<T>& Ref<T>::operator=(const Ref<T>& other)
{
    other.IncRef();
    DecRef();

    m_Instance = other.m_Instance;
    return *this;
}

template <typename T>
template <typename T2>
Ref<T>& Ref<T>::operator=(const Ref<T2>& other)
{
    other.IncRef();
    DecRef();

    m_Instance = other.m_Instance;
    return *this;
}

template <typename T>
template <typename T2>
Ref<T>& Ref<T>::operator=(Ref<T2>&& other)
{
    DecRef();

    m_Instance = other.m_Instance;
    other.m_Instance = nullptr;
    return *this;
}

template <typename T>
Ref<T>::operator bool()
{
    return m_Instance != nullptr;
}

template <typename T>
Ref<T>::operator bool() const
{
    return m_Instance != nullptr;
}

template <typename T>
T* Ref<T>::operator->()
{
    return m_Instance;
}

template <typename T>
const T* Ref<T>::operator->() const
{
    return m_Instance;
}

template <typename T>
T& Ref<T>::operator*()
{
    return *m_Instance;
}

template <typename T>
const T& Ref<T>::operator*() const
{
    return *m_Instance;
}

template <typename T>
bool Ref<T>::operator==(const Ref<T>& other) const
{
    return m_Instance == other.m_Instance;
}

template <typename T>
bool Ref<T>::operator!=(const Ref<T>& other) const
{
    return !(*this == other);
}

template <typename T>
Ref<T>::operator T*()
{
    return m_Instance;
}

template <typename T>
Ref<T>::operator T*() const
{
    return m_Instance;
}

template <typename T>
template <typename... Args>
Ref<T> Ref<T>::Create(Args&&... args)
{
    return Ref<T>(Engine::Allocator::Allocate<T>(std::forward<Args>(args)...));
}

template <typename T>
Ref<T> Ref<T>::CopyWithoutIncrement(const Ref<T>& other)
{
    Ref<T> result = nullptr;
    result->m_Instance = other.m_Instance;
    return result;
}

template <typename T>
T* Ref<T>::Raw()
{
    return m_Instance;
}

template <typename T>
const T* Ref<T>::Raw() const
{
    return m_Instance;
}

template <typename T>
void Ref<T>::Reset(T* instance)
{
    DecRef();
    m_Instance = instance;
}

template <typename T>
template <typename T2>
Ref<T2> Ref<T>::As() const
{
    return Ref<T2>(*this);
}

template <typename T>
bool Ref<T>::EqualsObject(const Ref<T>& other)
{
    if (!m_Instance || !other.m_Instance) return false;

    return *m_Instance == *other.m_Instance;
}

template <typename T>
void Ref<T>::IncRef() const
{
    if (m_Instance) { m_Instance->IncRefCount(); }
}

template <typename T>
void Ref<T>::DecRef() const
{
    if (m_Instance)
    {
        m_Instance->DecRefCount();
        if (m_Instance->GetRefCount() == 0)
        {
            std::lock_guard<std::mutex> lock(m);
            Engine::Allocator::Deallocate(m_Instance);
            m_Instance = nullptr;
        }
    }
}