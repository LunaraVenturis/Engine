#pragma once

/**
 * @file
 * @author Krusto Stoyanov ( k.stoianov2@gmail.com )
 * @brief 
 * @version 1.0
 * @date 
 * 
 * @section LICENSE
 * MIT License
 * 
 * Copyright (c) 2024 Christian and Krusto
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 * 
 * @section DESCRIPTION
 * 
 * Layer class definition
 */


/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include <Layer/Layer.hpp>
#include <Core/Result.hpp>
#include <string_view>
#include <vector>

/***********************************************************************************************************************
Enum Class definitions
***********************************************************************************************************************/
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

/***********************************************************************************************************************
Class definitions
***********************************************************************************************************************/
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