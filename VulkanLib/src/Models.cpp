#include "Models.h"
#include <string.h>
#include <time.h>

Square::Square(	VulkanSetup *_vulkanSetup,
				VkDescriptorPool _descriptorPool,
				TextOverlay *_textOverlay,
				mat4 *_mView,
				bool *_key,
				VertexData *vertexData,
				IndexData *indexData)
				:RenderObject(_vulkanSetup, _descriptorPool, _textOverlay, _mView, _key)
{
	vertexShader.load("C:/Home/Entwicklung/Vulkan/build/VulkanApp2/vs_default.spv");
	fragmentShader.load("C:/Home/Entwicklung/Vulkan/build/VulkanApp2/fs_test.spv");
	vertexOffset = vertexData->getOffset(0);
	indexCount = indexData->getIndexCount(0);
	firstIndex = indexData->getFirstIndex(0);
	stageCount = 2;
	attributeDescriptionCount = 3;
	VkFormat formats[] = { VK_FORMAT_R32G32B32_SFLOAT, VK_FORMAT_R32G32B32_SFLOAT, VK_FORMAT_R32G32_SFLOAT };
	uint32_t offsets[] = { offsetof(Vertex, pos), offsetof(Vertex, color), offsetof(Vertex, texCoords) };
	pAttributeDescriptions = getAttributeDescriptions(attributeDescriptionCount, formats, offsets);
	topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	bindingDescription = getBindingDescription(sizeof(Vertex));
	pTessellationStateCreateInfo = nullptr;
	pushConstantRangeCount = 0;
	pPushConstantRange = nullptr;
	uboBufferSize = 0x200;
	identity4(mModel);
	identity4(mProj);
	mProj[0][0] = (float)vulkanSetup->getSwapChainExtent().height / (float)vulkanSetup->getSwapChainExtent().width;
	color[0] = 0.0f; color[1] = 1.0f; color[2] = 0.0f; color[3] = 1.0f;
	texture = new Texture(vulkanSetup, "C:/Home/Entwicklung/Vulkan/textures/texture.jpg");
	getTrans4(mModel, 0.0f, 0.0f, 0.5f);
	createUniformBuffer();
	createPipelineLayout();
	createGraphicsPipeline();
	createDescriptorSet();
}

Square::~Square() {}

void Square::motion()
{
	mat4 Rz, tmp;
	float dphi = 0.0f;

	if (key[0x52] == true) {
		dphi = -0.05f;
	}
	if (key[0x54] == true) {
		dphi = 0.05f;
	}
	getRotZ4(Rz, dphi);
	dup4(tmp, mModel);
	mult4(mModel, Rz, tmp);
}

Tacho::Tacho(	VulkanSetup *_vulkanSetup,
				VkDescriptorPool _descriptorPool,
				TextOverlay *_textOverlay,
				mat4 *_mView,
				bool *_key,
				VertexData *vertexData,
				IndexData *indexData)
				: RenderObject(_vulkanSetup, _descriptorPool, _textOverlay, _mView, _key)
{
	vertexShader.load("C:/Home/Entwicklung/Vulkan/build/VulkanApp2/vs_default.spv");
	fragmentShader.load("C:/Home/Entwicklung/Vulkan/build/VulkanApp2/fs_powermeter.spv");
	vertexOffset = vertexData->getOffset(0);
	indexCount = indexData->getIndexCount(0);
	firstIndex = indexData->getFirstIndex(0);
	stageCount = 2;
	attributeDescriptionCount = 3;
	VkFormat formats[] = { VK_FORMAT_R32G32B32_SFLOAT, VK_FORMAT_R32G32B32_SFLOAT, VK_FORMAT_R32G32_SFLOAT };
	uint32_t offsets[] = { offsetof(Vertex, pos), offsetof(Vertex, color), offsetof(Vertex, texCoords) };
	pAttributeDescriptions = getAttributeDescriptions(attributeDescriptionCount, formats, offsets);
	topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	bindingDescription = getBindingDescription(sizeof(Vertex));
	pTessellationStateCreateInfo = nullptr;
	pushConstantRangeCount = 0;
	pPushConstantRange = nullptr;
	uboBufferSize = 0x200;
	identity4(mModel);
	identity4(mProj);
	mProj[0][0] = (float)vulkanSetup->getSwapChainExtent().height / (float)vulkanSetup->getSwapChainExtent().width;
	color[0] = 0.0f; color[1] = 1.0f; color[2] = 0.0f; color[3] = 1.0f;
	texture = new Texture(vulkanSetup, "C:/Home/Entwicklung/Vulkan/textures/texture.jpg");
	getTrans4(mModel, 0.0f, -5.0f, 0.5f);
	createUniformBuffer();
	createPipelineLayout();
	createGraphicsPipeline();
	createDescriptorSet();
}

Tacho::~Tacho() {}

FlatPerlin2d::FlatPerlin2d(	VulkanSetup *_vulkanSetup,
							VkDescriptorPool _descriptorPool,
							TextOverlay *_textOverlay,
							mat4 *_mView,
							bool *_key,
							VertexData *vertexData,
							IndexData *indexData)
							: RenderObject(_vulkanSetup, _descriptorPool, _textOverlay, _mView, _key)
{
	vertexShader.load("C:/Home/Entwicklung/Vulkan/build/VulkanApp2/vs_default.spv");
	fragmentShader.load("C:/Home/Entwicklung/Vulkan/build/VulkanApp2/fs_perlin2d.spv");
	vertexOffset = vertexData->getOffset(0);
	indexCount = indexData->getIndexCount(0);
	firstIndex = indexData->getFirstIndex(0);
	stageCount = 2;
	attributeDescriptionCount = 3;
	VkFormat formats[] = { VK_FORMAT_R32G32B32_SFLOAT, VK_FORMAT_R32G32B32_SFLOAT, VK_FORMAT_R32G32_SFLOAT };
	uint32_t offsets[] = { offsetof(Vertex, pos), offsetof(Vertex, color), offsetof(Vertex, texCoords) };
	pAttributeDescriptions = getAttributeDescriptions(attributeDescriptionCount, formats, offsets);
	topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	bindingDescription = getBindingDescription(sizeof(Vertex));
	pTessellationStateCreateInfo = nullptr;
	pushConstantRangeCount = 0;
	pPushConstantRange = nullptr;
	uboBufferSize = 0x200;
	identity4(mModel);
	identity4(mProj);
	mProj[0][0] = (float)vulkanSetup->getSwapChainExtent().height / (float)vulkanSetup->getSwapChainExtent().width;
	color[0] = 0.0f; color[1] = 1.0f; color[2] = 0.0f; color[3] = 1.0f;
	texture = new Texture(vulkanSetup, "C:/Home/Entwicklung/Vulkan/textures/texture.jpg");
	getTrans4(mModel, -5.0f, -5.0f, 0.5f);
	createUniformBuffer();
	createPipelineLayout();
	createGraphicsPipeline();
	createDescriptorSet();
}

FlatPerlin2d::~FlatPerlin2d() {}

Star::Star(	VulkanSetup *_vulkanSetup,
			VkDescriptorPool _descriptorPool,
			TextOverlay *_textOverlay,
			mat4 *_mView,
			bool *_key,
			VertexData *vertexData,
			IndexData *indexData)
			: RenderObject(_vulkanSetup, _descriptorPool, _textOverlay, _mView, _key)
{
	vertexShader.load("C:/Home/Entwicklung/Vulkan/build/VulkanApp2/vs_default.spv");
	fragmentShader.load("C:/Home/Entwicklung/Vulkan/build/VulkanApp2/fs_default.spv");
	vertexOffset = vertexData->getOffset(1);
	indexCount = indexData->getIndexCount(1);
	firstIndex = indexData->getFirstIndex(1);
	stageCount = 2;
	attributeDescriptionCount = 3;
	VkFormat formats[] = { VK_FORMAT_R32G32B32_SFLOAT, VK_FORMAT_R32G32B32_SFLOAT, VK_FORMAT_R32G32_SFLOAT };
	uint32_t offsets[] = { offsetof(Vertex, pos), offsetof(Vertex, color), offsetof(Vertex, texCoords) };
	pAttributeDescriptions = getAttributeDescriptions(attributeDescriptionCount, formats, offsets);
	topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	bindingDescription = getBindingDescription(sizeof(Vertex));
	pTessellationStateCreateInfo = nullptr;
	pushConstantRangeCount = 0;
	pPushConstantRange = nullptr;
	uboBufferSize = 0x200;
	identity4(mModel);
	identity4(mProj);
	mProj[0][0] = (float)vulkanSetup->getSwapChainExtent().height / (float)vulkanSetup->getSwapChainExtent().width;
	color[0] = 0.0f; color[1] = 1.0f; color[2] = 0.0f; color[3] = 1.0f;
	texture = new Texture(vulkanSetup, "C:/Home/Entwicklung/Vulkan/textures/texture.jpg");
	getTrans4(mModel, 5.0f, 5.0f, 0.2f);
	createUniformBuffer();
	createPipelineLayout();
	createGraphicsPipeline();
	createDescriptorSet();
}

Star::~Star() {}

void Star::motion()
{
	mat4 Rz, Rz2, tmp, tmp2;

	getRotZ4(Rz, 0.005f);
	getRotZ4(Rz2, 0.04f);
	dup4(tmp, mModel);
	mult4(tmp2, tmp, Rz2);
	mult4(mModel, Rz, tmp2);
}

FilledCircle::FilledCircle(	VulkanSetup *_vulkanSetup,
							VkDescriptorPool _descriptorPool,
							TextOverlay *_textOverlay,
							mat4 *_mView,
							bool *_key,
							VertexData *vertexData,
							IndexData *indexData)
							: RenderObject(_vulkanSetup, _descriptorPool, _textOverlay, _mView, _key)
{
	vertexShader.load("C:/Home/Entwicklung/Vulkan/build/VulkanApp2/vs_default.spv");
	fragmentShader.load("C:/Home/Entwicklung/Vulkan/build/VulkanApp2/fs_circleFilled.spv");
	vertexOffset = vertexData->getOffset(0);
	indexCount = indexData->getIndexCount(0);
	firstIndex = indexData->getFirstIndex(0);
	stageCount = 2;
	attributeDescriptionCount = 3;
	VkFormat formats[] = { VK_FORMAT_R32G32B32_SFLOAT, VK_FORMAT_R32G32B32_SFLOAT, VK_FORMAT_R32G32_SFLOAT };
	uint32_t offsets[] = { offsetof(Vertex, pos), offsetof(Vertex, color), offsetof(Vertex, texCoords) };
	pAttributeDescriptions = getAttributeDescriptions(attributeDescriptionCount, formats, offsets);
	topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	bindingDescription = getBindingDescription(sizeof(Vertex));
	pTessellationStateCreateInfo = nullptr;
	pushConstantRangeCount = 0;
	pPushConstantRange = nullptr;
	uboBufferSize = 0x200;
	identity4(mModel);
	identity4(mProj);
	mProj[0][0] = (float)vulkanSetup->getSwapChainExtent().height / (float)vulkanSetup->getSwapChainExtent().width;
	color[0] = 0.0f; color[1] = 1.0f; color[2] = 0.0f; color[3] = 1.0f;
	texture = new Texture(vulkanSetup, "C:/Home/Entwicklung/Vulkan/textures/texture.jpg");
	getTrans4(mModel, 5.0f, 5.0f, 0.1f);
	createUniformBuffer();
	createPipelineLayout();
	createGraphicsPipeline();
	createDescriptorSet();
}

FilledCircle::~FilledCircle() {}

void FilledCircle::motion()
{
	mat4 T, tmp;
	if (key[0x56] == true)
	{
		dup4(tmp, mModel);
		getTrans4(T, -0.1f, 0.0f, 0.0f);
		mult4(mModel, T, tmp);
	}

	if (key[0x4e] == true)
	{
		dup4(tmp, mModel);
		getTrans4(T, 0.1f, 0.0f, 0.0f);
		mult4(mModel, T, tmp);
	}

	if (key[0x47] == true)
	{
		dup4(tmp, mModel);
		getTrans4(T, 0.0f, -0.1f, 0.0f);
		mult4(mModel, T, tmp);
	}

	if (key[0x42] == true)
	{
		dup4(tmp, mModel);
		getTrans4(T, 0.0f, 0.1f, 0.0f);
		mult4(mModel, T, tmp);
	}
}

PerlinCircle::PerlinCircle(	VulkanSetup *_vulkanSetup,
							VkDescriptorPool _descriptorPool,
							TextOverlay *_textOverlay,
							mat4 *_mView,
							bool *_key,
							VertexData *vertexData,
							IndexData *indexData)
							: RenderObject(_vulkanSetup, _descriptorPool, _textOverlay, _mView, _key)
{
	vertexShader.load("C:/Home/Entwicklung/Vulkan/build/VulkanApp2/vs_perlinCircle.spv");
	fragmentShader.load("C:/Home/Entwicklung/Vulkan/build/VulkanApp2/fs_default.spv");
	vertexOffset = vertexData->getOffset(2);
	indexCount = indexData->getIndexCount(2);
	firstIndex = indexData->getFirstIndex(2);
	stageCount = 2;
	attributeDescriptionCount = 1;
	VkFormat formats[] = { VK_FORMAT_R32_SFLOAT };
	uint32_t offsets[] = { 0 };
	pAttributeDescriptions = getAttributeDescriptions(attributeDescriptionCount, formats, offsets);
	topology = VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
	bindingDescription = getBindingDescription(sizeof(float));
	pTessellationStateCreateInfo = nullptr;
	pushConstantRangeCount = 1;
	pPushConstantRange = createPushConstantRange(VK_SHADER_STAGE_VERTEX_BIT, sizeof(PushConstants));
	pushConsts.seed_u = 158;
	pushConsts.seed_v = 2767;
	uboBufferSize = 0x200;
	identity4(mModel);
	identity4(mProj);
	mProj[0][0] = (float)vulkanSetup->getSwapChainExtent().height / (float)vulkanSetup->getSwapChainExtent().width;
	color[0] = 0.0f; color[1] = 1.0f; color[2] = 0.0f; color[3] = 1.0f;
	texture = new Texture(vulkanSetup, "C:/Home/Entwicklung/Vulkan/textures/texture.jpg");
	getScale4(mModel, 1.5f, 1.5f, 1.0f);
	commandBuffer = new Buffer(vulkanSetup->getPhysicalDevice(), vulkanSetup->getDevice(), vulkanSetup->getCommandPool(), vulkanSetup->getQueue());
	createUniformBuffer();
	createPipelineLayout();
	createGraphicsPipeline();
	createDescriptorSet();
}

PerlinCircle::~PerlinCircle() {}

void PerlinCircle::motion()
{
	if (key)
	{
#ifndef ANDROID
		if (key[VK_SPACE] == true)
#endif
		{
			pushConsts.seed_u = (float)(clock() - startTime) / CLOCKS_PER_SEC;
			pushConsts.seed_v = (float)(clock() - startTime) / CLOCKS_PER_SEC + 245.0f;
			
			commandBuffer->beginSingleTimeCommands();

			vkCmdPushConstants(commandBuffer->getCommandBuffer(), pipelineLayout, pPushConstantRange->stageFlags, pPushConstantRange->offset, pPushConstantRange->size, &pushConsts);

			commandBuffer->endSingleTimeCommands();
		}
	}
}

Wave::Wave(	VulkanSetup *_vulkanSetup,
			VkDescriptorPool _descriptorPool,
			TextOverlay *_textOverlay,
			mat4 *_mView,
			bool *_key,
			VertexData *vertexData,
			IndexData *indexData)
			: RenderObject(_vulkanSetup, _descriptorPool, _textOverlay, _mView, _key)
{
	mat4 A, B;
	vertexShader.load("C:/Home/Entwicklung/Vulkan/build/VulkanApp2/vs_welle.spv");
	fragmentShader.load("C:/Home/Entwicklung/Vulkan/build/VulkanApp2/fs_default.spv");
	vertexOffset = vertexData->getOffset(2);
	indexCount = indexData->getIndexCount(2);
	firstIndex = indexData->getFirstIndex(2);
	stageCount = 2;
	attributeDescriptionCount = 1;
	VkFormat formats[] = { VK_FORMAT_R32_SFLOAT };
	uint32_t offsets[] = { 0 };
	pAttributeDescriptions = getAttributeDescriptions(attributeDescriptionCount, formats, offsets);
	topology = VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
	bindingDescription = getBindingDescription(sizeof(float));
	pTessellationStateCreateInfo = nullptr;
	pushConstantRangeCount = 0;
	pPushConstantRange = nullptr;
	uboBufferSize = 0x200;
	identity4(mModel);
	identity4(mProj);
	mProj[0][0] = (float)vulkanSetup->getSwapChainExtent().height / (float)vulkanSetup->getSwapChainExtent().width;
	color[0] = 0.0f; color[1] = 1.0f; color[2] = 0.0f; color[3] = 1.0f;
	texture = new Texture(vulkanSetup, "C:/Home/Entwicklung/Vulkan/textures/texture.jpg");
	getScale4(A, 2.0f, 1.0f, 1.0f);
	getTrans4(B, 0.0f, 2.0f, 0.0f);
	mult4(mModel, B, A);
	createUniformBuffer();
	createPipelineLayout();
	createGraphicsPipeline();
	createDescriptorSet();
}

Wave::~Wave() {}

void Wave::updateUniformBuffer()
{
	void* data;
	float time = (float)(clock() - startTime) / CLOCKS_PER_SEC;
	vkMapMemory(vulkanSetup->getDevice(), uniformBuffer->getBufferMemory(), 0, uboBufferSize, 0, &data);
		memcpy((char*)data, &mModel, sizeof(mat4));
		memcpy((char*)data + sizeof(mat4), mView, sizeof(mat4));
		memcpy((char*)data + 2 * sizeof(mat4), &mProj, sizeof(mat4));
		memcpy((char*)data + 0x100, &time, sizeof(float));
	vkUnmapMemory(vulkanSetup->getDevice(), uniformBuffer->getBufferMemory());
}

Perlin1d::Perlin1d(	VulkanSetup *_vulkanSetup,
					VkDescriptorPool _descriptorPool,
					TextOverlay *_textOverlay,
					mat4 *_mView,
					bool *_key,
					VertexData *vertexData,
					IndexData *indexData)
					: RenderObject(_vulkanSetup, _descriptorPool, _textOverlay, _mView, _key)
{
	mat4 A, B;
	vertexShader.load("C:/Home/Entwicklung/Vulkan/build/VulkanApp2/vs_perlin1d.spv");
	fragmentShader.load("C:/Home/Entwicklung/Vulkan/build/VulkanApp2/fs_default.spv");
	vertexOffset = vertexData->getOffset(2);
	indexCount = indexData->getIndexCount(2);
	firstIndex = indexData->getFirstIndex(2);
	stageCount = 2;
	attributeDescriptionCount = 1;
	VkFormat formats[] = { VK_FORMAT_R32_SFLOAT };
	uint32_t offsets[] = { 0 };
	pAttributeDescriptions = getAttributeDescriptions(attributeDescriptionCount, formats, offsets);
	topology = VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
	bindingDescription = getBindingDescription(sizeof(float));
	pTessellationStateCreateInfo = nullptr;
	pushConstantRangeCount = 0;
	pPushConstantRange = nullptr;
	uboBufferSize = 0x200;
	identity4(mModel);
	identity4(mProj);
	mProj[0][0] = (float)vulkanSetup->getSwapChainExtent().height / (float)vulkanSetup->getSwapChainExtent().width;
	color[0] = 0.0f; color[1] = 1.0f; color[2] = 0.0f; color[3] = 1.0f;
	texture = new Texture(vulkanSetup, "C:/Home/Entwicklung/Vulkan/textures/texture.jpg");
	getScale4(A, 2.0f, 1.0f, 1.0f);
	getTrans4(B, 2.0f, 0.0f, 0.0f);
	mult4(mModel, B, A);
	createUniformBuffer();
	createPipelineLayout();
	createGraphicsPipeline();
	createDescriptorSet();
}

Perlin1d::~Perlin1d() {}

CurveTessellator::CurveTessellator(	VulkanSetup *_vulkanSetup,
									VkDescriptorPool _descriptorPool,
									TextOverlay *_textOverlay,
									mat4 *_mView,
									bool *_key,
									VertexData *vertexData,
									IndexData *indexData)
									: RenderObject(_vulkanSetup, _descriptorPool, _textOverlay, _mView, _key)
{
	mat4 A, B;
	vertexShader.load("C:/Home/Entwicklung/Vulkan/build/VulkanApp2/vs_curveTessellator.spv");
	tessellationControlShader.load("C:/Home/Entwicklung/Vulkan/build/VulkanApp2/tcs_curveTessellator.spv");
	tessellationEvaluationShader.load("C:/Home/Entwicklung/Vulkan/build/VulkanApp2/tes_curveTessellator.spv");
	fragmentShader.load("C:/Home/Entwicklung/Vulkan/build/VulkanApp2/fs_curveTessellator.spv");
	vertexOffset = vertexData->getOffset(3);
	indexCount = indexData->getIndexCount(3);
	firstIndex = indexData->getFirstIndex(3);
	stageCount = 4;
	attributeDescriptionCount = 1;
	VkFormat formats[] = { VK_FORMAT_R32_SFLOAT };
	uint32_t offsets[] = { 0 };
	pAttributeDescriptions = getAttributeDescriptions(attributeDescriptionCount, formats, offsets);
	topology = VK_PRIMITIVE_TOPOLOGY_PATCH_LIST;
	bindingDescription = getBindingDescription(sizeof(float));
	pTessellationStateCreateInfo = getTessellationStateCreateInfo(2);
	pushConstantRangeCount = 0;
	pPushConstantRange = nullptr;
	uboBufferSize = 0x200;
	identity4(mModel);
	identity4(mProj);
	mProj[0][0] = (float)vulkanSetup->getSwapChainExtent().height / (float)vulkanSetup->getSwapChainExtent().width;
	color[0] = 1.0f; color[1] = 1.0f; color[2] = 0.0f; color[3] = 1.0f;
	texture = new Texture(vulkanSetup, "C:/Home/Entwicklung/Vulkan/textures/texture.jpg");
	getScale4(A, 2.0f, 1.0f, 1.0f);
	getTrans4(B, -5.0f, 0.0f, 0.0f);
	mult4(mModel, B, A);
	createUniformBuffer();
	createPipelineLayout();
	createGraphicsPipeline();
	createDescriptorSet();
}

CurveTessellator::~CurveTessellator() {}

Perlin1dTessellator::Perlin1dTessellator(	VulkanSetup *_vulkanSetup,
											VkDescriptorPool _descriptorPool,
											TextOverlay *_textOverlay,
											mat4 *_mView,
											bool *_key,
											VertexData *vertexData,
											IndexData *indexData)
											: RenderObject(_vulkanSetup, _descriptorPool, _textOverlay, _mView, _key)
{
	mat4 A, B;
	vertexShader.load("C:/Home/Entwicklung/Vulkan/build/VulkanApp2/vs_curveTessellator.spv");
	tessellationControlShader.load("C:/Home/Entwicklung/Vulkan/build/VulkanApp2/tcs_perlin1dTessellator.spv");
	tessellationEvaluationShader.load("C:/Home/Entwicklung/Vulkan/build/VulkanApp2/tes_perlin1dTessellator.spv");
	fragmentShader.load("C:/Home/Entwicklung/Vulkan/build/VulkanApp2/fs_curveTessellator.spv");
	vertexOffset = vertexData->getOffset(4);
	indexCount = indexData->getIndexCount(4);
	firstIndex = indexData->getFirstIndex(4);
	stageCount = 4;
	attributeDescriptionCount = 1;
	VkFormat formats[] = { VK_FORMAT_R32_SFLOAT };
	uint32_t offsets[] = { 0 };
	pAttributeDescriptions = getAttributeDescriptions(attributeDescriptionCount, formats, offsets);
	topology = VK_PRIMITIVE_TOPOLOGY_PATCH_LIST;
	bindingDescription = getBindingDescription(sizeof(float));
	pTessellationStateCreateInfo = getTessellationStateCreateInfo(2);
	pushConstantRangeCount = 0;
	pPushConstantRange = nullptr;
	uboBufferSize = 0x200;
	identity4(mModel);
	identity4(mProj);
	mProj[0][0] = (float)vulkanSetup->getSwapChainExtent().height / (float)vulkanSetup->getSwapChainExtent().width;
	color[0] = 1.0f; color[1] = 0.0f; color[2] = 0.0f; color[3] = 1.0f;
	texture = new Texture(vulkanSetup, "C:/Home/Entwicklung/Vulkan/textures/texture.jpg");
	getScale4(A, 2.0f, 1.0f, 1.0f);
	getTrans4(B, 0.0f, 10.0f, 0.0f);
	mult4(mModel, B, A);
	createUniformBuffer();
	createPipelineLayout();
	createGraphicsPipeline();
	createDescriptorSet();
}

Perlin1dTessellator::~Perlin1dTessellator() {}

Plane::Plane(	VulkanSetup *_vulkanSetup,
				VkDescriptorPool _descriptorPool,
				TextOverlay *_textOverlay,
				mat4 *_mView,
				bool *_key,
				VertexData *vertexData,
				IndexData *indexData)
				:RenderObject(_vulkanSetup, _descriptorPool, _textOverlay, _mView, _key)
{
	mat4 S, T, Rx, tmp;
	vertexShader.load("C:/Home/Entwicklung/Vulkan/build/VulkanApp2/vs_default.spv");
	fragmentShader.load("C:/Home/Entwicklung/Vulkan/build/VulkanApp2/fs_schachbrett.spv");
	vertexOffset = vertexData->getOffset(0);
	indexCount = indexData->getIndexCount(0);
	firstIndex = indexData->getFirstIndex(0);
	stageCount = 2;
	attributeDescriptionCount = 3;
	VkFormat formats[] = { VK_FORMAT_R32G32B32_SFLOAT, VK_FORMAT_R32G32B32_SFLOAT, VK_FORMAT_R32G32_SFLOAT };
	uint32_t offsets[] = { offsetof(Vertex, pos), offsetof(Vertex, color), offsetof(Vertex, texCoords) };
	pAttributeDescriptions = getAttributeDescriptions(attributeDescriptionCount, formats, offsets);
	topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	bindingDescription = getBindingDescription(sizeof(Vertex));
	pTessellationStateCreateInfo = nullptr;
	pushConstantRangeCount = 0;
	pPushConstantRange = nullptr;
	uboBufferSize = 0x200;
	identity4(mModel);
	getFrustum(mProj, 0.25f*(float)vulkanSetup->getSwapChainExtent().width / (float)vulkanSetup->getSwapChainExtent().height, 0.25f, 0.5f, 1000.0f);
	color[0] = 1.0f; color[1] = 1.0f; color[2] = 1.0f; color[3] = 1.0f;
	texture = new Texture(vulkanSetup, "C:/Home/Entwicklung/Vulkan/textures/texture.jpg");
	getScale4(S, 10.0f, 1.0f, 10.0f);
	getRotX4(Rx, PI / 2.0f);
	getTrans4(T, 0.0f, 100.0f, 0.0f);
	mult4(tmp, S, Rx);
	mult4(mModel, T, tmp);
	createUniformBuffer();
	createPipelineLayout();
	createGraphicsPipeline();
	createDescriptorSet();
}

Plane::~Plane() {}

Planet::Planet(	VulkanSetup *_vulkanSetup,
				VkDescriptorPool _descriptorPool,
				TextOverlay *_textOverlay,
				mat4 *_mView,
				bool *_key,
				VertexData *vertexData,
				IndexData *indexData)
				:RenderObject(_vulkanSetup, _descriptorPool, _textOverlay, _mView, _key)
{
	vertexShader.load("C:/Home/Entwicklung/Vulkan/build/VulkanApp2/vs_perlinSphere.spv");
	fragmentShader.load("C:/Home/Entwicklung/Vulkan/build/VulkanApp2/fs_defaultTex.spv");
	vertexOffset = vertexData->getOffset(5);
	indexCount = indexData->getIndexCount(5);
	firstIndex = indexData->getFirstIndex(5);
	stageCount = 2;
	attributeDescriptionCount = 1;
	VkFormat formats[] = { VK_FORMAT_R32G32_SFLOAT };
	uint32_t offsets[] = { 0 };
	pAttributeDescriptions = getAttributeDescriptions(attributeDescriptionCount, formats, offsets);
	topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	bindingDescription = getBindingDescription(2*sizeof(float));
	pTessellationStateCreateInfo = nullptr;
	pushConstantRangeCount = 0;
	pPushConstantRange = nullptr;
	uboBufferSize = 0x200;
	identity4(mModel);
	getFrustum(mProj, 0.25f*(float)vulkanSetup->getSwapChainExtent().width / (float)vulkanSetup->getSwapChainExtent().height, 0.25f, 0.5f, 1000.0f);
	color[0] = 0.0f; color[1] = 1.0f; color[2] = 0.0f; color[3] = 1.0f;
	texture = new Texture(vulkanSetup, "C:/Home/Entwicklung/Vulkan/textures/moon.png");
	getScale4(mModel, 100.0f, 100.0f, 100.0f);
	//getTrans4(mModel, 0.0f, 1.5f, 0.0f);
	createUniformBuffer();
	createPipelineLayout();
	createGraphicsPipeline();
	createDescriptorSet();
}

Planet::~Planet() {}

Sphere::Sphere(	VulkanSetup *_vulkanSetup,
				VkDescriptorPool _descriptorPool,
				TextOverlay *_textOverlay,
				mat4 *_mView,
				bool *_key,
				VertexData *vertexData,
				IndexData *indexData)
				:RenderObject(_vulkanSetup, _descriptorPool, _textOverlay, _mView, _key)
{
	vertexShader.load("C:/Home/Entwicklung/Vulkan/build/VulkanApp2/vs_sphere.spv");
	fragmentShader.load("C:/Home/Entwicklung/Vulkan/build/VulkanApp2/fs_test2.spv");
	vertexOffset = vertexData->getOffset(5);
	indexCount = indexData->getIndexCount(5);
	firstIndex = indexData->getFirstIndex(5);
	stageCount = 2;
	attributeDescriptionCount = 1;
	VkFormat formats[] = { VK_FORMAT_R32G32_SFLOAT };
	uint32_t offsets[] = { 0 };
	pAttributeDescriptions = getAttributeDescriptions(attributeDescriptionCount, formats, offsets);
	topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	bindingDescription = getBindingDescription(2 * sizeof(float));
	pTessellationStateCreateInfo = nullptr;
	pushConstantRangeCount = 0;
	pPushConstantRange = nullptr;
	uboBufferSize = 0x200;
	identity4(mModel);
	getFrustum(mProj, 0.25f*(float)vulkanSetup->getSwapChainExtent().width / (float)vulkanSetup->getSwapChainExtent().height, 0.25f, 0.5f, 1000.0f);
	color[0] = 1.0f; color[1] = 0.0f; color[2] = 1.0f; color[3] = 1.0f;
	texture = new Texture(vulkanSetup, "C:/Home/Entwicklung/Vulkan/textures/texture.jpg");
	//getScale4(mModel, 100.0f, 100.0f, 100.0f);
	getTrans4(mModel, 0.0f, 101.5f, 0.0f);
	createUniformBuffer();
	createPipelineLayout();
	createGraphicsPipeline();
	createDescriptorSet();
}

Sphere::~Sphere() {}

Cube::Cube(	VulkanSetup *_vulkanSetup,
			VkDescriptorPool _descriptorPool,
			TextOverlay *_textOverlay,
			mat4 *_mView,
			bool *_key,
			VertexData *vertexData,
			IndexData *indexData)
			:RenderObject(_vulkanSetup, _descriptorPool, _textOverlay, _mView, _key)
{
	vertexShader.load("C:/Home/Entwicklung/Vulkan/build/VulkanApp2/vs_default.spv");
	fragmentShader.load("C:/Home/Entwicklung/Vulkan/build/VulkanApp2/fs_test.spv");
	vertexOffset = vertexData->getOffset(6);
	indexCount = indexData->getIndexCount(6);
	firstIndex = indexData->getFirstIndex(6);
	stageCount = 2;
	attributeDescriptionCount = 3;
	VkFormat formats[] = { VK_FORMAT_R32G32B32_SFLOAT, VK_FORMAT_R32G32B32_SFLOAT, VK_FORMAT_R32G32_SFLOAT };
	uint32_t offsets[] = { offsetof(Vertex, pos), offsetof(Vertex, color), offsetof(Vertex, texCoords) };
	pAttributeDescriptions = getAttributeDescriptions(attributeDescriptionCount, formats, offsets);
	topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	bindingDescription = getBindingDescription(sizeof(Vertex));
	pTessellationStateCreateInfo = nullptr;
	pushConstantRangeCount = 0;
	pPushConstantRange = nullptr;
	uboBufferSize = 0x200;
	identity4(mModel);
	getFrustum(mProj, 0.25f*(float)vulkanSetup->getSwapChainExtent().width / (float)vulkanSetup->getSwapChainExtent().height, 0.25f, 0.5f, 1000.0f);
	color[0] = 1.0f; color[1] = 1.0f; color[2] = 0.0f; color[3] = 1.0f;
	texture = new Texture(vulkanSetup, "C:/Home/Entwicklung/Vulkan/textures/texture.jpg");
	getTrans4(mModel, 0.0f, 110.0f, 0.0f);
	createUniformBuffer();
	createPipelineLayout();
	createGraphicsPipeline();
	createDescriptorSet();
}

Cube::~Cube() {}

TxtObj::TxtObj(	VulkanSetup *_vulkanSetup,
				VkDescriptorPool _descriptorPool,
				TextOverlay *_textOverlay,
				mat4 *_mView,
				bool *_key,
				VertexData *vertexData,
				IndexData *indexData)
				: RenderObject(_vulkanSetup, _descriptorPool, _textOverlay, _mView, _key)
{
	vertexShader.load("C:/Home/Entwicklung/Vulkan/build/VulkanApp2/vs_text.spv");
	fragmentShader.load("C:/Home/Entwicklung/Vulkan/build/VulkanApp2/fs_text.spv");
	vertexOffset = vertexData->getOffset(0);
	indexCount = indexData->getIndexCount(0);
	firstIndex = indexData->getFirstIndex(0);
	stageCount = 2;
	attributeDescriptionCount = 2;
	VkFormat formats[] = { VK_FORMAT_R32G32_SFLOAT, VK_FORMAT_R32G32_SFLOAT };
	uint32_t offsets[] = { 0 , 2 * sizeof(float) };
	pAttributeDescriptions = getAttributeDescriptions(attributeDescriptionCount, formats, offsets);
	topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
	bindingDescription = getBindingDescription(sizeof(vec4));
	pTessellationStateCreateInfo = nullptr;
	pushConstantRangeCount = 0;
	pPushConstantRange = nullptr;
	uboBufferSize = 0x200;
	identity4(mModel);
	identity4(mProj);
	texture = new Texture(vulkanSetup, textOverlay);
	createUniformBuffer();
	createPipelineLayout();
	createGraphicsPipeline();
	createDescriptorSet();
}

TxtObj::~TxtObj() {}