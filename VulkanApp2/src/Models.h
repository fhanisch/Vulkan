#pragma once

#include "Vulkan.h"

class Square : RenderObject
{
public:
	Square(VulkanSetup *_vulkanSetup, VkDescriptorPool _descriptorPool, TextOverlay *_textOverlay);
	~Square();
};