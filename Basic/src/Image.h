#pragma once
#include <vulkan.h>
#include "Buffer.h"

class Image
{
protected:
	Vulkan* vulkan;
	VkImage image;
	VkImageView imageView;
	VkDeviceMemory imageMemory;
	Buffer* commandBuffer;
public:
	Image(Vulkan* _vulkan);
	~Image();
	bool hasStencilComponent(VkFormat format);
	void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties);
	void createImageView(VkFormat format, VkImageAspectFlags aspectFlags);
	void transitionImageLayout(VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
	void copyBufferToImage(VkBuffer buffer, uint32_t width, uint32_t height);
	void setImage(VkImage _image);
	VkImageView getImageView();
};