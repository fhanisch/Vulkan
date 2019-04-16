#pragma once
#include <time.h>
#include <matrix.h>
#include "Vulkan.h"
#include "Shader.h"
#include "Vertex.h"
#include "Texture.h"

class RenderObject
{
protected:
	// Attributs
	Vulkan* vulkan;
	VkDeviceSize uboBufferSize;
	Buffer* uniformBuffer;
	VkDescriptorSetLayout descriptorSetLayout;
	VkDescriptorPool descriptorPool;
	Shader vertexShader;
	Shader fragmentShader;
	Shader tessellationControlShader;
	Shader tessellationEvaluationShader;
	uint32_t stageCount;
	VkVertexInputBindingDescription bindingDescription;
	uint32_t attributeDescriptionCount;
	VkVertexInputAttributeDescription* pAttributeDescriptions;
	VkPrimitiveTopology topology;
	uint32_t pushConstantRangeCount;
	VkPushConstantRange* pPushConstantRange;
	VkPipelineLayout pipelineLayout;
	VkPipeline graphicsPipeline;
	Texture* texture;
	TextOverlay* textOverlay;
	VkDescriptorSet descriptorSet;
	vec4 color;
	uint64_t vertexOffset;
	uint32_t indexCount;
	uint32_t firstIndex;
	clock_t startTime;
	bool* key;
	MousePos* mousePos;
	// Methods
	VkShaderModule createShaderModule(Shader shader);
	VkPipelineShaderStageCreateInfo getShaderStageInfo(VkShaderStageFlagBits stage, VkShaderModule module);
	VkVertexInputBindingDescription getBindingDescription(uint32_t stride);
	VkVertexInputAttributeDescription* getAttributeDescriptions(uint32_t count, VkFormat* formats, uint32_t* offsets);
	VkPushConstantRange* createPushConstantRange(VkShaderStageFlags shaderStageFlags, uint32_t size);
	void createUniformBuffer();
	void createDescriptorSetLayout();
	void createPipelineLayout();
	void createGraphicsPipeline();
	void createDescriptorSet();
public:
	mat4 mModel;
	mat4* mView;
	mat4 mProj;
	RenderObject(Vulkan* _vulkan,
		VkDescriptorPool _descriptorPool,
		TextOverlay* _textOverlay,
		mat4* _mView,
		bool* _key,
		MousePos* _mousePos);
	~RenderObject();
	virtual void updateUniformBuffer();
	virtual void motion();
	uint32_t getPushConstantRangeCount();
	VkPushConstantRange getPushConstantRange();
	VkPipelineLayout getPipelineLayout();
	VkPipeline getGraphicsPipeline();
	VkDescriptorSet* getDescriptorSetPtr();
	VkBuffer getTextOverlayVertexBuffer();
	uint32_t getNumLetters();
	uint64_t getVertexOffset();
	uint32_t getIndexCount();
	uint32_t getFirstIndex();
};