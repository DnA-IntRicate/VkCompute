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

    std::println();

    vkc::Trace("Tracing");
    vkc::Info("Info-ing");
    vkc::Warn("Warning");
    vkc::Error("Error-ing");
    vkc::Critical("I am critical!");
    vkc::Success("Something went right!");

    return 0;
}
