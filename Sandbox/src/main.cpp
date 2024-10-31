#define ENGINE_VERBOSE_OUTPUT
#define ENGINE_MEMORY_DEBUG_OUTPUT
#include <OurEngine.hpp>
#include <cstdint>

class A: public RefCounted
{
public:
    A() { std::cout << "A created" << std::endl; }

    ~A() { std::cout << "A destroyed" << std::endl; }
};

int main()
{
    Ref<A> a = Ref<A>::Create();

    return 0;
}
