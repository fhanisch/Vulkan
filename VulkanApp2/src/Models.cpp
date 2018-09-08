#include "Models.h"

Square::Square(VulkanSetup *_vulkanSetup, VkDescriptorPool _descriptorPool, TextOverlay *_textOverlay)
	:RenderObject(_vulkanSetup, _descriptorPool, _textOverlay)
{
	vertexShader.load("C:/Home/Entwicklung/Vulkan/build/vs_default.spv");
	fragmentShader.load("C:/Home/Entwicklung/Vulkan/build/fs_test.spv");
	stageCount = 2;
}

Square::~Square() {}