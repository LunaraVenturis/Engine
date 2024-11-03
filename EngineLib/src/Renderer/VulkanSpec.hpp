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
 * Vulkan specific renderer spec structure definition
 */

#include <filesystem>
#include <vector>

#include "RendererSpec.hpp"

namespace Engine
{
    struct VulkanSpec {
        RendererSpec rendererSpec;
        std::vector<std::string> extensions;
    };

}// namespace Engine