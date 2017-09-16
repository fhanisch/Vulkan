BuildPath = build
SrcPath = BaseTemplate\src
SrcPathTriangle = Triangle\src
SrcPath2DAction = 2DAction\src
INC_VULKAN = C:\VulkanSDK\1.0.57.0\Include
LIB_VULKAN = C:\VulkanSDK\1.0.57.0\Lib
INC_GLFW = C:\Home\Entwicklung\glfw-3.2.1.bin.WIN64\include
LIB_GLFW = C:\Home\Entwicklung\glfw-3.2.1.bin.WIN64\lib-vc2015
LIBS = vulkan-1.lib glfw3.lib user32.lib gdi32.lib shell32.lib

App:
	cl /nologo /EHsc /I$(INC_VULKAN) /Fo$(BuildPath)\ $(SrcPath)\main.cpp /link /out:$(BuildPath)\app.exe

T1: build
	cl /nologo /W3 /EHsc /MD /I$(INC_VULKAN) /I$(INC_GLFW) /Fo$(BuildPath)\ $(SrcPathTriangle)\main.cpp \
		/link $(LIBS) /LIBPATH:$(LIB_VULKAN) /LIBPATH:$(LIB_GLFW) /out:$(BuildPath)\Triangle.exe
	del build\*.obj

T2: build
	cl /nologo /W3 /EHsc /I$(INC_VULKAN) /Fo$(BuildPath)\ $(SrcPath2DAction)\main.cpp \
		/link vulkan-1.lib user32.lib /LIBPATH:$(LIB_VULKAN) /out:$(BuildPath)\2DAction.exe
	del build\*.obj

Shader: build
	glslangValidator -V Triangle\shader\shader.vert -o build\vs.spv
	glslangValidator -V Triangle\shader\shader.frag -o build\fs.spv
	copy build\*.spv x64\Debug

build:
	mkdir build

clean:
	del build\*.exe build\*.obj build\*.spv
