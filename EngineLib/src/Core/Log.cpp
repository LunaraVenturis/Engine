#include "Log.hpp"

namespace Engine
{

    std::shared_ptr<Logger> Logger::s_Logger;

    void Logger::Create() { Logger::s_Logger = std::make_shared<Logger>(); }

#ifdef LWLOG
    void Logger::Init()
    {
        console = <lwlog::logger<lwlog::default_log_policy, lwlog::default_storage_policy,
                                 lwlog::single_threaded_policy, lwlog::sinks::stdout_sink>>("CONSOLE");
        console->set_pattern(".br_red([%T]) .green([%l]): .br_cyan(%v)");
        console->set_level_filter(lwlog::level::info | lwlog::level::debug | lwlog::level::critical);

#ifdef _WIN32
        // Set output mode to handle virtual terminal sequences
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        if (hOut == INVALID_HANDLE_VALUE) { console->error("INVALID_HANDLE_VALUE"); }

        DWORD dwMode = 0;
        if (!GetConsoleMode(hOut, &dwMode)) { console->error("GetConsoleMode"); }

        dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        if (!SetConsoleMode(hOut, dwMode)) { console->error("SetConsoleMode ENABLE_VIRTUAL_TERMINAL_PROCESSING"); }
#endif
    }

    void Logger::Destroy() { console.reset(); }

    Logger* Logger::GetInstance() { return s_Logger.get(); }

#else
    void Engine::Logger::Init() {}

    void Logger::Destroy() {}

    Logger* Logger::GetInstance() { return s_Logger.get(); }
#endif
}// namespace Engine