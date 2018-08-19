BuildPath = build
SrcPath = BaseTemplate\src
SrcPathTriangle = Triangle\src
SrcPath2DAction = 2DAction\src
SrcPathWorld = World3D\src
SrcPathMathLib = mathlib\src
INC_VULKAN = C:\VulkanSDK\1.1.77.0\Include
INC_GLFW = C:\Home\Entwicklung\glfw-3.2.1.bin.WIN64\include
INC_DEV = C:\Home\Entwicklung\inc
INC_STB = C:\Home\Entwicklung\stb
INC_USB = C:\Home\Entwicklung\libusb-1.0.21\include
LIB_VULKAN = C:\VulkanSDK\1.1.77.0\Lib
LIB_GLFW = C:\Home\Entwicklung\glfw-3.2.1.bin.WIN64\lib-vc2015
LIB_DEV = C:\Home\Entwicklung\lib
LIB_USB = C:\Home\Entwicklung\libusb-1.0.21\MS64\dll
LIBS_T1 = vulkan-1.lib glfw3.lib user32.lib gdi32.lib shell32.lib
LIBS_T2 = vulkan-1.lib mathlib.lib user32.lib Shcore.lib
LIBS_WORLD = vulkan-1.lib mathlib.lib user32.lib Shcore.lib libusb-1.0.lib

all: App T1 T2 World ML Shader1 Shader2 Tex

VA: build
	clang-cl VulkanApp2\src\main.cpp VulkanApp2\src\Window.cpp user32.lib Shcore.lib -o build\VulkanApp2a.exe
	cl /nologo /EHsc VulkanApp2\src\main.cpp VulkanApp2\src\Window.cpp /link user32.lib Shcore.lib /out:build\VulkanApp2b.exe

App: build
	clang $(SrcPath)\main.cpp -I $(INC_VULKAN) -o $(BuildPath)\test.exe 
	cl /nologo /EHsc /I$(INC_VULKAN) /Fo$(BuildPath)\ $(SrcPath)\main.cpp /link /out:$(BuildPath)\app.exe

T1: build
	cl /nologo /W3 /EHsc /MD /I$(INC_VULKAN) /I$(INC_GLFW) /Fo$(BuildPath)\ $(SrcPathTriangle)\main.cpp \
		/link $(LIBS_T1) /LIBPATH:$(LIB_VULKAN) /LIBPATH:$(LIB_GLFW) /out:$(BuildPath)\Triangle.exe
	del build\*.obj

T2: build
	clang $(SrcPath2DAction)\main.cpp -D NOCONSOLE -I $(INC_VULKAN) -I $(INC_DEV) -I $(INC_STB) -L $(LIB_VULKAN) -L $(LIB_DEV) -lmathlib -lvulkan-1 -luser32 -lShcore -o $(BuildPath)\test.exe
	cl /nologo /W3 /EHsc /DNOCONSOLE /I$(INC_VULKAN) /I$(INC_DEV) /I$(INC_STB) /Fo$(BuildPath)\ $(SrcPath2DAction)\main.cpp \
		/link $(LIBS_T2) /LIBPATH:$(LIB_VULKAN) /LIBPATH:$(LIB_DEV) /out:$(BuildPath)\2DAction.exe
	del build\*.obj

World: build
	clang $(SrcPathWorld)\main.cpp -D NOCONSOLE -I $(INC_VULKAN) -I $(INC_DEV) -I $(INC_STB) -I $(INC_USB) -L $(LIB_VULKAN) -L $(LIB_DEV) -L $(LIB_USB) -lvulkan-1 -lmathlib -luser32 -lShcore -llibusb-1.0 -o $(BuildPath)\test.exe
	cl /nologo /W3 /EHsc /DNOCONSOLE /I$(INC_VULKAN) /I$(INC_DEV) /I$(INC_STB) /I$(INC_USB) /Fo$(BuildPath)\ $(SrcPathWorld)\main.cpp \
		/link $(LIBS_WORLD) /LIBPATH:$(LIB_VULKAN) /LIBPATH:$(LIB_DEV) /LIBPATH:$(LIB_USB) /out:$(BuildPath)\World.exe
	del build\*.obj

ML: build
	cl /nologo /c /Fo$(BuildPath)\ $(SrcPathMathLib)\matrix.c
	lib /nologo /out:$(BuildPath)\mathlib.lib $(BuildPath)\matrix.obj
	copy build\mathlib.lib ..\lib
	copy mathlib\src\matrix.h ..\inc
	del build\*.obj

Shader1: build
	glslangValidator -V shader\triangle.vert -o build\vs_triangle.spv
	glslangValidator -V shader\2d.vert -o build\vs_2d.spv
	glslangValidator -V shader\circle.vert -o build\vs_circle.spv
	glslangValidator -V shader\welle.vert -o build\vs_welle.spv
	glslangValidator -V shader\perlin1d.vert -o build\vs_perlin1d.spv
	glslangValidator -V shader\triangle.frag -o build\fs_triangle.spv
	glslangValidator -V shader\2d.frag -o build\fs_2d.spv
	glslangValidator -V shader\2d_tex.frag -o build\fs_2d_tex.spv
	glslangValidator -V shader\powermeter.frag -o build\fs_powermeter.spv
	glslangValidator -V shader\circleFilled.frag -o build\fs_circleFilled.spv
	glslangValidator -V shader\perlin2d.frag -o build\fs_perlin2d.spv
	glslangValidator -V shader\muster.frag -o build\fs_muster.spv
	glslangValidator -V shader\curveTesselator.vert -o build\vs_curveTesselator.spv
	glslangValidator -V shader\curveTesselator.tesc -o build\tcs_curveTesselator.spv
	glslangValidator -V shader\curveTesselator.tese -o build\tes_curveTesselator.spv
	glslangValidator -V shader\curveTesselator.frag -o build\fs_curveTesselator.spv
	glslangValidator -V shader\perlin1dTesselator.vert -o build\vs_perlin1dTesselator.spv
	glslangValidator -V shader\perlin1dTesselator.tesc -o build\tcs_perlin1dTesselator.spv
	glslangValidator -V shader\perlin1dTesselator.tese -o build\tes_perlin1dTesselator.spv
	copy build\*.spv x64\Debug

Shader2: build
	glslangValidator -V shader\3d.vert -o build\vs_3d.spv
	glslangValidator -V shader\sphere.vert -o build\vs_sphere.spv
	glslangValidator -V shader\uvMesh.vert -o build\vs_uvMesh.spv
	glslangValidator -V shader\perlinSphere.vert -o build\vs_perlinSphere.spv
	glslangValidator -V shader\3d.frag -o build\fs_3d.spv
	glslangValidator -V shader\3d_tex.frag -o build\fs_3d_tex.spv
	glslangValidator -V shader\muster3.frag -o build\fs_muster3.spv
	glslangValidator -V shader\muster3_ADSperFrag.frag -o build\fs_muster3_ADSperFrag.spv
	glslangValidator -V shader\terrainTesselator.vert -o build\vs_terrainTesselator.spv
	glslangValidator -V shader\terrainTesselator.tesc -o build\tcs_terrainTesselator.spv
	glslangValidator -V shader\terrainTesselator.tese -o build\tes_terrainTesselator.spv
	glslangValidator -V shader\perlinSphere.tesc -o build\tcs_perlinSphere.spv
	glslangValidator -V shader\perlinSphere.tese -o build\tes_perlinSphere.spv
	copy build\*.spv x64\Debug

build:
	mkdir build

Tex:
	copy textures\texture.jpg x64\Debug
	copy textures\texture.jpg build
	copy textures\sky.png x64\Debug
	copy textures\sky.png build

clean:
	del build\*.exe build\*.obj build\*.spv build\*.lib build\*.txt
