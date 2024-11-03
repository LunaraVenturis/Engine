#pragma once

#include <mutex>
#include <atomic>

#include <Core/Core.hpp>

class RefCounted
{
public:
    void IncRefCount() const;

    void DecRefCount() const;

    uint32_t GetRefCount() const;

private:
    static void IncTotalRefCount();

    static void DecTotalRefCount();

    inline static size_t s_TotalRefCount = 0;
    mutable std::atomic<uint32_t> m_RefCount = 0;
};

inline static std::mutex m{};

template <typename T>
class Ref
{
public:
    Ref() = default;

    Ref(T* instance = nullptr);

    template <typename T2>
    Ref(const Ref<T2>& other);

    template <typename T2>
    Ref(Ref<T2>&& other);

    ~Ref();

    Ref(const Ref<T>& other);

public:
    Ref& operator=(std::nullptr_t);

    Ref& operator=(const Ref<T>& other);

    template <typename T2>
    Ref& operator=(const Ref<T2>& other);

    template <typename T2>
    Ref& operator=(Ref<T2>&& other);

    operator bool();

    operator bool() const;

    T* operator->();

    const T* operator->() const;
    T& operator*();
    const T& operator*() const;

    bool operator==(const Ref<T>& other) const;

    bool operator!=(const Ref<T>& other) const;

    operator T*();
    operator T*() const;

public:
    template <typename... Args>
    static Ref<T> Create(Args&&... args);

    static Ref<T> CopyWithoutIncrement(const Ref<T>& other);

public:
    T* Raw();

    const T* Raw() const;

    void Reset(T* instance = nullptr);
    template <typename T2>
    Ref<T2> As() const;

    bool EqualsObject(const Ref<T>& other);

private:
    void IncRef() const;
    void DecRef() const;

    template <class T2>
    friend class Ref;
    mutable T* m_Instance{};
};

template <typename T>
class WeakRef
{
public:
    WeakRef() = default;
    ~WeakRef() = default;

    WeakRef(Ref<T> ref) { m_Instance = ref.Raw(); }

    WeakRef(T* instance) { m_Instance = instance; }

public:
    bool IsValid() const { return m_Instance ? Engine::Allocator::IsLive(m_Instance) : false; }

    Ref<T> Lock() const
    {
        if (IsValid()) { return Ref<T>(m_Instance); }
        return Ref<T>();
    }

public:
    operator bool() const { return IsValid(); }

private:
    T* m_Instance = nullptr;
};

#include <Core/Ref.impl.hpp>