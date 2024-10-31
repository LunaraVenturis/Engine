#include "Platform/WindowInstance.hpp"
#include <string>
class Window
{
public:
    Window();

private:
    void InitWindow();
    const std::wstring name = L"Engine";


};