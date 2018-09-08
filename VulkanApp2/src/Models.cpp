#include "Models.h"

Square::Square(	VulkanSetup *_vulkanSetup,
				VkDescriptorPool _descriptorPool,
				TextOverlay *_textOverlay,
				mat4 *_mView,
				bool *_key,
				VertexData *vertexData,
				IndexData *indexData)
				:RenderObject(_vulkanSetup, _descriptorPool, _textOverlay, _mView, _key)
{
	vertexShader.load("C:/Home/Entwicklung/Vulkan/build/vs_default.spv");
	fragmentShader.load("C:/Home/Entwicklung/Vulkan/build/fs_test.spv");
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
	pushConstantRange = nullptr;
	uboBufferSize = 0x200;
	identity4(mModel);
	identity4(mProj);
	color[0] = 0.0f; color[1] = 1.0f; color[2] = 0.0f; color[3] = 1.0f;
	texture = new Texture(vulkanSetup, "C:/Home/Entwicklung/Vulkan/build/texture.jpg");
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
	vertexShader.load("C:/Home/Entwicklung/Vulkan/build/vs_default.spv");
	fragmentShader.load("C:/Home/Entwicklung/Vulkan/build/fs_powermeter.spv");
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
	pushConstantRange = nullptr;
	uboBufferSize = 0x200;
	identity4(mModel);
	identity4(mProj);
	color[0] = 0.0f; color[1] = 1.0f; color[2] = 0.0f; color[3] = 1.0f;
	texture = new Texture(vulkanSetup, "C:/Home/Entwicklung/Vulkan/build/texture.jpg");
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
	vertexShader.load("C:/Home/Entwicklung/Vulkan/build/vs_default.spv");
	fragmentShader.load("C:/Home/Entwicklung/Vulkan/build/fs_perlin2d.spv");
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
	pushConstantRange = nullptr;
	uboBufferSize = 0x200;
	identity4(mModel);
	identity4(mProj);
	color[0] = 0.0f; color[1] = 1.0f; color[2] = 0.0f; color[3] = 1.0f;
	texture = new Texture(vulkanSetup, "C:/Home/Entwicklung/Vulkan/build/texture.jpg");
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
	vertexShader.load("C:/Home/Entwicklung/Vulkan/build/vs_default.spv");
	fragmentShader.load("C:/Home/Entwicklung/Vulkan/build/fs_default.spv");
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
	pushConstantRange = nullptr;
	uboBufferSize = 0x200;
	identity4(mModel);
	identity4(mProj);
	color[0] = 0.0f; color[1] = 1.0f; color[2] = 0.0f; color[3] = 1.0f;
	texture = new Texture(vulkanSetup, "C:/Home/Entwicklung/Vulkan/build/texture.jpg");
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
	vertexShader.load("C:/Home/Entwicklung/Vulkan/build/vs_default.spv");
	fragmentShader.load("C:/Home/Entwicklung/Vulkan/build/fs_circleFilled.spv");
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
	pushConstantRange = nullptr;
	uboBufferSize = 0x200;
	identity4(mModel);
	identity4(mProj);
	color[0] = 0.0f; color[1] = 1.0f; color[2] = 0.0f; color[3] = 1.0f;
	texture = new Texture(vulkanSetup, "C:/Home/Entwicklung/Vulkan/build/texture.jpg");
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

	if (key[VK_LEFT] == true)
	{
		dup4(tmp, mModel);
		getTrans4(T, -0.1f, 0.0f, 0.0f);
		mult4(mModel, T, tmp);
	}

	if (key[VK_RIGHT] == true)
	{
		dup4(tmp, mModel);
		getTrans4(T, 0.1f, 0.0f, 0.0f);
		mult4(mModel, T, tmp);
	}

	if (key[VK_UP] == true)
	{
		dup4(tmp, mModel);
		getTrans4(T, 0.0f, -0.1f, 0.0f);
		mult4(mModel, T, tmp);
	}

	if (key[VK_DOWN] == true)
	{
		dup4(tmp, mModel);
		getTrans4(T, 0.0f, 0.1f, 0.0f);
		mult4(mModel, T, tmp);
	}
}

TxtObj::TxtObj(	VulkanSetup *_vulkanSetup,
				VkDescriptorPool _descriptorPool,
				TextOverlay *_textOverlay,
				mat4 *_mView,
				bool *_key,
				VertexData *vertexData,
				IndexData *indexData)
				: RenderObject(_vulkanSetup, _descriptorPool, _textOverlay, _mView, _key)
{
	vertexShader.load("C:/Home/Entwicklung/Vulkan/build/vs_text.spv");
	fragmentShader.load("C:/Home/Entwicklung/Vulkan/build/fs_text.spv");
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
	pushConstantRange = nullptr;
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