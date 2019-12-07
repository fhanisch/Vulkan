#include "VulkanSetup.h"
#include <string.h>
#include <stdlib.h>
#include <dlfcn.h>

#ifdef LOG
static char buf[128];
#define PRINT(...) \
sprintf(buf, __VA_ARGS__); \
fwrite(buf, strlen(buf), 1, file);
#else
#define PRINT(...) \
printf(__VA_ARGS__);
#endif

#define GET_FCN_PTR(fun) \
fun = (PFN_##fun)dlsym(libvulkan, #fun); \
if (!fun) \
{ \
    PRINT("Find Symbol '%s' failed!\n", #fun) \
    exit(1); \
}

static FILE* file;
void* libvulkan;

void VulkanSetup::printSupportedApiVersion()
{
	uint32_t apiVersion;
	vkEnumerateInstanceVersion(&apiVersion);
	PRINT("Supported Api Version: %u.%u.%u\n", VK_VERSION_MAJOR(apiVersion), VK_VERSION_MINOR(apiVersion), VK_VERSION_PATCH(apiVersion))
}

VulkanSetup::VulkanSetup(FILE* file_)
{
    file = file_;
    PRINT("Get Vulkan Functions.\n")
    libvulkan = dlopen("libvulkan.so", RTLD_NOW | RTLD_LOCAL);
    if (!libvulkan)
    {
        PRINT("Loading libvulkan.so failed!\n")
        exit(1);
    }
    GET_FCN_PTR(vkEnumerateInstanceVersion)
}

VulkanSetup::~VulkanSetup()
{
    PRINT("Close Vulkan.\n")
}

void VulkanSetup::init()
{
    PRINT("Init VulkanSetup.\n")
    printSupportedApiVersion();
}

/* Funktionfür dynamisches Laden */
extern "C" VulkanSetup* create_object(FILE* file_)
{
    return new VulkanSetup(file_);
}

PFN_vkEnumerateInstanceVersion vkEnumerateInstanceVersion;