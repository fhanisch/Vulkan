#pragma once
#include <vulkan.h>

class Vulkan;

class Buffer
{
protected:
	Vulkan* vulkan;
	size_t size;
	VkBuffer buffer = VK_NULL_HANDLE;
	VkDeviceMemory bufferMemory;
	VkCommandBuffer commandBuffer;
public:
	Buffer(Vulkan* _vulkan);
	~Buffer();
	uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
	void copyBuffer(VkBuffer srcBuffer, VkDeviceSize size);
	void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);
	void createDeviceLocalBuffer(const void* srcData, VkDeviceSize _size, VkBufferUsageFlags bufferUsageFlags);
	void beginSingleTimeCommands();
	void endSingleTimeCommands();
	VkBuffer getBuffer();
	VkCommandBuffer getCommandBuffer();
	VkDeviceMemory getBufferMemory();
};