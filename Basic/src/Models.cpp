#include "Models.h"

Square::Square(Vulkan* _vulkan,
	VkDescriptorPool _descriptorPool,
	TextOverlay* _textOverlay,
	mat4* _mView,
	bool* _key,
	MousePos* _mousePos,
	VertexData* vertexData,
	IndexData* indexData)
	:RenderObject(_vulkan, _descriptorPool, _textOverlay, _mView, _key, _mousePos)
{
	vertexShader.load("vs_default.spv");
	fragmentShader.load("fs_test.spv");
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
	pushConstantRangeCount = 0;
	pPushConstantRange = nullptr;
	uboBufferSize = 0x200;
	identity4(mModel);
	identity4(mProj);
	mProj[0][0] = (float)vulkan->getSwapChainExtent().height / (float)vulkan->getSwapChainExtent().width;
	color[0] = 0.0f; color[1] = 1.0f; color[2] = 0.0f; color[3] = 1.0f;
	texture = new Texture(vulkan, "C:/Home/Entwicklung/Vulkan/textures/texture.jpg");
	getTrans4(mModel, 0.0f, 0.0f, 0.0f);
	createUniformBuffer();
	createDescriptorSetLayout();
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

Tacho::Tacho(Vulkan* _vulkan,
	VkDescriptorPool _descriptorPool,
	TextOverlay* _textOverlay,
	mat4* _mView,
	bool* _key,
	MousePos* _mousePos,
	VertexData* vertexData,
	IndexData* indexData)
	: RenderObject(_vulkan, _descriptorPool, _textOverlay, _mView, _key, _mousePos)
{
	vertexShader.load("vs_default.spv");
	fragmentShader.load("fs_powermeter.spv");
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
	pushConstantRangeCount = 0;
	pPushConstantRange = nullptr;
	uboBufferSize = 0x200;
	identity4(mModel);
	identity4(mProj);
	mProj[0][0] = (float)vulkan->getSwapChainExtent().height / (float)vulkan->getSwapChainExtent().width;
	color[0] = 0.0f; color[1] = 1.0f; color[2] = 0.0f; color[3] = 1.0f;
	texture = nullptr;
	getTrans4(mModel, 0.0f, -5.0f, 0.0f);
	createUniformBuffer();
	createDescriptorSetLayout();
	createPipelineLayout();
	createGraphicsPipeline();
	createDescriptorSet();
}

Tacho::~Tacho() {}

FlatPerlin2d::FlatPerlin2d(Vulkan* _vulkan,
	VkDescriptorPool _descriptorPool,
	TextOverlay* _textOverlay,
	mat4* _mView,
	bool* _key,
	MousePos* _mousePos,
	VertexData* vertexData,
	IndexData* indexData)
	: RenderObject(_vulkan, _descriptorPool, _textOverlay, _mView, _key, _mousePos)
{
	vertexShader.load("vs_default.spv");
	fragmentShader.load("fs_perlin2d.spv");
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
	pushConstantRangeCount = 0;
	pPushConstantRange = nullptr;
	uboBufferSize = 0x200;
	identity4(mModel);
	identity4(mProj);
	mProj[0][0] = (float)vulkan->getSwapChainExtent().height / (float)vulkan->getSwapChainExtent().width;
	color[0] = 0.0f; color[1] = 1.0f; color[2] = 0.0f; color[3] = 1.0f;
	texture = nullptr;
	getTrans4(mModel, -5.0f, -5.0f, 0.0f);
	createUniformBuffer();
	createDescriptorSetLayout();
	createPipelineLayout();
	createGraphicsPipeline();
	createDescriptorSet();
}

FlatPerlin2d::~FlatPerlin2d() {}

Star::Star(Vulkan* _vulkan,
	VkDescriptorPool _descriptorPool,
	TextOverlay* _textOverlay,
	mat4* _mView,
	bool* _key,
	MousePos* _mousePos,
	VertexData* vertexData,
	IndexData* indexData)
	: RenderObject(_vulkan, _descriptorPool, _textOverlay, _mView, _key, _mousePos)
{
	vertexShader.load("vs_default.spv");
	fragmentShader.load("fs_default.spv");
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
	pushConstantRangeCount = 0;
	pPushConstantRange = nullptr;
	uboBufferSize = 0x200;
	identity4(mModel);
	identity4(mProj);
	mProj[0][0] = (float)vulkan->getSwapChainExtent().height / (float)vulkan->getSwapChainExtent().width;
	color[0] = 0.0f; color[1] = 1.0f; color[2] = 0.0f; color[3] = 1.0f;
	texture = nullptr;
	getTrans4(mModel, 5.0f, 5.0f, 0.0f);
	createUniformBuffer();
	createDescriptorSetLayout();
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

FilledCircle::FilledCircle(Vulkan* _vulkan,
	VkDescriptorPool _descriptorPool,
	TextOverlay* _textOverlay,
	mat4* _mView,
	bool* _key,
	MousePos* _mousePos,
	VertexData* vertexData,
	IndexData* indexData)
	: RenderObject(_vulkan, _descriptorPool, _textOverlay, _mView, _key, _mousePos)
{
	vertexShader.load("vs_default.spv");
	fragmentShader.load("fs_circleFilled.spv");
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
	pushConstantRangeCount = 0;
	pPushConstantRange = nullptr;
	uboBufferSize = 0x200;
	identity4(mModel);
	identity4(mProj);
	mProj[0][0] = (float)vulkan->getSwapChainExtent().height / (float)vulkan->getSwapChainExtent().width;
	color[0] = 0.0f; color[1] = 1.0f; color[2] = 0.0f; color[3] = 1.0f;
	texture = nullptr;
	getTrans4(mModel, 5.0f, 5.0f, 0.0f);
	createUniformBuffer();
	createDescriptorSetLayout();
	createPipelineLayout();
	createGraphicsPipeline();
	createDescriptorSet();
}

FilledCircle::~FilledCircle() {}

void FilledCircle::motion()
{
	mat4 T;
	getTrans4(T, (float)mousePos->x*2.0f/2000.0f-1.0f, (float)mousePos->y*2.0f/1500.0f-1.0f, 0.0f);
	dup4(mModel, T);
}

PerlinCircle::PerlinCircle(Vulkan* _vulkan,
	VkDescriptorPool _descriptorPool,
	TextOverlay* _textOverlay,
	mat4* _mView,
	bool* _key,
	MousePos* _mousePos,
	VertexData* vertexData,
	IndexData* indexData)
	: RenderObject(_vulkan, _descriptorPool, _textOverlay, _mView, _key, _mousePos)
{
	vertexShader.load("vs_perlinCircle.spv");
	fragmentShader.load("fs_default.spv");
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
	pushConstantRangeCount = 1;
	pPushConstantRange = createPushConstantRange(VK_SHADER_STAGE_VERTEX_BIT, sizeof(PushConstants));
	pushConsts.seed_u = 158;
	pushConsts.seed_v = 2767;
	uboBufferSize = 0x200;
	identity4(mModel);
	identity4(mProj);
	mProj[0][0] = (float)vulkan->getSwapChainExtent().height / (float)vulkan->getSwapChainExtent().width;
	color[0] = 0.0f; color[1] = 1.0f; color[2] = 0.0f; color[3] = 1.0f;
	texture = nullptr;
	getScale4(mModel, 1.5f, 1.5f, 0.0f);
	commandBuffer = new Buffer(vulkan);
	createUniformBuffer();
	createDescriptorSetLayout();
	createPipelineLayout();
	createGraphicsPipeline();
	createDescriptorSet();
}

PerlinCircle::~PerlinCircle() {}

void PerlinCircle::motion()
{
	if (key[VK_SPACE] == true)
	{
		pushConsts.seed_u = (float)(clock() - startTime) / CLOCKS_PER_SEC;
		pushConsts.seed_v = (float)(clock() - startTime) / CLOCKS_PER_SEC + 245.0f;

		commandBuffer->beginSingleTimeCommands();

		vkCmdPushConstants(commandBuffer->getCommandBuffer(), pipelineLayout, pPushConstantRange->stageFlags, pPushConstantRange->offset, pPushConstantRange->size, &pushConsts);

		commandBuffer->endSingleTimeCommands();
	}
}

Wave::Wave(Vulkan* _vulkan,
	VkDescriptorPool _descriptorPool,
	TextOverlay * _textOverlay,
	mat4 * _mView,
	bool* _key,
	MousePos* _mousePos,
	VertexData * vertexData,
	IndexData * indexData)
	: RenderObject(_vulkan, _descriptorPool, _textOverlay, _mView, _key, _mousePos)
{
	mat4 A, B;
	vertexShader.load("vs_welle.spv");
	fragmentShader.load("fs_default.spv");
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
	pushConstantRangeCount = 0;
	pPushConstantRange = nullptr;
	uboBufferSize = 0x200;
	identity4(mModel);
	identity4(mProj);
	mProj[0][0] = (float)vulkan->getSwapChainExtent().height / (float)vulkan->getSwapChainExtent().width;
	color[0] = 0.0f; color[1] = 1.0f; color[2] = 0.0f; color[3] = 1.0f;
	texture = nullptr;
	getScale4(A, 2.0f, 1.0f, 1.0f);
	getTrans4(B, 0.0f, 2.0f, 0.0f);
	mult4(mModel, B, A);
	createUniformBuffer();
	createDescriptorSetLayout();
	createPipelineLayout();
	createGraphicsPipeline();
	createDescriptorSet();
}

Wave::~Wave() {}

void Wave::updateUniformBuffer()
{
	void* data;
	float time = (float)(clock() - startTime) / CLOCKS_PER_SEC;
	vkMapMemory(vulkan->getDevice(), uniformBuffer->getBufferMemory(), 0, uboBufferSize, 0, &data);
	memcpy((char*)data, &mModel, sizeof(mat4));
	memcpy((char*)data + sizeof(mat4), mView, sizeof(mat4));
	memcpy((char*)data + 2 * sizeof(mat4), &mProj, sizeof(mat4));
	memcpy((char*)data + 0x100, &time, sizeof(float));
	vkUnmapMemory(vulkan->getDevice(), uniformBuffer->getBufferMemory());
}

Perlin1d::Perlin1d(Vulkan* _vulkan,
	VkDescriptorPool _descriptorPool,
	TextOverlay * _textOverlay,
	mat4 * _mView,
	bool* _key,
	MousePos* _mousePos,
	VertexData * vertexData,
	IndexData * indexData)
	: RenderObject(_vulkan, _descriptorPool, _textOverlay, _mView, _key, _mousePos)
{
	mat4 A, B;
	vertexShader.load("vs_perlin1d.spv");
	fragmentShader.load("fs_default.spv");
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
	pushConstantRangeCount = 0;
	pPushConstantRange = nullptr;
	uboBufferSize = 0x200;
	identity4(mModel);
	identity4(mProj);
	mProj[0][0] = (float)vulkan->getSwapChainExtent().height / (float)vulkan->getSwapChainExtent().width;
	color[0] = 0.0f; color[1] = 1.0f; color[2] = 0.0f; color[3] = 1.0f;
	texture = nullptr;
	getScale4(A, 2.0f, 1.0f, 1.0f);
	getTrans4(B, 2.0f, 0.0f, 0.0f);
	mult4(mModel, B, A);
	createUniformBuffer();
	createDescriptorSetLayout();
	createPipelineLayout();
	createGraphicsPipeline();
	createDescriptorSet();
}

Perlin1d::~Perlin1d() {}

TxtObj::TxtObj(Vulkan* _vulkan,
	VkDescriptorPool _descriptorPool,
	TextOverlay * _textOverlay,
	mat4 * _mView,
	bool* _key,
	MousePos* _mousePos,
	VertexData * vertexData,
	IndexData * indexData)
	: RenderObject(_vulkan, _descriptorPool, _textOverlay, _mView, _key, _mousePos)
{
	vertexShader.load("vs_text.spv");
	fragmentShader.load("fs_text.spv");
	stageCount = 2;
	attributeDescriptionCount = 2;
	VkFormat formats[] = { VK_FORMAT_R32G32_SFLOAT, VK_FORMAT_R32G32_SFLOAT };
	uint32_t offsets[] = { 0 , 2 * sizeof(float) };
	pAttributeDescriptions = getAttributeDescriptions(attributeDescriptionCount, formats, offsets);
	topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
	bindingDescription = getBindingDescription(sizeof(vec4));
	pushConstantRangeCount = 0;
	pPushConstantRange = nullptr;
	uboBufferSize = 0x200;
	identity4(mModel);
	identity4(mProj);
	texture = new Texture(vulkan, textOverlay);
	createUniformBuffer();
	createDescriptorSetLayout();
	createPipelineLayout();
	createGraphicsPipeline();
	createDescriptorSet();
}

TxtObj::~TxtObj() {}

float values[] = { -1.0f, -1.0f, 1.0f, 1.0f };

Graph::Graph(Vulkan* _vulkan,
	VkDescriptorPool _descriptorPool,
	TextOverlay* _textOverlay,
	mat4* _mView,
	bool* _key,
	MousePos* _mousePos,
	VertexData* vertexData,
	IndexData* indexData)
	: RenderObject(_vulkan, _descriptorPool, _textOverlay, _mView, _key, _mousePos)
{
	vertexShader.load("vs_graph.spv");
	fragmentShader.load("fs_graph.spv");
	stageCount = 2;
	attributeDescriptionCount = 1;
	VkFormat formats[] = { VK_FORMAT_R32G32_SFLOAT };
	uint32_t offsets[] = { 0 };
	pAttributeDescriptions = getAttributeDescriptions(attributeDescriptionCount, formats, offsets);
	topology = VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
	bindingDescription = getBindingDescription(sizeof(vec2));
	pushConstantRangeCount = 0;
	pPushConstantRange = nullptr;
	uboBufferSize = 0x200;
	identity4(mModel);
	identity4(mProj);
	color[0] = 0.0f; color[1] = 1.0f; color[2] = 0.0f; color[3] = 1.0f;
	texture = nullptr;
	createUniformBuffer();
	createDescriptorSetLayout();
	createPipelineLayout();
	createGraphicsPipeline();
	createDescriptorSet();

	vertexBufferSize = 2 * sizeof(vec2);
	vertexCount = 2;
	vertexBuffer = new Buffer(vulkan);
	vertexBuffer->createBuffer(vertexBufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	
	void* data;
	vkMapMemory(vulkan->getDevice(), vertexBuffer->getBufferMemory(), 0, vertexBufferSize, 0, &data);
	memcpy(data, values, vertexBufferSize);
	vkUnmapMemory(vulkan->getDevice(), vertexBuffer->getBufferMemory());
}

Graph::~Graph() {};

VkBuffer Graph::getVertexBuffer()
{
	return vertexBuffer->getBuffer();
}

uint32_t Graph::getVertexCount()
{
	return vertexCount;
}