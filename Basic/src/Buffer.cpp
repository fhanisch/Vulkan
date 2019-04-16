#include <iostream>
#include "Vulkan.h"
#include "Buffer.h"

Buffer::Buffer(Vulkan* _vulkan)
{
	vulkan = _vulkan;
}

Buffer::~Buffer() {}

uint32_t Buffer::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(vulkan->getPhysicalDevice(), &memProperties);

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

	if (vkCreateBuffer(vulkan->getDevice(), &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
		std::cout << "Failed to create vertex buffer!" << std::endl;
		exit(1);
	}

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(vulkan->getDevice(), buffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

	if (vkAllocateMemory(vulkan->getDevice(), &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
		std::cout << "Failed to allocate vertex buffer memory!" << std::endl;
		exit(1);
	}

	vkBindBufferMemory(vulkan->getDevice(), buffer, bufferMemory, 0);
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

void Buffer::createDeviceLocalBuffer(const void* srcData, VkDeviceSize _size, VkBufferUsageFlags bufferUsageFlags)
{
	void* data; // --> prüfen ob zu deleten
	Buffer stagingBuffer = Buffer(vulkan);
	stagingBuffer.createBuffer(_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	vkMapMemory(vulkan->getDevice(), stagingBuffer.getBufferMemory(), 0, _size, 0, &data);
	memcpy(data, srcData, _size);
	vkUnmapMemory(vulkan->getDevice(), stagingBuffer.getBufferMemory());
	createBuffer(_size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | bufferUsageFlags, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	copyBuffer(stagingBuffer.getBuffer(), _size);
	vkDestroyBuffer(vulkan->getDevice(), stagingBuffer.getBuffer(), nullptr);
	vkFreeMemory(vulkan->getDevice(), stagingBuffer.getBufferMemory(), nullptr);
}

void Buffer::beginSingleTimeCommands()
{
	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = vulkan->getCommandPool();
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = 1;

	vkAllocateCommandBuffers(vulkan->getDevice(), &allocInfo, &commandBuffer);

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

	vkQueueSubmit(vulkan->getQueue(), 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(vulkan->getQueue());

	vkFreeCommandBuffers(vulkan->getDevice(), vulkan->getCommandPool(), 1, &commandBuffer);
}

VkBuffer Buffer::getBuffer() { return buffer; }

VkCommandBuffer Buffer::getCommandBuffer() { return commandBuffer; }

VkDeviceMemory Buffer::getBufferMemory() { return bufferMemory; }