#pragma once
#include <string_view>
#include <filesystem>
#include <LunaraEngine/Core/CommonTypes.hpp>

namespace LunaraEngine
{
    typedef enum ApplicationResult
    {
        ApplicationResult_Success = 0,
        ApplicationResult_Fail = 1,
    } ApplicationResult;

    class Application
    {
    public:
        Application() = default;
        ~Application() = default;

    public:
        static ApplicationResult Create(ApplicationConfig&& config);

        static void Run();

        static void Close();

    private:
        static bool ValidateConfig(ApplicationConfig&& config);

    private:
        inline static ApplicationConfig s_Config;
    };
}// namespace LunaraEngine