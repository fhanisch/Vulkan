#include <iostream>
#include "Texture.h"
// stb_image.h --> muss hier inkludiert werden statt in Header-Datei, da sonst doppelter Code
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
//#include "stb_font_consolas_24_latin1.inl"
#include "stb_font_arial_50_latin1.inl"
// Max. number of chars the text overlay buffer can hold
#define TEXTOVERLAY_MAX_CHAR_COUNT 2048
stb_fontchar stbFontData[STB_SOMEFONT_NUM_CHARS];

Texture::Texture(Vulkan* _vulkan, const char* _filename)
{
	vulkan = _vulkan;
	filename = _filename;
	textureImage = new Image(vulkan);
	loadTexture();
	createTextureImage();
	createTextureImageView();
	createTextureSampler();
}

Texture::Texture(Vulkan* _vulkan, TextOverlay* _textOverlay)
{
	vulkan = _vulkan;
	texWidth = _textOverlay->texWidth;
	texHeight = _textOverlay->texHeight;
	imageSize = _textOverlay->tetxtureSize;
	pixels = _textOverlay->pixels;
	imageFormat = VK_FORMAT_R8_UNORM;
	textureImage = new Image(vulkan);
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
	Buffer stagingBuffer = Buffer(vulkan);

	stagingBuffer.createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	void* data;
	vkMapMemory(vulkan->getDevice(), stagingBuffer.getBufferMemory(), 0, imageSize, 0, &data);
	memcpy(data, pixels, imageSize);
	vkUnmapMemory(vulkan->getDevice(), stagingBuffer.getBufferMemory());

	//stbi_image_free(pixels);

	textureImage->createImage(texWidth, texHeight, imageFormat, VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	textureImage->transitionImageLayout(imageFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	textureImage->copyBufferToImage(stagingBuffer.getBuffer(), texWidth, texHeight);
	textureImage->transitionImageLayout(imageFormat, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	vkDestroyBuffer(vulkan->getDevice(), stagingBuffer.getBuffer(), nullptr);
	vkFreeMemory(vulkan->getDevice(), stagingBuffer.getBufferMemory(), nullptr);
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

	if (vkCreateSampler(vulkan->getDevice(), &samplerInfo, nullptr, &textureSampler) != VK_SUCCESS) {
		std::cout << "Failed to create texture sampler!" << std::endl;
		exit(1);
	}
}

VkImageView Texture::getTextureImageView() { return textureImage->getImageView(); }

VkSampler Texture::getTextureSampler() { return textureSampler; }

TextOverlay::TextOverlay(Vulkan* _vulkan)
{
	vulkan = _vulkan;
	const uint32_t fontWidth = STB_SOMEFONT_BITMAP_WIDTH;
	const uint32_t fontHeight = STB_SOMEFONT_BITMAP_HEIGHT_POW2;

	texWidth = fontWidth;
	texHeight = fontHeight;
	tetxtureSize = fontWidth * fontHeight;

	unsigned char font24pixels[fontHeight][fontWidth];
	STB_SOMEFONT_CREATE(stbFontData, font24pixels, fontHeight);

	pixels = &font24pixels[0][0];

	// Vertex buffer
	VkDeviceSize bufferSize = TEXTOVERLAY_MAX_CHAR_COUNT * sizeof(vec4);
	vertexBuffer = new Buffer(vulkan);
	vertexBuffer->createBuffer(bufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
}

TextOverlay::~TextOverlay() {}

// Map buffer 
void TextOverlay::beginTextUpdate()
{
	vkMapMemory(vulkan->getDevice(), vertexBuffer->getBufferMemory(), 0, VK_WHOLE_SIZE, 0, (void**)& charVertices);
	numLetters = 0;
}

// Add text to the current buffer
void TextOverlay::addText(std::string text, float x, float y)
{
	const uint32_t firstChar = STB_SOMEFONT_FIRST_CHAR;

	const float charW = 5.0f / vulkan->getSwapChainExtent().width;
	const float charH = 5.0f / vulkan->getSwapChainExtent().height;

	float fbW = (float)vulkan->getSwapChainExtent().width;
	float fbH = (float)vulkan->getSwapChainExtent().height;
	x = (x / fbW * 2.0f) - 1.0f;
	y = (y / fbH * 2.0f) - 1.0f;

	// Calculate text width
	float textWidth = 0;
	for (auto letter : text)
	{
		stb_fontchar* charData = &stbFontData[(uint32_t)letter - firstChar];
		textWidth += charData->advance * charW;
	}

	// Generate a uv mapped quad per char in the new text
	for (auto letter : text)
	{
		stb_fontchar* charData = &stbFontData[(uint32_t)letter - firstChar];

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
	vkUnmapMemory(vulkan->getDevice(), vertexBuffer->getBufferMemory());
	charVertices = nullptr;
}