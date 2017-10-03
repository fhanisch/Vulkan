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

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

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
};

struct VertexData
{
	float *data;
	uint32_t size;
};

struct IndexData
{
	uint16_t *data;
	uint32_t size;
};

class VertexHandler
{
public:
	uint32_t vertexDataCount;
	VertexData *vertexData;

	VertexHandler(uint32_t _vertexDataCount)
	{
		vertexDataCount = _vertexDataCount;
		vertexData = new VertexData[vertexDataCount];
	}
	uint32_t getSize()
	{
		uint32_t size = 0;
		for (uint32_t i = 0; i < vertexDataCount; i++)
		{
			size += vertexData[i].size;
		}
		return size;
	}
};

class IndexHandler
{
public:
	uint32_t indexDataCount;
	IndexData *indexData;

	IndexHandler(uint32_t _indexDataCount)
	{
		indexDataCount = _indexDataCount;
		indexData = new IndexData[indexDataCount];
	}
	uint32_t getSize()
	{
		uint32_t size = 0;
		for (uint32_t i = 0; i < indexDataCount; i++)
		{
			size += indexData[i].size;
		}
		return size;
	}
	uint32_t getOffset(uint32_t index)
	{
		uint32_t size = 0;
		for (uint32_t i = 0; i < index; i++)
		{
			size += indexData[i].size;
		}
		return size;
	}
};

const Vertex vertices[] = {
	{ {  -1.0f,  -1.0f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f } },  //0
	{ {   1.0f,  -1.0f }, { 0.0f, 1.0f, 0.0f }, { 1.0f, 1.0f } },  //1
	{ {   1.0f,   1.0f }, { 0.0f, 0.0f, 1.0f }, { 1.0f, 0.0f } },  //2
	{ {  -1.0f,   1.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f } },  //3
	{ {   0.0f,  -1.0f }, { 1.0f, 0.0f, 1.0f }, { 0.0f, 0.0f } },  //4
	{ {  0.25f, -0.25f }, { 1.0f, 0.0f, 1.0f }, { 0.0f, 0.0f } },  //5
	{ { -0.25f, -0.25f }, { 1.0f, 0.0f, 1.0f }, { 0.0f, 0.0f } },  //6
	{ {   1.0f,   0.0f }, { 1.0f, 0.0f, 1.0f }, { 0.0f, 0.0f } },  //7
	{ {  0.25f,  0.25f }, { 1.0f, 0.0f, 1.0f }, { 0.0f, 0.0f } },  //8
	{ {   0.0f,   1.0f }, { 1.0f, 0.0f, 1.0f }, { 0.0f, 0.0f } },  //9
	{ {  -0.25,  0.25f }, { 1.0f, 0.0f, 1.0f }, { 0.0f, 0.0f } },  //10
	{ {  -1.0f,   0.0f }, { 1.0f, 0.0f, 1.0f }, { 0.0f, 0.0f } },  //11
	{ {   0.0f,   0.0f }, { 1.0f, 1.0f, 0.0f }, { 0.0f, 0.0f } },  //12
	{ {   1.0f,   0.0f }, { 1.0f, 1.0f, 0.0f }, { 0.0f, 0.0f } },  //13
	{ {   2.0f,   0.0f }, { 1.0f, 1.0f, 0.0f }, { 0.0f, 0.0f } }   //14
};

const uint16_t indices[] = { 0, 1, 2, 2, 3, 0 };
const uint16_t indices_star[] = { 4, 5, 6, 5, 7, 8, 8, 9, 10, 10, 11, 6, 6, 5, 8, 8, 10, 6 };
const uint16_t indices_patches[] = { 12,13, 13,14 };

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
	char *tesselationControllShaderName;
	char *tesselationEvaluationShaderName;
	bool hasGraphicsPipeline;
	uint32_t stageCount;
	VkPipelineLayout pipelineLayout;
	VkPipeline graphicsPipeline;
	VkDescriptorSet descriptorSet;
	uint32_t indexCount;
	uint32_t firstIndex;
	VkDeviceSize uboOffset;
	VkVertexInputBindingDescription bindingDescription;
	uint32_t attributeDescriptionCount;
	VkVertexInputAttributeDescription *attributeDescriptions;
	VkPrimitiveTopology topology;
	VkPipelineTessellationStateCreateInfo tessellationStateCreateInfo;

	mat4 mModel;
	mat4 *mView;
	mat4 mProj;

	RenderObject(char *vertName, char *fragName, VkDeviceSize _uboOffset, mat4 *_mView)
	{
		init(vertName, fragName, _uboOffset, _mView);
	}
	RenderObject(char *vertName, char *tcsName, char *tesName, char *fragName, VkDeviceSize _uboOffset, mat4 *_mView)
	{
		init(vertName, fragName, _uboOffset, _mView);
		stageCount = 4;
		tesselationControllShaderName = tcsName;
		tesselationEvaluationShaderName = tesName;
		topology = VK_PRIMITIVE_TOPOLOGY_PATCH_LIST;
		tessellationStateCreateInfo = getTessellationStateCreateInfo(2);
	}
	void init(char *vertName, char *fragName, VkDeviceSize _uboOffset, mat4 *_mView)
	{
		vertexShaderFileName = vertName;
		fragmentShaderFileName = fragName;
		uboOffset = _uboOffset;
		stageCount = 2;
		hasGraphicsPipeline = true;
		bindingDescription = getBindingDescription(sizeof(Vertex));
		attributeDescriptionCount = 3;
		VkFormat formats[] = { VK_FORMAT_R32G32_SFLOAT, VK_FORMAT_R32G32B32_SFLOAT, VK_FORMAT_R32G32_SFLOAT };
		uint32_t offsets[] = { offsetof(Vertex, pos) ,offsetof(Vertex, color) ,offsetof(Vertex, texCoords) };
		attributeDescriptions = getAttributeDescriptions(attributeDescriptionCount, formats, offsets);
		topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		identity4(mModel);
		mView = _mView;
		identity4(mProj);
	}
	static VkVertexInputBindingDescription getBindingDescription(uint32_t stride)
	{
		VkVertexInputBindingDescription bindingDescription = {};
		bindingDescription.binding = 0;
		bindingDescription.stride = stride;
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return bindingDescription;
	}
	static VkVertexInputAttributeDescription *getAttributeDescriptions(uint32_t count, VkFormat *formats, uint32_t *offsets)
	{
		VkVertexInputAttributeDescription *attributeDescriptions = new VkVertexInputAttributeDescription[3];

		for (uint32_t i = 0; i < count; i++)
		{
			attributeDescriptions[i].binding = 0;
			attributeDescriptions[i].location = i;
			attributeDescriptions[i].format = formats[i];
			attributeDescriptions[i].offset = offsets[i];
		}

		return attributeDescriptions;
	}
	static VkPipelineTessellationStateCreateInfo getTessellationStateCreateInfo(uint32_t patchControlPoints)
	{
		VkPipelineTessellationStateCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
		createInfo.patchControlPoints = patchControlPoints;

		return createInfo;
	}
};

class CircleFilled : public RenderObject
{
public:
	CircleFilled(char *vertName, char *fragName, VkDeviceSize _uboOffset, mat4 *_mView)
		: RenderObject(vertName, fragName, _uboOffset, _mView){}

	void motion()
	{
		mat4 A, B;

		if (key[0x41] == true)
		{
			dup4(A, mModel);
			getTrans4(B, -0.1f, 0.0f, 0.0f);
			mult4(mModel, B, A);
		}
		if (key[0x44] == true)
		{
			dup4(A, mModel);
			getTrans4(B, 0.1f, 0.0f, 0.0f);
			mult4(mModel, B, A);
		}
		if (key[0x53] == true)
		{
			dup4(A, mModel);
			getTrans4(B, 0.0f, 0.1f, 0.0f);
			mult4(mModel, B, A);
		}
		if (key[0x57] == true)
		{
			dup4(A, mModel);
			getTrans4(B, 0.0f, -0.1f, 0.0f);
			mult4(mModel, B, A);
		}
	}
};

class RenderScene
{
public:
	mat4 mView;
	RenderObject *powerMeter, *square, *star, *circle, *welle, *perlin1d, *perlin2d, *muster,
		*curveTesselator, *perlin1dTesselator, *squareTex;
	CircleFilled *circleFilled;
	VertexHandler *hVertices;
	IndexHandler *hIndices;

	RenderScene()
	{
		mat4 A, B;
		identity4(mView);
		hVertices = new VertexHandler(3);
		hIndices = new IndexHandler(5);

		hVertices->vertexData[0].data = (float*)vertices;
		hVertices->vertexData[0].size = sizeof(vertices);

		hIndices->indexData[0].data = (uint16_t*)indices;
		hIndices->indexData[0].size = sizeof(indices);
		hIndices->indexData[1].data = (uint16_t*)indices_star;
		hIndices->indexData[1].size = sizeof(indices_star);
		hIndices->indexData[2].data = (uint16_t*)indices_patches;
		hIndices->indexData[2].size = sizeof(indices_patches);

		vecf(&hVertices->vertexData[1].data, &hVertices->vertexData[1].size, 0.0f, 0.01f, 101);
		vecs(&hIndices->indexData[3].data, &hIndices->indexData[3].size, sizeof(vertices)/sizeof(float), 101);
		
		uint32_t patchesCount = 100;
		vecf(&hVertices->vertexData[2].data, &hVertices->vertexData[2].size, 0.0f, 1.0f, patchesCount);
		hIndices->indexData[4].size = 2 * (patchesCount - 1) * sizeof(uint16_t);
		hIndices->indexData[4].data = new uint16_t[2 * (patchesCount - 1)];
		for (uint32_t i = 0; i < (patchesCount - 1); i++)
		{
			hIndices->indexData[4].data[2*i] = 206 + i;
			hIndices->indexData[4].data[2*i+1] = 207 + i;
		}

		powerMeter = new RenderObject("vs_2d.spv", "fs_powermeter.spv", 0, &mView);
		powerMeter->indexCount = hIndices->indexData[0].size / sizeof(uint16_t);
		powerMeter->firstIndex = 0;

		square = new RenderObject("vs_2d.spv", "fs_2d.spv", 0x100, &mView);
		square->indexCount = hIndices->indexData[0].size / sizeof(uint16_t);
		square->firstIndex = 0;

		star = new RenderObject("vs_2d.spv", "fs_2d.spv", 0x200, &mView);
		star->hasGraphicsPipeline = false;
		star->indexCount = hIndices->indexData[1].size / sizeof(uint16_t);
		star->firstIndex = hIndices->getOffset(1) / sizeof(uint16_t);

		curveTesselator = new RenderObject("vs_curveTesselator.spv", "tcs_curveTesselator.spv", "tes_curveTesselator.spv", "fs_curveTesselator.spv", 0x300, &mView);
		curveTesselator->indexCount = hIndices->indexData[2].size / sizeof(uint16_t);
		curveTesselator->firstIndex = hIndices->getOffset(2) / sizeof(uint16_t);

		circle = new RenderObject("vs_circle.spv", "fs_2d.spv", 0x400, &mView);
		circle->indexCount = hIndices->indexData[3].size / sizeof(uint16_t);
		circle->firstIndex = hIndices->getOffset(3) / sizeof(uint16_t);
		circle->bindingDescription = RenderObject::getBindingDescription(sizeof(float));
		circle->attributeDescriptionCount = 1;
		VkFormat format[] = { VK_FORMAT_R32_SFLOAT };
		uint32_t offset[] = { 0 };
		circle->attributeDescriptions = RenderObject::getAttributeDescriptions(1, format, offset);
		circle->topology = VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
		getScale4(circle->mModel, 1.5f, 1.5f, 1.0f);

		circleFilled = new CircleFilled("vs_2d.spv", "fs_circleFilled.spv", 0x500, &mView);
		circleFilled->indexCount = hIndices->indexData[0].size / sizeof(uint16_t);
		circleFilled->firstIndex = 0;
		getTrans4(circleFilled->mModel, 5.0f, 5.0f, 0.0f);

		welle = new RenderObject("vs_welle.spv", "fs_2d.spv", 0x600, &mView);
		welle->indexCount = hIndices->indexData[3].size / sizeof(uint16_t);
		welle->firstIndex = hIndices->getOffset(3) / sizeof(uint16_t);
		welle->bindingDescription = RenderObject::getBindingDescription(sizeof(float));
		welle->attributeDescriptionCount = 1;
		format[0] = { VK_FORMAT_R32_SFLOAT };
		offset[0] = { 0 };
		welle->attributeDescriptions = RenderObject::getAttributeDescriptions(1, format, offset);
		welle->topology = VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
		getScale4(A, 2.0f, 1.0f, 1.0f);
		getTrans4(B, 0.0f, -2.0f, 0.0f);
		mult4(welle->mModel, B, A);

		perlin1d = new RenderObject("vs_perlin1d.spv", "fs_2d.spv", 0x700, &mView);
		perlin1d->indexCount = hIndices->indexData[3].size / sizeof(uint16_t);
		perlin1d->firstIndex = hIndices->getOffset(3) / sizeof(uint16_t);
		perlin1d->bindingDescription = RenderObject::getBindingDescription(sizeof(float));
		perlin1d->attributeDescriptionCount = 1;
		format[0] = { VK_FORMAT_R32_SFLOAT };
		offset[0] = { 0 };
		perlin1d->attributeDescriptions = RenderObject::getAttributeDescriptions(1, format, offset);
		perlin1d->topology = VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
		getScale4(A, 2.0f, 0.5f, 1.0f);
		getTrans4(B, -1.0f, 2.0f, 0.0f);
		mult4(perlin1d->mModel, B, A);

		perlin2d = new RenderObject("vs_2d.spv", "fs_perlin2d.spv", 0x800, &mView);
		perlin2d->indexCount = hIndices->indexData[0].size / sizeof(uint16_t);
		perlin2d->firstIndex = 0;
		getTrans4(perlin2d->mModel, -5.0f, -5.0f, 0.0f);

		muster = new RenderObject("vs_2d.spv", "fs_muster.spv", 0x900, &mView);
		muster->indexCount = hIndices->indexData[0].size / sizeof(uint16_t);
		muster->firstIndex = 0;
		getScale4(A, 2.0f, 2.0f, 1.0f);
		getTrans4(B, -5.0f, 5.0f, 0.0f);
		mult4(muster->mModel, B, A);
		
		perlin1dTesselator = new RenderObject("vs_perlin1dTesselator.spv", "tcs_perlin1dTesselator.spv", "tes_perlin1dTesselator.spv", "fs_curveTesselator.spv", 0xa00, &mView);
		perlin1dTesselator->indexCount = hIndices->indexData[4].size / sizeof(uint16_t);
		perlin1dTesselator->firstIndex = hIndices->getOffset(4) / sizeof(uint16_t);
		perlin1dTesselator->bindingDescription = RenderObject::getBindingDescription(sizeof(float));
		perlin1dTesselator->attributeDescriptionCount = 1;
		format[0] = { VK_FORMAT_R32_SFLOAT };
		offset[0] = { 0 };
		perlin1dTesselator->attributeDescriptions = RenderObject::getAttributeDescriptions(1, format, offset);
		getScale4(A, 2.0f, 1.0f, 1.0f);
		getTrans4(B, 2.0f, -2.0f, 0.0f);
		mult4(perlin1dTesselator->mModel, B, A);

		squareTex = new RenderObject("vs_2d.spv", "fs_2d_tex.spv", 0xb00, &mView);
		squareTex->indexCount = hIndices->indexData[0].size / sizeof(uint16_t);
		squareTex->firstIndex = 0;
		getTrans4(squareTex->mModel, 0.0f, 5.0f, 0.0f);
	}
};

class App2DAction
{
public:
	App2DAction(RenderScene *scene)
	{
		addObject(scene->powerMeter);
		addObject(scene->square);
		addObject(scene->star);
		addObject(scene->curveTesselator);
		addObject(scene->circle);
		addObject(scene->welle);
		addObject(scene->perlin1d);
		addObject(scene->perlin2d);
		addObject(scene->muster);
		addObject(scene->circleFilled);
		addObject(scene->perlin1dTesselator);
		addObject(scene->squareTex);
		mView = &scene->mView;
		hVertices = scene->hVertices;
		hIndices = scene->hIndices;
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
	VkImage *swapChainImages, textureImage;
	VkFormat swapChainImageFormat;
	VkExtent2D swapChainExtent;
	VkImageView *swapChainImageViews, textureImageView;
	VkSampler textureSampler;
	VkFramebuffer *swapChainFramebuffers;
	VkRenderPass renderPass;
	VkDescriptorSetLayout descriptorSetLayout;
	VkCommandPool commandPool;
	VkBuffer vertexBuffer, indexBuffer, uniformBuffer;
	VkDeviceMemory vertexBufferMemory, indexBufferMemory, uniformBufferMemory, textureImageMemory;
	VkDescriptorPool descriptorPool;
	VkCommandBuffer *commandBuffers;
	VkSemaphore imageAvailableSemaphore;
	VkSemaphore renderFinishedSemaphore;

	VkDeviceSize uboBufferSize = 0x0;
	uint16_t maxObjectCount = 20;
	uint16_t objectCount = 0;
	RenderObject **renderObject = new RenderObject*[maxObjectCount];
	mat4 *mView;
	VertexHandler *hVertices;
	IndexHandler *hIndices;

	void addObject(RenderObject *obj)
	{
		uboBufferSize += 0x100;
		renderObject[objectCount] = obj;
		objectCount++;
	}
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
		
		//create pipeline
		for (uint32_t i = 0; i < objectCount; i++)
		{
			if (renderObject[i]->hasGraphicsPipeline)
				createGraphicsPipeline(renderObject[i]);
		}
		renderObject[2]->graphicsPipeline = renderObject[1]->graphicsPipeline;
		renderObject[2]->pipelineLayout = renderObject[1]->pipelineLayout;

		createFramebuffers();
		createCommandPool();
		createTextureImage();
		createTextureImageView();
		createTextureSampler();
		createVertexBuffer();
		createIndexBuffer();
		createUniformBuffer();
		createDescriptorPool();

		//create uniform descriptors
		for (uint32_t i = 0; i < objectCount; i++)
		{
			createDescriptorSet(renderObject[i]);
		}

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
		
		//destroy pipelines + layouts
		for (uint32_t i = 0; i < objectCount; i++)
		{
			if (renderObject[i]->hasGraphicsPipeline)
			{
				vkDestroyPipeline(device, renderObject[i]->graphicsPipeline, nullptr);
				vkDestroyPipelineLayout(device, renderObject[i]->pipelineLayout, nullptr);
			}
		}
		
		vkDestroyRenderPass(device, renderPass, nullptr);
		for (uint32_t i = 0; i < swapChainImagesCount; i++)
			vkDestroyImageView(device, swapChainImageViews[i], nullptr);
		vkDestroySwapchainKHR(device, swapChain, nullptr);
	}
	void cleanup()
	{
		cleanupSwapChain();
		vkDestroySampler(device, textureSampler, nullptr);
		vkDestroyImageView(device, textureImageView, nullptr);
		vkDestroyImage(device, textureImage, nullptr);
		vkFreeMemory(device, textureImageMemory, nullptr);
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
		deviceFeatures.tessellationShader = VK_TRUE;
		deviceFeatures.samplerAnisotropy = VK_TRUE;

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
			swapChainImageViews[i] = createImageView(swapChainImages[i], swapChainImageFormat);
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
		uboLayoutBinding.stageFlags = VK_SHADER_STAGE_ALL_GRAPHICS;
		uboLayoutBinding.pImmutableSamplers = nullptr;

		VkDescriptorSetLayoutBinding samplerLayoutBinding = {};
		samplerLayoutBinding.binding = 1;
		samplerLayoutBinding.descriptorCount = 1;
		samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
		samplerLayoutBinding.pImmutableSamplers = nullptr;

		VkDescriptorSetLayoutBinding bindings[] = { uboLayoutBinding,samplerLayoutBinding };

		VkDescriptorSetLayoutCreateInfo layoutInfo = {};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = 2;
		layoutInfo.pBindings = bindings;

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
		ShaderCode tcShaderCode;
		ShaderCode teShaderCode;
		if (obj->stageCount > 2)
		{
			tcShaderCode = loadShader(obj->tesselationControllShaderName);
			teShaderCode = loadShader(obj->tesselationEvaluationShaderName);
		}

		VkShaderModule vertShaderModule = createShaderModule(vertShaderCode);
		VkShaderModule fragShaderModule = createShaderModule(fragShaderCode);
		VkShaderModule tcShaderModule = VK_NULL_HANDLE;
		VkShaderModule teShaderModule = VK_NULL_HANDLE;
		if (obj->stageCount > 2)
		{
			tcShaderModule = createShaderModule(tcShaderCode);
			teShaderModule = createShaderModule(teShaderCode);
		}

		VkPipelineShaderStageCreateInfo vertShaderStageInfo = getShaderStageInfo(VK_SHADER_STAGE_VERTEX_BIT, vertShaderModule);
		VkPipelineShaderStageCreateInfo fragShaderStageInfo = getShaderStageInfo(VK_SHADER_STAGE_FRAGMENT_BIT, fragShaderModule);
		VkPipelineShaderStageCreateInfo tcShaderStageInfo = getShaderStageInfo(VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT, tcShaderModule);
		VkPipelineShaderStageCreateInfo teShaderStageInfo = getShaderStageInfo(VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT, teShaderModule);

		VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo, tcShaderStageInfo, teShaderStageInfo };

		VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputInfo.pNext = nullptr;
		vertexInputInfo.flags = 0;
		vertexInputInfo.vertexBindingDescriptionCount = 1;
		vertexInputInfo.pVertexBindingDescriptions = &obj->bindingDescription;
		vertexInputInfo.vertexAttributeDescriptionCount = obj->attributeDescriptionCount;
		vertexInputInfo.pVertexAttributeDescriptions = obj->attributeDescriptions;

		VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
		inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssembly.pNext = nullptr;
		inputAssembly.flags = 0;
		inputAssembly.topology = obj->topology;
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
		colorBlendAttachment.blendEnable = VK_TRUE;
		colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
		colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
		colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

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
		pipelineInfo.stageCount = obj->stageCount;
		pipelineInfo.pStages = shaderStages;
		pipelineInfo.pVertexInputState = &vertexInputInfo;
		pipelineInfo.pInputAssemblyState = &inputAssembly;
		pipelineInfo.pTessellationState = &obj->tessellationStateCreateInfo;
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
		vkDestroyShaderModule(device, tcShaderModule, nullptr);
		vkDestroyShaderModule(device, teShaderModule, nullptr);
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
	void createTextureImage()
	{
		int texWidth, texHeight, texChannels;
		stbi_uc *pixels = stbi_load("texture.jpg", &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);

		VkDeviceSize imageSize = texWidth * texHeight * 4;

		if (!pixels)
		{
			PRINT("Failed to load texture image!");
			exit(1);
		}

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;

		createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			&stagingBuffer, &stagingBufferMemory);

		void* data;
		vkMapMemory(device, stagingBufferMemory, 0, imageSize, 0, &data);
			memcpy(data, pixels, imageSize);
		vkUnmapMemory(device, stagingBufferMemory);

		stbi_image_free(pixels);

		createImage(texWidth, texHeight, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL,
			VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			&textureImage, &textureImageMemory);

		transitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
			copyBufferToImage(stagingBuffer, textureImage, texWidth, texHeight);
		transitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

		vkDestroyBuffer(device, stagingBuffer, nullptr);
		vkFreeMemory(device, stagingBufferMemory, nullptr);
	}
	void createTextureImageView()
	{
		textureImageView = createImageView(textureImage, VK_FORMAT_R8G8B8A8_UNORM);
	}
	void createTextureSampler()
	{
		VkSamplerCreateInfo samplerInfo = {};
		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo.magFilter = VK_FILTER_LINEAR;
		samplerInfo.minFilter = VK_FILTER_LINEAR;
		samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.anisotropyEnable = VK_TRUE;
		samplerInfo.maxAnisotropy = 16;
		samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		samplerInfo.unnormalizedCoordinates = VK_FALSE;
		samplerInfo.compareEnable = VK_FALSE;
		samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		samplerInfo.mipLodBias = 0.0f;
		samplerInfo.minLod = 0.0f;
		samplerInfo.maxLod = 0.0f;

		if (vkCreateSampler(device, &samplerInfo, nullptr, &textureSampler) != VK_SUCCESS)
		{
			PRINT("Failed to create texture sampler!");
			exit(1);
		}
	}
	VkImageView createImageView(VkImage image, VkFormat format)
	{
		VkImageViewCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.pNext = nullptr;
		createInfo.flags = 0;
		createInfo.image = image;
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = format;
		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;

		VkImageView imageView;
		if (vkCreateImageView(device, &createInfo, nullptr, &imageView) != VK_SUCCESS)
		{
			PRINT("Failed to create image views!");
			exit(1);
		}

		return imageView;
	}
	void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage,
		VkMemoryPropertyFlags properties, VkImage *image, VkDeviceMemory *imageMemory)
	{
		VkImageCreateInfo imageInfo = {};
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.imageType = VK_IMAGE_TYPE_2D;
		imageInfo.extent.width = width;
		imageInfo.extent.height = height;
		imageInfo.extent.depth = 1;
		imageInfo.mipLevels = 1;
		imageInfo.arrayLayers = 1;
		imageInfo.format = format;
		imageInfo.tiling = tiling;
		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageInfo.usage = usage;
		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageInfo.flags = 0; // Optional

		if (vkCreateImage(device, &imageInfo, nullptr, image) != VK_SUCCESS)
		{
			PRINT("Failed to create image!");
			exit(1);
		}

		VkMemoryRequirements memRequirements;
		vkGetImageMemoryRequirements(device, *image, &memRequirements);

		VkMemoryAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

		if (vkAllocateMemory(device, &allocInfo, nullptr, imageMemory) != VK_SUCCESS)
		{
			PRINT("Failed to allocate image memory!");
			exit(1);
		}

		vkBindImageMemory(device, *image, *imageMemory, 0);
	}
	void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout)
	{
		VkCommandBuffer commandBuffer = beginSingleTimeCommands();

		VkImageMemoryBarrier barrier = {};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.oldLayout = oldLayout;
		barrier.newLayout = newLayout;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.image = image;
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = 1;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;
		
		VkPipelineStageFlags sourceStage;
		VkPipelineStageFlags destinationStage;

		if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
		{
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		}
		else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
		{
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		}
		else
		{
			PRINT("Unsupported layout transition!");
		}

		vkCmdPipelineBarrier(commandBuffer, sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);

		endSingleTimeCommands(commandBuffer);
	}
	void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height)
	{
		VkCommandBuffer commandBuffer = beginSingleTimeCommands();

		VkBufferImageCopy region = {};
		region.bufferOffset = 0;
		region.bufferRowLength = 0;
		region.bufferImageHeight = 0;

		region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		region.imageSubresource.mipLevel = 0;
		region.imageSubresource.baseArrayLayer = 0;
		region.imageSubresource.layerCount = 1;

		region.imageOffset = { 0, 0, 0 };
		region.imageExtent = { width,height,1 };

		vkCmdCopyBufferToImage(commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

		endSingleTimeCommands(commandBuffer);
	}
	void createVertexBuffer()
	{
		VkDeviceSize bufferSize = hVertices->getSize();
		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;

		createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			&stagingBuffer, &stagingBufferMemory);

		void *data;
		vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
			for (uint32_t i = 0; i < hVertices->vertexDataCount; i++)
			{
				memcpy(data, hVertices->vertexData[i].data, hVertices->vertexData[i].size);
				data = ((char*)data + hVertices->vertexData[i].size);
			}
		vkUnmapMemory(device, stagingBufferMemory);

		createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &vertexBuffer, &vertexBufferMemory);

		copyBuffer(stagingBuffer, vertexBuffer, bufferSize);

		vkDestroyBuffer(device, stagingBuffer, nullptr);
		vkFreeMemory(device, stagingBufferMemory, nullptr);
	}
	void createIndexBuffer()
	{
		VkDeviceSize bufferSize = hIndices->getSize();
		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;

		createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			&stagingBuffer, &stagingBufferMemory);

		void *data;
		vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
			for (uint32_t i = 0; i < hIndices->indexDataCount; i++)
			{
				memcpy(data, hIndices->indexData[i].data, hIndices->indexData[i].size);
				data = ((char*)data + hIndices->indexData[i].size);
			}
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
		VkDescriptorPoolSize poolSize[2] = {};
		poolSize[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		poolSize[0].descriptorCount = objectCount;
		poolSize[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		poolSize[1].descriptorCount = objectCount;

		VkDescriptorPoolCreateInfo poolInfo = {};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.poolSizeCount = 2;
		poolInfo.pPoolSizes = poolSize;
		poolInfo.maxSets = objectCount;
		
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
		bufferInfo.range = 3 * sizeof(mat4) + sizeof(float);

		VkDescriptorImageInfo imageInfo = {};
		imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageInfo.imageView = textureImageView;
		imageInfo.sampler = textureSampler;
		
		VkWriteDescriptorSet descriptorWrites[2] = {};
		descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[0].dstSet = obj->descriptorSet;
		descriptorWrites[0].dstBinding = 0;
		descriptorWrites[0].dstArrayElement = 0;
		descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrites[0].descriptorCount = 1;
		descriptorWrites[0].pBufferInfo = &bufferInfo;
		descriptorWrites[0].pImageInfo = nullptr; // Optional
		descriptorWrites[0].pTexelBufferView = nullptr; // Optional

		descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[1].dstSet = obj->descriptorSet;
		descriptorWrites[1].dstBinding = 1;
		descriptorWrites[1].dstArrayElement = 0;
		descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorWrites[1].descriptorCount = 1;
		descriptorWrites[1].pImageInfo = &imageInfo;

		vkUpdateDescriptorSets(device, 2, descriptorWrites, 0, nullptr);
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
		
		VkCommandBuffer commandBuffer = beginSingleTimeCommands();

			VkBufferCopy copyRegion = {};
			copyRegion.srcOffset = 0;
			copyRegion.dstOffset = 0;
			copyRegion.size = size;
			vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

		endSingleTimeCommands(commandBuffer);
	}
	VkCommandBuffer beginSingleTimeCommands()
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

		return commandBuffer;
	}
	void endSingleTimeCommands(VkCommandBuffer commandBuffer)
	{
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

		float time = (float)(clock() - startTime) / CLOCKS_PER_SEC;

		getScale4(A, 0.5f, 0.5f, 0.0f);
		getRotZ4(B, time / 2.0f);
		mult4(renderObject[1]->mModel, B, A);

		getRotZ4(renderObject[2]->mModel, -time / 2.0f);

		((CircleFilled*)renderObject[9])->motion();

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
			memcpy((char*)data + 3*sizeof(mat4) + renderObject[i]->uboOffset, &time, sizeof(float));
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
	VkPipelineShaderStageCreateInfo getShaderStageInfo(VkShaderStageFlagBits stage, VkShaderModule module)
	{
		VkPipelineShaderStageCreateInfo shaderStageInfo = {};
		shaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		shaderStageInfo.pNext = nullptr;
		shaderStageInfo.flags = 0;
		shaderStageInfo.stage = stage;
		shaderStageInfo.module = module;
		shaderStageInfo.pName = "main";
		shaderStageInfo.pSpecializationInfo = nullptr;

		return shaderStageInfo;
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