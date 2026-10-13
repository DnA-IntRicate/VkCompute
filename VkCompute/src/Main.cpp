#include <print>


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

    return 0;
}
