#pragma once
#include "Vulkan.h"
#include "RenderObject.h"
#include "Texture.h"
#include "Vertex.h"

class Square : public RenderObject
{
public:
	Square(Vulkan* _vulkan,
		VkDescriptorPool _descriptorPool,
		TextOverlay* _textOverlay,
		mat4* _mView,
		bool* _key,
		MousePos* _mousePos,
		VertexData* vertexData,
		IndexData* indexData);
	~Square();
	void motion();
};

class Tacho : public RenderObject
{
public:
	Tacho(Vulkan* _vulkan,
		VkDescriptorPool _descriptorPool,
		TextOverlay* _textOverlay,
		mat4* _mView,
		bool* _key,
		MousePos* _mousePos,
		VertexData* vertexData,
		IndexData* indexData);
	~Tacho();
};

class FlatPerlin2d : public RenderObject
{
public:
	FlatPerlin2d(Vulkan* _vulkan,
		VkDescriptorPool _descriptorPool,
		TextOverlay* _textOverlay,
		mat4* _mView,
		bool* _key,
		MousePos* _mousePos,
		VertexData* vertexData,
		IndexData* indexData);
	~FlatPerlin2d();
};

class Star : public RenderObject
{
public:
	Star(Vulkan* _vulkan,
		VkDescriptorPool _descriptorPool,
		TextOverlay* _textOverlay,
		mat4* _mView,
		bool* _key,
		MousePos* _mousePos,
		VertexData* vertexData,
		IndexData* indexData);
	~Star();
	void motion();
};

class FilledCircle : public RenderObject
{
public:
	FilledCircle(Vulkan* _vulkan,
		VkDescriptorPool _descriptorPool,
		TextOverlay* _textOverlay,
		mat4* _mView,
		bool* _key,
		MousePos* _mousePos,
		VertexData* vertexData,
		IndexData* indexData);
	~FilledCircle();
	void motion();
};

class PerlinCircle : public RenderObject
{
protected:
	Buffer* commandBuffer;
	struct PushConstants
	{
		float seed_u;
		float seed_v;
	} pushConsts;
public:
	PerlinCircle(Vulkan* _vulkan,
		VkDescriptorPool _descriptorPool,
		TextOverlay* _textOverlay,
		mat4* _mView,
		bool* _key,
		MousePos* _mousePos,
		VertexData* vertexData,
		IndexData* indexData);
	~PerlinCircle();
	void motion();
};

class Wave : public RenderObject
{
public:
	Wave(Vulkan* _vulkan,
		VkDescriptorPool _descriptorPool,
		TextOverlay* _textOverlay,
		mat4* _mView,
		bool* _key,
		MousePos* _mousePos,
		VertexData* vertexData,
		IndexData* indexData);
	~Wave();
	void updateUniformBuffer();
};

class Perlin1d : public RenderObject
{
public:
	Perlin1d(Vulkan* _vulkan,
		VkDescriptorPool _descriptorPool,
		TextOverlay* _textOverlay,
		mat4* _mView,
		bool* _key,
		MousePos* _mousePos,
		VertexData* vertexData,
		IndexData* indexData);
	~Perlin1d();
};

class TxtObj : public RenderObject
{
public:
	TxtObj(Vulkan* _vulkan,
		VkDescriptorPool _descriptorPool,
		TextOverlay* _textOverlay,
		mat4* _mView,
		bool* _key,
		MousePos* _mousePos,
		VertexData* vertexData,
		IndexData* indexData);
	~TxtObj();
};

class Graph : public RenderObject
{
	Buffer *vertexBuffer;
	VkDeviceSize vertexBufferSize;
	uint32_t vertexCount;
public:
	Graph(Vulkan* _vulkan,
		VkDescriptorPool _descriptorPool,
		TextOverlay* _textOverlay,
		mat4* _mView,
		bool* _key,
		MousePos* _mousePos,
		VertexData* vertexData,
		IndexData* indexData);
	~Graph();
	VkBuffer getVertexBuffer();
	uint32_t getVertexCount();
};