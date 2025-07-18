#pragma once
#include <string_view>
#include <filesystem>
#include <Core/STDTypes.h>

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
        static ApplicationResult Create(std::filesystem::path workingDirectory, std::string_view name, uint32_t width,
                                        uint32_t height);

        static void Run();

        static void Close();

    private:
        inline static std::filesystem::path m_WorkingDirectory;
    };
}// namespace LunaraEngine