#include <iostream>
#include <algorithm>
#include "Vulkan.h"

Vulkan::Vulkan(const char* _appName, const char* _engineName, Window* _window)
{
	appName = _appName;
	engineName = _engineName;
	window = _window;

	printSupportedApiVersion();
	printInstanceLayers();
	printInstanceExtensions();
	createInstance();
	createSurface();
	//printPhysicalDevices();
	choosePhysicalDevice();
	createLogicalDevice();
	createCommandPool();
	createSwapChain();
	createSwapChainImages();
	createRenderPass();
	createFramebuffers();
	createSemaphores();
}

Vulkan::~Vulkan()
{
	/*
	vkDestroySwapchainKHR(device, swapChain, nullptr);
	vkDestroyCommandPool(device, commandPool, nullptr);
	vkDestroyDevice(device, nullptr);
	vkDestroySurfaceKHR(instance, surface, nullptr);
	vkDestroyInstance(instance, nullptr);
	*/
}

void Vulkan::printSupportedApiVersion()
{
	std::cout << "\nInit Vulkan..." << std::endl;
	vkEnumerateInstanceVersion(&apiVersion);
	std::cout << "Supported Api Version: " << VK_VERSION_MAJOR(apiVersion) << "." << VK_VERSION_MINOR(apiVersion) << "." << VK_VERSION_PATCH(apiVersion) << std::endl;
}

void Vulkan::printInstanceLayers()
{
	uint32_t instanceLayerCount;
	VkLayerProperties* instanceLayers;
	
	vkEnumerateInstanceLayerProperties(&instanceLayerCount, nullptr);
	instanceLayers = new VkLayerProperties[instanceLayerCount];
	vkEnumerateInstanceLayerProperties(&instanceLayerCount, instanceLayers);
	std::cout << "\nAvailable Instance Layers: " << instanceLayerCount << std::endl;
	for (uint32_t i = 0; i < instanceLayerCount; i++)
	{
		std::cout << "\t#" << i + 1 << "\t" << instanceLayers[i].layerName << " (" << instanceLayers[i].description << ")" << std::endl;
	}
	delete[] instanceLayers;
}

void Vulkan::printInstanceExtensions()
{
	uint32_t instanceExtensionCount;
	VkExtensionProperties* instanceExtensions;

	vkEnumerateInstanceExtensionProperties(nullptr, &instanceExtensionCount, nullptr);
	instanceExtensions = new VkExtensionProperties[instanceExtensionCount];
	std::cout << "\nAvailable Instance Extensions: " << instanceExtensionCount << std::endl;
	vkEnumerateInstanceExtensionProperties(nullptr, &instanceExtensionCount, instanceExtensions);
	for (uint32_t i = 0; i < instanceExtensionCount; i++)
	{
		std::cout << "\t#" << i + 1 << "\t" << instanceExtensions[i].extensionName << std::endl;
	}
	delete[] instanceExtensions;
}

void Vulkan::createInstance()
{
	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pNext = NULL;
	appInfo.pApplicationName = appName;
	appInfo.applicationVersion = VK_MAKE_VERSION(0, 1, 0);
	appInfo.pEngineName = engineName;
	appInfo.engineVersion = VK_MAKE_VERSION(0, 1, 0);
	appInfo.apiVersion = VK_API_VERSION_1_1;

	const unsigned int validationLayerCount = 1;
	const char* validationLayers[] = { "VK_LAYER_LUNARG_standard_validation" };
	const unsigned int globalExtensionCount = 2;
	const char* globalExtensions[] = { VK_KHR_SURFACE_EXTENSION_NAME, VK_KHR_WIN32_SURFACE_EXTENSION_NAME };
	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pNext = NULL;
	createInfo.flags = 0;
	createInfo.pApplicationInfo = &appInfo;
	createInfo.enabledLayerCount = validationLayerCount;
	createInfo.ppEnabledLayerNames = validationLayers;
	createInfo.enabledExtensionCount = globalExtensionCount;
	createInfo.ppEnabledExtensionNames = globalExtensions;

	if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
		std::cout << "Failed to create instance!" << std::endl;
		exit(1);
	}
}

void Vulkan::createSurface()
{
	VkWin32SurfaceCreateInfoKHR createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	createInfo.hwnd = window->getWindow();
	createInfo.hinstance = window->getInstance();

	if (vkCreateWin32SurfaceKHR(instance, &createInfo, nullptr, &surface) != VK_SUCCESS) {
		std::cout << "Failed to create window surface!" << std::endl;
		exit(1);
	}
}

void Vulkan::choosePhysicalDevice()
{
	uint32_t deviceCount;
	vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
	if (deviceCount == 0) {
		std::cout << "Failed to find GPUs with Vulkan support!" << std::endl;
		exit(1);
	}
	VkPhysicalDevice* devices = new VkPhysicalDevice[deviceCount];
	vkEnumeratePhysicalDevices(instance, &deviceCount, devices);
	for (unsigned int i = 0; i < deviceCount; i++) {
		if (isDeviceSuitable(devices[i])) {
			physicalDevice = devices[i];
			break;
		}
	}
	if (physicalDevice == VK_NULL_HANDLE) {
		std::cout << "Failed to find a suitable GPU!" << std::endl;
		exit(1);
	}
	delete[] devices;
}

void Vulkan::createLogicalDevice()
{
	uint32_t queueFamilyIndex = findQueueFamilies(physicalDevice);
	float queuePriority = 1.0f;

	VkDeviceQueueCreateInfo queueCreateInfo = {};
	queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queueCreateInfo.pNext = NULL;
	queueCreateInfo.flags = 0;
	queueCreateInfo.queueFamilyIndex = queueFamilyIndex;
	queueCreateInfo.queueCount = 1;
	queueCreateInfo.pQueuePriorities = &queuePriority;

	VkPhysicalDeviceFeatures deviceFeatures = {};
	deviceFeatures.samplerAnisotropy = VK_TRUE;
	deviceFeatures.wideLines = VK_TRUE;

	const unsigned int deviceExtensionCount = 1;
	const char* deviceExtensions[] = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
	VkDeviceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.pNext = NULL;
	createInfo.flags = 0;
	createInfo.queueCreateInfoCount = 1;
	createInfo.pQueueCreateInfos = &queueCreateInfo;
	createInfo.enabledLayerCount = 0;
	createInfo.ppEnabledLayerNames = NULL; // evtl. auch hier den validationLayer eintragen
	createInfo.enabledExtensionCount = deviceExtensionCount;
	createInfo.ppEnabledExtensionNames = deviceExtensions;
	createInfo.pEnabledFeatures = &deviceFeatures;

	if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS) {
		std::cout << "Failed to create logical device!" << std::endl;
		exit(1);
	}

	vkGetDeviceQueue(device, queueFamilyIndex, 0, &queue);
}

void Vulkan::createCommandPool()
{
	int queueFamilyIndex = findQueueFamilies(physicalDevice);

	VkCommandPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.flags = 0; // Optional
	poolInfo.queueFamilyIndex = queueFamilyIndex;

	if (vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool) != VK_SUCCESS) {
		std::cout << "failed to create command pool!" << std::endl;
		exit(1);
	}
}

void Vulkan::createSwapChain()
{

	SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice); // --> ToDo: delete des Inhalts beim Beenden 
	VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formatCount, swapChainSupport.formats);
	VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModeCount, swapChainSupport.presentModes);
	VkExtent2D extent = chooseSwapExtent(&swapChainSupport.capabilities);
	uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
	if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
		imageCount = swapChainSupport.capabilities.maxImageCount;

	VkSwapchainCreateInfoKHR createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.pNext = nullptr;
	createInfo.flags = 0;
	createInfo.surface = surface;
	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	createInfo.queueFamilyIndexCount = 0;
	createInfo.pQueueFamilyIndices = nullptr;
	createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;
	createInfo.oldSwapchain = VK_NULL_HANDLE;

	if (vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapChain) != VK_SUCCESS)
	{
		std::cout << "Failed to create swap chain!" << std::endl;
		exit(1);
	}
	swapChainImageFormat = surfaceFormat.format;
	swapChainExtent = extent;
}

void Vulkan::createSwapChainImages()
{
	vkGetSwapchainImagesKHR(device, swapChain, &swapChainImagesCount, nullptr);
	VkImage* tmp = new VkImage[swapChainImagesCount];
	swapChainImages = new Image*[swapChainImagesCount];
	vkGetSwapchainImagesKHR(device, swapChain, &swapChainImagesCount, tmp);

	for (uint32_t i = 0; i < swapChainImagesCount; i++)
	{
		swapChainImages[i] = new Image(this);
		swapChainImages[i]->setImage(tmp[i]);
		swapChainImages[i]->createImageView(swapChainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT);
	}
	std::cout << swapChainImagesCount << " image views created." << std::endl;
	delete[] tmp;
}

void Vulkan::createRenderPass()
{
	VkAttachmentDescription colorAttachment = {};
	colorAttachment.flags = 0;
	colorAttachment.format = swapChainImageFormat;
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference colorAttachmentRef = {};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass = {};
	subpass.flags = 0;
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;
	subpass.pDepthStencilAttachment = nullptr;

	VkSubpassDependency dependency = {};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	VkAttachmentDescription attachments[] = { colorAttachment };

	VkRenderPassCreateInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = 1;
	renderPassInfo.pAttachments = attachments;
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;
	renderPassInfo.dependencyCount = 1;
	renderPassInfo.pDependencies = &dependency;

	if (vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS) {
		std::cout << "Failed to create render pass!" << std::endl;
		exit(1);
	}
}

void Vulkan::createFramebuffers()
{
	swapChainFramebuffers = new VkFramebuffer[swapChainImagesCount];

	for (uint32_t i = 0; i < swapChainImagesCount; i++) {
		VkImageView attachments[] = { swapChainImages[i]->getImageView() };
		VkFramebufferCreateInfo framebufferInfo = {};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = renderPass;
		framebufferInfo.attachmentCount = 1;
		framebufferInfo.pAttachments = attachments;
		framebufferInfo.width = swapChainExtent.width;
		framebufferInfo.height = swapChainExtent.height;
		framebufferInfo.layers = 1;

		if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &swapChainFramebuffers[i]) != VK_SUCCESS) {
			std::cout << "failed to create framebuffer!" << std::endl;
			exit(1);
		}
	}
}

void Vulkan::createSemaphores()
{
	VkSemaphoreCreateInfo semaphoreInfo = {};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &imageAvailableSemaphore) != VK_SUCCESS ||
		vkCreateSemaphore(device, &semaphoreInfo, nullptr, &renderFinishedSemaphore) != VK_SUCCESS) {

		std::cout << "failed to create semaphores!" << std::endl;
		exit(1);
	}
}

/* Helper Functions */
void Vulkan::printPhysicalDevices()
{
	if (instance == VK_NULL_HANDLE) {
		std::cout << "Create Instance first!" << std::endl;
		return;
	}
	uint32_t deviceCount;
	vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
	if (deviceCount == 0) {
		std::cout << "Failed to find GPUs with Vulkan support!" << std::endl;
		return;
	}
	VkPhysicalDevice* devices = new VkPhysicalDevice[deviceCount];
	vkEnumeratePhysicalDevices(instance, &deviceCount, devices);
	std::cout << "\nCount of Devices: " << deviceCount << std::endl << std::endl;
	for (uint32_t i = 0; i < deviceCount; i++) {
		uint32_t extensionCount;
		vkEnumerateDeviceExtensionProperties(devices[i], nullptr, &extensionCount, nullptr);
		VkExtensionProperties* extensions = new VkExtensionProperties[extensionCount];
		vkEnumerateDeviceExtensionProperties(devices[i], nullptr, &extensionCount, extensions);
		std::cout << "Device #" << i << " Extensions:" << std::endl;
		for (uint32_t j = 0; j < extensionCount; j++) {
			std::cout << "\t#" << j << "\t" << extensions[j].extensionName << std::endl;
		}
		delete[] extensions;

		VkPhysicalDeviceProperties deviceProperties;
		vkGetPhysicalDeviceProperties(devices[i], &deviceProperties);
		std::cout << "Device #" << i << " Properties:" << std::endl;
		std::cout << "\t" << "Api Version:    " << deviceProperties.apiVersion << std::endl;
		std::cout << "\t" << "Device ID:      " << deviceProperties.deviceID << std::endl;
		std::cout << "\t" << "Device Name:    " << deviceProperties.deviceName << std::endl;
		std::cout << "\t" << "Device Type:    " << deviceProperties.deviceType << std::endl;
		std::cout << "\t" << "Driver Version: " << deviceProperties.driverVersion << std::endl;
		std::cout << "\t" << "Vendor ID:      " << deviceProperties.vendorID << std::endl;

		VkPhysicalDeviceFeatures deviceFeatures;
		vkGetPhysicalDeviceFeatures(devices[i], &deviceFeatures);
		std::cout << "Device #" << i << " Featers:" << std::endl;
		std::cout << "\t" << "Tessellation Shader: " << deviceFeatures.tessellationShader << std::endl;
		std::cout << "\t" << "Geometry Shader:     " << deviceFeatures.geometryShader << std::endl;
		std::cout << "\t" << "fullDrawIndexUint32: " << deviceFeatures.fullDrawIndexUint32 << std::endl;

		uint32_t queueFamilyCount;
		vkGetPhysicalDeviceQueueFamilyProperties(devices[i], &queueFamilyCount, nullptr);
		VkQueueFamilyProperties* queueFamilies = new VkQueueFamilyProperties[queueFamilyCount];
		vkGetPhysicalDeviceQueueFamilyProperties(devices[i], &queueFamilyCount, queueFamilies);
		std::cout << "Device #" << i << " Queue Families:" << std::endl;
		for (uint32_t j = 0; j < queueFamilyCount; j++)
		{
			std::cout << "\t" << "Queue Family #" << j << ":" << std::endl;
			std::cout << "\t\t" << "VK_QUEUE_GRAPHICS_BIT:           " << ((queueFamilies[j].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0) << std::endl;
			std::cout << "\t\t" << "VK_QUEUE_COMPUTE_BIT:            " << ((queueFamilies[j].queueFlags & VK_QUEUE_COMPUTE_BIT) != 0) << std::endl;
			std::cout << "\t\t" << "VK_QUEUE_TRANSFER_BIT:           " << ((queueFamilies[j].queueFlags & VK_QUEUE_TRANSFER_BIT) != 0) << std::endl;
			std::cout << "\t\t" << "VK_QUEUE_SPARSE_BINDING_BIT:     " << ((queueFamilies[j].queueFlags & VK_QUEUE_SPARSE_BINDING_BIT) != 0) << std::endl;
			std::cout << "\t\t" << "Queue Count:                     " << queueFamilies[j].queueCount << std::endl;
			std::cout << "\t\t" << "Timestamp Valid Bits:            " << queueFamilies[j].timestampValidBits << std::endl;
			uint32_t width = queueFamilies[j].minImageTransferGranularity.width;
			uint32_t height = queueFamilies[j].minImageTransferGranularity.height;
			uint32_t depth = queueFamilies[j].minImageTransferGranularity.depth;
			std::cout << "\t\t" << "Min Image Timestamp Granularity: " << width << ", " << height << ", " << depth << std::endl;
		}
		delete[] queueFamilies;

		VkSurfaceCapabilitiesKHR capabilities;
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(devices[i], surface, &capabilities);
		std::cout << "Device #" << i << " Surface capabilities:" << std::endl;
		std::cout << "\t" << "minImageCount:           " << capabilities.minImageCount << std::endl;
		std::cout << "\t" << "maxImageCount:           " << capabilities.maxImageCount << std::endl;
		std::cout << "\t" << "currentExtent:           " << capabilities.currentExtent.width << "," << capabilities.currentExtent.height << std::endl;
		std::cout << "\t" << "minImageExtent:          " << capabilities.minImageExtent.width << "," << capabilities.minImageExtent.height << std::endl;
		std::cout << "\t" << "maxImageExtent:          " << capabilities.maxImageExtent.width << "," << capabilities.maxImageExtent.height << std::endl;
		std::cout << "\t" << "maxImageArrayLayers:     " << capabilities.maxImageArrayLayers << std::endl;
		std::cout << "\t" << "supportedTransforms:     " << capabilities.supportedTransforms << std::endl;
		std::cout << "\t" << "currentTransform:        " << capabilities.currentTransform << std::endl;
		std::cout << "\t" << "supportedCompositeAlpha: " << capabilities.supportedCompositeAlpha << std::endl;
		std::cout << "\t" << "supportedUsageFlags:     " << capabilities.supportedUsageFlags << std::endl;

		uint32_t formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(devices[i], surface, &formatCount, nullptr);
		VkSurfaceFormatKHR * formats = new VkSurfaceFormatKHR[formatCount];
		vkGetPhysicalDeviceSurfaceFormatsKHR(devices[i], surface, &formatCount, formats);
		std::cout << "Device #" << i << " Supported formats:" << std::endl;
		for (uint32_t j = 0; j < formatCount; j++)
		{
			std::cout << "\t" << "Format #" << j << ":" << std::endl;
			std::cout << "\t\t" << "format:     " << formats[j].format << std::endl;
			std::cout << "\t\t" << "colorSpace: " << formats[j].colorSpace << std::endl;
		}
		delete[] formats;

		uint32_t presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(devices[i], surface, &presentModeCount, nullptr);
		VkPresentModeKHR* presentModes = new VkPresentModeKHR[presentModeCount];
		vkGetPhysicalDeviceSurfacePresentModesKHR(devices[i], surface, &presentModeCount, presentModes);
		std::cout << "Device #" << i << " Supported presentation modes:" << std::endl;
		for (uint32_t j = 0; j < presentModeCount; j++)
		{
			std::cout << "\t" << "Present mode #" << j << ": " << presentModes[j] << std::endl;
		}
		delete[] presentModes;
	}
	delete[] devices;
}

int Vulkan::findQueueFamilies(VkPhysicalDevice physDevice)
{
	int index = -1;
	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(physDevice, &queueFamilyCount, nullptr);
	VkQueueFamilyProperties* queueFamilies = new VkQueueFamilyProperties[queueFamilyCount];
	vkGetPhysicalDeviceQueueFamilyProperties(physDevice, &queueFamilyCount, queueFamilies);

	for (unsigned int i = 0; i < queueFamilyCount; i++)
	{
		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(physDevice, i, surface, &presentSupport);
		if (queueFamilies[i].queueCount > 0 && queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT && presentSupport) index = i;
		if (index >= 0) break;
	}
	delete[] queueFamilies;
	return index;
}

bool Vulkan::isDeviceSuitable(VkPhysicalDevice physDevice)
{
	int index = findQueueFamilies(physDevice);
	bool extensionsSupported = checkDeviceExtensionSupport(physDevice);
	bool swapChainAdequate = false;

	if (extensionsSupported)
	{
		SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physDevice);
		swapChainAdequate = swapChainSupport.formatCount && swapChainSupport.presentModeCount;
	}

	VkSurfaceCapabilitiesKHR capabilities;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physDevice, surface, &capabilities);

	return index >= 0 && extensionsSupported && swapChainAdequate && (capabilities.maxImageCount > 2);
}

bool Vulkan::checkDeviceExtensionSupport(VkPhysicalDevice physDevice)
{
	bool extensionsSupported = false;

	uint32_t extensionCount;
	vkEnumerateDeviceExtensionProperties(physDevice, nullptr, &extensionCount, nullptr);
	VkExtensionProperties* availableExtensions = new VkExtensionProperties[extensionCount];
	vkEnumerateDeviceExtensionProperties(physDevice, nullptr, &extensionCount, availableExtensions);

	for (uint32_t i = 0; i < extensionCount; i++) {
		if (std::strcmp(availableExtensions[i].extensionName, VK_KHR_SWAPCHAIN_EXTENSION_NAME) == 0) {
			extensionsSupported = true;
			break;
		}
	}
	delete[] availableExtensions;
	return extensionsSupported;
}

Vulkan::SwapChainSupportDetails Vulkan::querySwapChainSupport(VkPhysicalDevice physDevice)
{
	SwapChainSupportDetails details;

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physDevice, surface, &details.capabilities);

	vkGetPhysicalDeviceSurfaceFormatsKHR(physDevice, surface, &details.formatCount, nullptr);
	details.formats = new VkSurfaceFormatKHR[details.formatCount];
	vkGetPhysicalDeviceSurfaceFormatsKHR(physDevice, surface, &details.formatCount, details.formats);

	vkGetPhysicalDeviceSurfacePresentModesKHR(physDevice, surface, &details.presentModeCount, nullptr);
	details.presentModes = new VkPresentModeKHR[details.presentModeCount];
	vkGetPhysicalDeviceSurfacePresentModesKHR(physDevice, surface, &details.presentModeCount, details.presentModes);

	return details;
}

VkSurfaceFormatKHR Vulkan::chooseSwapSurfaceFormat(uint32_t formatCount, VkSurfaceFormatKHR * availableFormats)
{
	if (formatCount == 1 && availableFormats[0].format == VK_FORMAT_UNDEFINED) {
		return { VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
	}
	for (uint32_t i = 0; i < formatCount; i++) {
		if (availableFormats[i].format == VK_FORMAT_B8G8R8A8_UNORM && availableFormats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			return availableFormats[i];
	}
	return availableFormats[0];
}

VkPresentModeKHR Vulkan::chooseSwapPresentMode(uint32_t presentModeCount, VkPresentModeKHR * availablePresentModes)
{
	VkPresentModeKHR bestMode = VK_PRESENT_MODE_FIFO_KHR;

	for (uint32_t i = 0; i < presentModeCount; i++) {
		if (availablePresentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR)
			return availablePresentModes[i];
		else if (availablePresentModes[i] == VK_PRESENT_MODE_IMMEDIATE_KHR)
			bestMode = availablePresentModes[i];
	}

	return bestMode;
}

VkExtent2D Vulkan::chooseSwapExtent(VkSurfaceCapabilitiesKHR * capabilities)
{
	if (capabilities->currentExtent.width != std::numeric_limits<uint32_t>::max())
		return capabilities->currentExtent;
	else {
		VkExtent2D actualExtent = { (uint32_t)window->getWidth(), (uint32_t)window->getHeight() };
		actualExtent.width = std::max(capabilities->minImageExtent.width, std::min(capabilities->maxImageExtent.width, actualExtent.width));
		actualExtent.height = std::max(capabilities->minImageExtent.height, std::min(capabilities->maxImageExtent.height, actualExtent.height));

		return actualExtent;
	}
}

VkDevice Vulkan::getDevice() { return device; }

VkPhysicalDevice Vulkan::getPhysicalDevice() { return physicalDevice; }

VkExtent2D Vulkan::getSwapChainExtent() { return swapChainExtent; }

VkRenderPass Vulkan::getRenderPass() { return renderPass; }

VkCommandPool Vulkan::getCommandPool() { return commandPool; }

VkQueue Vulkan::getQueue() { return queue; }

uint32_t Vulkan::getSwapChainImagesCount() { return swapChainImagesCount; }

VkFramebuffer* Vulkan::getSwapChainFramebuffers() { return swapChainFramebuffers; }

VkSwapchainKHR Vulkan::getSwapChain() { return swapChain; }

VkSemaphore Vulkan::getImageAvailableSemaphore() { return imageAvailableSemaphore; }

VkSemaphore Vulkan::getRenderFinishedSemaphore() { return renderFinishedSemaphore; }