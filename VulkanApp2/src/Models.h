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

class Circle : public RenderObject
{
public:
	Circle(	VulkanSetup *_vulkanSetup,
			VkDescriptorPool _descriptorPool,
			TextOverlay *_textOverlay,
			mat4 *_mView,
			bool *_key,
			VertexData *vertexData,
			IndexData *indexData);
	~Circle();
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