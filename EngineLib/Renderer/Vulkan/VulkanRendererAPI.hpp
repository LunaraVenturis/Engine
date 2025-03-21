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
#include <Core/STDTypes.h>
#include <Renderer/RendererAPI.hpp>
#include <Renderer/RendererCommands.hpp>
#include <Renderer/Window.hpp>

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
        VkQueue gfxQueue;
        VkSurfaceKHR surface;
        float width;
        float height;
    };

    struct QueueFamilyIndices;

    class VulkanRendererAPI: public RendererAPI
    {
    public:
        VulkanRendererAPI() = default;
        ~VulkanRendererAPI() = default;
        VulkanRendererAPI(const VulkanRendererAPI& other) = delete;
        void operator=(const VulkanRendererAPI&) = delete;

    public:
        virtual Window* GetWindow() override { return m_RendererData->window; };

        virtual void Present() override { (void) nullptr; }

        virtual void Init() override;
        virtual void Destroy() override;

    private:
        void CreateInstance();
        void CreateDevice();
        void PickPhysicalDevice();
        void CreateLogicalDevice();

    private:
        VkInstance* GetVkInstance() { return &m_RendererData->instance; }

        VkDevice* GetDevice() { return &m_RendererData->device; }

        VkPhysicalDevice* GetPhysicalDevice() { return &m_RendererData->physicalDevice; }

        VkDebugUtilsMessengerEXT* GetDebugHandler() { return &m_RendererData->debug; }

        void CleanUpVulkan();

        void SetPhysicalDevice(VkPhysicalDevice device) { m_RendererData->physicalDevice = device; }

        void SetDevice(VkDevice device) { m_RendererData->device = device; }

        void GetPlatformExtensions(std::vector<const char*>& extensions);
        bool IsDeviceSuitable(VkPhysicalDevice device);
        QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);

    private:
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
    };

}// namespace LunaraEngine