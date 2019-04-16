#pragma once
#include <matrix.h>
#include "Vulkan.h"
#include "RenderObject.h"
#include "Vertex.h"
#include "Texture.h"
#include "Models.h"

const Vertex verticesPlane[] = {
	{ {  -1.0f,   1.0f,  0.0f }, { 1.0f, 0.0f, 1.0f }, { 0.0f, 0.0f } },
	{ {   1.0f,   1.0f,  0.0f }, { 1.0f, 0.0f, 1.0f }, { 1.0f, 0.0f } },
	{ {   1.0f,  -1.0f,  0.0f }, { 1.0f, 0.0f, 1.0f }, { 1.0f, 1.0f } },
	{ {  -1.0f,  -1.0f,  0.0f }, { 1.0f, 0.0f, 1.0f }, { 0.0f, 1.0f } }
};

const Vertex verticesStar[] = {
	{ {   0.0f,  -1.0f,  0.0f },  { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f } },
	{ {  0.25f, -0.25f,  0.0f },  { 1.0f, 0.0f, 1.0f }, { 0.0f, 0.0f } },
	{ { -0.25f, -0.25f,  0.0f },  { 1.0f, 0.0f, 1.0f }, { 0.0f, 0.0f } },
	{ {   1.0f,   0.0f,  0.0f },  { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f } },
	{ {  0.25f,  0.25f,  0.0f },  { 1.0f, 0.0f, 1.0f }, { 0.0f, 0.0f } },
	{ {   0.0f,   1.0f,  0.0f },  { 1.0f, 1.0f, 0.0f }, { 0.0f, 0.0f } },
	{ {  -0.25,  0.25f,  0.0f },  { 1.0f, 0.0f, 1.0f }, { 0.0f, 0.0f } },
	{ {  -1.0f,   0.0f,  0.0f },  { 1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f } }
};
static float* verticesCurve;
static uint32_t verticesCurveSize;

const Vertex verticesLine[] = {
	{ {  -1.0f,   0.0f,  0.0f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f } },
	{ {   1.0f,   0.0f,  0.0f }, { 1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f } }
};

const uint16_t indicesPlane[] = { 0, 1, 2, 2, 3, 0 };
const uint16_t indicesStar[] = { 0, 1, 2, 1, 3, 4, 4, 5, 6, 6, 7, 2, 2, 1, 4, 4, 6, 2 };
static uint16_t* indicesCurve;
static uint32_t indicesCurveSize;

class RenderScene
{
protected:
	bool* key;
	MousePos* mousePos;
	mat4 mView;
	Vulkan* vulkan;
	uint32_t objectCount;
	RenderObject** obj;
	TxtObj* txtObj;
	Graph* graph;
	VertexData* vertexData;
	IndexData* indexData;
	Buffer* vertexBuffer;
	Buffer* indexBuffer;
	TextOverlay* textOverlay;
	VkDescriptorPool descriptorPool;
	VkCommandBuffer* commandBuffers;
	void printMatrix(mat4 M, float x, float y);
	void createVertexBuffer();
	void createIndexBuffer();
	void createDescriptorPool();
	void createCommandBuffers();
public:
	RenderScene(Vulkan* _vulkan, bool* _key, MousePos* _mousePos);
	~RenderScene();
	void updateUniformBuffers();
	void camMotion();
	void updateTextOverlay(uint32_t fps);
	void drawFrame();
};