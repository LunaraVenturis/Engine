#include "Window.hpp"
#include <iostream>

Window::WindowClass Window::WindowClass::wndClass;

Window::WindowClass::WindowClass() noexcept : hInstance{GetModuleHandle(nullptr)}
{
    WNDCLASSEX winClass;
    winClass = {};
    winClass.cbSize = sizeof(winClass);
    winClass.lpszClassName = GetName();
    winClass.lpfnWndProc = HandleMessageSetup;
    winClass.cbClsExtra = 0;
    winClass.cbWndExtra = 0;
    winClass.hIcon = nullptr;
    winClass.hCursor = nullptr;
    winClass.hbrBackground = nullptr;
    winClass.lpszMenuName = nullptr;
    winClass.hIconSm = nullptr;
    winClass.style = CS_VREDRAW | CS_HREDRAW;
    winClass.hInstance = GetInstance();
    RegisterClassEx(&winClass);
}

HINSTANCE Window::WindowClass::GetInstance() noexcept { return wndClass.hInstance; }

const char* Window::WindowClass::GetName() noexcept { return windowName; }

Window::WindowClass::~WindowClass() { UnregisterClass(wndClass.windowName, GetInstance()); }

Window::Window(i32 w, i32 h, const char* name) noexcept : width(w), height(h)
{

    RECT windowRect = {};
    windowRect.left = 100;
    windowRect.bottom = 100;
    windowRect.right = windowRect.left + width;
    windowRect.top = windowRect.bottom + height;
    AdjustWindowRect(&windowRect, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE);
    hwnd = CreateWindow(WindowClass::GetName(), name, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
                        windowRect.right - windowRect.left, windowRect.top - windowRect.bottom, nullptr, nullptr,
                        WindowClass::GetInstance(), this);
    if (!hwnd)
    {
        MessageBox(hwnd, "Couldn't init hwnd", "Error!", MB_OK);
        return;
    }
    ShowWindow(hwnd, SW_SHOWDEFAULT);
 //   pGfx = std::make_unique<Graphics>(hwnd, width, height);
}

Window::~Window() { DestroyWindow(hwnd); }

LRESULT CALLBACK Window::HandleMessageSetup(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
    if (msg == WM_NCCREATE)
    {
        //extract the ptr and link the pointer to the window procedure.
        const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
        Window* const pWnd = static_cast<Window*>(pCreate->lpCreateParams);
        SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd));
        SetWindowLongPtr(hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Window::HandleMessageThunk));

        return pWnd->HandleMessage(hwnd, msg, wParam, lParam);
    }
    //if we get a message before NCCREATE we handle it here
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

LRESULT CALLBACK Window::HandleMessageThunk(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
    Window* const pWnd = reinterpret_cast<Window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    return pWnd->HandleMessage(hwnd, msg, wParam, lParam);
}

LRESULT Window::HandleMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
    switch (msg)
    {
        case WM_CLOSE:
            if (MessageBox(hwnd, "Do you really want to close?", "Engine", MB_OKCANCEL) == IDOK)
            {
                PostQuitMessage(0);
            }
            break;
        case WM_KILLFOCUS:
           // kbd.ClearState();
            break;
        case WM_KEYDOWN:
          //  kbd.onKeyPressed(static_cast<u8>(wParam));
            break;
        case WM_KEYUP:
          //  kbd.onKeyReleased(static_cast<u8>(wParam));
            break;
        case WM_CHAR:
          //  kbd.onChar(static_cast<u8>(wParam));
            break;
        case WM_PAINT:
            /*
            if (pGfx)
            {
                pGfx->OnUpdate();
                pGfx->OnRender();
            }
            */
            break;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

std::optional<i32> Window::ProcessMessages()
{
    MSG msg = {};
    while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
    {
        if (msg.message == WM_QUIT) { return msg.wParam; }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return {};
}
