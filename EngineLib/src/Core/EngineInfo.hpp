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
 * Engine Info Definition
 */


#include <string>

namespace Engine
{
    struct EngineInfo {
        std::string Name = "Engine";
        uint32_t Version = 0.1;
    };

    const inline EngineInfo ENGINE_INFO;
}// namespace Engine