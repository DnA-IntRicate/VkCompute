#include <VkCompute/Log.hpp>
#include <VkCompute/VkcInstance.hpp>


vkc::VkcInstance g_Instance;

int main(int argc, char** argv)
{
    vkc::Info("Hello world from VkCompute!");
    g_Instance.Initialize("VkCompute-Demo");
    g_Instance.Shutdown();

    vkc::Trace("Tracing");
    vkc::Info("Info-ing");
    vkc::Warn("Warning");
    vkc::Error("Error-ing");
    vkc::Critical("I am critical!");
    vkc::Success("Something went right!");

    return 0;
}
