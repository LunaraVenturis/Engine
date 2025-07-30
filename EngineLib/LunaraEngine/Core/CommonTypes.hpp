#pragma once

#include <cstdint>
#include <filesystem>

using f32 = float;
using u32 = uint32_t;
using i32 = int32_t;

namespace LunaraEngine
{

    struct ApplicationConfig {
        std::filesystem::path workingDirectory;
        std::filesystem::path assetsDirectory;
        std::filesystem::path shadersDirectory;
        std::filesystem::path texturesDirectory;
        std::string_view windowName;
        uint32_t initialWidth;
        uint32_t initialHeight;
    };
}// namespace LunaraEngine