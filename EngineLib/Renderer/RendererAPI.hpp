/**
 * @file
 * @author Krusto Stoyanov ( k.stoianov2@gmail.com ) 
 * @coauthor Neyko Naydenov (neyko641@gmail.com)
 * @brief 
 * @version 1.0
 * @date 
 * 
 * @section LICENSE
 * MIT License
 * 
 * Copyright (c) 2025 Krusto, Neyko
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
 * Renderer declarations
 */

#pragma once

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "Core/STDTypes.h"
#include "RendererCommands.hpp"
#include "Window.hpp"

#include <vulkan/vulkan.h>
#include <SDL3/SDL_render.h>

#include <string_view>
#include <vector>
#include <memory>

namespace LunaraEngine
{

    /***********************************************************************************************************************
Macro definitions
***********************************************************************************************************************/

    struct RendererDataType {
        std::vector<RendererCommand*> command_stack;
        LunaraEngine::Window* window;
        SDL_Renderer* renderer;
        SDL_Surface* surface;
        VkInstance instance;
        VkDebugUtilsMessengerEXT debug;
        VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
        VkDevice device;
        float width;
        float height;
    };

    struct QueueFamilyIndices;

    class RendererAPI
    {
    public:
        RendererAPI() = default;
        ~RendererAPI() = default;
        RendererAPI(const RendererAPI& other) = delete;
        void operator=(const RendererAPI&) = delete;

        inline static RendererAPI* GetAPIInstance() { return &s_Instance; }

    public:
        static void InitRendererAPI();
        static void DestroyRendererAPI();

    public:
        static void CreateInstance();
        static void CreateDevice();

    public:
        static VkInstance* GetVkInstance() { return &s_Instance.m_RendererData->instance; }

        static Window* GetWindow() { return s_Instance.m_RendererData->window; }

        static VkDevice GetDevice() { return s_Instance.m_RendererData->device; }

        static VkPhysicalDevice* GetPhysicalDevice() { return &s_Instance.m_RendererData->physicalDevice; }

        static VkDebugUtilsMessengerEXT* GetDebugHandler() { return &s_Instance.m_RendererData->debug; }

    private:
        void CleanUpVulkan();

        void SetPhysicalDevice(VkPhysicalDevice device) { s_Instance.m_RendererData->physicalDevice = device; }

        void SetDevice(VkDevice device) { s_Instance.m_RendererData->device = device; }

        void GetPlatformExtensions(std::vector<const char*>& extensions);
        void PickPhysicalDevice();
        bool IsDeviceSuitable(VkPhysicalDevice device);
        QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);

        static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                            VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                            const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                                            void* pUserData);
        static void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger,
                                                  const VkAllocationCallbacks* pAllocator);

        static VkResult CreateDebugUtilsMessengerEXT(VkInstance instance,
                                                     const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
                                                     const VkAllocationCallbacks* pAllocator,
                                                     VkDebugUtilsMessengerEXT* pDebugMessenger);

    private:
        std::unique_ptr<RendererDataType> m_RendererData;

    private:
        static RendererAPI s_Instance;
    };

}// namespace LunaraEngine