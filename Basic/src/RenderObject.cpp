#include "RenderObject.h"

RenderObject::RenderObject(Vulkan* _vulkan,
	VkDescriptorPool _descriptorPool,
	TextOverlay* _textOverlay,
	mat4* _mView,
	bool* _key,
	MousePos* _mousePos)
{
	vulkan = _vulkan;
	descriptorPool = _descriptorPool;
	textOverlay = _textOverlay;
	mView = _mView;
	key = _key;
	mousePos = _mousePos;
	startTime = clock();
}

RenderObject::~RenderObject() {}

void RenderObject::updateUniformBuffer()
{
	void* data;
	vkMapMemory(vulkan->getDevice(), uniformBuffer->getBufferMemory(), 0, uboBufferSize, 0, &data);
	memcpy((char*)data, &mModel, sizeof(mat4));
	memcpy((char*)data + sizeof(mat4), mView, sizeof(mat4));
	memcpy((char*)data + 2 * sizeof(mat4), &mProj, sizeof(mat4));
	memcpy((char*)data + 0x100, &color, sizeof(color));
	vkUnmapMemory(vulkan->getDevice(), uniformBuffer->getBufferMemory());
}

void RenderObject::motion() {}

VkShaderModule RenderObject::createShaderModule(Shader shader)
{
	VkShaderModuleCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.pNext = nullptr;
	createInfo.flags = 0;
	createInfo.codeSize = shader.getSize();
	createInfo.pCode = shader.getCode(); // ! alignment Anforderungen von uint32_t beachten !

	VkShaderModule shaderModule;
	if (vkCreateShaderModule(vulkan->getDevice(), &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
		std::cout << "Failed to create shader module!" << std::endl;
		exit(1);
	}

	return shaderModule;
}

VkPipelineShaderStageCreateInfo RenderObject::getShaderStageInfo(VkShaderStageFlagBits stage, VkShaderModule module)
{
	VkPipelineShaderStageCreateInfo shaderStageInfo = {};
	shaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	shaderStageInfo.pNext = nullptr;
	shaderStageInfo.flags = 0;
	shaderStageInfo.stage = stage;
	shaderStageInfo.module = module;
	shaderStageInfo.pName = "main";
	shaderStageInfo.pSpecializationInfo = nullptr;

	return shaderStageInfo;
}

VkVertexInputBindingDescription RenderObject::getBindingDescription(uint32_t stride)
{
	VkVertexInputBindingDescription bindingDscr = {};
	bindingDscr.binding = 0;
	bindingDscr.stride = stride;
	bindingDscr.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	return bindingDscr;
}

VkVertexInputAttributeDescription* RenderObject::getAttributeDescriptions(uint32_t count, VkFormat* formats, uint32_t* offsets)
{
	VkVertexInputAttributeDescription* attributeDescriptions = new VkVertexInputAttributeDescription[count];

	for (uint32_t i = 0; i < count; i++)
	{
		attributeDescriptions[i].binding = 0;
		attributeDescriptions[i].location = i;
		attributeDescriptions[i].format = formats[i];
		attributeDescriptions[i].offset = offsets[i];
	}

	return attributeDescriptions;
}

VkPushConstantRange* RenderObject::createPushConstantRange(VkShaderStageFlags shaderStageFlags, uint32_t size)
{
	VkPushConstantRange* pushConstantRange = new VkPushConstantRange;

	pushConstantRange->stageFlags = shaderStageFlags;
	pushConstantRange->offset = 0;
	pushConstantRange->size = size;

	return pushConstantRange;
}

void RenderObject::createUniformBuffer()
{
	uniformBuffer = new Buffer(vulkan);
	uniformBuffer->createBuffer(uboBufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
}

void RenderObject::createDescriptorSetLayout()
{
	uint32_t bindingCount = 2;

	if (texture) bindingCount = 3;

	VkDescriptorSetLayoutBinding uboLayoutBinding = {};
	uboLayoutBinding.binding = 0;
	uboLayoutBinding.descriptorCount = 1;
	uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uboLayoutBinding.stageFlags = VK_SHADER_STAGE_ALL_GRAPHICS;
	uboLayoutBinding.pImmutableSamplers = nullptr;

	VkDescriptorSetLayoutBinding uboMaterialBinding = {};
	uboMaterialBinding.binding = 1;
	uboMaterialBinding.descriptorCount = 1;
	uboMaterialBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uboMaterialBinding.stageFlags = VK_SHADER_STAGE_ALL_GRAPHICS;
	uboMaterialBinding.pImmutableSamplers = nullptr;

	VkDescriptorSetLayoutBinding samplerLayoutBinding = {};
	samplerLayoutBinding.binding = 2;
	samplerLayoutBinding.descriptorCount = 1;
	samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	samplerLayoutBinding.pImmutableSamplers = nullptr;

	VkDescriptorSetLayoutBinding bindings[] = { uboLayoutBinding, uboMaterialBinding, samplerLayoutBinding };

	VkDescriptorSetLayoutCreateInfo layoutInfo = {};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = bindingCount;
	layoutInfo.pBindings = bindings;

	if (vkCreateDescriptorSetLayout(vulkan->getDevice(), &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS) {
		std::cout << "Failed to create descriptor set layout!" << std::endl;
		exit(1);
	}
}

void RenderObject::createPipelineLayout()
{
	VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.pNext = nullptr;
	pipelineLayoutInfo.flags = 0;
	pipelineLayoutInfo.setLayoutCount = 1;
	pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;
	pipelineLayoutInfo.pushConstantRangeCount = pushConstantRangeCount;
	pipelineLayoutInfo.pPushConstantRanges = pPushConstantRange;

	if (vkCreatePipelineLayout(vulkan->getDevice(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
		std::cout << "Failed to create pipeline layout!" << std::endl;
		exit(1);
	}
}

void RenderObject::createGraphicsPipeline()
{
	VkShaderModule vertexShaderModule = createShaderModule(vertexShader);
	VkShaderModule fragmentShaderModule = createShaderModule(fragmentShader);
	VkPipelineShaderStageCreateInfo vertexShaderStageInfo = getShaderStageInfo(VK_SHADER_STAGE_VERTEX_BIT, vertexShaderModule);
	VkPipelineShaderStageCreateInfo fragmentShaderStageInfo = getShaderStageInfo(VK_SHADER_STAGE_FRAGMENT_BIT, fragmentShaderModule);
	VkPipelineShaderStageCreateInfo* shaderStages;

	if (stageCount > 2)
	{
		VkShaderModule tessellationControlShaderModule = createShaderModule(tessellationControlShader);
		VkShaderModule tessellationEvaluationShaderModule = createShaderModule(tessellationEvaluationShader);
		VkPipelineShaderStageCreateInfo tessellationControlShaderStageInfo = getShaderStageInfo(VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT, tessellationControlShaderModule);
		VkPipelineShaderStageCreateInfo tessellationEvaluationShaderStageInfo = getShaderStageInfo(VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT, tessellationEvaluationShaderModule);
		shaderStages = new VkPipelineShaderStageCreateInfo[4];
		shaderStages[0] = vertexShaderStageInfo;
		shaderStages[1] = tessellationControlShaderStageInfo;
		shaderStages[2] = tessellationEvaluationShaderStageInfo;
		shaderStages[3] = fragmentShaderStageInfo;
	}
	else
	{
		shaderStages = new VkPipelineShaderStageCreateInfo[2];
		shaderStages[0] = vertexShaderStageInfo;
		shaderStages[1] = fragmentShaderStageInfo;
	}

	VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.pNext = nullptr;
	vertexInputInfo.flags = 0;
	vertexInputInfo.vertexBindingDescriptionCount = 1;
	vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
	vertexInputInfo.vertexAttributeDescriptionCount = attributeDescriptionCount;
	vertexInputInfo.pVertexAttributeDescriptions = pAttributeDescriptions;

	VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.pNext = nullptr;
	inputAssembly.flags = 0;
	inputAssembly.topology = topology;
	inputAssembly.primitiveRestartEnable = VK_FALSE;

	VkViewport viewport = {};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float)vulkan->getSwapChainExtent().width;
	viewport.height = (float)vulkan->getSwapChainExtent().height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 0.0f;

	VkRect2D scissor = {};
	scissor.offset = { 0, 0 };
	scissor.extent = vulkan->getSwapChainExtent();

	VkPipelineViewportStateCreateInfo viewportState = {};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.pNext = nullptr;
	viewportState.flags = 0;
	viewportState.viewportCount = 1;
	viewportState.pViewports = &viewport;
	viewportState.scissorCount = 1;
	viewportState.pScissors = &scissor;

	VkPipelineRasterizationStateCreateInfo rasterizer = {};
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.pNext = nullptr;
	rasterizer.flags = 0;
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizer.lineWidth = 5.0f; // --> Feature muss extra aktiviert werden
	rasterizer.cullMode = VK_CULL_MODE_NONE; // Front and Back werden gerendert
	rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
	rasterizer.depthBiasEnable = VK_FALSE;
	rasterizer.depthBiasConstantFactor = 0.0f; // Optional
	rasterizer.depthBiasClamp = 0.0f; // Optional
	rasterizer.depthBiasSlopeFactor = 0.0f; // Optional

	VkPipelineMultisampleStateCreateInfo multisampling = {};
	multisampling.pNext = nullptr;
	multisampling.flags = 0;
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	multisampling.minSampleShading = 1.0f; // Optional
	multisampling.pSampleMask = nullptr; // Optional
	multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
	multisampling.alphaToOneEnable = VK_FALSE; // Optional

	VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = VK_TRUE;
	colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
	colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

	VkPipelineColorBlendStateCreateInfo colorBlending = {};
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.pNext = nullptr;
	colorBlending.flags = 0;
	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlendAttachment;
	colorBlending.blendConstants[0] = 0.0f; // Optional
	colorBlending.blendConstants[1] = 0.0f; // Optional
	colorBlending.blendConstants[2] = 0.0f; // Optional
	colorBlending.blendConstants[3] = 0.0f; // Optional

	VkGraphicsPipelineCreateInfo pipelineInfo = {};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = stageCount;
	pipelineInfo.pStages = shaderStages;
	pipelineInfo.pVertexInputState = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &inputAssembly;
	pipelineInfo.pTessellationState = nullptr;
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &rasterizer;
	pipelineInfo.pMultisampleState = &multisampling;
	pipelineInfo.pDepthStencilState = nullptr;
	pipelineInfo.pColorBlendState = &colorBlending;
	pipelineInfo.pDynamicState = nullptr; // Optional
	pipelineInfo.layout = pipelineLayout;
	pipelineInfo.renderPass = vulkan->getRenderPass();
	pipelineInfo.subpass = 0;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
	pipelineInfo.basePipelineIndex = -1; // Optional

	if (vkCreateGraphicsPipelines(vulkan->getDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline) != VK_SUCCESS) {
		std::cout << "failed to create graphics pipeline!" << std::endl;
		exit(1);
	}

	std::cout << "Graphics Pipeline created." << std::endl;
}

void RenderObject::createDescriptorSet()
{
	uint32_t descriptorWritesCount = 2;
	VkDescriptorSetAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = descriptorPool;
	allocInfo.descriptorSetCount = 1;
	allocInfo.pSetLayouts = &descriptorSetLayout;

	if (vkAllocateDescriptorSets(vulkan->getDevice(), &allocInfo, &descriptorSet) != VK_SUCCESS) {
		std::cout << "Failed to allocate descriptor set!" << std::endl;
		exit(1);
	}

	VkDescriptorBufferInfo bufferInfo = {};
	bufferInfo.buffer = uniformBuffer->getBuffer();
	bufferInfo.offset = 0;
	bufferInfo.range = 3 * sizeof(mat4);

	VkDescriptorBufferInfo materialBufferInfo = {};
	materialBufferInfo.buffer = uniformBuffer->getBuffer();
	materialBufferInfo.offset = 0x100;
	materialBufferInfo.range = sizeof(color);

	VkDescriptorImageInfo imageInfo = {};
	if (texture)
	{
		imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageInfo.imageView = texture->getTextureImageView();
		imageInfo.sampler = texture->getTextureSampler();

		descriptorWritesCount = 3;
	}

	VkWriteDescriptorSet descriptorWrites[3] = {};
	descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrites[0].dstSet = descriptorSet;
	descriptorWrites[0].dstBinding = 0;
	descriptorWrites[0].dstArrayElement = 0;
	descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	descriptorWrites[0].descriptorCount = 1;
	descriptorWrites[0].pBufferInfo = &bufferInfo;
	descriptorWrites[0].pImageInfo = nullptr; // Optional
	descriptorWrites[0].pTexelBufferView = nullptr; // Optional

	descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrites[1].dstSet = descriptorSet;
	descriptorWrites[1].dstBinding = 1;
	descriptorWrites[1].dstArrayElement = 0;
	descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	descriptorWrites[1].descriptorCount = 1;
	descriptorWrites[1].pBufferInfo = &materialBufferInfo;
	descriptorWrites[1].pImageInfo = nullptr; // Optional
	descriptorWrites[1].pTexelBufferView = nullptr; // Optional

	descriptorWrites[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrites[2].dstSet = descriptorSet;
	descriptorWrites[2].dstBinding = 2;
	descriptorWrites[2].dstArrayElement = 0;
	descriptorWrites[2].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	descriptorWrites[2].descriptorCount = 1;
	descriptorWrites[2].pImageInfo = &imageInfo;

	vkUpdateDescriptorSets(vulkan->getDevice(), descriptorWritesCount, descriptorWrites, 0, nullptr);
}

uint32_t RenderObject::getPushConstantRangeCount() { return pushConstantRangeCount; }
VkPushConstantRange RenderObject::getPushConstantRange() { return *pPushConstantRange; }
VkPipelineLayout RenderObject::getPipelineLayout() { return pipelineLayout; }
VkPipeline RenderObject::getGraphicsPipeline() { return graphicsPipeline; }
VkDescriptorSet* RenderObject::getDescriptorSetPtr() { return &descriptorSet; }
VkBuffer RenderObject::getTextOverlayVertexBuffer() { return textOverlay->vertexBuffer->getBuffer(); }
uint32_t RenderObject::getNumLetters() { return textOverlay->numLetters; }
uint64_t RenderObject::getVertexOffset() { return vertexOffset; }
uint32_t RenderObject::getIndexCount() { return indexCount; }
uint32_t RenderObject::getFirstIndex() { return firstIndex; }