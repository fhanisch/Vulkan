#include "Vulkan.h"
#include "Models.h"
// stb_image.h --> muss hier inkludiert werden statt in Header-Datei, da sonst doppelter Code
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"
#include "stb_font_consolas_24_latin1.inl"

// Max. number of chars the text overlay buffer can hold
#define TEXTOVERLAY_MAX_CHAR_COUNT 2048
stb_fontchar stbFontData[STB_FONT_consolas_24_latin1_NUM_CHARS];

VertexData::VertexData()
{
	data = nullptr;
	size = 0;
	offsets = nullptr;
	dataSetCount = 0;
}

VertexData::~VertexData() {}

void VertexData::addData(float *_data, uint64_t _size)
{
	uint64_t *tmp1 = offsets;
	offsets = new uint64_t[dataSetCount + 1];
	if (tmp1) memcpy(offsets, tmp1, dataSetCount * sizeof(uint64_t));
	offsets[dataSetCount] = size;
	if (tmp1) delete[] tmp1;

	float *tmp2 = data;
	data = (float*)malloc(size + _size);
	if (tmp2) memcpy(data, tmp2, size);
	memcpy((char*)data + size, _data, _size);
	dataSetCount++;
	size += _size;
	if (tmp2) free(tmp2);
}

uint64_t VertexData::getOffset(uint32_t index)
{
	return offsets[index];
}

float *VertexData::getData()
{
	return data;
}

uint64_t VertexData::getSize()
{
	return size;
}

IndexData::IndexData()
{
	data = nullptr;
	size = 0;
	indexCount = nullptr;
	firstIndex = nullptr;
	dataSetCount = 0;
}

IndexData::~IndexData() {}

void IndexData::addData(uint16_t *_data, uint64_t _size)
{
	uint32_t *tmp1 = indexCount;
	indexCount = new uint32_t[dataSetCount + 1];
	if (tmp1) memcpy(indexCount, tmp1, dataSetCount * sizeof(uint32_t));
	indexCount[dataSetCount] = (uint32_t)_size / (uint32_t)sizeof(uint16_t);
	if (tmp1) delete[] tmp1;

	tmp1 = firstIndex;
	firstIndex = new uint32_t[dataSetCount + 1];
	if (tmp1) memcpy(firstIndex, tmp1, dataSetCount * sizeof(uint32_t));
	firstIndex[dataSetCount] = (uint32_t)size / (uint32_t)sizeof(uint16_t);
	if (tmp1) delete[] tmp1;

	uint16_t *tmp2 = data;
	data = (uint16_t*)malloc(size + _size);
	if (tmp2) memcpy(data, tmp2, size);
	memcpy((char*)data + size, _data, _size);
	dataSetCount++;
	size += _size;
	if (tmp2) free(tmp2);
}

uint16_t *IndexData::getData()
{
	return data;
}

uint64_t IndexData::getSize()
{
	return size;
}

uint32_t IndexData::getIndexCount(uint32_t index)
{
	return indexCount[index];
}

uint32_t IndexData::getFirstIndex(uint32_t index)
{
	return firstIndex[index];
}

Buffer::Buffer(VkPhysicalDevice _physicalDevice, VkDevice _device, VkCommandPool _commandPool, VkQueue _queue)
{
	physicalDevice = _physicalDevice;
	device = _device;
	commandPool = _commandPool;
	queue = _queue;
}

Buffer::~Buffer() {}

uint32_t Buffer::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
		if (typeFilter & (1 << i) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
			return i;
	}

	std::cout << "Failed to find suitable memory type!" << std::endl;
	exit(1);
}

void Buffer::createBuffer(VkDeviceSize _size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties)
{
	size = _size;
	VkBufferCreateInfo bufferInfo = {};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateBuffer(device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
		std::cout << "Failed to create vertex buffer!" << std::endl;
		exit(1);
	}

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(device, buffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

	if (vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
		std::cout << "Failed to allocate vertex buffer memory!" << std::endl;
		exit(1);
	}

	vkBindBufferMemory(device, buffer, bufferMemory, 0);
}

void Buffer::copyBuffer(VkBuffer srcBuffer, VkDeviceSize _size)
{

	beginSingleTimeCommands();

	VkBufferCopy copyRegion = {};
	copyRegion.srcOffset = 0;
	copyRegion.dstOffset = 0;
	copyRegion.size = _size;
	vkCmdCopyBuffer(commandBuffer, srcBuffer, buffer, 1, &copyRegion);

	endSingleTimeCommands();
}

void Buffer::createDeviceLocalBuffer(const void *srcData, VkDeviceSize _size, VkBufferUsageFlags bufferUsageFlags)
{
	void *data; // --> prüfen ob zu deleten
	Buffer stagingBuffer = Buffer(physicalDevice, device, commandPool, queue);
	stagingBuffer.createBuffer(_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	vkMapMemory(device, stagingBuffer.getBufferMemory(), 0, _size, 0, &data);
		memcpy(data, srcData, _size);
	vkUnmapMemory(device, stagingBuffer.getBufferMemory());
	createBuffer(_size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | bufferUsageFlags, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	copyBuffer(stagingBuffer.getBuffer(), _size);
	vkDestroyBuffer(device, stagingBuffer.getBuffer(), nullptr);
	vkFreeMemory(device, stagingBuffer.getBufferMemory(), nullptr);
}

void Buffer::beginSingleTimeCommands()
{
	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = 1;

	vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(commandBuffer, &beginInfo);
}

void Buffer::endSingleTimeCommands()
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

VkBuffer Buffer::getBuffer() { return buffer; }

VkCommandBuffer Buffer::getCommandBuffer() { return commandBuffer; }

VkDeviceMemory Buffer::getBufferMemory() { return bufferMemory; }

Image::Image(VkPhysicalDevice _physicalDevice, VkDevice _device, VkCommandPool _commandPool, VkQueue _queue)
{
	physicalDevice = _physicalDevice;
	device = _device;
	commandPool = _commandPool;
	queue = _queue;
	commandBuffer = new Buffer(physicalDevice, device, commandPool, queue);
}

Image::~Image() {}

bool Image::hasStencilComponent(VkFormat format)
{
	return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
}

void Image::createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties)
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

	if (vkCreateImage(device, &imageInfo, nullptr, &image) != VK_SUCCESS) {
		std::cout << "Failed to create image!" << std::endl;
		exit(1);
	}

	VkMemoryRequirements memRequirements;
	vkGetImageMemoryRequirements(device, image, &memRequirements);

	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = commandBuffer->findMemoryType(memRequirements.memoryTypeBits, properties);

	if (vkAllocateMemory(device, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS) {
		std::cout << "Failed to allocate image memory!" << std::endl;
		exit(1);
	}

	vkBindImageMemory(device, image, imageMemory, 0);
}

void Image::createImageView(VkFormat format, VkImageAspectFlags aspectFlags)
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
	createInfo.subresourceRange.aspectMask = aspectFlags;
	createInfo.subresourceRange.baseMipLevel = 0;
	createInfo.subresourceRange.levelCount = 1;
	createInfo.subresourceRange.baseArrayLayer = 0;
	createInfo.subresourceRange.layerCount = 1;

	if (vkCreateImageView(device, &createInfo, nullptr, &imageView) != VK_SUCCESS) {
		std::cout << "Failed to create image views!" << std::endl;
		exit(1);
	}
}

void Image::transitionImageLayout(VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout)
{
	commandBuffer->beginSingleTimeCommands();

	VkImageMemoryBarrier barrier = {};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.oldLayout = oldLayout;
	barrier.newLayout = newLayout;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = image;
	if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
		if (hasStencilComponent(format)) {
			barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
		}
	}
	else {
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	}
	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = 1;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1;

	VkPipelineStageFlags sourceStage = {};
	VkPipelineStageFlags destinationStage = {};

	if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	}
	else {
		std::cout << "Unsupported layout transition!" << std::endl;
	}

	vkCmdPipelineBarrier(commandBuffer->getCommandBuffer(), sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);

	commandBuffer->endSingleTimeCommands();
}

void Image::copyBufferToImage(VkBuffer buffer, uint32_t width, uint32_t height)
{
	commandBuffer->beginSingleTimeCommands();

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

	vkCmdCopyBufferToImage(commandBuffer->getCommandBuffer(), buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

	commandBuffer->endSingleTimeCommands();
}

void Image::setImage(VkImage _image) { image = _image; }

VkImageView Image::getImageView() { return imageView; }

VulkanSetup::VulkanSetup(const char *_appNAme, const char *_engineName, Window *_window)
{
	appName = _appNAme;
	engineName = _engineName;
	window = _window;

	printSupportedApiVersion();
	printLayers();
	printExtensions();
	createInstance();
	createSurface();
	printPhysicalDevices();
	choosePhysicalDevice();
	createLogicalDevice();
	createSwapChain();
	createSwapChainImages();
	createRenderPass();
	createDescriptorSetLayout();
	createCommandPool();
	createDepthImage();
	createFramebuffers();
	createSemaphores();
}

VulkanSetup::~VulkanSetup()
{
	// Clean Up!
	//vkDestroyInstance(instance, nullptr);
}

int VulkanSetup::findQueueFamilies(VkPhysicalDevice physDevice)
{
	int index = -1;
	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(physDevice, &queueFamilyCount, nullptr);
	VkQueueFamilyProperties *queueFamilies = new VkQueueFamilyProperties[queueFamilyCount];
	vkGetPhysicalDeviceQueueFamilyProperties(physDevice, &queueFamilyCount, queueFamilies);

	for (unsigned int i = 0; i < queueFamilyCount; i++) {
		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(physDevice, i, surface, &presentSupport);
		if (queueFamilies[i].queueCount > 0 && queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT && presentSupport) {
			index = i;
		}
		if (index >= 0) break;
	}
	delete[] queueFamilies;
	return index;
}

VulkanSetup::SwapChainSupportDetails VulkanSetup::querySwapChainSupport(VkPhysicalDevice physDevice)
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

bool VulkanSetup::checkDeviceExtensionSupport(VkPhysicalDevice physDevice)
{
	bool extensionsSupported = false;

	uint32_t extensionCount;
	vkEnumerateDeviceExtensionProperties(physDevice, nullptr, &extensionCount, nullptr);
	VkExtensionProperties *availableExtensions = new VkExtensionProperties[extensionCount];
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

bool VulkanSetup::isDeviceSuitable(VkPhysicalDevice physDevice)
{
	int index = findQueueFamilies(physDevice);
	bool extensionsSupported = checkDeviceExtensionSupport(physDevice);
	bool swapChainAdequate = false;

	if (extensionsSupported) {
		SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physDevice);
		swapChainAdequate = swapChainSupport.formatCount && swapChainSupport.presentModeCount;
	}

	VkSurfaceCapabilitiesKHR capabilities;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physDevice, surface, &capabilities);

	return index >= 0 && extensionsSupported && swapChainAdequate && (capabilities.maxImageCount>2);
}

VkSurfaceFormatKHR VulkanSetup::chooseSwapSurfaceFormat(uint32_t formatCount, VkSurfaceFormatKHR *availableFormats)
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

VkPresentModeKHR VulkanSetup::chooseSwapPresentMode(uint32_t presentModeCount, VkPresentModeKHR *availablePresentModes)
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

VkExtent2D VulkanSetup::chooseSwapExtent(VkSurfaceCapabilitiesKHR *capabilities)
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

VkFormat VulkanSetup::findSupportedFormat(uint32_t candidatesCount, VkFormat *candidates, VkImageTiling tiling, VkFormatFeatureFlags features)
{
	for (uint32_t i = 0; i < candidatesCount; i++) {
		VkFormatProperties properties;
		vkGetPhysicalDeviceFormatProperties(physicalDevice, candidates[i], &properties);
		if (tiling == VK_IMAGE_TILING_LINEAR && (properties.linearTilingFeatures & features) == features) {
			return candidates[i];
		}
		else if (tiling == VK_IMAGE_TILING_OPTIMAL && (properties.optimalTilingFeatures & features) == features) {
			return candidates[i];
		}
	}
	std::cout << "Failed to find supported format!" << std::endl;
	exit(1);
}

VkFormat VulkanSetup::findDepthFormat()
{
	VkFormat formats[] = { VK_FORMAT_D32_SFLOAT };
	return findSupportedFormat(1, formats, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
}

void VulkanSetup::printSupportedApiVersion()
{
	uint32_t apiVersion;
	vkEnumerateInstanceVersion(&apiVersion);
	std::cout << "Supported Api Version: " << VK_VERSION_MAJOR(apiVersion) << "." << VK_VERSION_MINOR(apiVersion) << "." << VK_VERSION_PATCH(apiVersion) << std::endl;
}

void VulkanSetup::printLayers()
{
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
	VkLayerProperties *availableLayers = new VkLayerProperties[layerCount];
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers);
	std::cout << "Available Intance Layers:" << std::endl;
	for (uint32_t i = 0; i < layerCount; i++) {
		std::cout << "\t#" << i << "\t" << availableLayers[i].layerName << std::endl;
	}
	delete[] availableLayers;
}

void VulkanSetup::printExtensions()
{
	uint32_t extensionCount;
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
	VkExtensionProperties *extensions = new VkExtensionProperties[extensionCount];
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions);
	std::cout << "Available Intance Extensions:" << std::endl;
	for (uint32_t i = 0; i < extensionCount; i++) {
		std::cout << "\t#" << i << "\t" << extensions[i].extensionName << std::endl;
	}
	delete[] extensions;
}

void VulkanSetup::printPhysicalDevices()
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
	VkPhysicalDevice *devices = new VkPhysicalDevice[deviceCount];
	vkEnumeratePhysicalDevices(instance, &deviceCount, devices);

	for (uint32_t i = 0; i < deviceCount; i++) {
		uint32_t extensionCount;
		vkEnumerateDeviceExtensionProperties(devices[i], nullptr, &extensionCount, nullptr);
		VkExtensionProperties *extensions = new VkExtensionProperties[extensionCount];
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
		VkQueueFamilyProperties *queueFamilies = new VkQueueFamilyProperties[queueFamilyCount];
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
		VkSurfaceFormatKHR *formats = new VkSurfaceFormatKHR[formatCount];
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
		VkPresentModeKHR *presentModes = new VkPresentModeKHR[presentModeCount];
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

void VulkanSetup::createInstance()
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
	const char *validationLayers[] = { "VK_LAYER_LUNARG_standard_validation" };
	const unsigned int globalExtensionCount = 2;
	const char *globalExtensions[] = { VK_KHR_SURFACE_EXTENSION_NAME, VK_KHR_WIN32_SURFACE_EXTENSION_NAME };
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

void VulkanSetup::createSurface()
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

void VulkanSetup::choosePhysicalDevice()
{
	uint32_t deviceCount;
	vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
	if (deviceCount == 0) {
		std::cout << "Failed to find GPUs with Vulkan support!" << std::endl;
		exit(1);
	}
	VkPhysicalDevice *devices = new VkPhysicalDevice[deviceCount];
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

void VulkanSetup::createLogicalDevice()
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
	deviceFeatures.tessellationShader = VK_TRUE;
	deviceFeatures.samplerAnisotropy = VK_TRUE;
	deviceFeatures.wideLines = VK_TRUE;

	const unsigned int deviceExtensionCount = 1;
	const char *deviceExtensions[] = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
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

void VulkanSetup::createCommandPool()
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

void VulkanSetup::createSwapChain()
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
	
	if (vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapChain) != VK_SUCCESS) {
		std::cout << "Failed to create swap chain!" << std::endl;
		exit(1);
	}
	swapChainImageFormat = surfaceFormat.format;
	swapChainExtent = extent;
}

void VulkanSetup::createSwapChainImages()
{
	vkGetSwapchainImagesKHR(device, swapChain, &swapChainImagesCount, nullptr);
	VkImage *tmp = new VkImage[swapChainImagesCount];
	swapChainImages = new Image*[swapChainImagesCount];
	vkGetSwapchainImagesKHR(device, swapChain, &swapChainImagesCount, tmp);

	for (uint32_t i = 0; i < swapChainImagesCount; i++) {
		swapChainImages[i] = new Image(physicalDevice, device, commandPool, queue);
		swapChainImages[i]->setImage(tmp[i]);
		swapChainImages[i]->createImageView(swapChainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT);
	}
	std::cout << swapChainImagesCount << " image views created." << std::endl;
	delete[] tmp;
}

void VulkanSetup::createRenderPass()
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

	VkAttachmentDescription depthAttachment = {};
	depthAttachment.format = VK_FORMAT_D32_SFLOAT;
	depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkAttachmentReference colorAttachmentRef = {};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkAttachmentReference depthAttachmentRef = {};
	depthAttachmentRef.attachment = 1;
	depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass = {};
	subpass.flags = 0;
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;
	subpass.pDepthStencilAttachment = &depthAttachmentRef;

	VkSubpassDependency dependency = {};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	VkAttachmentDescription attachments[] = { colorAttachment, depthAttachment };

	VkRenderPassCreateInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = 2;
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

void VulkanSetup::createDescriptorSetLayout()
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

	VkDescriptorSetLayoutBinding uboMaterialBinding = {};
	uboMaterialBinding.binding = 2;
	uboMaterialBinding.descriptorCount = 1;
	uboMaterialBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uboMaterialBinding.stageFlags = VK_SHADER_STAGE_ALL_GRAPHICS;
	uboMaterialBinding.pImmutableSamplers = nullptr;

	VkDescriptorSetLayoutBinding bindings[] = { uboLayoutBinding, samplerLayoutBinding, uboMaterialBinding };

	VkDescriptorSetLayoutCreateInfo layoutInfo = {};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = 3;
	layoutInfo.pBindings = bindings;

	if (vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS) {
		std::cout << "Failed to create descriptor set layout!" << std::endl;
		exit(1);
	}
}

void VulkanSetup::createDepthImage()
{
	VkFormat depthFormat = findDepthFormat();
	depthImage = new Image(physicalDevice, device, commandPool, queue);
	depthImage->createImage(swapChainExtent.width, swapChainExtent.height, depthFormat, VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	depthImage->createImageView(depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);
	depthImage->transitionImageLayout(depthFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
}

void VulkanSetup::createFramebuffers()
{
	swapChainFramebuffers = new VkFramebuffer[swapChainImagesCount];

	for (uint32_t i = 0; i < swapChainImagesCount; i++) {
		VkImageView attachments[] = { swapChainImages[i]->getImageView(), depthImage->getImageView() };
		VkFramebufferCreateInfo framebufferInfo = {};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = renderPass;
		framebufferInfo.attachmentCount = 2;
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

void VulkanSetup::createSemaphores()
{
	VkSemaphoreCreateInfo semaphoreInfo = {};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &imageAvailableSemaphore) != VK_SUCCESS ||
		vkCreateSemaphore(device, &semaphoreInfo, nullptr, &renderFinishedSemaphore) != VK_SUCCESS) {

		std::cout << "failed to create semaphores!" << std::endl;
		exit(1);
	}
}

VkDevice VulkanSetup::getDevice() { return device; }

VkPhysicalDevice VulkanSetup::getPhysicalDevice() { return physicalDevice; }

VkExtent2D VulkanSetup::getSwapChainExtent() { return swapChainExtent; }

VkDescriptorSetLayout *VulkanSetup::getDescriptorSetLayoutPtr() { return &descriptorSetLayout; }

VkRenderPass VulkanSetup::getRenderPass() { return renderPass; }

VkCommandPool VulkanSetup::getCommandPool() { return commandPool; }

VkQueue VulkanSetup::getQueue() { return queue; }

uint32_t VulkanSetup::getSwapChainImagesCount() { return swapChainImagesCount; }

VkFramebuffer *VulkanSetup::getSwapChainFramebuffers() { return swapChainFramebuffers; }

VkSwapchainKHR VulkanSetup::getSwapChain() { return swapChain; }

VkSemaphore VulkanSetup::getImageAvailableSemaphore() { return imageAvailableSemaphore; }

VkSemaphore VulkanSetup::getRenderFinishedSemaphore() { return renderFinishedSemaphore; }

Shader::Shader() {}

Shader::~Shader() {}

void Shader::load(const char *fileName)
{
	file = fopen(fileName, "rb");
	if (file == NULL) {
		std::cout << "Could not open " << fileName << "!" << std::endl;
		exit(1);
	}
	fseek(file, 0, SEEK_END);
	size = ftell(file);
	rewind(file);
	code = (uint32_t*)malloc(size);
	fread(code, size, 1, file);
	fclose(file);
}

size_t Shader::getSize() { return size; }
uint32_t *Shader::getCode() { return code; }

Texture::Texture(VulkanSetup *_vulkanSetup, const char *_filename)
{
	vulkanSetup = _vulkanSetup;
	filename = _filename;
	textureImage = new Image(vulkanSetup->getPhysicalDevice(), vulkanSetup->getDevice(), vulkanSetup->getCommandPool(), vulkanSetup->getQueue());
	loadTexture();
	createTextureImage();
	createTextureImageView();
	createTextureSampler();
}

Texture::Texture(VulkanSetup *_vulkanSetup, TextOverlay *_textOverlay)
{
	vulkanSetup = _vulkanSetup;
	texWidth = _textOverlay->texWidth;
	texHeight = _textOverlay->texHeight;
	imageSize = _textOverlay->tetxtureSize;
	pixels = _textOverlay->pixels;
	imageFormat = VK_FORMAT_R8_UNORM;
	textureImage = new Image(vulkanSetup->getPhysicalDevice(), vulkanSetup->getDevice(), vulkanSetup->getCommandPool(), vulkanSetup->getQueue());
	createTextureImage();
	createTextureImageView();
	createTextureSampler();
}

Texture::~Texture() {}

void Texture::loadTexture()
{
	pixels = stbi_load(filename, &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
	imageSize = texWidth * texHeight * 4;
	imageFormat = VK_FORMAT_R8G8B8A8_UNORM;
	if (!pixels) {
		std::cout << "Failed to load texture image!" << std::endl;
		exit(1);
	}
}

void Texture::createTextureImage()
{
	Buffer stagingBuffer = Buffer(vulkanSetup->getPhysicalDevice(), vulkanSetup->getDevice(), vulkanSetup->getCommandPool(), vulkanSetup->getQueue());

	stagingBuffer.createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	void* data;
	vkMapMemory(vulkanSetup->getDevice(), stagingBuffer.getBufferMemory(), 0, imageSize, 0, &data);
	memcpy(data, pixels, imageSize);
	vkUnmapMemory(vulkanSetup->getDevice(), stagingBuffer.getBufferMemory());

	//stbi_image_free(pixels);

	textureImage->createImage(texWidth, texHeight, imageFormat, VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	textureImage->transitionImageLayout(imageFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	textureImage->copyBufferToImage(stagingBuffer.getBuffer(), texWidth, texHeight);
	textureImage->transitionImageLayout(imageFormat, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	vkDestroyBuffer(vulkanSetup->getDevice(), stagingBuffer.getBuffer(), nullptr);
	vkFreeMemory(vulkanSetup->getDevice(), stagingBuffer.getBufferMemory(), nullptr);
}

void Texture::createTextureImageView()
{
	textureImage->createImageView(imageFormat, VK_IMAGE_ASPECT_COLOR_BIT);
}

void Texture::createTextureSampler()
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

	if (vkCreateSampler(vulkanSetup->getDevice(), &samplerInfo, nullptr, &textureSampler) != VK_SUCCESS) {
		std::cout << "Failed to create texture sampler!" << std::endl;
		exit(1);
	}
}

VkImageView Texture::getTextureImageView() { return textureImage->getImageView(); }

VkSampler Texture::getTextureSampler() { return textureSampler; }

ObjectModel::ObjectModel(const char *_filename, VertexData *_vertexData, IndexData *_indexData)
{
	filename = _filename;
	vertexData = _vertexData;
	indexData = _indexData;
	loadModel();
}

ObjectModel::~ObjectModel() {}

void ObjectModel::loadModel()
{
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string err;
	Vertex *vertices;
	uint16_t *indices;

	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &err, filename))
	{
		std::cout << "Failed to load object model!" << std::endl;
		exit(1);
	}
	vertices = new Vertex[shapes[0].mesh.indices.size()];
	indices = new uint16_t[shapes[0].mesh.indices.size()];
	for (uint16_t i = 0; i < shapes[0].mesh.indices.size(); i++)
	{
		vertices[i].pos[0] = attrib.vertices[3 * shapes[0].mesh.indices[i].vertex_index + 0];
		vertices[i].pos[1] = attrib.vertices[3 * shapes[0].mesh.indices[i].vertex_index + 1];
		vertices[i].pos[2] = attrib.vertices[3 * shapes[0].mesh.indices[i].vertex_index + 2];
		vertices[i].texCoords[0] = attrib.texcoords[2 * shapes[0].mesh.indices[i].texcoord_index + 0];
		vertices[i].texCoords[1] = attrib.texcoords[2 * shapes[0].mesh.indices[i].texcoord_index + 1];
		vertices[i].color[0] = 1.0; vertices[i].color[1] = 0.0; vertices[i].color[2] = 0.0;
		indices[i] = i;
	}
	/*
			Todo: reduce multiple vertices
	*/
	vertexData->addData((float*)vertices, shapes[0].mesh.indices.size() *sizeof(Vertex));
	indexData->addData(indices, shapes[0].mesh.indices.size() * sizeof(uint16_t));
	delete[] vertices;
	delete[] indices;
}

RenderObject::RenderObject(	VulkanSetup *_vulkanSetup,
							VkDescriptorPool _descriptorPool,
							TextOverlay *_textOverlay,
							mat4 *_mView,
							bool *_key)
{
	vulkanSetup = _vulkanSetup;
	descriptorPool = _descriptorPool;
	textOverlay = _textOverlay;
	mView = _mView;
	key = _key;
	startTime = clock();
}

RenderObject::~RenderObject() {}

void RenderObject::updateUniformBuffer()
{
	void* data;
	vkMapMemory(vulkanSetup->getDevice(), uniformBuffer->getBufferMemory(), 0, uboBufferSize, 0, &data);
		memcpy((char*)data, &mModel, sizeof(mat4));
		memcpy((char*)data + sizeof(mat4), mView, sizeof(mat4));
		memcpy((char*)data + 2 * sizeof(mat4), &mProj, sizeof(mat4));
		memcpy((char*)data + 0x100, &color, sizeof(color));
	vkUnmapMemory(vulkanSetup->getDevice(), uniformBuffer->getBufferMemory());
}

void RenderObject::motion() {}

VkShaderModule RenderObject::createShaderModule(Shader shader)
{
	VkShaderModuleCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.pNext = nullptr;
	createInfo.flags = 0;
	createInfo.codeSize = shader.getSize();
	createInfo.pCode = shader.getCode(); // ! alignment Anforderungen von uint32_t beachten !

	VkShaderModule shaderModule;
	if (vkCreateShaderModule(vulkanSetup->getDevice(), &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
		std::cout << "Failed to create shader module!" << std::endl;
		exit(1);
	}

	return shaderModule;
}

VkPipelineShaderStageCreateInfo RenderObject::getShaderStageInfo(VkShaderStageFlagBits stage, VkShaderModule module)
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

VkVertexInputBindingDescription RenderObject::getBindingDescription(uint32_t stride)
{
	VkVertexInputBindingDescription bindingDscr = {};
	bindingDscr.binding = 0;
	bindingDscr.stride = stride;
	bindingDscr.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	return bindingDscr;
}

VkVertexInputAttributeDescription *RenderObject::getAttributeDescriptions(uint32_t count, VkFormat *formats, uint32_t *offsets)
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

VkPipelineTessellationStateCreateInfo *RenderObject::getTessellationStateCreateInfo(uint32_t patchControlPoints)
{
	VkPipelineTessellationStateCreateInfo *createInfo = new VkPipelineTessellationStateCreateInfo;
	*createInfo = {};
	createInfo->sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
	createInfo->patchControlPoints = patchControlPoints;

	return createInfo;
}

VkPushConstantRange *RenderObject::createPushConstantRange(VkShaderStageFlags shaderStageFlags, uint32_t size)
{
	VkPushConstantRange *pushConstantRange = new VkPushConstantRange;

	pushConstantRange->stageFlags = shaderStageFlags;
	pushConstantRange->offset = 0;
	pushConstantRange->size = size;

	return pushConstantRange;
}

void RenderObject::createUniformBuffer()
{
	uniformBuffer = new Buffer(vulkanSetup->getPhysicalDevice(), vulkanSetup->getDevice(), vulkanSetup->getCommandPool(), vulkanSetup->getQueue());
	uniformBuffer->createBuffer(uboBufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
}

void RenderObject::createPipelineLayout()
{
	VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.pNext = nullptr;
	pipelineLayoutInfo.flags = 0;
	pipelineLayoutInfo.setLayoutCount = 1;
	pipelineLayoutInfo.pSetLayouts = vulkanSetup->getDescriptorSetLayoutPtr();
	pipelineLayoutInfo.pushConstantRangeCount = pushConstantRangeCount;
	pipelineLayoutInfo.pPushConstantRanges = pPushConstantRange;

	if (vkCreatePipelineLayout(vulkanSetup->getDevice(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
		std::cout << "Failed to create pipeline layout!" << std::endl;
		exit(1);
	}
}

void RenderObject::createGraphicsPipeline()
{
	VkShaderModule vertexShaderModule = createShaderModule(vertexShader);
	VkShaderModule fragmentShaderModule = createShaderModule(fragmentShader);
	VkPipelineShaderStageCreateInfo vertexShaderStageInfo = getShaderStageInfo(VK_SHADER_STAGE_VERTEX_BIT, vertexShaderModule);
	VkPipelineShaderStageCreateInfo fragmentShaderStageInfo = getShaderStageInfo(VK_SHADER_STAGE_FRAGMENT_BIT, fragmentShaderModule);
	VkPipelineShaderStageCreateInfo *shaderStages;

	if (stageCount > 2)
	{
		VkShaderModule tessellationControlShaderModule = createShaderModule(tessellationControlShader);
		VkShaderModule tessellationEvaluationShaderModule = createShaderModule(tessellationEvaluationShader);
		VkPipelineShaderStageCreateInfo tessellationControlShaderStageInfo = getShaderStageInfo(VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT, tessellationControlShaderModule);
		VkPipelineShaderStageCreateInfo tessellationEvaluationShaderStageInfo = getShaderStageInfo(VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT, tessellationEvaluationShaderModule);
		shaderStages = new VkPipelineShaderStageCreateInfo[4];
		shaderStages[0] = vertexShaderStageInfo;
		shaderStages[1] = tessellationControlShaderStageInfo;
		shaderStages[2] = tessellationEvaluationShaderStageInfo;
		shaderStages[3] = fragmentShaderStageInfo;
	}
	else
	{
		shaderStages = new VkPipelineShaderStageCreateInfo[2];
		shaderStages[0] = vertexShaderStageInfo;
		shaderStages[1] = fragmentShaderStageInfo;
	}

	VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.pNext = nullptr;
	vertexInputInfo.flags = 0;
	vertexInputInfo.vertexBindingDescriptionCount = 1;
	vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
	vertexInputInfo.vertexAttributeDescriptionCount = attributeDescriptionCount;
	vertexInputInfo.pVertexAttributeDescriptions = pAttributeDescriptions;

	VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.pNext = nullptr;
	inputAssembly.flags = 0;
	inputAssembly.topology = topology;
	inputAssembly.primitiveRestartEnable = VK_FALSE;

	VkViewport viewport = {};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float)vulkanSetup->getSwapChainExtent().width;
	viewport.height = (float)vulkanSetup->getSwapChainExtent().height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkRect2D scissor = {};
	scissor.offset = { 0, 0 };
	scissor.extent = vulkanSetup->getSwapChainExtent();

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
	rasterizer.lineWidth = 2.0f; // --> Feature muss extra aktiviert werden
	rasterizer.cullMode = VK_CULL_MODE_NONE; // Front and Back werden gerendert
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

	VkPipelineDepthStencilStateCreateInfo depthStencil = {};
	depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthStencil.depthTestEnable = VK_TRUE;
	depthStencil.depthWriteEnable = VK_TRUE;
	depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
	depthStencil.depthBoundsTestEnable = VK_FALSE;
	depthStencil.minDepthBounds = 0.0f; // Optional
	depthStencil.maxDepthBounds = 1.0f; // Optional
	depthStencil.stencilTestEnable = VK_FALSE;
	depthStencil.front = {}; // Optional
	depthStencil.back = {}; // Optional

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

	VkGraphicsPipelineCreateInfo pipelineInfo = {};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = stageCount;
	pipelineInfo.pStages = shaderStages;
	pipelineInfo.pVertexInputState = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &inputAssembly;
	pipelineInfo.pTessellationState = pTessellationStateCreateInfo;
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &rasterizer;
	pipelineInfo.pMultisampleState = &multisampling;
	pipelineInfo.pDepthStencilState = &depthStencil;
	pipelineInfo.pColorBlendState = &colorBlending;
	pipelineInfo.pDynamicState = nullptr; // Optional
	pipelineInfo.layout = pipelineLayout;
	pipelineInfo.renderPass = vulkanSetup->getRenderPass();
	pipelineInfo.subpass = 0;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
	pipelineInfo.basePipelineIndex = -1; // Optional

	if (vkCreateGraphicsPipelines(vulkanSetup->getDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline) != VK_SUCCESS) {
		std::cout << "failed to create graphics pipeline!" << std::endl;
		exit(1);
	}

	std::cout << "Graphics Pipeline created." << std::endl;
}

void RenderObject::createDescriptorSet()
{
	VkDescriptorSetAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = descriptorPool;
	allocInfo.descriptorSetCount = 1;
	allocInfo.pSetLayouts = vulkanSetup->getDescriptorSetLayoutPtr();

	if (vkAllocateDescriptorSets(vulkanSetup->getDevice(), &allocInfo, &descriptorSet) != VK_SUCCESS) {
		std::cout << "Failed to allocate descriptor set!" << std::endl;
		exit(1);
	}

	VkDescriptorBufferInfo bufferInfo = {};
	bufferInfo.buffer = uniformBuffer->getBuffer();
	bufferInfo.offset = 0;
	bufferInfo.range = 3 * sizeof(mat4);

	VkDescriptorBufferInfo materialBufferInfo = {};
	materialBufferInfo.buffer = uniformBuffer->getBuffer();
	materialBufferInfo.offset = 0x100;
	materialBufferInfo.range = sizeof(color);

	VkDescriptorImageInfo imageInfo = {};
	imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	imageInfo.imageView = texture->getTextureImageView();
	imageInfo.sampler = texture->getTextureSampler();

	VkWriteDescriptorSet descriptorWrites[3] = {};
	descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrites[0].dstSet = descriptorSet;
	descriptorWrites[0].dstBinding = 0;
	descriptorWrites[0].dstArrayElement = 0;
	descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	descriptorWrites[0].descriptorCount = 1;
	descriptorWrites[0].pBufferInfo = &bufferInfo;
	descriptorWrites[0].pImageInfo = nullptr; // Optional
	descriptorWrites[0].pTexelBufferView = nullptr; // Optional

	descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrites[1].dstSet = descriptorSet;
	descriptorWrites[1].dstBinding = 1;
	descriptorWrites[1].dstArrayElement = 0;
	descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	descriptorWrites[1].descriptorCount = 1;
	descriptorWrites[1].pImageInfo = &imageInfo;

	descriptorWrites[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrites[2].dstSet = descriptorSet;
	descriptorWrites[2].dstBinding = 2;
	descriptorWrites[2].dstArrayElement = 0;
	descriptorWrites[2].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	descriptorWrites[2].descriptorCount = 1;
	descriptorWrites[2].pBufferInfo = &materialBufferInfo;
	descriptorWrites[2].pImageInfo = nullptr; // Optional
	descriptorWrites[2].pTexelBufferView = nullptr; // Optional

	vkUpdateDescriptorSets(vulkanSetup->getDevice(), 3, descriptorWrites, 0, nullptr);
}

uint32_t RenderObject::getPushConstantRangeCount() { return pushConstantRangeCount; }
VkPushConstantRange RenderObject::getPushConstantRange() { return *pPushConstantRange; }
VkPipelineLayout RenderObject::getPipelineLayout() { return pipelineLayout; }
VkPipeline RenderObject::getGraphicsPipeline() { return graphicsPipeline; }
VkDescriptorSet *RenderObject::getDescriptorSetPtr() { return &descriptorSet; }
VkBuffer RenderObject::getTextOverlayVertexBuffer() { return textOverlay->vertexBuffer->getBuffer(); }
uint32_t RenderObject::getNumLetters() { return textOverlay->numLetters; }
uint64_t RenderObject::getVertexOffset() { return vertexOffset; }
uint32_t RenderObject::getIndexCount() { return indexCount; }
uint32_t RenderObject::getFirstIndex() { return firstIndex; }

TextOverlay::TextOverlay(VulkanSetup *_vulkanSetup)
{
	vulkanSetup = _vulkanSetup;
	const uint32_t fontWidth = STB_FONT_consolas_24_latin1_BITMAP_WIDTH;
	const uint32_t fontHeight = STB_FONT_consolas_24_latin1_BITMAP_WIDTH;

	texWidth = fontWidth;
	texHeight = fontHeight;
	tetxtureSize = fontWidth * fontHeight;

	unsigned char font24pixels[fontWidth][fontHeight];
	stb_font_consolas_24_latin1(stbFontData, font24pixels, fontHeight);

	pixels = &font24pixels[0][0];

	// Vertex buffer
	VkDeviceSize bufferSize = TEXTOVERLAY_MAX_CHAR_COUNT * sizeof(vec4);
	vertexBuffer = new Buffer(vulkanSetup->getPhysicalDevice(), vulkanSetup->getDevice(), vulkanSetup->getCommandPool(), vulkanSetup->getQueue());
	vertexBuffer->createBuffer(bufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
}

TextOverlay::~TextOverlay() {}

// Map buffer 
void TextOverlay::beginTextUpdate()
{
	vkMapMemory(vulkanSetup->getDevice(), vertexBuffer->getBufferMemory(), 0, VK_WHOLE_SIZE, 0, (void**)&charVertices);
	numLetters = 0;
}

// Add text to the current buffer
void TextOverlay::addText(std::string text, float x, float y)
{
	const uint32_t firstChar = STB_FONT_consolas_24_latin1_FIRST_CHAR;

	const float charW = 3.0f / vulkanSetup->getSwapChainExtent().width;
	const float charH = 3.0f / vulkanSetup->getSwapChainExtent().height;

	float fbW = (float)vulkanSetup->getSwapChainExtent().width;
	float fbH = (float)vulkanSetup->getSwapChainExtent().height;
	x = (x / fbW * 2.0f) - 1.0f;
	y = (y / fbH * 2.0f) - 1.0f;

	// Calculate text width
	float textWidth = 0;
	for (auto letter : text)
	{
		stb_fontchar *charData = &stbFontData[(uint32_t)letter - firstChar];
		textWidth += charData->advance * charW;
	}

	// Generate a uv mapped quad per char in the new text
	for (auto letter : text)
	{
		stb_fontchar *charData = &stbFontData[(uint32_t)letter - firstChar];

		(*charVertices)[0] = (x + (float)charData->x0 * charW);
		(*charVertices)[1] = (y + (float)charData->y0 * charH);
		(*charVertices)[2] = charData->s0;
		(*charVertices)[3] = charData->t0;
		charVertices++;

		(*charVertices)[0] = (x + (float)charData->x1 * charW);
		(*charVertices)[1] = (y + (float)charData->y0 * charH);
		(*charVertices)[2] = charData->s1;
		(*charVertices)[3] = charData->t0;
		charVertices++;

		(*charVertices)[0] = (x + (float)charData->x0 * charW);
		(*charVertices)[1] = (y + (float)charData->y1 * charH);
		(*charVertices)[2] = charData->s0;
		(*charVertices)[3] = charData->t1;
		charVertices++;

		(*charVertices)[0] = (x + (float)charData->x1 * charW);
		(*charVertices)[1] = (y + (float)charData->y1 * charH);
		(*charVertices)[2] = charData->s1;
		(*charVertices)[3] = charData->t1;
		charVertices++;

		x += charData->advance * charW;

		numLetters++;
	}
}

// Unmap buffer and update command buffers
void TextOverlay::endTextUpdate()
{
	vkUnmapMemory(vulkanSetup->getDevice(), vertexBuffer->getBufferMemory());
	charVertices = nullptr;
}

RenderScene::RenderScene(VulkanSetup *_vulkanSetup, bool *_key)
{
	vulkanSetup = _vulkanSetup;
	key = _key;
	identity4(mView);
	identity4(mView2);
	identity4(cam.M);
	//getTrans4(cam.M, 0.0f, 101.0f, 0.0f);
	cam.yPos = 101.0f;
	cam.zPos = 0.0f;
	vertexData = new VertexData;
	indexData = new IndexData;
	// Vertex Data
	vertexData->addData((float*)verticesPlane, sizeof(verticesPlane));
	vertexData->addData((float*)verticesStar, sizeof(verticesStar));
	vecf(&verticesCurve, &verticesCurveSize, 0.0f, 0.001f, 1001);
	vertexData->addData(verticesCurve, verticesCurveSize);
	vertexData->addData((float*)verticesPatches, sizeof(verticesPatches));
	uint16_t patchesCount = 101;
	vecf(&verticesPatches2, &verticesPatches2Size, 0.0f, 1.0f, patchesCount);
	vertexData->addData(verticesPatches2, verticesPatches2Size);
	createMeshGrid(&meshGridVertices, &meshGridVerticesSize, 101, 101);
	vertexData->addData(meshGridVertices, meshGridVerticesSize);
	// Index Data
	indexData->addData((uint16_t*)indicesPlane, sizeof(indicesPlane));
	indexData->addData((uint16_t*)indicesStar, sizeof(indicesStar));
	vecs(&indicesCurve, &indicesCurveSize, 0, 1001);
	indexData->addData(indicesCurve, indicesCurveSize);
	indexData->addData((uint16_t*)indicesPatches, sizeof(indicesPatches));
	indicesPatches2Size = 2 * (patchesCount - 1) * sizeof(uint16_t);
	indicesPatches2 = new uint16_t[2 * (patchesCount - 1)];
	for (uint16_t i = 0; i < (patchesCount - 1); i++)
	{
		indicesPatches2[2 * i] = i;
		indicesPatches2[2 * i + 1] = i + 1;
	}
	indexData->addData(indicesPatches2, indicesPatches2Size);
	createMeshGridIndices(&meshGridIndices, &meshGridIndicesSize, 101, 101, 0);
	indexData->addData(meshGridIndices, meshGridIndicesSize);
	// Load Object Models from file
	ObjectModel cube("C:/Home/Entwicklung/Vulkan/objects/cube.obj", vertexData, indexData);
	// Objects
	objectCount = 14;
	obj = new RenderObject*[objectCount];
	createDescriptorPool();
	obj[0] = new Square(vulkanSetup, descriptorPool, nullptr, &mView2, key, vertexData, indexData);
	obj[1] = new Tacho(vulkanSetup, descriptorPool, nullptr, &mView2, key, vertexData, indexData);
	obj[2] = new FlatPerlin2d(vulkanSetup, descriptorPool, nullptr, &mView2, key, vertexData, indexData);
	obj[3] = new Star(vulkanSetup, descriptorPool, nullptr, &mView2, key, vertexData, indexData);
	obj[4] = new FilledCircle(vulkanSetup, descriptorPool, nullptr, &mView2, key, vertexData, indexData);
	obj[5] = new PerlinCircle(vulkanSetup, descriptorPool, nullptr, &mView2, key, vertexData, indexData);
	obj[6] = new Wave(vulkanSetup, descriptorPool, nullptr, &mView2, key, vertexData, indexData);
	obj[7] = new Perlin1d(vulkanSetup, descriptorPool, nullptr, &mView2, key, vertexData, indexData);
	obj[8] = new CurveTessellator(vulkanSetup, descriptorPool, nullptr, &mView2, key, vertexData, indexData);
	obj[9] = new Perlin1dTessellator(vulkanSetup, descriptorPool, nullptr, &mView2, key, vertexData, indexData);
	obj[10] = new Plane(vulkanSetup, descriptorPool, nullptr, &mView, key, vertexData, indexData);
	obj[11] = new Planet(vulkanSetup, descriptorPool, nullptr, &mView, key, vertexData, indexData);
	obj[12] = new Sphere(vulkanSetup, descriptorPool, nullptr, &mView, key, vertexData, indexData);
	obj[13] = new Cube(vulkanSetup, descriptorPool, nullptr, &mView, key, vertexData, indexData);
	textOverlay = new TextOverlay(vulkanSetup);
	txtObj = new TxtObj(vulkanSetup, descriptorPool, textOverlay, &mView2, key, vertexData, indexData);
	char str[32];
	textOverlay->beginTextUpdate();
	sprintf(str, "FPS: %-4u", 0);
	textOverlay->addText(str, 5.0f, 5.0f);
	printMatrix(cam.M, 5.0, 35.0);
	textOverlay->endTextUpdate();
	createVertexBuffer();
	createIndexBuffer();
	createCommandBuffers();
}

RenderScene::~RenderScene() {}

void RenderScene::printMatrix(mat4 M, float x, float y)
{
	char str[32];
	float xOffset;
	for (unsigned int i = 0; i < 4; i++)
	{
		xOffset = x;
		for (unsigned int j = 0; j < 4; j++)
		{
			sprintf(str, "%6.2f", M[j][i]);
			textOverlay->addText(str, xOffset, y);
			xOffset += 125.0;
		}
		y += 30.0;
	}
}

void RenderScene::createVertexBuffer()
{
	vertexBuffer = new Buffer(vulkanSetup->getPhysicalDevice(), vulkanSetup->getDevice(), vulkanSetup->getCommandPool(), vulkanSetup->getQueue());
	vertexBuffer->createDeviceLocalBuffer(vertexData->getData(), vertexData->getSize(), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
}

void RenderScene::createIndexBuffer()
{
	indexBuffer = new Buffer(vulkanSetup->getPhysicalDevice(), vulkanSetup->getDevice(), vulkanSetup->getCommandPool(), vulkanSetup->getQueue());
	indexBuffer->createDeviceLocalBuffer(indexData->getData(), indexData->getSize(), VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
}

void RenderScene::createDescriptorPool()
{
	VkDescriptorPoolSize poolSize[2] = {};
	poolSize[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSize[0].descriptorCount = (objectCount+1)*2;
	poolSize[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	poolSize[1].descriptorCount = objectCount+1;

	VkDescriptorPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = 2;
	poolInfo.pPoolSizes = poolSize;
	poolInfo.maxSets = objectCount+1;

	if (vkCreateDescriptorPool(vulkanSetup->getDevice(), &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
		std::cout << "Failed to create descriptor pool!" << std::endl;
		exit(1);
	}
}

void RenderScene::createCommandBuffers()
{
	commandBuffers = new VkCommandBuffer[vulkanSetup->getSwapChainImagesCount()];

	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = vulkanSetup->getCommandPool();
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = vulkanSetup->getSwapChainImagesCount();

	if (vkAllocateCommandBuffers(vulkanSetup->getDevice(), &allocInfo, commandBuffers) != VK_SUCCESS) {
		std::cout << "failed to allocate command buffers!" << std::endl;
		exit(1);
	}

	for (uint32_t i = 0; i < vulkanSetup->getSwapChainImagesCount(); i++) {
		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
		beginInfo.pInheritanceInfo = nullptr; // Optional

		vkBeginCommandBuffer(commandBuffers[i], &beginInfo);

			VkRenderPassBeginInfo renderPassInfo = {};
			renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			renderPassInfo.renderPass = vulkanSetup->getRenderPass();
			renderPassInfo.framebuffer = vulkanSetup->getSwapChainFramebuffers()[i];
			renderPassInfo.renderArea.offset = { 0, 0 };
			renderPassInfo.renderArea.extent = vulkanSetup->getSwapChainExtent();
			VkClearValue clearValues[2] = {};
			clearValues[0].color = { {0.0f, 0.0f, 0.0f, 1.0f} };
			clearValues[1].depthStencil = { 1.0f, 0 };
			renderPassInfo.clearValueCount = 2;
			renderPassInfo.pClearValues = clearValues;

			vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
			{
				VkBuffer vB[] = { vertexBuffer->getBuffer() };
				vkCmdBindIndexBuffer(commandBuffers[i], indexBuffer->getBuffer(), 0, VK_INDEX_TYPE_UINT16);

				for (uint32_t j = 0; j < objectCount; j++)
				{
					VkDeviceSize offsets[] = { obj[j]->getVertexOffset() };
					vkCmdBindVertexBuffers(commandBuffers[i], 0, 1, vB, offsets);
					vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, obj[j]->getGraphicsPipeline());
					vkCmdBindDescriptorSets(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, obj[j]->getPipelineLayout(), 0, 1, obj[j]->getDescriptorSetPtr(), 0, nullptr);
					vkCmdDrawIndexed(commandBuffers[i], obj[j]->getIndexCount(), 1, obj[j]->getFirstIndex(), 0, 0);
				}

				vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, txtObj->getGraphicsPipeline());
				vkCmdBindDescriptorSets(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, txtObj->getPipelineLayout(), 0, 1, txtObj->getDescriptorSetPtr(), 0, nullptr);
				VkDeviceSize offsets1 = 0;
				VkBuffer vB1[] = { txtObj->getTextOverlayVertexBuffer() };
				vkCmdBindVertexBuffers(commandBuffers[i], 0, 1, vB1, &offsets1);
				for (uint32_t j = 0; j < txtObj->getNumLetters(); j++)
				{
					vkCmdDraw(commandBuffers[i], 4, 1, j * 4, 0);
				}
			}
			vkCmdEndRenderPass(commandBuffers[i]);

		if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS) {
			std::cout << "failed to record command buffer!" <<  std::endl;
			exit(1);
		}
	}
}

void RenderScene::updateUniformBuffers()
{
	for (uint32_t i = 0; i < objectCount; i++)
	{
		obj[i]->updateUniformBuffer();
	}
}

void RenderScene::camMotion()
{
	for (uint32_t i = 0; i < objectCount; i++) obj[i]->motion();

	//2d cam motion
	mat4 A, /*B,*/ T, dT, R, Rx, Ry, /*Rz,*/ tmp;

	if (key[0x4a] == true)
	{
		dup4(tmp, mView2);
		getTrans4(T, 0.1f, 0.0f, 0.0f);
		mult4(mView2, T, tmp);
	}

	if (key[0x4c] == true)
	{
		dup4(tmp, mView2);
		getTrans4(T, -0.1f, 0.0f, 0.0f);
		mult4(mView2, T, tmp);
	}

	if (key[0x4b] == true)
	{
		dup4(tmp, mView2);
		getTrans4(T, 0.0f, -0.1f, 0.0f);
		mult4(mView2, T, tmp);
	}

	if (key[0x49] == true)
	{
		dup4(tmp, mView2);
		getTrans4(T, 0.0f, 0.1f, 0.0f);
		mult4(mView2, T, tmp);
	}

	//3d cam motion
	
	float dx = 0.0f, dy = 0.0f, dz = 0.0f, dphi = 0.0f, dtheta = 0.0f, /*dpsi = 0.0f,*/ v = 0.2f, w = 0.05f;
	if (key[0x57]) dz = -v;
	if (key[0x53]) dz =  v;
	if (key[0x41]) dx = -v;
	if (key[0x44]) dx =  v;
	if (key[0x58]) dy = -w;
	if (key[0x59]) dy =  w;
	if (key[VK_LEFT])	dphi =  w;
	if (key[VK_RIGHT])	dphi = -w;
	if (key[VK_UP])		dtheta = -w;
	if (key[VK_DOWN])	dtheta =  w;

	// translatorisch
	getRotX4(Rx, cam.xAngle += dtheta);
	getRotY4(Ry, cam.yAngle += dphi);
	mult4(R, Ry, Rx);
	getTrans4(T, cam.xPos, cam.yPos, cam.zPos);
	getTrans4(dT, dx, dy, dz);
	mult4(A, T, R);
	mult4(cam.M, A, dT);
	cam.xPos = cam.M[3][0]; cam.yPos = cam.M[3][1]; cam.zPos = cam.M[3][2];
	invert4(mView, cam.M); // --> Todo: invertieren durch teilw. transponieren
	//

	/* rotatorisch
	getRotY4(Ry, cam.yAngle += dphi);
	getRotX4(Rx, cam.xAngle += cos(cam.yAngle)*dz - sin(cam.yAngle)*dx);
	getRotZ4(Rz, cam.zAngle += -sin(cam.yAngle)*dz - cos(cam.yAngle)*dx);
	getTrans4(T, cam.xPos, cam.yPos+=dy, cam.zPos);
	mult4(R, Rz, Rx);
	mult4(A, R, T);
	mult4(cam.M, A, Ry);
	invert4(mView, cam.M); // --> Todo: invertieren durch teilw. transponieren
	*/
}

void RenderScene::updateTextOverlay(uint32_t fps)
{
	char str[32];
	textOverlay->beginTextUpdate();
	sprintf(str, "FPS: %-4u", fps);
	textOverlay->addText(str, 5.0f, 5.0f);
	printMatrix(cam.M, 5.0, 35.0);
	textOverlay->endTextUpdate();
}

void RenderScene::drawFrame()
{
	uint32_t imageIndex;
	vkAcquireNextImageKHR(vulkanSetup->getDevice(), vulkanSetup->getSwapChain(), std::numeric_limits<uint64_t>::max(), vulkanSetup->getImageAvailableSemaphore(), VK_NULL_HANDLE, &imageIndex);

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	VkSemaphore waitSemaphores[] = { vulkanSetup->getImageAvailableSemaphore() };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffers[imageIndex];
	VkSemaphore signalSemaphores[] = { vulkanSetup->getRenderFinishedSemaphore() };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	if (vkQueueSubmit(vulkanSetup->getQueue(), 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS) {
		std::cout << "failed to submit draw command buffer!" << std::endl;
		exit(1);
	}

	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;
	VkSwapchainKHR swapChains[] = { vulkanSetup->getSwapChain() };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &imageIndex;

	vkQueuePresentKHR(vulkanSetup->getQueue(), &presentInfo);

	vkQueueWaitIdle(vulkanSetup->getQueue()); // Synchronisation der App mit der GPU (nicht notwendig)
}