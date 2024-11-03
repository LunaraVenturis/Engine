#include <Platform/WindowInstance.hpp>
#include <types.hpp>
#include <optional>
class Window
{
#ifdef _WIN32
private:
    class WindowClass
    {
    public:
        static const char* GetName() noexcept;
        static HINSTANCE GetInstance() noexcept;

    private:
        WindowClass() noexcept;
        ~WindowClass();
        WindowClass& operator=(const WindowClass&) = delete;
        WindowClass(const WindowClass&) = delete;
        static constexpr const char* windowName = "Engine";
        static WindowClass wndClass;
        HINSTANCE hInstance;
    };

public:
    Window() = delete;
    Window(i32 w, i32 h, const char* name) noexcept;
    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;
    ~Window();

    [[nodiscard]] HWND GetHWND() const { return hwnd; }

   // [[nodiscard]] Graphics& Gfx() { return *pGfx; }

    [[nodiscard]] u32 GetWindowWidth() const { return width; }

    [[nodiscard]] u32 GetWindowHeight() const { return height; }

    static LRESULT CALLBACK HandleMessageSetup(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
    static LRESULT CALLBACK HandleMessageThunk(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
    LRESULT HandleMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
    static std::optional<i32> ProcessMessages();

public:
  //  Keyboard kbd;

private:
    HWND hwnd;
    u32 width;
    u32 height;
    //std::unique_ptr<Graphics> pGfx;

#elif
    //linux code
#endif
};