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
 * Layer class definition
 */

#include <Layer/Layer.hpp>
#include <Core/Result.hpp>
#include <string_view>
#include <vector>

namespace Engine
{
    enum class LayerStatus
    {
        None,
        Added,
        Removed,
        NotAdded,
        AlreadyExist,
        Error,
        Success
    };
    enum class LayerStackStatus
    {
        None,
        Initialized,
        Destroyed,
        Error,
        Success,
    };
}// namespace Engine

namespace Engine
{

    class LayerStack
    {
    public:
        LayerStack() = default;
        ~LayerStack() = default;

        static ResultValueType<LayerStackStatus> Init();
        template <typename T>
        static ResultValueType<LayerStatus> AddLayer();
        static ResultValueType<LayerStatus> RemoveLayer(std::string_view name);
        static ResultValueType<LayerStackStatus> Destroy();
        static ResultValue<LayerStackStatus, LayerStack*> Get();
        static ResultValue<LayerStatus, Layer*> GetLayer(std::string_view name);
        static ResultValue<LayerStatus, std::vector<Layer*>*> GetLayers();
        static ResultValueType<LayerStackStatus> InitLayers();
        static ResultValueType<LayerStackStatus> DestroyLayers();

    private:
        static LayerStack* s_LayerStack;

        std::vector<Layer*> m_Layers;
    };

}// namespace Engine

#include "LayerStack.impl.hpp"