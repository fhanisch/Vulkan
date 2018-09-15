#pragma once

#include "Vulkan.h"

class Square : public RenderObject
{
public:
	Square(	VulkanSetup *_vulkanSetup,
			VkDescriptorPool _descriptorPool,
			TextOverlay *_textOverlay,
			mat4 *_mView,
			bool *_key,
			VertexData *vertexData,
			IndexData *indexData);
	~Square();
	void motion();
};

class Tacho : public RenderObject
{
public:
	Tacho(	VulkanSetup *_vulkanSetup,
			VkDescriptorPool _descriptorPool,
			TextOverlay *_textOverlay,
			mat4 *_mView,
			bool *_key,
			VertexData *vertexData,
			IndexData *indexData);
	~Tacho();
};

class FlatPerlin2d : public RenderObject
{
public:
	FlatPerlin2d(	VulkanSetup *_vulkanSetup,
					VkDescriptorPool _descriptorPool,
					TextOverlay *_textOverlay,
					mat4 *_mView,
					bool *_key,
					VertexData *vertexData,
					IndexData *indexData);
	~FlatPerlin2d();
};

class Star : public RenderObject
{
public:
	Star(	VulkanSetup *_vulkanSetup,
			VkDescriptorPool _descriptorPool,
			TextOverlay *_textOverlay,
			mat4 *_mView,
			bool *_key,
			VertexData *vertexData,
			IndexData *indexData);
	~Star();
	void motion();
};

class FilledCircle : public RenderObject
{
public:
	FilledCircle(	VulkanSetup *_vulkanSetup,
					VkDescriptorPool _descriptorPool,
					TextOverlay *_textOverlay,
					mat4 *_mView,
					bool *_key,
					VertexData *vertexData,
					IndexData *indexData);
	~FilledCircle();
	void motion();
};

class PerlinCircle : public RenderObject
{
protected:
	Buffer *commandBuffer;
	struct PushConstants
	{
		float seed_u;
		float seed_v;
	} pushConsts;
public:
	PerlinCircle(	VulkanSetup *_vulkanSetup,
					VkDescriptorPool _descriptorPool,
					TextOverlay *_textOverlay,
					mat4 *_mView,
					bool *_key,
					VertexData *vertexData,
					IndexData *indexData);
	~PerlinCircle();
	void motion();
};

class Wave : public RenderObject
{
public:
	Wave(	VulkanSetup *_vulkanSetup,
			VkDescriptorPool _descriptorPool,
			TextOverlay *_textOverlay,
			mat4 *_mView,
			bool *_key,
			VertexData *vertexData,
			IndexData *indexData);
	~Wave();
	void updateUniformBuffer();
};

class Perlin1d : public RenderObject
{
public:
	Perlin1d(	VulkanSetup *_vulkanSetup,
				VkDescriptorPool _descriptorPool,
				TextOverlay *_textOverlay,
				mat4 *_mView,
				bool *_key,
				VertexData *vertexData,
				IndexData *indexData);
	~Perlin1d();
};

class CurveTessellator : public RenderObject
{
public:
	CurveTessellator(	VulkanSetup *_vulkanSetup,
						VkDescriptorPool _descriptorPool,
						TextOverlay *_textOverlay,
						mat4 *_mView,
						bool *_key,
						VertexData *vertexData,
						IndexData *indexData);
	~CurveTessellator();
};

class Perlin1dTessellator : public RenderObject
{
public:
	Perlin1dTessellator(VulkanSetup *_vulkanSetup,
						VkDescriptorPool _descriptorPool,
						TextOverlay *_textOverlay,
						mat4 *_mView,
						bool *_key,
						VertexData *vertexData,
						IndexData *indexData);
	~Perlin1dTessellator();
};

class TxtObj : public RenderObject
{
public:
	TxtObj(	VulkanSetup *_vulkanSetup,
			VkDescriptorPool _descriptorPool,
			TextOverlay *_textOverlay,
			mat4 *_mView,
			bool *_key,
			VertexData *vertexData,
			IndexData *indexData);
	~TxtObj();
};