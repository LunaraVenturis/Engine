#pragma once

/**
 * @file
 * @author Krusto Stoyanov ( k.stoianov2@gmail.com )
 * @brief 
 * @version 1.0
 * @date 
 * 
 * @section DESCRIPTION
 * 
 * RendererSpec structure definition
 */

#include <filesystem>

namespace Engine
{
    struct RendererSpec {
        std::filesystem::path WorkingDirectory;
        std::string_view AppName;
        uint32_t width;
        uint32_t height;
    };

}// namespace Engine