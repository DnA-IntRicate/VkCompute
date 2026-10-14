#include <print>
#include <Ilm/Ilm.hpp>


template<typename... Args>
constexpr void Eval(const Args&... args) noexcept
{
    (
        [&]()
        {
            std::println("{}", args);
        }(),
        ...
    );
}

int main(int argc, char** argv)
{
    std::println("Hello world from VkCompute!");
    Eval(21, 22, "wafawfg", 22.1f);

    ilm::Ref<int> i = ilm::CreateRef<int>(21);
    std::println("{}", *i);

    return 0;
}
