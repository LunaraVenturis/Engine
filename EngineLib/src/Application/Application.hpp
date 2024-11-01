#pragma once

/**
 * @author Krusto Stoyanov ( k.stoianov2@gmail.com )
 * @date 31.10.2024
 * 
 * @section DESCRIPTION
 * 
 * Application class definition
 */

#include <filesystem>

namespace Engine
{
    struct ApplicationSpec {
        std::string ApplicationName;
        std::filesystem::path WorkingDirectory;
        uint32_t StartupWidth;
        uint32_t StartupHeight;
    };

    class Application
    {
    public:
        Application() = default;
        ~Application() = default;

    public:
        static void Init(ApplicationSpec applicationSpec);

        template <typename T>
        static void AddLayer();

        static void Run();

        static void Destroy();

        static Application* Get();
        static ApplicationSpec& GetSpec();

    private:
        static Application* s_Application;

    private:
        bool m_StoppedFlag{};
        ApplicationSpec m_ApplicationSpec{};
    };

}// namespace Engine

#include "Application.impl.hpp"