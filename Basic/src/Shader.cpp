#include "Shader.h"

Shader::Shader()
{
	size = 0;
	file = nullptr;
	code = nullptr;
}

Shader::~Shader() {}

void Shader::load(const char* fileName)
{
	file = fopen(fileName, "rb");
	if (file == NULL) {
		std::cout << "Could not open " << fileName << "!" << std::endl;
		exit(1);
	}
	fseek(file, 0, SEEK_END);
	size = ftell(file);
	rewind(file);
	code = (uint32_t*)malloc(size);
	fread((void*)code, size, 1, file);
	fclose(file);
}

size_t Shader::getSize() { return size; }
uint32_t* Shader::getCode() { return code; }