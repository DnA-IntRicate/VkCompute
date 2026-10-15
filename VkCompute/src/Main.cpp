#include <print>
#include <VkCompute/Log.hpp>
#include <VkCompute/VkcInstance.hpp>
#include <Ilm/Ilm.hpp>


vkc::VkcInstance g_Instance;

int main(int argc, char** argv)
{
    std::println("Hello world from VkCompute!");
    g_Instance.Initialize("VkCompute-Demo");
    g_Instance.Shutdown();

    vkc::Log(vkc::LogLevel::Info, "Hello {} {} - {}", "From VKC", "Log", 21);


    return 0;
}
