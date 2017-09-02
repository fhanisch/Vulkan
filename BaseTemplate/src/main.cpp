/*
	Erstellt: 02.09.2017

	Vulkan Loader wird dynamisch geladen --> Function Pointer werden dynamisch geladen

*/

#define VK_NO_PROTOTYPES
#include <Windows.h>
#include <iostream>
#include <vulkan\vulkan.h>

class BaseVulkanApp
{
public:
	void run()
	{
		loadVulkanLibrary();
		initVulkan();
	}
private:
	HINSTANCE vulkanLibrary;
	PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr;
	PFN_vkEnumerateInstanceLayerProperties vkEnumerateInstanceLayerProperties;
	PFN_vkEnumerateInstanceExtensionProperties vkEnumerateInstanceExtensionProperties;

	void loadVulkanLibrary()
	{
		vulkanLibrary = LoadLibrary("vulkan-1.dll");
		if (vulkanLibrary == nullptr)
		{
			std::cout << "Could not load Vulkan library!" << std::endl;
			exit(1);
		}
		vkGetInstanceProcAddr = (PFN_vkGetInstanceProcAddr)GetProcAddress(vulkanLibrary, "vkGetInstanceProcAddr");
		if (vkGetInstanceProcAddr == nullptr)
		{
			std::cout << "Could not get vkGetInstanceProcAddr" << std::endl;
			exit(1);
		}
		vkEnumerateInstanceLayerProperties = (PFN_vkEnumerateInstanceLayerProperties)vkGetInstanceProcAddr(nullptr, "vkEnumerateInstanceLayerProperties");
		if (vkEnumerateInstanceLayerProperties == nullptr)
		{
			std::cout << "Could not get vkEnumerateInstanceLayerProperties" << std::endl;
			exit(1);
		}
		vkEnumerateInstanceExtensionProperties = (PFN_vkEnumerateInstanceExtensionProperties)vkGetInstanceProcAddr(nullptr, "vkEnumerateInstanceExtensionProperties");
		if (vkEnumerateInstanceExtensionProperties == nullptr)
		{
			std::cout << "Could not get vkEnumerateInstanceExtensionProperties" << std::endl;
			exit(1);
		}
	}

	void initVulkan()
	{
		createInstance();
	}

	void createInstance()
	{
		uint32_t layerCount;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
		VkLayerProperties *availableLayers = new VkLayerProperties[layerCount];
		vkEnumerateInstanceLayerProperties(&layerCount, availableLayers);
		std::cout << "\n" << "Available layers:" << std::endl;
		for (uint32_t i = 0; i < layerCount; i++)
		{
			std::cout << "\t#" << i << "\t" << availableLayers[i].layerName << std::endl;
		}

		uint32_t extensionCount = 0;
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
		VkExtensionProperties *extensions = new VkExtensionProperties[extensionCount];
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions);
		std::cout << "\n" << "Available extensions:" << std::endl;
		for (uint32_t i = 0; i < extensionCount; i++) {
			std::cout << "\t" << extensions[i].extensionName << std::endl;
		}
	}
};

int main(int argc, char *argv[])
{
	BaseVulkanApp app;

	std::cout << "***** Hello Vulkan !!! *****" << std::endl;
	std::cout << "============================" << std::endl;

	app.run();

	return 0;
}