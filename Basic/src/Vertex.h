#pragma once
#include <iostream> 
#include <matrix.h>

struct Vertex {
	vec3 pos;
	vec3 color;
	vec2 texCoords;
};

class VertexData
{
protected:
	float* data;
	uint64_t size;
	uint64_t* offsets;
	uint32_t dataSetCount;
public:
	VertexData();
	~VertexData();
	void addData(float* _data, uint64_t _size);
	float* getData();
	uint64_t getSize();
	uint64_t getOffset(uint32_t index);
};

class IndexData
{
protected:
	uint16_t* data;
	uint64_t size;
	uint32_t* indexCount;
	uint32_t* firstIndex;
	uint32_t dataSetCount;
public:
	IndexData();
	~IndexData();
	void addData(uint16_t* _data, uint64_t _size);
	uint16_t* getData();
	uint64_t getSize();
	uint32_t getIndexCount(uint32_t index);
	uint32_t getFirstIndex(uint32_t index);
};