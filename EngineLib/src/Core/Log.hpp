#pragma once
#include <cstdlib>
#include <iostream>
#include <memory>
#include <stdio.h>

namespace Engine
{
    class Logger
    {
    public:
        Logger() { Init(); }

        ~Logger() { Destroy(); };

    public:
        void Init();
        void Destroy();

    public:
        static void Create();

        static Logger* GetInstance();
#ifdef LWLOG
        std::shared_ptr<lwlog::logger<lwlog::default_log_policy, lwlog::default_storage_policy,
                                      lwlog::single_threaded_policy, lwlog::sinks::stdout_sink>>
                console;
#endif
    private:
        static std::shared_ptr<Logger> s_Logger;
    };

}// namespace Engine


#ifdef LWLOG
#define LOG(...)                                                                                                       \
    Engine::Logger::GetInstance()->console->info(__VA_ARGS__);                                                         \
    std::flush(std::cout);
#define LOG_INFO(...)                                                                                                  \
    Engine::Logger::GetInstance()->console->info(__VA_ARGS__);                                                         \
    std::flush(std::cout);
#define LOG_ERROR(...)                                                                                                 \
    Engine::Logger::GetInstance()->console->error(__VA_ARGS__);                                                        \
    std::flush(std::cout);
#define LOG_DEBUG(...)                                                                                                 \
    Engine::Logger::GetInstance()->console->debug(__VA_ARGS__);                                                        \
    std::flush(std::cout);
#define LOG_WARNING(...)                                                                                               \
    Engine::Logger::GetInstance()->console->warning(__VA_ARGS__);                                                      \
    std::flush(std::cout);
#define LOG_CRITICAL(...)                                                                                              \
    Engine::Logger::GetInstance()->console->critical(__VA_ARGS__);                                                     \
    std::flush(std::cout);
#else
#define LOG(...) printf_s(__VA_ARGS__)

template <typename... Args>
inline static void LOG_INFO(const char* format, Args... args)
{
#ifdef ENGINE_ENABLE_VERBOSE_LOG
    LOG("[INFO] ");
    LOG(format, args...);
#endif
}

template <typename T>
inline static void LOG_INFO(const T* format)
{
#ifdef ENGINE_ENABLE_VERBOSE_LOG
    LOG("[INFO] ");
    LOG("%s", format);
#endif
}

template <typename... Args>
inline static void LOG_DEBUG(const char* format, Args... args)
{
#ifdef ENGINE_ENABLE_DEBUG_LOG
    LOG("[DEBUG] ");
    LOG(format, args...);
#endif
}

inline static void LOG_DEBUG(const char* format)
{
#ifdef ENGINE_ENABLE_DEBUG_LOG
    LOG("[DEBUG] ");
    LOG(format);
#endif
}

template <typename... Args>
inline static void LOG_ERROR(const char* format, Args... args)
{
    LOG("[ERROR] ");
    LOG(format, args...);
}

inline static void LOG_ERROR(std::string_view format)
{
    LOG("[ERROR] ");
    LOG(format.data());
}

inline static void LOG_ERROR(const std::string& format)
{
    LOG("[ERROR] ");
    LOG(format.c_str());
}

template <typename... Args>
inline static void LOG_WARNING(const char* format, Args... args)
{
    LOG("[WARNING] ");
    LOG(format, args...);
}

inline static void LOG_WARNING(const char* format)
{
    LOG("[WARNING] ");
    LOG(format);
}

template <typename... Args>
inline static void LOG_MEMORY_ALLOC(const char* format, Args... args)
{
#ifdef ENGINE_ENABLE_MEMORY_DEBUG_LOG
    LOG("[MEMORY] ");
    LOG(format, args...);
#endif
}

inline static void LOG_MEMORY_ALLOC(const char* format)
{
#ifdef ENGINE_ENABLE_MEMORY_DEBUG_LOG
    LOG("[MEMORY] ");
    LOG(format);
#endif
}

#endif