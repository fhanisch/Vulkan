#pragma once

#define _CRT_SECURE_NO_WARNINGS
#define VK_USE_PLATFORM_WIN32_KHR

#include <vulkan/vulkan.h>
#include "Window.h"
#include <iostream>
#include <algorithm>
#include <matrix.h>
#include <time.h>

#undef min
#undef max

struct Vertex {
	vec3 pos;
	vec3 color;
	vec2 texCoords;
};

class VertexData
{
protected:
	float *data;
	uint64_t size;
	uint64_t *offsets;
	uint32_t dataSetCount;
public:
	VertexData();
	~VertexData();
	void addData(float *_data, uint64_t _size);
	float *getData();
	uint64_t getSize();
	uint64_t getOffset(uint32_t index);
};

class IndexData
{
protected:
	uint16_t *data;
	uint64_t size;
	uint32_t *indexCount;
	uint32_t *firstIndex;
	uint32_t dataSetCount;
public:
	IndexData();
	~IndexData();
	void addData(uint16_t *_data, uint64_t _size);
	uint16_t *getData();
	uint64_t getSize();
	uint32_t getIndexCount(uint32_t index);
	uint32_t getFirstIndex(uint32_t index);
};

const Vertex verticesPlane[] = {
	{ {  -1.0f,   1.0f,  0.0f }, { 1.0f, 0.0f, 1.0f }, { 0.0f, 0.0f } },
	{ {   1.0f,   1.0f,  0.0f }, { 1.0f, 0.0f, 1.0f }, { 1.0f, 0.0f } },
	{ {   1.0f,  -1.0f,  0.0f }, { 1.0f, 0.0f, 1.0f }, { 1.0f, 1.0f } },
	{ {  -1.0f,  -1.0f,  0.0f }, { 1.0f, 0.0f, 1.0f }, { 0.0f, 1.0f } }
};

const Vertex verticesStar[] = {
	{ {   0.0f,  -1.0f,  0.0f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f } },
	{ {  0.25f, -0.25f,  0.0f }, { 1.0f, 0.0f, 1.0f }, { 0.0f, 0.0f } },
	{ { -0.25f, -0.25f,  0.0f }, { 1.0f, 0.0f, 1.0f }, { 0.0f, 0.0f } },
	{ {   1.0f,   0.0f,  0.0f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f } },
	{ {  0.25f,  0.25f,  0.0f }, { 1.0f, 0.0f, 1.0f }, { 0.0f, 0.0f } },
	{ {   0.0f,   1.0f,  0.0f }, { 1.0f, 1.0f, 0.0f }, { 0.0f, 0.0f } },
	{ {  -0.25,  0.25f,  0.0f }, { 1.0f, 0.0f, 1.0f }, { 0.0f, 0.0f } },
	{ {  -1.0f,   0.0f,  0.0f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f } }
};
const float verticesPatches[] = { 0.0f, 1.0f, 2.0f };
static float *verticesCurve;
static uint32_t verticesCurveSize;
static float *verticesPatches2;
static uint32_t verticesPatches2Size;
static float *meshGridVertices;
static uint32_t meshGridVerticesSize;

const uint16_t indicesPlane[] = { 0, 1, 2, 2, 3, 0 };
const uint16_t indicesStar[] = { 0, 1, 2, 1, 3, 4, 4, 5, 6, 6, 7, 2, 2, 1, 4, 4, 6, 2 };
const uint16_t indicesPatches[] = { 0,1, 1,2 };
static uint16_t *indicesCurve;
static uint32_t indicesCurveSize;
static uint16_t *indicesPatches2;
static uint32_t indicesPatches2Size;
static uint16_t *meshGridIndices;
static uint32_t meshGridIndicesSize;

class Buffer
{
protected:
	size_t size;
	VkBuffer buffer;
	VkDeviceMemory bufferMemory;
	VkCommandBuffer commandBuffer;
	VkPhysicalDevice physicalDevice;
	VkDevice device;
	VkCommandPool commandPool;
	VkQueue queue;
public:
	Buffer(VkPhysicalDevice _physicalDevice, VkDevice _device, VkCommandPool _commandPool, VkQueue _queue);
	~Buffer();
	uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
	void copyBuffer(VkBuffer srcBuffer, VkDeviceSize size);
	void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);
	void createDeviceLocalBuffer(const void *srcData, VkDeviceSize _size, VkBufferUsageFlags bufferUsageFlags);
	void beginSingleTimeCommands();
	void endSingleTimeCommands();
	VkBuffer getBuffer();
	VkCommandBuffer getCommandBuffer();
	VkDeviceMemory getBufferMemory();
};

class Image
{
protected:
	VkImage image;
	VkImageView imageView;
	VkDeviceMemory imageMemory;
	VkPhysicalDevice physicalDevice;
	VkDevice device;
	VkCommandPool commandPool;
	VkQueue queue;
	Buffer *commandBuffer;
public:
	Image(VkPhysicalDevice _physicalDevice, VkDevice _device, VkCommandPool _commandPool, VkQueue _queue);
	~Image();
	bool hasStencilComponent(VkFormat format);
	void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties);
	void createImageView(VkFormat format, VkImageAspectFlags aspectFlags);
	void transitionImageLayout(VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
	void copyBufferToImage(VkBuffer buffer, uint32_t width, uint32_t height);
	void setImage(VkImage _image);
	VkImageView getImageView();
};

class VulkanSetup
{
protected:
	// Attributs
	struct SwapChainSupportDetails {
		VkSurfaceCapabilitiesKHR capabilities;
		uint32_t formatCount;
		VkSurfaceFormatKHR *formats;
		uint32_t presentModeCount;
		VkPresentModeKHR *presentModes;
	};
	const char *appName;
	const char *engineName;
	Window *window;
	VkInstance instance;
	VkSurfaceKHR surface;
	VkPhysicalDevice physicalDevice;
	VkDevice device;
	VkQueue queue;
	VkSwapchainKHR swapChain;
	uint32_t swapChainImagesCount;
	Image **swapChainImages;
	Image *depthImage;
	VkFormat swapChainImageFormat;
	VkExtent2D swapChainExtent;
	VkRenderPass renderPass;
	VkDescriptorSetLayout descriptorSetLayout;
	VkCommandPool commandPool;
	VkFramebuffer *swapChainFramebuffers;
	VkSemaphore imageAvailableSemaphore;
	VkSemaphore renderFinishedSemaphore;
	// Methods
	int findQueueFamilies(VkPhysicalDevice device);
	SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
	bool checkDeviceExtensionSupport(VkPhysicalDevice device);
	bool isDeviceSuitable(VkPhysicalDevice device);
	VkSurfaceFormatKHR chooseSwapSurfaceFormat(uint32_t formatCount, VkSurfaceFormatKHR *availableFormats);
	VkPresentModeKHR chooseSwapPresentMode(uint32_t presentModeCount, VkPresentModeKHR *availablePresentModes);
	VkExtent2D chooseSwapExtent(VkSurfaceCapabilitiesKHR *capabilities);
	VkFormat findSupportedFormat(uint32_t candidatesCount, VkFormat *candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
	VkFormat findDepthFormat();
	void printSupportedApiVersion();
	void printLayers();
	void printExtensions();
	void printPhysicalDevices();
	void createInstance();
	void createSurface();
	void choosePhysicalDevice();
	void createLogicalDevice();
	void createCommandPool();
	void createSwapChain();
	void createSwapChainImages();
	void createRenderPass();
	void createDescriptorSetLayout();
	void createDepthImage();
	void createFramebuffers();
	void createSemaphores();
public:
	VulkanSetup(const char *_appNAme, const char *_engineName, Window *_window);
	~VulkanSetup();
	VkDevice getDevice();
	VkPhysicalDevice getPhysicalDevice();
	VkExtent2D getSwapChainExtent();
	VkDescriptorSetLayout *getDescriptorSetLayoutPtr();
	VkRenderPass getRenderPass();
	VkCommandPool getCommandPool();
	VkQueue getQueue();
	uint32_t getSwapChainImagesCount();
	VkFramebuffer *getSwapChainFramebuffers();
	VkSwapchainKHR getSwapChain();
	VkSemaphore getImageAvailableSemaphore();
	VkSemaphore getRenderFinishedSemaphore();
};

class Shader
{
protected:
	FILE *file;
	size_t size;
	uint32_t *code;
public:
	Shader();
	~Shader();
	void load(const char *fileName);
	size_t getSize();
	uint32_t *getCode();
};

class TextOverlay
{
protected:
	VulkanSetup *vulkanSetup;
	vec4 *charVertices;
public:
	TextOverlay(VulkanSetup *_vulkanSetup);
	~TextOverlay();
	int texWidth, texHeight;
	VkDeviceSize tetxtureSize;
	unsigned char *pixels;
	uint32_t numLetters;
	Buffer *vertexBuffer;
	void beginTextUpdate();
	void addText(std::string text, float x, float y);
	void endTextUpdate();
};

class Texture
{
protected:
	VulkanSetup *vulkanSetup;
	const char *filename;
	Image *textureImage;
	int texWidth, texHeight, texChannels;
	VkDeviceSize imageSize;
	unsigned char *pixels;
	VkFormat imageFormat;
	VkSampler textureSampler;
	void loadTexture();
	void createTextureImage();
	void createTextureImageView();
	void createTextureSampler();
public:
	Texture(VulkanSetup *_vulkanSetup, const char *_filename);
	Texture(VulkanSetup *_vulkanSetup, TextOverlay *_textOverlay);
	~Texture();
	VkImageView getTextureImageView();
	VkSampler getTextureSampler();
};

class RenderObject
{
protected:
	// Attributs
	VulkanSetup *vulkanSetup;
	VkDeviceSize uboBufferSize;
	Buffer *uniformBuffer;
	VkDescriptorPool descriptorPool;
	Shader vertexShader;
	Shader fragmentShader;
	Shader tessellationControlShader;
	Shader tessellationEvaluationShader;
	uint32_t stageCount;
	VkVertexInputBindingDescription bindingDescription;
	uint32_t attributeDescriptionCount;
	VkVertexInputAttributeDescription *pAttributeDescriptions;
	VkPrimitiveTopology topology;
	VkPipelineTessellationStateCreateInfo *pTessellationStateCreateInfo;
	uint32_t pushConstantRangeCount;
	VkPushConstantRange *pPushConstantRange;
	VkPipelineLayout pipelineLayout;
	VkPipeline graphicsPipeline;
	Texture *texture;
	TextOverlay *textOverlay;
	VkDescriptorSet descriptorSet;
	vec4 color;
	uint64_t vertexOffset;
	uint32_t indexCount;
	uint32_t firstIndex;
	clock_t startTime;
	bool *key;
	// Methods
	VkShaderModule createShaderModule(Shader shader);
	VkPipelineShaderStageCreateInfo getShaderStageInfo(VkShaderStageFlagBits stage, VkShaderModule module);
	VkVertexInputBindingDescription getBindingDescription(uint32_t stride);
	VkVertexInputAttributeDescription *getAttributeDescriptions(uint32_t count, VkFormat *formats, uint32_t *offsets);
	VkPipelineTessellationStateCreateInfo *getTessellationStateCreateInfo(uint32_t patchControlPoints);
	VkPushConstantRange *createPushConstantRange(VkShaderStageFlags shaderStageFlags, uint32_t size);
	void createUniformBuffer();
	void createPipelineLayout();
	void createGraphicsPipeline();
	void createDescriptorSet();
public:
	mat4 mModel;
	mat4 *mView;
	mat4 mProj;
	RenderObject(	VulkanSetup *_vulkanSetup,
					VkDescriptorPool _descriptorPool,
					TextOverlay *_textOverlay,
					mat4 *_mView,
					bool *_key);
	~RenderObject();
	virtual void updateUniformBuffer();
	virtual void motion();
	uint32_t getPushConstantRangeCount();
	VkPushConstantRange getPushConstantRange();
	VkPipelineLayout getPipelineLayout();
	VkPipeline getGraphicsPipeline();
	VkDescriptorSet *getDescriptorSetPtr();
	VkBuffer getTextOverlayVertexBuffer();
	uint32_t getNumLetters();
	uint64_t getVertexOffset();
	uint32_t getIndexCount();
	uint32_t getFirstIndex();
};

class RenderScene
{
protected:
	struct Camera {
		float xPos, yPos, zPos;
		float xAngle, yAngle, zAngle;
		mat4 M;
	} cam;
	bool *key;
	mat4 mView, mView2;
	VulkanSetup *vulkanSetup;
	uint32_t objectCount;
	RenderObject **obj;
	RenderObject *txtObj;
	VertexData *vertexData;
	IndexData *indexData;
	Buffer *vertexBuffer;
	Buffer *indexBuffer;
	TextOverlay *textOverlay;
	VkDescriptorPool descriptorPool;
	VkCommandBuffer *commandBuffers;
	void printMatrix(mat4 M, float x, float y);
	void createVertexBuffer();
	void createIndexBuffer();
	void createDescriptorPool();
	void createCommandBuffers();
public:
	RenderScene(VulkanSetup *_vulkanSetup, bool *_key);
	~RenderScene();
	void updateUniformBuffers();
	void camMotion();
	void updateTextOverlay(uint32_t fps);
	void drawFrame();
};