/*
	2D Action

	Erstellt: 16.09.2017
*/

#define _CRT_SECURE_NO_WARNINGS
#define VK_USE_PLATFORM_WIN32_KHR
//#define NOCONSOLE

#include <Windows.h>
#include <ShellScalingApi.h> // notwendig für high dpi scaling
#include <vulkan\vulkan.h>
#include <matrix.h>
#include <iostream>
#include <string>
#include <fstream>
#include <algorithm>
#include <time.h>

#undef min
#undef max

#ifndef NOCONSOLE
	#define OUTPUT std::cout
#else
	#define OUTPUT logfile
#endif

#define PRINT(msg) OUTPUT << msg << std::endl

static bool	key[256];
const char WINDOW_NAME[] = "2D Action";
const char APP_NAME[] = "2D Action";
const char ENGINE_NAME[] = "MyVulkanEngine";
const int WIDTH = 1800;
const int HEIGHT = 1800;
const unsigned int validationLayerCount = 1;
const char *validationLayers[] = { "VK_LAYER_LUNARG_standard_validation" };
const unsigned int globalExtensionCount = 2;
const char *globalExtensions[] = { VK_KHR_SURFACE_EXTENSION_NAME, VK_KHR_WIN32_SURFACE_EXTENSION_NAME };
const unsigned int deviceExtensionCount = 1;
const char *deviceExtensions[] = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
std::ofstream logfile;

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

struct Vertex
{
	vec2 pos;
	vec3 color;
	vec2 texCoords;
	
	static VkVertexInputBindingDescription getBindingDescription()
	{
		VkVertexInputBindingDescription bindingDescription = {};
		bindingDescription.binding = 0;
		bindingDescription.stride = sizeof(Vertex);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return bindingDescription;
	}

	static VkVertexInputAttributeDescription *getAttributeDescriptions()
	{
		VkVertexInputAttributeDescription *attributeDescriptions = new VkVertexInputAttributeDescription[3];

		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(Vertex, pos);

		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(Vertex, color);

		attributeDescriptions[2].binding = 0;
		attributeDescriptions[2].location = 2;
		attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[2].offset = offsetof(Vertex, texCoords);

		return attributeDescriptions;
	}
};

const Vertex vertices[] = {
	{ {  -1.0f,  -1.0f }, { 1.0f, 0.0f, 0.0f }, { -1.0f, -1.0f } },  //0
	{ {   1.0f,  -1.0f }, { 0.0f, 1.0f, 0.0f }, {  1.0f, -1.0f } },  //1
	{ {   1.0f,   1.0f }, { 0.0f, 0.0f, 1.0f }, {  1.0f,  1.0f } },  //2
	{ {  -1.0f,   1.0f }, { 1.0f, 1.0f, 1.0f }, { -1.0f,  1.0f } },  //3
	{ {   0.0f,  -1.0f }, { 1.0f, 0.0f, 1.0f }, { -1.0f,  1.0f } },  //4
	{ {  0.25f, -0.25f }, { 1.0f, 0.0f, 1.0f }, { -1.0f,  1.0f } },  //5
	{ { -0.25f, -0.25f }, { 1.0f, 0.0f, 1.0f }, { -1.0f,  1.0f } },  //6
	{ {   1.0f,   0.0f }, { 1.0f, 0.0f, 1.0f }, { -1.0f,  1.0f } },  //7
	{ {  0.25f,  0.25f }, { 1.0f, 0.0f, 1.0f }, { -1.0f,  1.0f } },  //8
	{ {   0.0f,   1.0f }, { 1.0f, 0.0f, 1.0f }, { -1.0f,  1.0f } },  //9
	{ {  -0.25,  0.25f }, { 1.0f, 0.0f, 1.0f }, { -1.0f,  1.0f } },  //10
	{ {  -1.0f,   0.0f }, { 1.0f, 0.0f, 1.0f }, { -1.0f,  1.0f } }   //11
};

const uint16_t indices[] = { 0, 1, 2, 2, 3, 0 };
const uint16_t indices_star[] = { 4, 5, 6, 5, 7, 8, 8, 9, 10, 10, 11, 6, 6, 5, 8, 8, 10, 6 };

struct ShaderCode
{
	uint32_t filesize;
	char *data;
};

struct SwapChainSupportDetails
{
	VkSurfaceCapabilitiesKHR capabilities;
	uint32_t formatCount;
	VkSurfaceFormatKHR *formats;
	uint32_t presentModeCount;
	VkPresentModeKHR *presentModes;
};

LRESULT CALLBACK mainWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_KEYDOWN:
		key[wParam] = TRUE;
		break;
	case WM_KEYUP:
		key[wParam] = FALSE;
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

class RenderObject
{
public:
	char *vertexShaderFileName;
	char *fragmentShaderFileName;
	VkPipelineLayout pipelineLayout;
	VkPipeline graphicsPipeline;
	VkDescriptorSet descriptorSet;
	uint32_t indexCount;
	uint32_t firstIndex;
	VkDeviceSize uboOffset;

	mat4 mModel;
	mat4 *mView;
	mat4 mProj;

	RenderObject(char *vertName, char *fragName, VkDeviceSize _uboOffset, mat4 *_mView)
	{
		vertexShaderFileName = vertName;
		fragmentShaderFileName = fragName;
		uboOffset = _uboOffset;
		identity4(mModel);
		mView = _mView;
		identity4(mProj);
	}
};

class RenderScene
{
public:
	mat4 mView;
	RenderObject *powerMeter;
	RenderObject *square;
	RenderObject *star;

	RenderScene()
	{
		identity4(mView);
		powerMeter = new RenderObject("vs_2d.spv", "fs_powermeter.spv", 0, &mView);
		powerMeter->indexCount = sizeof(indices) / sizeof(uint16_t);
		powerMeter->firstIndex = 0;

		square = new RenderObject("vs_2d.spv", "fs_2d.spv", 0x100, &mView);
		square->indexCount = sizeof(indices) / sizeof(uint16_t);
		square->firstIndex = 0;

		star = new RenderObject("vs_2d.spv", "fs_2d.spv", 0x200, &mView);
		star->indexCount = sizeof(indices_star) / sizeof(uint16_t);
		star->firstIndex = 6;
	}
};

class App2DAction
{
public:
	App2DAction(RenderScene *scene)
	{
		renderObject[0] = scene->powerMeter;
		renderObject[1] = scene->square;
		renderObject[2] = scene->star;
		mView = &scene->mView;
	}
	void run()
	{
		initWindow(WINDOW_NAME);
		initVulkan();
		mainLoop();
		cleanup();
	}
private:
	HWND window;
	HINSTANCE hInstance;
	VkInstance instance;
	VkSurfaceKHR surface;
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	VkDevice device;
	VkQueue queue;
	VkSwapchainKHR swapChain;
	uint32_t swapChainImagesCount;
	VkImage *swapChainImages;
	VkFormat swapChainImageFormat;
	VkExtent2D swapChainExtent;
	VkImageView *swapChainImageViews;
	VkFramebuffer *swapChainFramebuffers;
	VkRenderPass renderPass;
	VkDescriptorSetLayout descriptorSetLayout;
	VkCommandPool commandPool;
	VkBuffer vertexBuffer, indexBuffer, uniformBuffer;
	VkDeviceMemory vertexBufferMemory, indexBufferMemory, uniformBufferMemory;
	VkDescriptorPool descriptorPool;
	VkCommandBuffer *commandBuffers;
	VkSemaphore imageAvailableSemaphore;
	VkSemaphore renderFinishedSemaphore;

	VkDeviceSize indexBufferSize = sizeof(indices) + sizeof(indices_star);
	VkDeviceSize uboBufferSize = 0x300;
	static const uint16_t objectCount = 3;
	RenderObject *renderObject[objectCount];
	mat4 *mView;

	void initWindow(const char *windowName)
	{
		SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE); // notwendig für high dpi scaling
		
		hInstance = GetModuleHandle(nullptr);

		WNDCLASS wc;

		wc.lpszClassName = "Test";
		wc.lpszMenuName = NULL;
		wc.hInstance = hInstance;
		wc.hIcon = NULL;
		wc.lpfnWndProc = mainWndProc;
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		wc.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		RegisterClass(&wc);
		window = CreateWindow(wc.lpszClassName, windowName, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
			0, 0, WIDTH, HEIGHT, NULL, NULL, hInstance, NULL);
		RECT rcClient, rcWindow;
		GetClientRect(window, &rcClient);
		GetWindowRect(window, &rcWindow);
		MoveWindow(window, rcWindow.left, rcWindow.top, WIDTH + (rcWindow.right - rcWindow.left) - rcClient.right,
			HEIGHT + (rcWindow.bottom - rcWindow.top) - rcClient.bottom, FALSE);
	}
	void initVulkan()
	{
		createInstance();
		createSurface();
		pickPhysicalDevice();
		createLogicalDevice();
		createSwapChain();
		createImageViews();
		createRenderPass();
		createDescriptorSetLayout();
		createGraphicsPipeline(renderObject[0]);
		createGraphicsPipeline(renderObject[1]);
		renderObject[2]->graphicsPipeline = renderObject[1]->graphicsPipeline;
		renderObject[2]->pipelineLayout = renderObject[1]->pipelineLayout;
		createFramebuffers();
		createCommandPool();
		createVertexBuffer();
		createIndexBuffer();
		createUniformBuffer();
		createDescriptorPool();
		createDescriptorSet(renderObject[0]);
		createDescriptorSet(renderObject[1]);
		createDescriptorSet(renderObject[2]);
		createCommandBuffers();
		createSemaphores();
	}
	void mainLoop()
	{
		bool quit = false;
		MSG msg;
		clock_t start_t, delta_t;
		uint32_t framecount = 0;

		ShowWindow(window, SW_SHOW);

		start_t = clock(); //FPS
		while (!quit)
		{
			delta_t = clock() - start_t;
			if (delta_t >= CLOCKS_PER_SEC)
			{
#ifndef NOCONSOLE
				PRINT("FPS = " << framecount);
#endif
				start_t = clock();
				framecount = 0;
			}
			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				if (msg.message == WM_QUIT)
				{
					quit = TRUE;
				}
				else
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
			}
			else
			{
				if (key[27]) quit = TRUE;
			}
			updateUniformBuffer();
			drawFrame();
			framecount++;
		}

		vkDeviceWaitIdle(device);
	}
	void cleanupSwapChain()
	{
		for (uint32_t i = 0; i < swapChainImagesCount; i++)
			vkDestroyFramebuffer(device, swapChainFramebuffers[i], nullptr);
		vkFreeCommandBuffers(device, commandPool, swapChainImagesCount, commandBuffers);
		for (uint32_t i = 0; i < 2; i++)
		{
			vkDestroyPipeline(device, renderObject[i]->graphicsPipeline, nullptr);
			vkDestroyPipelineLayout(device, renderObject[i]->pipelineLayout, nullptr);
		}
		vkDestroyRenderPass(device, renderPass, nullptr);
		for (uint32_t i = 0; i < swapChainImagesCount; i++)
			vkDestroyImageView(device, swapChainImageViews[i], nullptr);
		vkDestroySwapchainKHR(device, swapChain, nullptr);
	}
	void cleanup()
	{
		cleanupSwapChain();
		vkDestroyDescriptorPool(device, descriptorPool, nullptr);
		vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);
		vkDestroyBuffer(device, uniformBuffer, nullptr);
		vkFreeMemory(device, uniformBufferMemory, nullptr);
		vkDestroyBuffer(device, indexBuffer, nullptr);
		vkFreeMemory(device, indexBufferMemory, nullptr);
		vkDestroyBuffer(device, vertexBuffer, nullptr);
		vkFreeMemory(device, vertexBufferMemory, nullptr);
		vkDestroySemaphore(device, renderFinishedSemaphore, nullptr);
		vkDestroySemaphore(device, imageAvailableSemaphore, nullptr);
		vkDestroyCommandPool(device, commandPool, nullptr);
		vkDestroyDevice(device, nullptr);
		vkDestroySurfaceKHR(instance, surface, nullptr);
		vkDestroyInstance(instance, nullptr);
		DestroyWindow(window);
	}
	void createInstance()
	{
		uint32_t layerCount;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
		VkLayerProperties *availableLayers = new VkLayerProperties[layerCount];
		vkEnumerateInstanceLayerProperties(&layerCount, availableLayers);
		PRINT("\n" << "Available layers:");
		for (uint32_t i = 0; i < layerCount; i++)
		{
			PRINT("\t#" << i << "\t" << availableLayers[i].layerName);
		}

		uint32_t extensionCount = 0;
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
		VkExtensionProperties *extensions = new VkExtensionProperties[extensionCount];
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions);
		PRINT("\n" << "Available instance extensions:");
		for (uint32_t i = 0; i < extensionCount; i++) {
			PRINT("\t#" << i << "\t" << extensions[i].extensionName);
		}

		VkApplicationInfo appInfo = {};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pNext = NULL;
		appInfo.pApplicationName = APP_NAME;
		appInfo.applicationVersion = VK_MAKE_VERSION(0, 0, 0);
		appInfo.pEngineName = ENGINE_NAME;
		appInfo.engineVersion = VK_MAKE_VERSION(0, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_0;

		unsigned int enabledLayerCount = 0;
		for (uint32_t i = 0; i < layerCount; i++)
		{
			if (std::strcmp(availableLayers[i].layerName, validationLayers[0]) == 0)
			{
				PRINT("\n" << "Layers: " << availableLayers[i].layerName);
				enabledLayerCount = validationLayerCount;
				break;
			}
		}

		VkInstanceCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pNext = NULL;
		createInfo.flags = 0;
		createInfo.pApplicationInfo = &appInfo;
		createInfo.enabledLayerCount = enabledLayerCount;
		createInfo.ppEnabledLayerNames = validationLayers;
		createInfo.enabledExtensionCount = globalExtensionCount;
		createInfo.ppEnabledExtensionNames = globalExtensions;

		if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS)
		{
			PRINT("Failed to create instance!");
			exit(1);
		}
		
		delete[] extensions;
		delete[] availableLayers;
	}
	void createSurface()
	{
		VkWin32SurfaceCreateInfoKHR createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
		createInfo.hwnd = window;
		createInfo.hinstance = hInstance;

		if (vkCreateWin32SurfaceKHR(instance, &createInfo, nullptr, &surface) != VK_SUCCESS)
		{
			PRINT("Failed to create window surface!");
			exit(1);
		}
	}
	void pickPhysicalDevice()
	{
		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
		if (deviceCount == 0) {
			PRINT("failed to find GPUs with Vulkan support!");
		}
		VkPhysicalDevice *devices = new VkPhysicalDevice[deviceCount];
		vkEnumeratePhysicalDevices(instance, &deviceCount, devices);

		for (unsigned int i = 0; i < deviceCount; i++)
		{
			PRINT("\n" << "Device Stats (Device #" << i << "):");
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
			PRINT("Failed to find a suitable GPU!");
			exit(1);
		}
		delete[] devices;
	}
	void createLogicalDevice()
	{
		int indices = findQueueFamilies(physicalDevice);

		VkDeviceQueueCreateInfo queueCreateInfo = {};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.pNext = NULL;
		queueCreateInfo.flags = 0;
		queueCreateInfo.queueFamilyIndex = indices;
		queueCreateInfo.queueCount = 1;
		float queuePriority = 1.0f;
		queueCreateInfo.pQueuePriorities = &queuePriority;

		VkPhysicalDeviceFeatures deviceFeatures = {};

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

		if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS)
		{
			PRINT("Failed to create logical device!");
			exit(1);
		}
		
		vkGetDeviceQueue(device, indices, 0, &queue);
	}
	void createSwapChain()
	{
		SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice);

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
			PRINT("Failed to create swap chain!");
			exit(1);
		}
		vkGetSwapchainImagesKHR(device, swapChain, &swapChainImagesCount, nullptr);
		swapChainImages = new VkImage[swapChainImagesCount];
		vkGetSwapchainImagesKHR(device, swapChain, &swapChainImagesCount, swapChainImages);

		swapChainImageFormat = surfaceFormat.format;
		swapChainExtent = extent;
	}
	void createImageViews()
	{
		swapChainImageViews = new VkImageView[swapChainImagesCount];

		for (uint32_t i = 0; i < swapChainImagesCount; i++)
		{
			VkImageViewCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			createInfo.pNext = nullptr;
			createInfo.flags = 0;
			createInfo.image = swapChainImages[i];
			createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			createInfo.format = swapChainImageFormat;
			createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			createInfo.subresourceRange.baseMipLevel = 0;
			createInfo.subresourceRange.levelCount = 1;
			createInfo.subresourceRange.baseArrayLayer = 0;
			createInfo.subresourceRange.layerCount = 1;

			if (vkCreateImageView(device, &createInfo, nullptr, &swapChainImageViews[i]) != VK_SUCCESS)
			{
				PRINT("Failed to create image views!");
				exit(1);
			}
		}
		PRINT("\n" << swapChainImagesCount << " image views created.");
	}
	void createRenderPass()
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

		VkSubpassDependency dependency = {};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.srcAccessMask = 0;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		VkRenderPassCreateInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = 1;
		renderPassInfo.pAttachments = &colorAttachment;
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpass;
		renderPassInfo.dependencyCount = 1;
		renderPassInfo.pDependencies = &dependency;

		if (vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS)
		{
			PRINT("Failed to create render pass!");
			exit(1);
		}
	}
	void createDescriptorSetLayout()
	{
		VkDescriptorSetLayoutBinding uboLayoutBinding = {};
		uboLayoutBinding.binding = 0;
		uboLayoutBinding.descriptorCount = 1;
		uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		uboLayoutBinding.pImmutableSamplers = nullptr;

		VkDescriptorSetLayoutCreateInfo layoutInfo = {};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = 1;
		layoutInfo.pBindings = &uboLayoutBinding;

		if (vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS)
		{
			PRINT("Failed to create descriptor set layout!");
			exit(1);
		}
	}
	void createGraphicsPipeline(RenderObject *obj)
	{
		ShaderCode vertShaderCode = loadShader(obj->vertexShaderFileName);
		ShaderCode fragShaderCode = loadShader(obj->fragmentShaderFileName);

		VkShaderModule vertShaderModule = createShaderModule(vertShaderCode);
		VkShaderModule fragShaderModule = createShaderModule(fragShaderCode);

		VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};
		vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertShaderStageInfo.pNext = nullptr;
		vertShaderStageInfo.flags = 0;
		vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
		vertShaderStageInfo.module = vertShaderModule;
		vertShaderStageInfo.pName = "main";
		vertShaderStageInfo.pSpecializationInfo = nullptr;

		VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};
		fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		fragShaderStageInfo.pNext = nullptr;
		fragShaderStageInfo.flags = 0;
		fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		fragShaderStageInfo.module = fragShaderModule;
		fragShaderStageInfo.pName = "main";
		fragShaderStageInfo.pSpecializationInfo = nullptr;

		VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo,fragShaderStageInfo };

		VkVertexInputBindingDescription bindingDescription = Vertex::getBindingDescription();
		VkVertexInputAttributeDescription *attributeDescriptions = Vertex::getAttributeDescriptions();

		VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputInfo.pNext = nullptr;
		vertexInputInfo.flags = 0;
		vertexInputInfo.vertexBindingDescriptionCount = 1;
		vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
		vertexInputInfo.vertexAttributeDescriptionCount = 3;
		vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions;

		VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
		inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssembly.pNext = nullptr;
		inputAssembly.flags = 0;
		inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		inputAssembly.primitiveRestartEnable = VK_FALSE;

		VkViewport viewport = {};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = (float)swapChainExtent.width;
		viewport.height = (float)swapChainExtent.height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		VkRect2D scissor = {};
		scissor.offset = { 0, 0 };
		scissor.extent = swapChainExtent;

		VkPipelineViewportStateCreateInfo viewportState = {};
		viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportState.pNext = nullptr;
		viewportState.flags = 0;
		viewportState.viewportCount = 1;
		viewportState.pViewports = &viewport;
		viewportState.scissorCount = 1;
		viewportState.pScissors = &scissor;

		VkPipelineRasterizationStateCreateInfo rasterizer = {};
		rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizer.pNext = nullptr;
		rasterizer.flags = 0;
		rasterizer.depthClampEnable = VK_FALSE;
		rasterizer.rasterizerDiscardEnable = VK_FALSE;
		rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
		rasterizer.lineWidth = 1.0f;
		rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
		rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
		rasterizer.depthBiasEnable = VK_FALSE;
		rasterizer.depthBiasConstantFactor = 0.0f; // Optional
		rasterizer.depthBiasClamp = 0.0f; // Optional
		rasterizer.depthBiasSlopeFactor = 0.0f; // Optional

		VkPipelineMultisampleStateCreateInfo multisampling = {};
		multisampling.pNext = nullptr;
		multisampling.flags = 0;
		multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampling.sampleShadingEnable = VK_FALSE;
		multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		multisampling.minSampleShading = 1.0f; // Optional
		multisampling.pSampleMask = nullptr; // Optional
		multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
		multisampling.alphaToOneEnable = VK_FALSE; // Optional

		VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
		colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		colorBlendAttachment.blendEnable = VK_FALSE;
		colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
		colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
		colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
		colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
		colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
		colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional

		VkPipelineColorBlendStateCreateInfo colorBlending = {};
		colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlending.pNext = nullptr;
		colorBlending.flags = 0;
		colorBlending.logicOpEnable = VK_FALSE;
		colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
		colorBlending.attachmentCount = 1;
		colorBlending.pAttachments = &colorBlendAttachment;
		colorBlending.blendConstants[0] = 0.0f; // Optional
		colorBlending.blendConstants[1] = 0.0f; // Optional
		colorBlending.blendConstants[2] = 0.0f; // Optional
		colorBlending.blendConstants[3] = 0.0f; // Optional

		VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.pNext = nullptr;
		pipelineLayoutInfo.flags = 0;
		pipelineLayoutInfo.setLayoutCount = 1;
		pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;
		pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
		pipelineLayoutInfo.pPushConstantRanges = 0; // Optional

		if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &obj->pipelineLayout) != VK_SUCCESS)
		{
			PRINT("Failed to create pipeline layout!");
			exit(1);
		}

		VkGraphicsPipelineCreateInfo pipelineInfo = {};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.stageCount = 2;
		pipelineInfo.pStages = shaderStages;
		pipelineInfo.pVertexInputState = &vertexInputInfo;
		pipelineInfo.pInputAssemblyState = &inputAssembly;
		pipelineInfo.pViewportState = &viewportState;
		pipelineInfo.pRasterizationState = &rasterizer;
		pipelineInfo.pMultisampleState = &multisampling;
		pipelineInfo.pDepthStencilState = nullptr; // Optional
		pipelineInfo.pColorBlendState = &colorBlending;
		pipelineInfo.pDynamicState = nullptr; // Optional
		pipelineInfo.layout = obj->pipelineLayout;
		pipelineInfo.renderPass = renderPass;
		pipelineInfo.subpass = 0;
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
		pipelineInfo.basePipelineIndex = -1; // Optional

		if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &obj->graphicsPipeline) != VK_SUCCESS)
		{
			PRINT("failed to create graphics pipeline!");
			exit(1);
		}

		PRINT("\n" << "Graphics Pipeline created.");

		vkDestroyShaderModule(device, fragShaderModule, nullptr);
		vkDestroyShaderModule(device, vertShaderModule, nullptr);
	}
	void createFramebuffers()
	{
		swapChainFramebuffers = new VkFramebuffer[swapChainImagesCount];

		for (uint32_t i = 0; i < swapChainImagesCount; i++)
		{
			VkImageView attachments[] = { swapChainImageViews[i] };
			VkFramebufferCreateInfo framebufferInfo = {};
			framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferInfo.renderPass = renderPass;
			framebufferInfo.attachmentCount = 1;
			framebufferInfo.pAttachments = attachments;
			framebufferInfo.width = swapChainExtent.width;
			framebufferInfo.height = swapChainExtent.height;
			framebufferInfo.layers = 1;

			if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &swapChainFramebuffers[i]) != VK_SUCCESS)
			{
				PRINT("failed to create framebuffer!");
				exit(1);
			}
		}
	}
	void createCommandPool()
	{
		int queueFamilyIndex = findQueueFamilies(physicalDevice);

		VkCommandPoolCreateInfo poolInfo = {};
		poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		poolInfo.flags = 0; // Optional
		poolInfo.queueFamilyIndex = queueFamilyIndex;

		if (vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool) != VK_SUCCESS)
		{
			PRINT("failed to create command pool!");
			exit(1);
		}
	}
	void createVertexBuffer()
	{
		VkDeviceSize bufferSize = sizeof(vertices);
		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;

		createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			&stagingBuffer, &stagingBufferMemory);

		void *data;
		vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
			memcpy(data, vertices, bufferSize);
		vkUnmapMemory(device, stagingBufferMemory);

		createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &vertexBuffer, &vertexBufferMemory);

		copyBuffer(stagingBuffer, vertexBuffer, bufferSize);

		vkDestroyBuffer(device, stagingBuffer, nullptr);
		vkFreeMemory(device, stagingBufferMemory, nullptr);
	}
	void createIndexBuffer()
	{
		VkDeviceSize bufferSize = indexBufferSize;
		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;

		createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			&stagingBuffer, &stagingBufferMemory);

		void *data;
		vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, indices, sizeof(indices));
		memcpy((char*)data + sizeof(indices), indices_star, sizeof(indices_star));
		vkUnmapMemory(device, stagingBufferMemory);

		createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &indexBuffer, &indexBufferMemory);

		copyBuffer(stagingBuffer, indexBuffer, bufferSize);

		vkDestroyBuffer(device, stagingBuffer, nullptr);
		vkFreeMemory(device, stagingBufferMemory, nullptr);
	}
	void createUniformBuffer()
	{
		VkDeviceSize bufferSize = uboBufferSize;
		createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			&uniformBuffer, &uniformBufferMemory);
	}
	void createDescriptorPool()
	{
		VkDescriptorPoolSize poolSize = {};
		poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		poolSize.descriptorCount = 3;

		VkDescriptorPoolCreateInfo poolInfo = {};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.poolSizeCount = 1;
		poolInfo.pPoolSizes = &poolSize;
		poolInfo.maxSets = 3;
		
		if (vkCreateDescriptorPool(device, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS)
		{
			PRINT("Failed to create descriptor pool!");
			exit(1);
		}
	}
	void createDescriptorSet(RenderObject *obj)
	{
		VkDescriptorSetLayout layouts[] = { descriptorSetLayout };
		VkDescriptorSetAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = descriptorPool;
		allocInfo.descriptorSetCount = 1;
		allocInfo.pSetLayouts = layouts;

		if (vkAllocateDescriptorSets(device, &allocInfo, &obj->descriptorSet) != VK_SUCCESS)
		{
			PRINT("Failed to allocate descriptor set!");
			exit(1);
		}

		VkDescriptorBufferInfo bufferInfo = {};
		bufferInfo.buffer = uniformBuffer;
		bufferInfo.offset = obj->uboOffset;
		bufferInfo.range = 3 * sizeof(mat4);
		
		VkWriteDescriptorSet descriptorWrite = {};
		descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrite.dstSet = obj->descriptorSet;
		descriptorWrite.dstBinding = 0;
		descriptorWrite.dstArrayElement = 0;
		descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrite.descriptorCount = 1;
		descriptorWrite.pBufferInfo = &bufferInfo;
		descriptorWrite.pImageInfo = nullptr; // Optional
		descriptorWrite.pTexelBufferView = nullptr; // Optional

		vkUpdateDescriptorSets(device, 1, &descriptorWrite, 0, nullptr);
	}
	void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer *buffer, VkDeviceMemory *bufferMemory)
	{
		VkBufferCreateInfo bufferInfo = {};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = size;
		bufferInfo.usage = usage;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		if (vkCreateBuffer(device, &bufferInfo, nullptr, buffer) != VK_SUCCESS)
		{
			PRINT("Failed to create vertex buffer!");
			exit(1);
		}

		VkMemoryRequirements memRequirements;
		vkGetBufferMemoryRequirements(device, *buffer, &memRequirements);

		VkMemoryAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

		if (vkAllocateMemory(device, &allocInfo, nullptr, bufferMemory) != VK_SUCCESS)
		{
			PRINT("Failed to allocate vertex buffer memory!");
			exit(1);
		}

		vkBindBufferMemory(device, *buffer, *bufferMemory, 0);
	}
	void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
	{
		VkCommandBufferAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = commandPool;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = 1;

		VkCommandBuffer commandBuffer;
		vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(commandBuffer, &beginInfo);

			VkBufferCopy copyRegion = {};
			copyRegion.srcOffset = 0;
			copyRegion.dstOffset = 0;
			copyRegion.size = size;

			vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

		vkEndCommandBuffer(commandBuffer);

		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;
		
		vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(queue);

		vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
	}
	uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
	{
		VkPhysicalDeviceMemoryProperties memProperties;
		vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

		for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
		{
			if (typeFilter & (1 << i) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
				return i;
		}

		PRINT("Failed to find suitable memory type!");
		exit(1);
	}
	void createCommandBuffers()
	{
		commandBuffers = new VkCommandBuffer[swapChainImagesCount];

		VkCommandBufferAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = commandPool;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = swapChainImagesCount;

		if (vkAllocateCommandBuffers(device, &allocInfo, commandBuffers) != VK_SUCCESS)
		{
			PRINT("failed to allocate command buffers!");
			exit(1);
		}

		for (uint32_t i = 0; i < swapChainImagesCount; i++)
		{
			VkCommandBufferBeginInfo beginInfo = {};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
			beginInfo.pInheritanceInfo = nullptr; // Optional

			vkBeginCommandBuffer(commandBuffers[i], &beginInfo);
				
				VkRenderPassBeginInfo renderPassInfo = {};
				renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
				renderPassInfo.renderPass = renderPass;
				renderPassInfo.framebuffer = swapChainFramebuffers[i];
				renderPassInfo.renderArea.offset = { 0, 0 };
				renderPassInfo.renderArea.extent = swapChainExtent;
				VkClearValue clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
				renderPassInfo.clearValueCount = 1;
				renderPassInfo.pClearValues = &clearColor;

				vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
				
					VkDeviceSize offsets[] = { 0 };
					vkCmdBindVertexBuffers(commandBuffers[i], 0, 1, &vertexBuffer, offsets);
					vkCmdBindIndexBuffer(commandBuffers[i], indexBuffer, 0, VK_INDEX_TYPE_UINT16);

					for (uint32_t j = 0; j < objectCount; j++)
					{
						vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS,
							renderObject[j]->graphicsPipeline);
						vkCmdBindDescriptorSets(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS,
							renderObject[j]->pipelineLayout, 0, 1, &renderObject[j]->descriptorSet, 0, nullptr);
						vkCmdDrawIndexed(commandBuffers[i], renderObject[j]->indexCount, 1,
							renderObject[j]->firstIndex, 0, 0);
					}
				
				vkCmdEndRenderPass(commandBuffers[i]);

			if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS)
			{
				PRINT("failed to record command buffer!");
				exit(1);
			}
		}
	}
	void createSemaphores()
	{
		VkSemaphoreCreateInfo semaphoreInfo = {};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &imageAvailableSemaphore) != VK_SUCCESS ||
			vkCreateSemaphore(device, &semaphoreInfo, nullptr, &renderFinishedSemaphore) != VK_SUCCESS)
		{

			PRINT("failed to create semaphores!");
			exit(1);
		}
	}
	void updateUniformBuffer()
	{
		static clock_t startTime = clock();
		VkDeviceSize bufferSize = uboBufferSize;
		mat4 A, B;

		getScale4(A, 0.5f, 0.5f, 0.0f);
		getRotZ4(B, (float)(clock() - startTime) / CLOCKS_PER_SEC / 2.0f);
		mult4(renderObject[1]->mModel, B, A);

		getRotZ4(renderObject[2]->mModel, -(float)(clock() - startTime) / CLOCKS_PER_SEC / 2.0f);

		if (key[VK_UP] == true)
		{
			dup4(A, *mView);
			getTrans4(B, 0.0f, 0.1f, 0.0f);
			mult4(*mView, B, A);
		}

		if (key[VK_DOWN] == true)
		{
			dup4(A, *mView);
			getTrans4(B, 0.0f, -0.1f, 0.0f);
			mult4(*mView, B, A);
		}

		if (key[VK_LEFT] == true)
		{
			dup4(A, *mView);
			getTrans4(B, 0.1f, 0.0f, 0.0f);
			mult4(*mView, B, A);
		}

		if (key[VK_RIGHT] == true)
		{
			dup4(A, *mView);
			getTrans4(B, -0.1f, 0.0f, 0.0f);
			mult4(*mView, B, A);
		}

		void* data;
		vkMapMemory(device, uniformBufferMemory, 0, bufferSize, 0, &data);
		for (uint32_t i = 0; i < objectCount; i++)
		{
			memcpy((char*)data + renderObject[i]->uboOffset, &renderObject[i]->mModel, sizeof(mat4));
			memcpy((char*)data + sizeof(mat4) + renderObject[i]->uboOffset, renderObject[i]->mView, sizeof(mat4));
			memcpy((char*)data + 2*sizeof(mat4) + renderObject[i]->uboOffset, &renderObject[i]->mProj, sizeof(mat4));
		}
		vkUnmapMemory(device, uniformBufferMemory);
	}
	void drawFrame()
	{
		uint32_t imageIndex;
		vkAcquireNextImageKHR(device, swapChain, std::numeric_limits<uint64_t>::max(), imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);

		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		VkSemaphore waitSemaphores[] = { imageAvailableSemaphore };
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffers[imageIndex];
		VkSemaphore signalSemaphores[] = { renderFinishedSemaphore };
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		if (vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS)
		{
			PRINT("failed to submit draw command buffer!");
			exit(1);
		}

		VkPresentInfoKHR presentInfo = {};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;
		VkSwapchainKHR swapChains[] = { swapChain };
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;
		presentInfo.pImageIndices = &imageIndex;

		vkQueuePresentKHR(queue, &presentInfo);

		vkQueueWaitIdle(queue); // Synchronisation der App mit der GPU (nicht notwendig)
	}
	void printDeviceStats(VkPhysicalDevice device)
	{
		uint32_t extensionCount;
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);
		VkExtensionProperties *extensions = new VkExtensionProperties[extensionCount];
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, extensions);
		PRINT("\t" << "Available device extensions:");
		for (uint32_t i = 0; i < extensionCount; i++) {
			PRINT("\t\t#" << i << "\t" << extensions[i].extensionName);
		}

		VkPhysicalDeviceProperties deviceProperties;
		vkGetPhysicalDeviceProperties(device, &deviceProperties);
		PRINT("\t" << "Device Properties:");
		PRINT("\t\t" << "Api Version:    " << deviceProperties.apiVersion);
		PRINT("\t\t" << "Device ID:      " << deviceProperties.deviceID);
		PRINT("\t\t" << "Device Name:    " << deviceProperties.deviceName);
		PRINT("\t\t" << "Device Type:    " << deviceProperties.deviceType);
		PRINT("\t\t" << "Driver Version: " << deviceProperties.driverVersion);
		PRINT("\t\t" << "Vendor ID:      " << deviceProperties.vendorID);

		VkPhysicalDeviceFeatures deviceFeatures;
		vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
		PRINT("\t" << "Device Featers:");
		PRINT("\t\t" << "Tessellation Shader: " << deviceFeatures.tessellationShader);
		PRINT("\t\t" << "Geometry Shader:     " << deviceFeatures.geometryShader);
		PRINT("\t\t" << "fullDrawIndexUint32: " << deviceFeatures.fullDrawIndexUint32);

		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
		VkQueueFamilyProperties *queueFamilies = new VkQueueFamilyProperties[queueFamilyCount];
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies);

		PRINT("\t" << "Queue Families:");
		for (uint32_t i = 0; i < queueFamilyCount; i++)
		{
			PRINT("\t\t" << "Queue Family (#" << i <<"):");
			PRINT("\t\t\t" << "VK_QUEUE_GRAPHICS_BIT:           " << ((queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0));
			PRINT("\t\t\t" << "VK_QUEUE_COMPUTE_BIT:            " << ((queueFamilies[i].queueFlags & VK_QUEUE_COMPUTE_BIT) != 0));
			PRINT("\t\t\t" << "VK_QUEUE_TRANSFER_BIT:           " << ((queueFamilies[i].queueFlags & VK_QUEUE_TRANSFER_BIT) != 0));
			PRINT("\t\t\t" << "VK_QUEUE_SPARSE_BINDING_BIT:     " << ((queueFamilies[i].queueFlags & VK_QUEUE_SPARSE_BINDING_BIT) != 0));
			PRINT("\t\t\t" << "Queue Count:                     " << queueFamilies[i].queueCount);
			PRINT("\t\t\t" << "Timestamp Valid Bits:            " << queueFamilies[i].timestampValidBits);
			uint32_t width = queueFamilies[i].minImageTransferGranularity.width;
			uint32_t height = queueFamilies[i].minImageTransferGranularity.height;
			uint32_t depth = queueFamilies[i].minImageTransferGranularity.depth;
			PRINT("\t\t\t" << "Min Image Timestamp Granularity: " << width << ", " << height << ", " << depth);
		}

		VkSurfaceCapabilitiesKHR capabilities;
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &capabilities);
		PRINT("\t" << "Surface capabilities:");
		PRINT("\t\t" << "minImageCount:           " << capabilities.minImageCount);
		PRINT("\t\t" << "maxImageCount:           " << capabilities.maxImageCount);
		PRINT("\t\t" << "currentExtent:           " << capabilities.currentExtent.width << "," << capabilities.currentExtent.height);
		PRINT("\t\t" << "minImageExtent:          " << capabilities.minImageExtent.width << "," << capabilities.minImageExtent.height);
		PRINT("\t\t" << "maxImageExtent:          " << capabilities.maxImageExtent.width << "," << capabilities.maxImageExtent.height);
		PRINT("\t\t" << "maxImageArrayLayers:     " << capabilities.maxImageArrayLayers);
		PRINT("\t\t" << "supportedTransforms:     " << capabilities.supportedTransforms);
		PRINT("\t\t" << "currentTransform:        " << capabilities.currentTransform);
		PRINT("\t\t" << "supportedCompositeAlpha: " << capabilities.supportedCompositeAlpha);
		PRINT("\t\t" << "supportedUsageFlags:     " << capabilities.supportedUsageFlags);

		uint32_t formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);
		VkSurfaceFormatKHR *formats = new VkSurfaceFormatKHR[formatCount];
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, formats);
		PRINT("\t" << "Supported formats:");
		for (uint32_t i = 0; i < formatCount; i++)
		{
			PRINT("\t\t" << "Format #" << i << ":");
			PRINT("\t\t\t" << "format:     " << formats[i].format);
			PRINT("\t\t\t" << "colorSpace: " << formats[i].colorSpace);
		}

		uint32_t presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);
		VkPresentModeKHR *presentModes = new VkPresentModeKHR[presentModeCount];
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, presentModes);
		PRINT("\t" << "Supported presentation modes:");
		for (uint32_t i = 0; i < presentModeCount; i++)
		{
			PRINT("\t\t" << "Present mode #" << i << ": " << presentModes[i]);
		}

		delete[] presentModes;
		delete[] formats;
		delete[] queueFamilies;
	}
	int findQueueFamilies(VkPhysicalDevice device)
	{
		int indices = -1;

		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
		VkQueueFamilyProperties *queueFamilies = new VkQueueFamilyProperties[queueFamilyCount];
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies);

		for (unsigned int i = 0; i < queueFamilyCount; i++)
		{
			VkBool32 presentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
			if (queueFamilies[i].queueCount > 0 && queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT && presentSupport)
			{
				indices = i;
			}

			if (indices >= 0) break;
		}

		delete[] queueFamilies;
		return indices;
	}
	bool checkDeviceExtensionSupport(VkPhysicalDevice device)
	{
		bool extensionsSupported = false;

		uint32_t extensionCount;
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);
		VkExtensionProperties *availableExtensions = new VkExtensionProperties[extensionCount];
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions);

		for (uint32_t i = 0; i < extensionCount; i++)
		{
			if (std::strcmp(availableExtensions[i].extensionName, deviceExtensions[0]) == 0)
			{
				extensionsSupported = true;
				break;
			}
		}
		delete[] availableExtensions;
		return extensionsSupported;
	}
	SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device)
	{
		SwapChainSupportDetails details;

		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &details.formatCount, nullptr);
		details.formats = new VkSurfaceFormatKHR[details.formatCount];
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &details.formatCount, details.formats);

		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &details.presentModeCount, nullptr);
		details.presentModes = new VkPresentModeKHR[details.presentModeCount];
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &details.presentModeCount, details.presentModes);

		return details;
	}
	bool isDeviceSuitable(VkPhysicalDevice device)
	{
		int indices = findQueueFamilies(device);
		bool extensionsSupported = checkDeviceExtensionSupport(device);
		bool swapChainAdequate = false;

		if (extensionsSupported)
		{
			SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
			swapChainAdequate = swapChainSupport.formatCount && swapChainSupport.presentModeCount;
		}

		return indices >= 0 && extensionsSupported && swapChainAdequate;
	}
	VkSurfaceFormatKHR chooseSwapSurfaceFormat(uint32_t formatCount, VkSurfaceFormatKHR *availableFormats)
	{
		if (formatCount == 1 && availableFormats[0].format == VK_FORMAT_UNDEFINED)
		{
			return { VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
		}
		for (uint32_t i = 0; i < formatCount; i++)
		{
			if (availableFormats[i].format == VK_FORMAT_B8G8R8A8_UNORM &&
				availableFormats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
				return availableFormats[i];
		}
		return availableFormats[0];
	}
	VkPresentModeKHR chooseSwapPresentMode(uint32_t presentModeCount, VkPresentModeKHR *availablePresentModes)
	{
		VkPresentModeKHR bestMode = VK_PRESENT_MODE_FIFO_KHR;

		for (uint32_t i = 0; i < presentModeCount; i++)
		{
			if (availablePresentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR)
				return availablePresentModes[i];
			else if (availablePresentModes[i] == VK_PRESENT_MODE_IMMEDIATE_KHR)
				bestMode = availablePresentModes[i];
		}

		return bestMode;
	}
	VkExtent2D chooseSwapExtent(VkSurfaceCapabilitiesKHR *capabilities)
	{
		if (capabilities->currentExtent.width != std::numeric_limits<uint32_t>::max())
			return capabilities->currentExtent;
		else
		{
			VkExtent2D actualExtent = { WIDTH, HEIGHT };
			actualExtent.width = std::max(capabilities->minImageExtent.width, std::min(capabilities->maxImageExtent.width, actualExtent.width));
			actualExtent.height = std::max(capabilities->minImageExtent.height, std::min(capabilities->maxImageExtent.height, actualExtent.height));

			return actualExtent;
		}
	}
	ShaderCode loadShader(char *fileName)
	{
		ShaderCode code;

		FILE *file = fopen(fileName, "rb"); // ***** !!! Sehr Wichtig: hier muss das b bei "rb" übergeben werden !!! b steht für binäre Datei *****

		fseek(file, 0, SEEK_END);
		code.filesize = ftell(file);
		rewind(file);
		code.data = (char*)malloc(code.filesize);
		fread(code.data, 1, code.filesize, file);
		PRINT("\n" << fileName << " geladen. Filesize: " << code.filesize);
		fclose(file);

		return code;
	}
	VkShaderModule createShaderModule(ShaderCode code)
	{
		VkShaderModuleCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.pNext = nullptr;
		createInfo.flags = 0;
		createInfo.codeSize = code.filesize;
		createInfo.pCode = (uint32_t*)code.data; // ! alignment Anforderungen von uint32_t beachten !

		VkShaderModule shaderModule;
		if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
		{
			PRINT("Failed to create shader module!");
			exit(1);
		}

		return shaderModule;
	}
};

#ifndef NOCONSOLE
int main(int argc, char *argv[])
#else
int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
#endif
{
	RenderScene scene;
	App2DAction app(&scene);

	logfile.open("log.txt");

	PRINT("***** 2D Action !!! *****");
	PRINT("=========================");

	app.run();

	logfile.close();

	return 0;
}