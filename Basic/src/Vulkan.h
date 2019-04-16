#pragma once
#define _CRT_SECURE_NO_WARNINGS 
#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan.h>
#include "Window.h"
#include "Image.h"

#undef min
#undef max

class Vulkan
{
	struct SwapChainSupportDetails {
		VkSurfaceCapabilitiesKHR capabilities;
		uint32_t formatCount;
		VkSurfaceFormatKHR* formats;
		uint32_t presentModeCount;
		VkPresentModeKHR* presentModes;
	};

	const char* appName;
	const char* engineName;
	uint32_t apiVersion;
	Window* window;
	VkInstance instance = VK_NULL_HANDLE;
	VkSurfaceKHR surface = VK_NULL_HANDLE;
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	VkDevice device = VK_NULL_HANDLE;
	VkQueue queue = VK_NULL_HANDLE;
	VkCommandPool commandPool;
	VkSwapchainKHR swapChain = VK_NULL_HANDLE;
	VkFormat swapChainImageFormat = {};
	VkExtent2D swapChainExtent = {};
	uint32_t swapChainImagesCount;
	Image** swapChainImages;
	VkRenderPass renderPass;
	VkFramebuffer* swapChainFramebuffers;
	VkSemaphore imageAvailableSemaphore;
	VkSemaphore renderFinishedSemaphore;

	void createInstance();
	void createSurface();
	void choosePhysicalDevice();
	void createLogicalDevice();
	void createCommandPool();
	void createSwapChain();
	void createSwapChainImages();
	void createRenderPass();
	void createFramebuffers();
	void createSemaphores();

	/* Helper Functions */
	void printPhysicalDevices();
	int findQueueFamilies(VkPhysicalDevice physDevice);
	bool isDeviceSuitable(VkPhysicalDevice physDevice);
	bool checkDeviceExtensionSupport(VkPhysicalDevice physDevice);
	void printSupportedApiVersion();
	void printInstanceLayers();
	void printInstanceExtensions();
	SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice physDevice);
	VkSurfaceFormatKHR chooseSwapSurfaceFormat(uint32_t formatCount, VkSurfaceFormatKHR* availableFormats);
	VkPresentModeKHR chooseSwapPresentMode(uint32_t presentModeCount, VkPresentModeKHR* availablePresentModes);
	VkExtent2D chooseSwapExtent(VkSurfaceCapabilitiesKHR* capabilities);
public:
	Vulkan(const char* _appName, const char* _engineName, Window* _window);
	~Vulkan();
	VkDevice getDevice();
	VkPhysicalDevice getPhysicalDevice();
	VkExtent2D getSwapChainExtent();
	VkRenderPass getRenderPass();
	VkCommandPool getCommandPool();
	VkQueue getQueue();
	uint32_t getSwapChainImagesCount();
	VkFramebuffer* getSwapChainFramebuffers();
	VkSwapchainKHR getSwapChain();
	VkSemaphore getImageAvailableSemaphore();
	VkSemaphore getRenderFinishedSemaphore();
};