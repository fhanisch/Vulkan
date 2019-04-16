#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>

class Shader
{
protected:
	FILE* file;
	size_t size;
	uint32_t* code;
public:
	Shader();
	~Shader();
	void load(const char* fileName);
	size_t getSize();
	uint32_t* getCode();
};