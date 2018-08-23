#pragma once

#define _CRT_SECURE_NO_WARNINGS
#define VK_USE_PLATFORM_WIN32_KHR

#include <vulkan/vulkan.h>
#include "Window.h"
#include <iostream>
#include <algorithm>
#include <matrix.h>
#include "stb_font_consolas_24_latin1.inl"

#undef min
#undef max

// Max. number of chars the text overlay buffer can hold
#define TEXTOVERLAY_MAX_CHAR_COUNT 2048

struct Vertex {
	vec3 pos;
	vec3 color;
	vec2 texCoords;
};

struct VertexData {
	float *data;
	uint32_t size;
	uint32_t offset;
};

struct IndexData {
	uint16_t *data;
	uint32_t size;
	uint32_t offset;
};

const Vertex vertices[] = {
	{ { -1.0f,  1.0f,  0.0f }, { 1.0f, 0.0f, 1.0f }, { 0.0f, 0.0f } },
	{ {  1.0f,  1.0f,  0.0f }, { 1.0f, 0.0f, 1.0f }, { 1.0f, 0.0f } },
	{ {  1.0f, -1.0f,  0.0f }, { 1.0f, 0.0f, 1.0f }, { 1.0f, 1.0f } },
	{ { -1.0f, -1.0f,  0.0f }, { 1.0f, 0.0f, 1.0f }, { 0.0f, 1.0f } }
};

const uint16_t indices[] = { 0,1,2,2,3,0 };

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
	stb_fontchar stbFontData[STB_FONT_consolas_24_latin1_NUM_CHARS];
public:
	TextOverlay(VulkanSetup *_vulkanSetup);
	~TextOverlay();
	uint32_t numLetters;
	Buffer *vertexBuffer;
	Image *fontImage;
	VkSampler fontTextureSampler;
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
	VkSampler textureSampler;
	void loadTexture();
	void loadFontTexture();
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
	uint32_t stageCount;
	VkVertexInputBindingDescription bindingDescription;
	uint32_t attributeDescriptionCount;
	VkVertexInputAttributeDescription *pAttributeDescriptions;
	VkPrimitiveTopology topology;
	VkPipelineTessellationStateCreateInfo *pTessellationStateCreateInfo;
	uint32_t pushConstantRangeCount;
	VkPushConstantRange *pushConstantRange;
	VkPipelineLayout pipelineLayout;
	VkPipeline graphicsPipeline;
	//Texture *texture;
	VkDescriptorSet descriptorSet;
	vec4 color;
	// Methods
	VkShaderModule createShaderModule(Shader shader);
	VkPipelineShaderStageCreateInfo getShaderStageInfo(VkShaderStageFlagBits stage, VkShaderModule module);
	VkVertexInputBindingDescription getBindingDescription(uint32_t stride);
	VkVertexInputAttributeDescription *getAttributeDescriptions(uint32_t count, VkFormat *formats, uint32_t *offsets);
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
					const char *vertexShaderFileName,
					const char *fragmentShaderFileName,
					const char *textureFileName,
					TextOverlay *_textOverly,
					VkPrimitiveTopology _topology,
					mat4 *_mView);
	~RenderObject();
	void updateUniformBuffer();
	uint32_t getPushConstantRangeCount();
	VkPipelineLayout getPipelineLayout();
	VkPipeline getGraphicsPipeline();
	VkDescriptorSet *getDescriptorSetPtr();

	//TMP
	TextOverlay *textOverlay;
};

class RenderScene
{
protected:
	bool *key;
	mat4 cam;
	VulkanSetup *vulkanSetup;
	uint32_t objectCount;
	RenderObject **obj;
	VertexData *vertexData;
	IndexData *indexData;
	Buffer *vertexBuffer;
	Buffer *indexBuffer;
	TextOverlay *textOverlay;
	VkDescriptorPool descriptorPool;
	VkCommandBuffer *commandBuffers;
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