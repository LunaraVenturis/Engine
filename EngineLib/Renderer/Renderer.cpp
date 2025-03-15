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
 * Copyright (c) 2025 Krusto
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
 * Renderer definitions
 */

#ifndef ENGINELIB_RENDERER_RENDERER_CPP
#define ENGINELIB_RENDERER_RENDERER_CPP

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "Renderer.hpp"
#include "Texture.h"
#include "Fonts.h"
#include <string_view>

namespace LunaraEngine
{

    RendererResultType Renderer::Init(std::string_view window_name, uint32_t width, uint32_t height)
    { 
        return RendererResultType::Renderer_Result_Error;
    }

    void Renderer::Destroy() { }

    void Renderer::Present() { }

    /*
    void Renderer::DrawQuad(const FRect& rect, const Color4& color)
    {
        RendererCommandDrawQuad quad;
        quad.x = rect.x;
        quad.y = rect.y;
        quad.width = rect.w;
        quad.height = rect.h;
        quad.r = (uint8_t) color.r;
        quad.g = (uint8_t) color.g;
        quad.b = (uint8_t) color.b;
        quad.a = (uint8_t) color.a;
        //RendererCmdDrawQuad(&quad);
    }

    void Renderer::DrawTexture(float x, float y, Texture* texture)
    {
        RendererCommandDrawTexture tex;
        tex.x = x;
        tex.y = y;
        tex.texture = texture;
       // RendererCmdDrawTexture(&tex);
    }

    void Renderer::DrawCircle(float x, float y, float radius, const Color4& color)
    {
        RendererCommandDrawCircle circle;
        circle.x = x;
        circle.y = y;
        circle.radius = radius;
        circle.r = (uint8_t) color.r;
        circle.g = (uint8_t) color.g;
        circle.b = (uint8_t) color.b;
        circle.a = (uint8_t) color.a;
      //  RendererCmdDrawCircle(&circle);
    }

    void Renderer::DrawText(std::string_view text, Font* font, float x, float y, const Color4& color,
                            RendererTextAlignAttribute align)
    {
        RendererCommandDrawText text_cmd;
        text_cmd.text = (char*) text.data();
        text_cmd.font = (void*) font;
        text_cmd.x = x;
        text_cmd.y = y;
        text_cmd.r = (uint8_t) color.r;
        text_cmd.g = (uint8_t) color.g;
        text_cmd.b = (uint8_t) color.b;
        text_cmd.a = (uint8_t) color.a;
        text_cmd.align = align;
       // RendererCmdDrawText(&text_cmd);
    }

    void Renderer::Clear(const Color4& color)
    {
        RendererCommandClear clear;
        clear.r = (uint8_t) color.r;
        clear.g = (uint8_t) color.g;
        clear.b = (uint8_t) color.b;
        clear.a = (uint8_t) color.a;
        //RendererCmdClear(&clear);
    }
    */
    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                        VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                                        void* pUserData)
    {
        if (messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
            printf("%s\n", std::string(pCallbackData->pMessage));
        if (messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
            printf("%s\n", std::string(pCallbackData->pMessage));

        return VK_FALSE;
    }

    void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger,
                                       const VkAllocationCallbacks* pAllocator)
    {
        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance,
                                                                                "vkDestroyDebugUtilsMessengerEXT");
        if (func != nullptr) { func(instance, debugMessenger, pAllocator); }
    }

    VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
                                          const VkAllocationCallbacks* pAllocator,
                                          VkDebugUtilsMessengerEXT* pDebugMessenger)
    {
        auto func =
                (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
        if (func != nullptr) { return func(instance, pCreateInfo, pAllocator, pDebugMessenger); }
        else { return VK_ERROR_EXTENSION_NOT_PRESENT; }
    }

    void Renderer::BeginRenderPass() {  }

    void Renderer::EndRenderPass() {  }

    void Renderer::Flush() {  }

    void Renderer::CreateInstance() 
    { 
        VkApplicationInfo appInfo {};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "Tobenamed";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "KrustyoManja";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_MAKE_VERSION(1, 3, 0);

        VkInstanceCreateInfo createInfo {};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;

        std::vector<const char*> names = {"VK_KHR_surface"};
        GetPlatformExtensions(names);
        #ifdef _DEBUG
        createInfo.enabledLayerCount = 1;
        std::array<const char*, 1> layers = {"VK_LAYER_KHRONOS_validation"};
        createInfo.ppEnabledLayerNames = layers.data();
        names.push_back("VK_EXT_debug_utils");
        names.push_back("VK_EXT_debug_report");
        #else
        createInfo.enabledLayerCount = 0;
        createInfo.ppEnabledLayerNames = nullptr;
        #endif
        createInfo.enabledExtensionCount = names.size();
        createInfo.ppEnabledExtensionNames = names.data();

        if (vkCreateInstance(&createInfo, nullptr, Rend::GetInstance()) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create instance!");
        }
        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
        debugCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        debugCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                                          VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                          VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        debugCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                                      VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                                      VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        debugCreateInfo.pfnUserCallback = debugCallback;
        debugCreateInfo.pUserData = nullptr;// Optional

        CreateDebugUtilsMessengerEXT(*Rend::GetInstance(), &debugCreateInfo, nullptr, Rend::GetDebug());
    }

    void Renderer::GetPlatformExtensions(std::vector<const char*>& extensions) 
    {
       #ifdef _WIN32
        extensions.emplace_back("VK_KHR_win32_surface");
       #elif LINUX
        extensions.emplace_back("VK_KHR_xlib_surface");
       #endif 
    }

    Window* Renderer::GetWindow() { return Rend::GetWindow(); }

    void Renderer::CleanUpVulkan() 
    { 
        Rend::DestroyRend();
    }

}// namespace LunaraEngine
#endif