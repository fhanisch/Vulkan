/*
	Erstellt: 27.08.2017
*/

#define GLFW_INCLUDE_VULKAN

#include <GLFW\glfw3.h>
#include <iostream>
#include <string>

const char WINDOW_NAME[] = "Hello Vulkan Triangle";
const char APP_NAME[] = "Triangle";
const char ENGINE_NAME[] = "MyVulkanEngine";
const int WIDTH = 800;
const int HEIGHT = 800;
const unsigned int validationLayerCount = 1;
const char *validationLayers[] = { "VK_LAYER_LUNARG_standard_validation" };

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

struct QueueFamilyIndices {
	int graphicsFamily = -1;

	bool isComplete() {
		return graphicsFamily >= 0;
	}
};

class TriangleApp
{
public:
	void run()
	{
		initWindow(WINDOW_NAME);
		initVulkan();
		mainLoop();
		cleanup();
	}
private:
	GLFWwindow *window;
	VkInstance instance;
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

	void initWindow(const char *windowName)
	{
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
		window = glfwCreateWindow(WIDTH, HEIGHT, windowName, nullptr, nullptr);
	}
	void initVulkan()
	{
		createInstance(APP_NAME, ENGINE_NAME);
		pickPhysicalDevice();
	}
	void mainLoop()
	{
		while (!glfwWindowShouldClose(window)) {
			glfwPollEvents();
		}
	}
	void cleanup()
	{
		vkDestroyInstance(instance, nullptr);
		glfwDestroyWindow(window);
		glfwTerminate();
	}
	void createInstance(const char *appName, const char *engineName)
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
		appInfo.pApplicationName = appName;
		appInfo.applicationVersion = VK_MAKE_VERSION(0, 0, 0);
		appInfo.pEngineName = engineName;
		appInfo.engineVersion = VK_MAKE_VERSION(0, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_0;

		unsigned int enabledLayerCount = 0;
		for (uint32_t i = 0; i < layerCount; i++)
		{
			if (std::strcmp(availableLayers[i].layerName, validationLayers[0]) == 0)
			{
				std::cout << "\n" << "Layers: " << availableLayers[i].layerName << std::endl;
				enabledLayerCount = validationLayerCount;
				break;
			}
		}

		unsigned int glfwExtensionCount = 0;
		const char** glfwExtensions;
		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		VkInstanceCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pNext = NULL;
		createInfo.flags = 0;
		createInfo.pApplicationInfo = &appInfo;
		createInfo.enabledLayerCount = enabledLayerCount;
		createInfo.ppEnabledLayerNames = validationLayers;
		createInfo.enabledExtensionCount = glfwExtensionCount;
		createInfo.ppEnabledExtensionNames = glfwExtensions;

		if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS)
		{
			std::cout << "Failed to create instance!" << std::endl;
			exit(1);
		}
		
		delete[] extensions;
		delete[] availableLayers;
	}
	void pickPhysicalDevice()
	{
		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
		if (deviceCount == 0) {
			std::cout << "failed to find GPUs with Vulkan support!" << std::endl;
		}
		VkPhysicalDevice *devices = new VkPhysicalDevice[deviceCount];
		vkEnumeratePhysicalDevices(instance, &deviceCount, devices);

		for (unsigned int i = 0; i < deviceCount; i++)
		{
			std::cout << "\n" << "Device Stats (Device #" << i << "):" << std::endl;
			printDeviceStats(devices[i]);
		}
		for (unsigned int i = 0; i < deviceCount; i++)
		{
			if (isDeviceSuitable(devices[i]))
			{
				physicalDevice = devices[i];
				break;
			}
		}
		if (physicalDevice == VK_NULL_HANDLE)
		{
			std::cout << "Failed to find a suitable GPU!" << std::endl;
			exit(1);
		}
		delete[] devices;
	}
	void printDeviceStats(VkPhysicalDevice device)
	{
		VkPhysicalDeviceProperties deviceProperties;
		vkGetPhysicalDeviceProperties(device, &deviceProperties);
		
		std::cout << "\t" << "Device Properties:" << std::endl;
		std::cout << "\t\t" << "Api Version:" << "\t" << deviceProperties.apiVersion << std::endl;
		std::cout << "\t\t" << "Device ID:" << "\t" << deviceProperties.deviceID << std::endl;
		std::cout << "\t\t" << "Device Name:" << "\t" << deviceProperties.deviceName << std::endl;
		std::cout << "\t\t" << "Device Type:" << "\t" << deviceProperties.deviceType << std::endl;
		std::cout << "\t\t" << "Driver Version:" << "\t" << deviceProperties.driverVersion << std::endl;
		std::cout << "\t\t" << "Vendor ID:" << "\t" << deviceProperties.vendorID << std::endl;

		VkPhysicalDeviceFeatures deviceFeatures;
		vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
		std::cout << "\t" << "Device Featers:" << std::endl;
		std::cout << "\t\t" << "Tessellation Shader:" << "\t" << deviceFeatures.tessellationShader << std::endl;
		std::cout << "\t\t" << "Geometry Shader:" << "\t" << deviceFeatures.geometryShader << std::endl;
		std::cout << "\t\t" << "fullDrawIndexUint32:" << "\t" << deviceFeatures.fullDrawIndexUint32 << std::endl;

		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
		VkQueueFamilyProperties *queueFamilies = new VkQueueFamilyProperties[queueFamilyCount];
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies);

		std::cout<< "\t" << "Queue Families:" << std::endl;
		for (uint32_t i = 0; i < queueFamilyCount; i++)
		{
			std::cout << "\t\t" << "Queue Family (#" << i <<"):" << std::endl;
			std::cout << "\t\t\t" << "VK_QUEUE_GRAPHICS_BIT:" << "\t\t" << ((queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0) << std::endl;
			std::cout << "\t\t\t" << "VK_QUEUE_COMPUTE_BIT:" << "\t\t" << ((queueFamilies[i].queueFlags & VK_QUEUE_COMPUTE_BIT) != 0) << std::endl;
			std::cout << "\t\t\t" << "VK_QUEUE_TRANSFER_BIT:" << "\t\t" << ((queueFamilies[i].queueFlags & VK_QUEUE_TRANSFER_BIT) != 0) << std::endl;
			std::cout << "\t\t\t" << "VK_QUEUE_SPARSE_BINDING_BIT:" << "\t" << ((queueFamilies[i].queueFlags & VK_QUEUE_SPARSE_BINDING_BIT) != 0) << std::endl;
			std::cout << "\t\t\t" << "Queue Count:" << "\t\t\t" << queueFamilies[i].queueCount << std::endl;
			std::cout << "\t\t\t" << "Timestamp Valid Bits:" << "\t\t" << queueFamilies[i].timestampValidBits << std::endl;
			uint32_t width = queueFamilies[i].minImageTransferGranularity.width;
			uint32_t height = queueFamilies[i].minImageTransferGranularity.height;
			uint32_t depth = queueFamilies[i].minImageTransferGranularity.depth;
			std::cout << "\t\t\t" << "Min Image Timestamp Granularity:" << width << ", " << height << ", " << depth << std::endl;
		}
		delete[] queueFamilies;
	}
	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device)
	{
		QueueFamilyIndices indices;

		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
		VkQueueFamilyProperties *queueFamilies = new VkQueueFamilyProperties[queueFamilyCount];
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies);

		for (unsigned int i = 0; i < queueFamilyCount; i++)
		{
			if (queueFamilies[i].queueCount > 0 && queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
			{
				indices.graphicsFamily = i;
			}
			if (indices.isComplete()) break;
		}

		delete[] queueFamilies;
		return indices;
	}
	bool isDeviceSuitable(VkPhysicalDevice device)
	{
		VkPhysicalDeviceProperties deviceProperties;
		VkPhysicalDeviceFeatures deviceFeatures;
		vkGetPhysicalDeviceProperties(device, &deviceProperties);
		vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

		return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && deviceFeatures.geometryShader;
	}
};


int main(int argc, char *argv[])
{
	TriangleApp app;

	std::cout << "***** Hello Vulkan Triangle !!! *****" << std::endl;
	std::cout << "=====================================" << std::endl;

	app.run();

	return 0;
}