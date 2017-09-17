BuildPath = build
SrcPath = BaseTemplate\src
SrcPathTriangle = Triangle\src
SrcPath2DAction = 2DAction\src
SrcPathMathLib = mathlib\src
INC_VULKAN = C:\VulkanSDK\1.0.57.0\Include
INC_GLFW = C:\Home\Entwicklung\glfw-3.2.1.bin.WIN64\include
INC_DEV = C:\Home\Entwicklung\inc
LIB_VULKAN = C:\VulkanSDK\1.0.57.0\Lib
LIB_GLFW = C:\Home\Entwicklung\glfw-3.2.1.bin.WIN64\lib-vc2015
LIB_DEV = C:\Home\Entwicklung\lib
LIBS_T1 = vulkan-1.lib glfw3.lib user32.lib gdi32.lib shell32.lib
LIBS_T2 = vulkan-1.lib mathlib.lib user32.lib Shcore.lib

App:
	cl /nologo /EHsc /I$(INC_VULKAN) /Fo$(BuildPath)\ $(SrcPath)\main.cpp /link /out:$(BuildPath)\app.exe

T1: build
	cl /nologo /W3 /EHsc /MD /I$(INC_VULKAN) /I$(INC_GLFW) /Fo$(BuildPath)\ $(SrcPathTriangle)\main.cpp \
		/link $(LIBS_T1) /LIBPATH:$(LIB_VULKAN) /LIBPATH:$(LIB_GLFW) /out:$(BuildPath)\Triangle.exe
	del build\*.obj

T2: build
	cl /nologo /W3 /EHsc /I$(INC_VULKAN) /I$(INC_DEV) /Fo$(BuildPath)\ $(SrcPath2DAction)\main.cpp \
		/link $(LIBS_T2) /LIBPATH:$(LIB_VULKAN) /LIBPATH:$(LIB_DEV) /out:$(BuildPath)\2DAction.exe
	del build\*.obj

ML: build
	cl /nologo /c /Fo$(BuildPath)\ $(SrcPathMathLib)\matrix.c
	lib /nologo /out:$(BuildPath)\mathlib.lib $(BuildPath)\matrix.obj
	copy build\mathlib.lib ..\lib
	copy mathlib\src\matrix.h ..\inc
	del build\*.obj

Shader: build
	glslangValidator -V shader\shader.vert -o build\vs.spv
	glslangValidator -V shader\2d.vert -o build\2d.spv
	glslangValidator -V shader\shader.frag -o build\fs.spv
	copy build\*.spv x64\Debug

build:
	mkdir build

clean:
	del build\*.exe build\*.obj build\*.spv
