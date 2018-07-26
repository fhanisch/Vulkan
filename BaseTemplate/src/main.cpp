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
	PFN_vkCreateInstance vkCreateInstance;
	VkInstance instance;

	void loadVulkanLibrary()
	{
		#define VK_GLOBAL_LEVEL_FUNCTION( fun )														\
			fun = (PFN_##fun)vkGetInstanceProcAddr( nullptr, #fun );								\
			if (fun == nullptr)																		\
			{																						\
			  std::cout << "Could not load global level function: " << #fun << "!" << std::endl;	\
			  exit(1);																				\
			}
		vulkanLibrary = LoadLibrary("vulkan-1.dll");
		if (vulkanLibrary == nullptr)
		{
			std::cout << "Could not load Vulkan library!" << std::endl;
			exit(1);
		}
		vkGetInstanceProcAddr = (PFN_vkGetInstanceProcAddr)GetProcAddress(vulkanLibrary, "vkGetInstanceProcAddr");
		if (vkGetInstanceProcAddr == nullptr)
		{
			std::cout << "Could not load vkGetInstanceProcAddr" << std::endl;
			exit(1);
		}
		VK_GLOBAL_LEVEL_FUNCTION(vkEnumerateInstanceLayerProperties);
		VK_GLOBAL_LEVEL_FUNCTION(vkEnumerateInstanceExtensionProperties);
		VK_GLOBAL_LEVEL_FUNCTION(vkCreateInstance);
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
			std::cout << "\t#" << i << "\t" << extensions[i].extensionName << std::endl;
		}

		VkApplicationInfo appInfo = {};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pNext = NULL;
		appInfo.pApplicationName = "";
		appInfo.applicationVersion = VK_MAKE_VERSION(0, 0, 0);
		appInfo.pEngineName = "";
		appInfo.engineVersion = VK_MAKE_VERSION(0, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_1;

		VkInstanceCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pNext = NULL;
		createInfo.flags = 0;
		createInfo.pApplicationInfo = &appInfo;
		createInfo.enabledLayerCount = 0;
		createInfo.ppEnabledLayerNames = nullptr;
		createInfo.enabledExtensionCount = 0;
		createInfo.ppEnabledExtensionNames = nullptr;

		if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS)
		{
			std::cout << "Failed to create instance!" << std::endl;
			exit(1);
		}

		delete[] extensions;
		delete[] availableLayers;
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