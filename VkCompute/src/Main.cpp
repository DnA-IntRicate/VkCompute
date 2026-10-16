#include <VkCompute/Assert.hpp>
#include <VkCompute/Log.hpp>
#include <VkCompute/VkcInstance.hpp>


vkc::VkcInstance g_Instance;

int main(int argc, char** argv)
{
    vkc::Info("Hello world from VkCompute!");
    g_Instance.Initialize("VkCompute-Demo");
    g_Instance.Shutdown();

    return 0;
}
