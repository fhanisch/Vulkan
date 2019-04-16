#pragma once
#include <matrix.h>
#include "Vulkan.h"

class TextOverlay
{
protected:
	Vulkan* vulkan;
	vec4* charVertices;
public:
	TextOverlay(Vulkan* _vulkan);
	~TextOverlay();
	int texWidth, texHeight;
	VkDeviceSize tetxtureSize;
	unsigned char* pixels;
	uint32_t numLetters;
	Buffer* vertexBuffer;
	void beginTextUpdate();
	void addText(std::string text, float x, float y);
	void endTextUpdate();
};

class Texture
{
protected:
	Vulkan* vulkan;
	const char* filename;
	Image* textureImage;
	int texWidth, texHeight, texChannels;
	VkDeviceSize imageSize;
	unsigned char* pixels;
	VkFormat imageFormat;
	VkSampler textureSampler;
	void loadTexture();
	void createTextureImage();
	void createTextureImageView();
	void createTextureSampler();
public:
	Texture(Vulkan* _vulkan, const char* _filename);
	Texture(Vulkan* _vulkan, TextOverlay* _textOverlay);
	~Texture();
	VkImageView getTextureImageView();
	VkSampler getTextureSampler();
};