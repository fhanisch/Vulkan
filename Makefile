
BuildPath = build
SrcPath = BaseTemplate\src
INC_VULKAN = C:\VulkanSDK\1.0.57.0\Include

App:
	cl /nologo /EHsc /I$(INC_VULKAN) /Fo$(BuildPath)\ $(SrcPath)\main.cpp /link /out:$(BuildPath)\app.exe

clean:
	del build\*.exe build\*.obj
