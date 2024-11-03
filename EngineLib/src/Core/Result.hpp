#pragma once

/**
 * @author Krusto Stoyanov ( k.stoianov2@gmail.com )
 * @version 1.0
 * @date 31.10.2024

 * @section DESCRIPTION
 * 
 * Result struct definition
 */

#include <cstdint>
#include <vulkan/vulkan.hpp>

namespace Engine
{
    template <typename T = VkResult, typename E = uint32_t>
    struct ResultValue {
        T status;
        E value;

        ResultValue() {}

        explicit ResultValue(T status) : status(status) {}

        explicit ResultValue(E& value) : value(value) {}

        explicit ResultValue(E&& value) : value(value) {}

        ResultValue(T status, const E& value) : status(status), value(value) {}

        ResultValue(T status, E&& value) : status(status), value(std::move(value)) {}

        operator T() { return status; }

        operator const T() const { return status; }
    };

    template <typename StatusType = VkResult>
    struct ResultValueType {
        StatusType status;

        ResultValueType() {}

        ResultValueType(StatusType status) : status(status) {}

        operator StatusType() { return status; }

        operator const StatusType() const { return status; }
    };
}// namespace Engine