#ifndef VULKAN_SETUP_H
#define VULKAN_SETUP_H

#define VK_NO_PROTOTYPES
#ifdef ANDROID
#define VK_USE_PLATFORM_ANDROID_KHR
#elif LINUX
#define VK_USE_PLATFORM_XLIB_KHR
#elif WINDOWS
#define VK_USE_PLATFORM_WIN32_KHR
#endif

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <vulkan/vulkan.h>
#include "Window.h"
#include "matrix.h"

#ifdef WINDOWS
#define KEY_ESC VK_ESCAPE
#define KEY_SPACE VK_SPACE
#define KEY_LEFT VK_LEFT
#define KEY_RIGHT VK_RIGHT
#define KEY_UP VK_UP
#define KEY_DOWN VK_DOWN
#define KEY_SHIFT VK_SHIFT
#define KEY_A 0x41
#define KEY_D 0x44
#define KEY_W 0x57
#define KEY_S 0x53
#define KEY_X 0x58
#define KEY_Y 0x59
#else
#define KEY_ESC 0x9
#define KEY_SPACE 0x41
#define KEY_LEFT 0x71
#define KEY_RIGHT 0x72
#define KEY_UP 0x6f
#define KEY_DOWN 0x74
#define KEY_SHIFT 0xff
#define KEY_A 0xff
#define KEY_D 0xff
#define KEY_W 0x19
#define KEY_S 0x27
#define KEY_X 0xff
#define KEY_Y 0xff
#endif

char* strCat(const char* dest, const char* src);

struct Vertex {
	vec3 pos;
	vec3 normal;
	vec3 color;
	vec2 texCoords;
};

class VertexData
{
protected:
	float *data;
	uint64_t size;
	uint64_t *offsets;
	uint32_t dataSetCount;
public:
	VertexData();
	~VertexData();
	void addData(float *_data, uint64_t _size);
	float *getData();
	uint64_t getSize();
	uint64_t getOffset(uint32_t index);
};

class IndexData
{
protected:
	uint16_t *data;
	uint64_t size;
	uint32_t *indexCount;
	uint32_t *firstIndex;
	uint32_t dataSetCount;
public:
	IndexData();
	~IndexData();
	void addData(uint16_t *_data, uint64_t _size);
	uint16_t *getData();
	uint64_t getSize();
	uint32_t getIndexCount(uint32_t index);
	uint32_t getFirstIndex(uint32_t index);
};

const Vertex verticesPlane[] = {
	{ {  -1.0f,   1.0f,  0.0f }, { 0.0f, 0.0f, 1.0f }, { 1.0f, 0.0f, 1.0f }, { 0.0f, 0.0f } },
	{ {   1.0f,   1.0f,  0.0f }, { 0.0f, 0.0f, 1.0f }, { 1.0f, 0.0f, 1.0f }, { 1.0f, 0.0f } },
	{ {   1.0f,  -1.0f,  0.0f }, { 0.0f, 0.0f, 1.0f }, { 1.0f, 0.0f, 1.0f }, { 1.0f, 1.0f } },
	{ {  -1.0f,  -1.0f,  0.0f }, { 0.0f, 0.0f, 1.0f }, { 1.0f, 0.0f, 1.0f }, { 0.0f, 1.0f } }
};

const Vertex verticesCube[] = {
	{ {  -1.0f,   1.0f,  0.0f }, {  0.0f,  1.0f,  0.0f }, { 1.0f, 0.0f, 1.0f }, { 0.0f, 0.0f } },
	{ {   1.0f,   1.0f,  0.0f }, {	0.0f,  0.0f, -1.0f }, { 1.0f, 0.0f, 1.0f }, { 1.0f, 0.0f } },
	{ {   1.0f,  -1.0f,  0.0f }, {	0.0f,  0.0f,  0.0f }, { 1.0f, 0.0f, 1.0f }, { 1.0f, 1.0f } },
	{ {  -1.0f,  -1.0f,  0.0f }, {	0.0f,  0.0f,  0.0f }, { 1.0f, 0.0f, 1.0f }, { 0.0f, 1.0f } },

	{ {  -1.0f,   1.0f,  0.0f }, {	0.0f,  1.0f,  0.0f }, { 1.0f, 0.0f, 1.0f }, { 0.0f, 0.0f } },
	{ {   1.0f,   1.0f,  0.0f }, {	0.0f,  0.0f,  1.0f }, { 1.0f, 0.0f, 1.0f }, { 1.0f, 0.0f } },
	{ {   1.0f,  -1.0f,  0.0f }, {	0.0f,  0.0f,  0.0f }, { 1.0f, 0.0f, 1.0f }, { 1.0f, 1.0f } },
	{ {  -1.0f,  -1.0f,  0.0f }, {	0.0f,  0.0f,  0.0f }, { 1.0f, 0.0f, 1.0f }, { 0.0f, 1.0f } },

	{ {  -1.0f,   1.0f,  0.0f }, {  0.0f,  1.0f,  0.0f }, { 1.0f, 0.0f, 1.0f }, { 0.0f, 0.0f } },
	{ {   1.0f,   1.0f,  0.0f }, {  1.0f,  0.0f,  0.0f }, { 1.0f, 0.0f, 1.0f }, { 1.0f, 0.0f } },
	{ {   1.0f,  -1.0f,  0.0f }, {  0.0f,  0.0f,  0.0f }, { 1.0f, 0.0f, 1.0f }, { 1.0f, 1.0f } },
	{ {  -1.0f,  -1.0f,  0.0f }, {  0.0f,  0.0f,  0.0f }, { 1.0f, 0.0f, 1.0f }, { 0.0f, 1.0f } },

	{ {  -1.0f,   1.0f,  0.0f }, {  0.0f,  1.0f,  0.0f }, { 1.0f, 0.0f, 1.0f }, { 0.0f, 0.0f } },
	{ {   1.0f,   1.0f,  0.0f }, { -1.0f,  0.0f,  0.0f }, { 1.0f, 0.0f, 1.0f }, { 1.0f, 0.0f } },
	{ {   1.0f,  -1.0f,  0.0f }, {  0.0f,  0.0f,  0.0f }, { 1.0f, 0.0f, 1.0f }, { 1.0f, 1.0f } },
	{ {  -1.0f,  -1.0f,  0.0f }, {  0.0f,  0.0f,  0.0f }, { 1.0f, 0.0f, 1.0f }, { 0.0f, 1.0f } },

	{ {  -1.0f,   1.0f,  0.0f }, {  0.0f,  0.0f,  0.0f }, { 1.0f, 0.0f, 1.0f }, { 0.0f, 0.0f } },
	{ {   1.0f,   1.0f,  0.0f }, {  0.0f,  0.0f,  0.0f }, { 1.0f, 0.0f, 1.0f }, { 1.0f, 0.0f } },
	{ {   1.0f,  -1.0f,  0.0f }, { -1.0f,  0.0f,  0.0f }, { 1.0f, 0.0f, 1.0f }, { 1.0f, 1.0f } },
	{ {  -1.0f,  -1.0f,  0.0f }, {  0.0f,  0.0f, -1.0f }, { 1.0f, 0.0f, 1.0f }, { 0.0f, 1.0f } },

	{ {  -1.0f,   1.0f,  0.0f }, {  0.0f,  0.0f,  0.0f }, { 1.0f, 0.0f, 1.0f }, { 0.0f, 0.0f } },
	{ {   1.0f,   1.0f,  0.0f }, {  0.0f,  0.0f,  0.0f }, { 1.0f, 0.0f, 1.0f }, { 1.0f, 0.0f } },
	{ {   1.0f,  -1.0f,  0.0f }, { -1.0f,  0.0f,  0.0f }, { 1.0f, 0.0f, 1.0f }, { 1.0f, 1.0f } },
	{ {  -1.0f,  -1.0f,  0.0f }, {  0.0f,  0.0f,  1.0f }, { 1.0f, 0.0f, 1.0f }, { 0.0f, 1.0f } }
};

const Vertex verticesStar[] = {
	{ {   0.0f,  -1.0f,  0.0f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f } },
	{ {  0.25f, -0.25f,  0.0f }, { 0.0f, 0.0f, 1.0f }, { 1.0f, 0.0f, 1.0f }, { 0.0f, 0.0f } },
	{ { -0.25f, -0.25f,  0.0f }, { 0.0f, 0.0f, 1.0f }, { 1.0f, 0.0f, 1.0f }, { 0.0f, 0.0f } },
	{ {   1.0f,   0.0f,  0.0f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f } },
	{ {  0.25f,  0.25f,  0.0f }, { 0.0f, 0.0f, 1.0f }, { 1.0f, 0.0f, 1.0f }, { 0.0f, 0.0f } },
	{ {   0.0f,   1.0f,  0.0f }, { 0.0f, 0.0f, 1.0f }, { 1.0f, 1.0f, 0.0f }, { 0.0f, 0.0f } },
	{ {  -0.25,  0.25f,  0.0f }, { 0.0f, 0.0f, 1.0f }, { 1.0f, 0.0f, 1.0f }, { 0.0f, 0.0f } },
	{ {  -1.0f,   0.0f,  0.0f }, { 0.0f, 0.0f, 1.0f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f } }
};
const float verticesPatches[] = { 0.0f, 1.0f, 2.0f };
static float *verticesCurve;
static uint32_t verticesCurveSize;
static float *verticesPatches2;
static uint32_t verticesPatches2Size;
static float *meshGridVertices;
static uint32_t meshGridVerticesSize;

const float teapotPatchVertices[] = {
											1.4f,0.0f,2.4f,
											1.4f,-0.784f,2.4f,
											0.784f,-1.4f,2.4f,
											0.0f,-1.4f,2.4f,
											1.3375f,0.0f,2.53125f,
											1.3375f,-0.749f,2.53125f,
											0.749f,-1.3375f,2.53125f,
											0.0f,-1.3375f,2.53125f,
											1.4375f,0.0f,2.53125f,
											1.4375f,-0.805f,2.53125f,
											0.805f,-1.4375f,2.53125f,
											0.0f,-1.4375f,2.53125f,
											1.5f,0.0f,2.4f,
											1.5f,-0.84f,2.4f,
											0.84f,-1.5f,2.4f,
											0.0f,-1.5f,2.4f,
											-0.784f,-1.4f,2.4f,
											-1.4f,-0.784f,2.4f,
											-1.4f,0.0f,2.4f,
											-0.749f,-1.3375f,2.53125f,
											-1.3375f,-0.749f,2.53125f,
											-1.3375f,0.0f,2.53125f,
											-0.805f,-1.4375f,2.53125f,
											-1.4375f,-0.805f,2.53125f,
											-1.4375f,0.0f,2.53125f,
											-0.84f,-1.5f,2.4f,
											-1.5f,-0.84f,2.4f,
											-1.5f,0.0f,2.4f,
											-1.4f,0.784f,2.4f,
											-0.784f,1.4f,2.4f,
											0.0f,1.4f,2.4f,
											-1.3375f,0.749f,2.53125f,
											-0.749f,1.3375f,2.53125f,
											0.0f,1.3375f,2.53125f,
											-1.4375f,0.805f,2.53125f,
											-0.805f,1.4375f,2.53125f,
											0.0f,1.4375f,2.53125f,
											-1.5f,0.84f,2.4f,
											-0.84f,1.5f,2.4f,
											0.0f,1.5f,2.4f,
											0.784f,1.4f,2.4f,
											1.4f,0.784f,2.4f,
											0.749f,1.3375f,2.53125f,
											1.3375f,0.749f,2.53125f,
											0.805f,1.4375f,2.53125f,
											1.4375f,0.805f,2.53125f,
											0.84f,1.5f,2.4f,
											1.5f,0.84f,2.4f,
											1.75f,0.0f,1.875f,
											1.75f,-0.98f,1.875f,
											0.98f,-1.75f,1.875f,
											0.0f,-1.75f,1.875f,
											2.0f,0.0f,1.35f,
											2.0f,-1.12f,1.35f,
											1.12f,-2.0f,1.35f,
											0.0f,-2.0f,1.35f,
											2.0f,0.0f,0.9f,
											2.0f,-1.12f,0.9f,
											1.12f,-2.0f,0.9f,
											0.0f,-2.0f,0.9f,
											-0.98f,-1.75f,1.875f,
											-1.75f,-0.98f,1.875f,
											-1.75f,0.0f,1.875f,
											-1.12f,-2.0f,1.35f,
											-2.0f,-1.12f,1.35f,
											-2.0f,0.0f,1.35f,
											-1.12f,-2.0f,0.9f,
											-2.0f,-1.12f,0.9f,
											-2.0f,0.0f,0.9f,
											-1.75f,0.98f,1.875f,
											-0.98f,1.75f,1.875f,
											0.0f,1.75f,1.875f,
											-2.0f,1.12f,1.35f,
											-1.12f,2.0f,1.35f,
											0.0f,2.0f,1.35f,
											-2.0f,1.12f,0.9f,
											-1.12f,2.0f,0.9f,
											0.0f,2.0f,0.9f,
											0.98f,1.75f,1.875f,
											1.75f,0.98f,1.875f,
											1.12f,2.0f,1.35f,
											2.0f,1.12f,1.35f,
											1.12f,2.0f,0.9f,
											2.0f,1.12f,0.9f,
											2.0f,0.0f,0.45f,
											2.0f,-1.12f,0.45f,
											1.12f,-2.0f,0.45f,
											0.0f,-2.0f,0.45f,
											1.5f,0.0f,0.225f,
											1.5f,-0.84f,0.225f,
											0.84f,-1.5f,0.225f,
											0.0f,-1.5f,0.225f,
											1.5f,0.0f,0.15f,
											1.5f,-0.84f,0.15f,
											0.84f,-1.5f,0.15f,
											0.0f,-1.5f,0.15f,
											-1.12f,-2.0f,0.45f,
											-2.0f,-1.12f,0.45f,
											-2.0f,0.0f,0.45f,
											-0.84f,-1.5f,0.225f,
											-1.5f,-0.84f,0.225f,
											-1.5f,0.0f,0.225f,
											-0.84f,-1.5f,0.15f,
											-1.5f,-0.84f,0.15f,
											-1.5f,0.0f,0.15f,
											-2.0f,1.12f,0.45f,
											-1.12f,2.0f,0.45f,
											0.0f,2.0f,0.45f,
											-1.5f,0.84f,0.225f,
											-0.84f,1.5f,0.225f,
											0.0f,1.5f,0.225f,
											-1.5f,0.84f,0.15f,
											-0.84f,1.5f,0.15f,
											0.0f,1.5f,0.15f,
											1.12f,2.0f,0.45f,
											2.0f,1.12f,0.45f,
											0.84f,1.5f,0.225f,
											1.5f,0.84f,0.225f,
											0.84f,1.5f,0.15f,
											1.5f,0.84f,0.15f,
											-1.6f,0.0f,2.025f,
											-1.6f,-0.3f,2.025f,
											-1.5f,-0.3f,2.25f,
											-1.5f,0.0f,2.25f,
											-2.3f,0.0f,2.025f,
											-2.3f,-0.3f,2.025f,
											-2.5f,-0.3f,2.25f,
											-2.5f,0.0f,2.25f,
											-2.7f,0.0f,2.025f,
											-2.7f,-0.3f,2.025f,
											-3.0f,-0.3f,2.25f,
											-3.0f,0.0f,2.25f,
											-2.7f,0.0f,1.8f,
											-2.7f,-0.3f,1.8f,
											-3.0f,-0.3f,1.8f,
											-3.0f,0.0f,1.8f,
											-1.5f,0.3f,2.25f,
											-1.6f,0.3f,2.025f,
											-2.5f,0.3f,2.25f,
											-2.3f,0.3f,2.025f,
											-3.0f,0.3f,2.25f,
											-2.7f,0.3f,2.025f,
											-3.0f,0.3f,1.8f,
											-2.7f,0.3f,1.8f,
											-2.7f,0.0f,1.575f,
											-2.7f,-0.3f,1.575f,
											-3.0f,-0.3f,1.35f,
											-3.0f,0.0f,1.35f,
											-2.5f,0.0f,1.125f,
											-2.5f,-0.3f,1.125f,
											-2.65f,-0.3f,0.9375f,
											-2.65f,0.0f,0.9375f,
											-2.0f,-0.3f,0.9f,
											-1.9f,-0.3f,0.6f,
											-1.9f,0.0f,0.6f,
											-3.0f,0.3f,1.35f,
											-2.7f,0.3f,1.575f,
											-2.65f,0.3f,0.9375f,
											-2.5f,0.3f,1.125f,
											-1.9f,0.3f,0.6f,
											-2.0f,0.3f,0.9f,
											1.7f,0.0f,1.425f,
											1.7f,-0.66f,1.425f,
											1.7f,-0.66f,0.6f,
											1.7f,0.0f,0.6f,
											2.6f,0.0f,1.425f,
											2.6f,-0.66f,1.425f,
											3.1f,-0.66f,0.825f,
											3.1f,0.0f,0.825f,
											2.3f,0.0f,2.1f,
											2.3f,-0.25f,2.1f,
											2.4f,-0.25f,2.025f,
											2.4f,0.0f,2.025f,
											2.7f,0.0f,2.4f,
											2.7f,-0.25f,2.4f,
											3.3f,-0.25f,2.4f,
											3.3f,0.0f,2.4f,
											1.7f,0.66f,0.6f,
											1.7f,0.66f,1.425f,
											3.1f,0.66f,0.825f,
											2.6f,0.66f,1.425f,
											2.4f,0.25f,2.025f,
											2.3f,0.25f,2.1f,
											3.3f,0.25f,2.4f,
											2.7f,0.25f,2.4f,
											2.8f,0.0f,2.475f,
											2.8f,-0.25f,2.475f,
											3.525f,-0.25f,2.49375f,
											3.525f,0.0f,2.49375f,
											2.9f,0.0f,2.475f,
											2.9f,-0.15f,2.475f,
											3.45f,-0.15f,2.5125f,
											3.45f,0.0f,2.5125f,
											2.8f,0.0f,2.4f,
											2.8f,-0.15f,2.4f,
											3.2f,-0.15f,2.4f,
											3.2f,0.0f,2.4f,
											3.525f,0.25f,2.49375f,
											2.8f,0.25f,2.475f,
											3.45f,0.15f,2.5125f,
											2.9f,0.15f,2.475f,
											3.2f,0.15f,2.4f,
											2.8f,0.15f,2.4f,
											0.0f,0.0f,3.15f,
											0.0f,-0.002f,3.15f,
											0.002f,0.0f,3.15f,
											0.8f,0.0f,3.15f,
											0.8f,-0.45f,3.15f,
											0.45f,-0.8f,3.15f,
											0.0f,-0.8f,3.15f,
											0.0f,0.0f,2.85f,
											0.2f,0.0f,2.7f,
											0.2f,-0.112f,2.7f,
											0.112f,-0.2f,2.7f,
											0.0f,-0.2f,2.7f,
											-0.002f,0.0f,3.15f,
											-0.45f,-0.8f,3.15f,
											-0.8f,-0.45f,3.15f,
											-0.8f,0.0f,3.15f,
											-0.112f,-0.2f,2.7f,
											-0.2f,-0.112f,2.7f,
											-0.2f,0.0f,2.7f,
											0.0f,0.002f,3.15f,
											-0.8f,0.45f,3.15f,
											-0.45f,0.8f,3.15f,
											0.0f,0.8f,3.15f,
											-0.2f,0.112f,2.7f,
											-0.112f,0.2f,2.7f,
											0.0f,0.2f,2.7f,
											0.45f,0.8f,3.15f,
											0.8f,0.45f,3.15f,
											0.112f,0.2f,2.7f,
											0.2f,0.112f,2.7f,
											0.4f,0.0f,2.55f,
											0.4f,-0.224f,2.55f,
											0.224f,-0.4f,2.55f,
											0.0f,-0.4f,2.55f,
											1.3f,0.0f,2.55f,
											1.3f,-0.728f,2.55f,
											0.728f,-1.3f,2.55f,
											0.0f,-1.3f,2.55f,
											1.3f,0.0f,2.4f,
											1.3f,-0.728f,2.4f,
											0.728f,-1.3f,2.4f,
											0.0f,-1.3f,2.4f,
											-0.224f,-0.4f,2.55f,
											-0.4f,-0.224f,2.55f,
											-0.4f,0.0f,2.55f,
											-0.728f,-1.3f,2.55f,
											-1.3f,-0.728f,2.55f,
											-1.3f,0.0f,2.55f,
											-0.728f,-1.3f,2.4f,
											-1.3f,-0.728f,2.4f,
											-1.3f,0.0f,2.4f,
											-0.4f,0.224f,2.55f,
											-0.224f,0.4f,2.55f,
											0.0f,0.4f,2.55f,
											-1.3f,0.728f,2.55f,
											-0.728f,1.3f,2.55f,
											0.0f,1.3f,2.55f,
											-1.3f,0.728f,2.4f,
											-0.728f,1.3f,2.4f,
											0.0f,1.3f,2.4f,
											0.224f,0.4f,2.55f,
											0.4f,0.224f,2.55f,
											0.728f,1.3f,2.55f,
											1.3f,0.728f,2.55f,
											0.728f,1.3f,2.4f,
											1.3f,0.728f,2.4f,
											0.0f,0.0f,0.0f,
											1.5f,0.0f,0.15f,
											1.5f,0.84f,0.15f,
											0.84f,1.5f,0.15f,
											0.0f,1.5f,0.15f,
											1.5f,0.0f,0.075f,
											1.5f,0.84f,0.075f,
											0.84f,1.5f,0.075f,
											0.0f,1.5f,0.075f,
											1.425f,0.0f,0.0f,
											1.425f,0.798f,0.0f,
											0.798f,1.425f,0.0f,
											0.0f,1.425f,0.0f,
											-0.84f,1.5f,0.15f,
											-1.5f,0.84f,0.15f,
											-1.5f,0.0f,0.15f,
											-0.84f,1.5f,0.075f,
											-1.5f,0.84f,0.075f,
											-1.5f,0.0f,0.075f,
											-0.798f,1.425f,0.0f,
											-1.425f,0.798f,0.0f,
											-1.425f,0.0f,0.0f,
											-1.5f,-0.84f,0.15f,
											-0.84f,-1.5f,0.15f,
											0.0f,-1.5f,0.15f,
											-1.5f,-0.84f,0.075f,
											-0.84f,-1.5f,0.075f,
											0.0f,-1.5f,0.075f,
											-1.425f,-0.798f,0.0f,
											-0.798f,-1.425f,0.0f,
											0.0f,-1.425f,0.0f,
											0.84f,-1.5f,0.15f,
											1.5f,-0.84f,0.15f,
											0.84f,-1.5f,0.075f,
											1.5f,-0.84f,0.075f,
											0.798f,-1.425f,0.0f,
											1.425f,-0.798f,0.0f
};

const uint16_t indicesPlane[] = { 0, 1, 2, 2, 3, 0 };
const uint16_t indicesStar[] = { 0, 1, 2, 1, 3, 4, 4, 5, 6, 6, 7, 2, 2, 1, 4, 4, 6, 2 };
const uint16_t indicesPatches[] = { 0,1, 1,2 };
const uint16_t indicesCubeSpherePatches[] = { 0,1,2,3, 4,5,6,7, 8,9,10,11, 12,13,14,15, 16,17,18,19, 20,21,22,23 };
static uint16_t *indicesCurve;
static uint32_t indicesCurveSize;
static uint16_t *indicesPatches2;
static uint32_t indicesPatches2Size;
static uint16_t *meshGridIndices;
static uint32_t meshGridIndicesSize;

const uint16_t teapotPatchIndices[] = {
										0,	1,	2,	3,	4,	5,	6,	7,	8,	9,	10,	11,	12,	13,	14,	15,
										3,	16,	17,	18,	7,	19,	20,	21,	11,	22,	23,	24,	15,	25,	26,	27,
										18,	28,	29,	30,	21,	31,	32,	33,	24,	34,	35,	36,	27,	37,	38,	39,
										30,	40,	41,	0,	33,	42,	43,	4,	36,	44,	45,	8,	39,	46,	47,	12,
										12,	13,	14,	15,	48,	49,	50,	51,	52,	53,	54,	55,	56,	57,	58,	59,
										15,	25,	26,	27,	51,	60,	61,	62,	55,	63,	64,	65,	59,	66,	67,	68,
										27,	37,	38,	39,	62,	69,	70,	71,	65,	72,	73,	74,	68,	75,	76,	77,
										39,	46,	47,	12,	71,	78,	79,	48,	74,	80,	81,	52,	77,	82,	83,	56,
										56,	57,	58,	59,	84,	85,	86,	87,	88,	89,	90,	91,	92,	93,	94,	95,
										59,	66,	67,	68,	87,	96,	97,	98,	91,	99,	100,101,95,	102,103,104,
										68,	75,	76,	77,	98,	105,106,107,101,108,109,110,104,111,112,113,
										77,	82,	83,	56,	107,114,115,84,	110,116,117,88,	113,118,119,92,
										120,121,122,123,124,125,126,127,128,129,130,131,132,133,134,135,
										123,136,137,120,127,138,139,124,131,140,141,128,135,142,143,132,
										132,133,134,135,144,145,146,147,148,149,150,151,68,	152,153,154,
										135,142,143,132,147,155,156,144,151,157,158,148,154,159,160,68,
										161,162,163,164,165,166,167,168,169,170,171,172,173,174,175,176,
										164,177,178,161,168,179,180,165,172,181,182,169,176,183,184,173,
										173,174,175,176,185,186,187,188,189,190,191,192,193,194,195,196,
										176,183,184,173,188,197,198,185,192,199,200,189,196,201,202,193,
										203,203,203,203,206,207,208,209,210,210,210,210,211,212,213,214,
										203,203,203,203,209,216,217,218,210,210,210,210,214,219,220,221,
										203,203,203,203,218,223,224,225,210,210,210,210,221,226,227,228,
										203,203,203,203,225,229,230,206,210,210,210,210,228,231,232,211,
										211,212,213,214,233,234,235,236,237,238,239,240,241,242,243,244,
										214,219,220,221,236,245,246,247,240,248,249,250,244,251,252,253,
										221,226,227,228,247,254,255,256,250,257,258,259,253,260,261,262,
										228,231,232,211,256,263,264,233,259,265,266,237,262,267,268,241,
										269,269,269,269,278,279,280,281,274,275,276,277,270,271,272,273,
										269,269,269,269,281,288,289,290,277,285,286,287,273,282,283,284,
										269,269,269,269,290,297,298,299,287,294,295,296,284,291,292,293,
										269,269,269,269,299,304,305,278,296,302,303,274,293,300,301,270
};

class Buffer
{
protected:
	size_t size;
	VkBuffer buffer;
	VkDeviceMemory bufferMemory;
	VkCommandBuffer commandBuffer;
	VkPhysicalDevice physicalDevice;
	VkDevice device;
	VkCommandPool commandPool;
	VkQueue queue;
public:
	Buffer(VkPhysicalDevice _physicalDevice, VkDevice _device, VkCommandPool _commandPool, VkQueue _queue);
	~Buffer();
	uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
	void copyBuffer(VkBuffer srcBuffer, VkDeviceSize size);
	void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);
	void createDeviceLocalBuffer(const void *srcData, VkDeviceSize _size, VkBufferUsageFlags bufferUsageFlags);
	void beginSingleTimeCommands();
	void endSingleTimeCommands();
	VkBuffer getBuffer();
	VkCommandBuffer getCommandBuffer();
	VkDeviceMemory getBufferMemory();
};

class Image
{
protected:
	VkImage image;
	VkImageView imageView;
	VkDeviceMemory imageMemory;
	VkPhysicalDevice physicalDevice;
	VkDevice device;
	VkCommandPool commandPool;
	VkQueue queue;
	Buffer *commandBuffer;
public:
	Image(VkPhysicalDevice _physicalDevice, VkDevice _device, VkCommandPool _commandPool, VkQueue _queue);
	~Image();
	bool hasStencilComponent(VkFormat format);
	void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties);
	void createImageView(VkFormat format, VkImageAspectFlags aspectFlags);
	void transitionImageLayout(VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
	void copyBufferToImage(VkBuffer buffer, uint32_t width, uint32_t height);
	void setImage(VkImage _image);
	VkImageView getImageView();
};

class VulkanSetup
{
protected:
    const char* appName;
	const char* engineName;
	uint32_t apiVersion;
    MyWindow window;
    const char* libName;

    struct SwapChainSupportDetails {
		VkSurfaceCapabilitiesKHR capabilities;
		uint32_t formatCount;
		VkSurfaceFormatKHR *formats;
		uint32_t presentModeCount;
		VkPresentModeKHR *presentModes;
	};

    VkInstance instance;
	VkDebugUtilsMessengerEXT debugMessenger;
	VkDebugReportCallbackEXT debugReport;
	VkSurfaceKHR surface;
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkDevice device;
	VkQueue queue;
    VkSwapchainKHR swapChain;
    VkFormat swapChainImageFormat;
	VkExtent2D swapChainExtent;
    VkCommandPool commandPool;
    uint32_t swapChainImagesCount;
	Image **swapChainImages;
    VkRenderPass renderPass;
    VkDescriptorSetLayout descriptorSetLayout;
	Image *depthImage;
	VkFramebuffer *swapChainFramebuffers;
	VkSemaphore imageAvailableSemaphore;
	VkSemaphore renderFinishedSemaphore;

    void printSupportedApiVersion();
    void printLayers();
    void printExtensions();
    void createInstance();
	void setupDebugMessenger();
	void setupDebugReport();
	void createSurface();
    void choosePhysicalDevice();
    void createLogicalDevice();
    void createSwapChain();
    void createCommandPool();
    void createSwapChainImages();
    void createRenderPass();
    void createDescriptorSetLayout();
	void createDepthImage();
	void createFramebuffers();
	void createSemaphores();

    /* Helper Functions */
	void getDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT* createInfo);
	void getDebugReportCreateInfo(VkDebugReportCallbackCreateInfoEXT* createInfo);
    void printPhysicalDevices();
    int findQueueFamilies(VkPhysicalDevice device);
	SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
	bool checkDeviceExtensionSupport(VkPhysicalDevice device);
	bool isDeviceSuitable(VkPhysicalDevice device);
	VkSurfaceFormatKHR chooseSwapSurfaceFormat(uint32_t formatCount, VkSurfaceFormatKHR *availableFormats);
	VkPresentModeKHR chooseSwapPresentMode(uint32_t presentModeCount, VkPresentModeKHR *availablePresentModes);
	VkExtent2D chooseSwapExtent(VkSurfaceCapabilitiesKHR *capabilities);
	VkFormat findSupportedFormat(uint32_t candidatesCount, VkFormat *candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
	VkFormat findDepthFormat();

public:
    VulkanSetup(const char* _appNAme, const char* _engineName, const char* _libName, FILE* _logfile);
    /* 'virtual' ermöglicht dynamisches Laden der Klasse in einem shared object */
    virtual ~VulkanSetup();
    virtual void init(MyWindow _window);
	virtual void cleanUp();
    /* Getter */
    VkDevice getDevice();
	VkPhysicalDevice getPhysicalDevice();
	VkExtent2D getSwapChainExtent();
	VkDescriptorSetLayout *getDescriptorSetLayoutPtr();
	VkRenderPass getRenderPass();
	VkCommandPool getCommandPool();
	VkQueue getQueue();
	uint32_t getSwapChainImagesCount();
	VkFramebuffer *getSwapChainFramebuffers();
	VkSwapchainKHR getSwapChain();
	VkSemaphore getImageAvailableSemaphore();
	VkSemaphore getRenderFinishedSemaphore();
};

class Shader
{
protected:
	FILE *file;
	size_t size;
	uint32_t *code;
public:
	Shader();
	~Shader();
	void load(const char *fileName);
	size_t getSize();
	uint32_t *getCode();
};

class TextOverlay
{
protected:
	VulkanSetup *vulkanSetup;
	vec4 *charVertices;
public:
	TextOverlay(VulkanSetup *_vulkanSetup);
	~TextOverlay();
	int texWidth, texHeight;
	VkDeviceSize tetxtureSize;
	unsigned char *pixels;
	uint32_t numLetters;
	Buffer *vertexBuffer;
	void beginTextUpdate();
	void addText(char* text, float x, float y);
	void endTextUpdate();
};

class Texture
{
protected:
	VulkanSetup *vulkanSetup;
	const char *filename;
	Image *textureImage;
	int texWidth, texHeight, texChannels;
	VkDeviceSize imageSize;
	unsigned char *pixels;
	VkFormat imageFormat;
	VkSampler textureSampler;
	void loadTexture();
	void createTextureImage();
	void createTextureImageView();
	void createTextureSampler();
public:
	Texture(VulkanSetup *_vulkanSetup, const char *_filename);
	Texture(VulkanSetup *_vulkanSetup, TextOverlay *_textOverlay);
	~Texture();
	VkImageView getTextureImageView();
	VkSampler getTextureSampler();
};

class ObjectModel
{
protected:
	const char *filename;
	VertexData *vertexData;
	IndexData *indexData;
	void loadModel();
public:
	ObjectModel(const char *_filename, VertexData *_vertexData, IndexData *_indexData);
	~ObjectModel();
};

class RenderObject
{
protected:
	const char* resourcesPath;
	VulkanSetup *vulkanSetup;
	VkDeviceSize uboBufferSize;
	Buffer *uniformBuffer;
	VkDescriptorPool descriptorPool;
	Shader vertexShader;
	Shader fragmentShader;
	Shader tessellationControlShader;
	Shader tessellationEvaluationShader;
	uint32_t stageCount;
	VkVertexInputBindingDescription bindingDescription;
	uint32_t attributeDescriptionCount;
	VkVertexInputAttributeDescription *pAttributeDescriptions;
	VkPrimitiveTopology topology;
	VkPipelineTessellationStateCreateInfo *pTessellationStateCreateInfo;
	uint32_t pushConstantRangeCount;
	VkPushConstantRange* pPushConstantRange;
	VkPipelineLayout pipelineLayout;
	VkPipeline graphicsPipeline;
	Texture *texture;
	TextOverlay *textOverlay;
	VkDescriptorSet descriptorSet;
	vec4 color;
	uint64_t vertexOffset;
	uint32_t indexCount;
	uint32_t firstIndex;
	clock_t startTime;
	bool *key;
	// Methods
	VkShaderModule createShaderModule(Shader shader);
	VkPipelineShaderStageCreateInfo getShaderStageInfo(VkShaderStageFlagBits stage, VkShaderModule module);
	VkVertexInputBindingDescription getBindingDescription(uint32_t stride);
	VkVertexInputAttributeDescription *getAttributeDescriptions(uint32_t count, VkFormat *formats, uint32_t *offsets);
	VkPipelineTessellationStateCreateInfo *getTessellationStateCreateInfo(uint32_t patchControlPoints);
	VkPushConstantRange *createPushConstantRange(VkShaderStageFlags shaderStageFlags, uint32_t size);
	void createUniformBuffer();
	void createPipelineLayout();
	void createGraphicsPipeline();
	void createDescriptorSet();
public:
	mat4 mModel;
	mat4 *mView;
	mat4 mProj;
	RenderObject(	VulkanSetup *_vulkanSetup,
					VkDescriptorPool _descriptorPool,
					TextOverlay *_textOverlay,
					mat4 *_mView,
					bool *_key,
					const char* resPath);
	~RenderObject();
	virtual void updateUniformBuffer();
	virtual void motion();
	uint32_t getPushConstantRangeCount();
	VkPushConstantRange* getPushConstantRange();
	VkPipelineLayout getPipelineLayout();
	VkPipeline getGraphicsPipeline();
	VkDescriptorSet *getDescriptorSetPtr();
	VkBuffer getTextOverlayVertexBuffer();
	uint32_t getNumLetters();
	uint64_t getVertexOffset();
	uint32_t getIndexCount();
	uint32_t getFirstIndex();
};

class ObjectHandler {
public:
	ObjectHandler();
	~ObjectHandler();
	void addObject(RenderObject* obj);
	void removeObject();
};

class RenderScene
{
protected:
	const char* resourcesPath;
	struct Camera {
		float xPos, yPos, zPos;
		float xAngle, yAngle, zAngle;
		mat4 M;
	} cam;
	float elevation;
	float xi;
	bool *key;
	MotionPos* motionPos;
	MotionPos motionPosIst;
	mat4 mView, mView2, mGlobal;
	VulkanSetup *vulkanSetup;
	uint32_t objectCount;
	RenderObject **obj;
	RenderObject *txtObj;
	VertexData *vertexData;
	IndexData *indexData;
	Buffer *vertexBuffer;
	Buffer *indexBuffer;
	TextOverlay *textOverlay;
	VkDescriptorPool descriptorPool;
	VkCommandBuffer *commandBuffers;
	void printMatrix(mat4 M, float x, float y);
	void createVertexBuffer();
	void createIndexBuffer();
	void createDescriptorPool();
	void createCommandBuffers();
public:
	RenderScene(VulkanSetup* _vulkanSetup, bool* _key, MotionPos* _motionPos, const char* resPath);
	~RenderScene();
	void updateUniformBuffers();
	void camMotion();
	void updateTextOverlay(uint32_t fps, int xMotionPos, int yMotionPos);
	void drawFrame();
};

/* Vulkan Function Pointer */
extern PFN_vkEnumerateInstanceVersion vkEnumerateInstanceVersion;
extern PFN_vkCreateInstance vkCreateInstance;
extern PFN_vkEnumerateInstanceLayerProperties vkEnumerateInstanceLayerProperties;
extern PFN_vkEnumerateInstanceExtensionProperties vkEnumerateInstanceExtensionProperties;
#ifdef ANDROID
extern PFN_vkCreateAndroidSurfaceKHR vkCreateAndroidSurfaceKHR;
#elif LINUX
extern PFN_vkCreateXlibSurfaceKHR vkCreateXlibSurfaceKHR;
#elif WINDOWS
extern PFN_vkCreateWin32SurfaceKHR vkCreateWin32SurfaceKHR;
#endif
extern PFN_vkEnumeratePhysicalDevices vkEnumeratePhysicalDevices;
extern PFN_vkEnumerateDeviceExtensionProperties vkEnumerateDeviceExtensionProperties;
extern PFN_vkGetPhysicalDeviceProperties vkGetPhysicalDeviceProperties;
extern PFN_vkGetPhysicalDeviceFeatures vkGetPhysicalDeviceFeatures;
extern PFN_vkGetPhysicalDeviceQueueFamilyProperties vkGetPhysicalDeviceQueueFamilyProperties;
extern PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR vkGetPhysicalDeviceSurfaceCapabilitiesKHR;
extern PFN_vkGetPhysicalDeviceSurfaceFormatsKHR vkGetPhysicalDeviceSurfaceFormatsKHR;
extern PFN_vkGetPhysicalDeviceSurfacePresentModesKHR vkGetPhysicalDeviceSurfacePresentModesKHR;
extern PFN_vkGetPhysicalDeviceSurfaceSupportKHR vkGetPhysicalDeviceSurfaceSupportKHR;
extern PFN_vkCreateDevice vkCreateDevice;
extern PFN_vkGetDeviceQueue vkGetDeviceQueue;
extern PFN_vkCreateSwapchainKHR vkCreateSwapchainKHR;
extern PFN_vkCreateCommandPool vkCreateCommandPool;
extern PFN_vkCreateImage vkCreateImage;
extern PFN_vkGetImageMemoryRequirements vkGetImageMemoryRequirements;
extern PFN_vkAllocateMemory vkAllocateMemory;
extern PFN_vkBindImageMemory vkBindImageMemory;
extern PFN_vkCreateImageView vkCreateImageView;
extern PFN_vkCmdPipelineBarrier vkCmdPipelineBarrier;
extern PFN_vkCmdCopyBufferToImage vkCmdCopyBufferToImage;
extern PFN_vkGetSwapchainImagesKHR vkGetSwapchainImagesKHR;
extern PFN_vkGetPhysicalDeviceMemoryProperties vkGetPhysicalDeviceMemoryProperties;
extern PFN_vkCreateBuffer vkCreateBuffer;
extern PFN_vkGetBufferMemoryRequirements vkGetBufferMemoryRequirements;
extern PFN_vkBindBufferMemory vkBindBufferMemory;
extern PFN_vkCmdCopyBuffer vkCmdCopyBuffer;
extern PFN_vkMapMemory vkMapMemory;
extern PFN_vkUnmapMemory vkUnmapMemory;
extern PFN_vkDestroyBuffer vkDestroyBuffer;
extern PFN_vkFreeMemory vkFreeMemory;
extern PFN_vkAllocateCommandBuffers vkAllocateCommandBuffers;
extern PFN_vkBeginCommandBuffer vkBeginCommandBuffer;
extern PFN_vkEndCommandBuffer vkEndCommandBuffer;
extern PFN_vkQueueSubmit vkQueueSubmit;
extern PFN_vkQueueWaitIdle vkQueueWaitIdle;
extern PFN_vkFreeCommandBuffers vkFreeCommandBuffers;
extern PFN_vkCreateRenderPass vkCreateRenderPass;
extern PFN_vkCreateDescriptorSetLayout vkCreateDescriptorSetLayout;
extern PFN_vkGetPhysicalDeviceFormatProperties vkGetPhysicalDeviceFormatProperties;
extern PFN_vkCreateFramebuffer vkCreateFramebuffer;
extern PFN_vkCreateSemaphore vkCreateSemaphore;
extern PFN_vkCreateDescriptorPool vkCreateDescriptorPool;
extern PFN_vkCmdPushConstants vkCmdPushConstants;
extern PFN_vkCreateSampler vkCreateSampler;
extern PFN_vkCreateShaderModule vkCreateShaderModule;
extern PFN_vkCreatePipelineLayout vkCreatePipelineLayout;
extern PFN_vkCreateGraphicsPipelines vkCreateGraphicsPipelines;
extern PFN_vkAllocateDescriptorSets vkAllocateDescriptorSets;
extern PFN_vkUpdateDescriptorSets vkUpdateDescriptorSets;
extern PFN_vkCmdBeginRenderPass vkCmdBeginRenderPass;
extern PFN_vkCmdBindIndexBuffer vkCmdBindIndexBuffer;
extern PFN_vkCmdBindVertexBuffers vkCmdBindVertexBuffers;
extern PFN_vkCmdBindPipeline vkCmdBindPipeline;
extern PFN_vkCmdBindDescriptorSets vkCmdBindDescriptorSets;
extern PFN_vkCmdDrawIndexed vkCmdDrawIndexed;
extern PFN_vkCmdDraw vkCmdDraw;
extern PFN_vkCmdEndRenderPass vkCmdEndRenderPass;
extern PFN_vkAcquireNextImageKHR vkAcquireNextImageKHR;
extern PFN_vkQueuePresentKHR vkQueuePresentKHR;
extern PFN_vkDestroyInstance vkDestroyInstance;
extern PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr;
extern PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessengerEXT;
extern PFN_vkCreateDebugReportCallbackEXT vkCreateDebugReportCallbackEXT;

#endif /* VULKAN_SETUP_H */