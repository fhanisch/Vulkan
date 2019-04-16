#include "Vertex.h"

VertexData::VertexData()
{
	data = nullptr;
	size = 0;
	offsets = nullptr;
	dataSetCount = 0;
}

VertexData::~VertexData() {}

void VertexData::addData(float* _data, uint64_t _size)
{
	uint64_t* tmp1 = offsets;
	offsets = new uint64_t[dataSetCount + 1];
	if (tmp1) memcpy(offsets, tmp1, dataSetCount * sizeof(uint64_t));
	offsets[dataSetCount] = size;
	if (tmp1) delete[] tmp1;

	float* tmp2 = data;
	data = (float*)malloc(size + _size);
	if (tmp2) memcpy(data, tmp2, size);
	memcpy((char*)data + size, _data, _size);
	dataSetCount++;
	size += _size;
	if (tmp2) free(tmp2);
}

uint64_t VertexData::getOffset(uint32_t index)
{
	return offsets[index];
}

float* VertexData::getData()
{
	return data;
}

uint64_t VertexData::getSize()
{
	return size;
}

IndexData::IndexData()
{
	data = nullptr;
	size = 0;
	indexCount = nullptr;
	firstIndex = nullptr;
	dataSetCount = 0;
}

IndexData::~IndexData() {}

void IndexData::addData(uint16_t * _data, uint64_t _size)
{
	uint32_t* tmp1 = indexCount;
	indexCount = new uint32_t[dataSetCount + 1];
	if (tmp1) memcpy(indexCount, tmp1, dataSetCount * sizeof(uint32_t));
	indexCount[dataSetCount] = (uint32_t)_size / (uint32_t)sizeof(uint16_t);
	if (tmp1) delete[] tmp1;

	tmp1 = firstIndex;
	firstIndex = new uint32_t[dataSetCount + 1];
	if (tmp1) memcpy(firstIndex, tmp1, dataSetCount * sizeof(uint32_t));
	firstIndex[dataSetCount] = (uint32_t)size / (uint32_t)sizeof(uint16_t);
	if (tmp1) delete[] tmp1;

	uint16_t* tmp2 = data;
	data = (uint16_t*)malloc(size + _size);
	if (tmp2) memcpy(data, tmp2, size);
	memcpy((char*)data + size, _data, _size);
	dataSetCount++;
	size += _size;
	if (tmp2) free(tmp2);
}

uint16_t * IndexData::getData()
{
	return data;
}

uint64_t IndexData::getSize()
{
	return size;
}

uint32_t IndexData::getIndexCount(uint32_t index)
{
	return indexCount[index];
}

uint32_t IndexData::getFirstIndex(uint32_t index)
{
	return firstIndex[index];
}