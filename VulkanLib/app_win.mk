VK_INC_WIN = C:/VulkanSDK/1.1.126.0/Include
dst=build/win64
obj=$(dst)/main.cpp.o $(dst)/Window.cpp.o $(dst)/VulkanSetup.cpp.o $(dst)/Models.cpp.o $(dst)/matrix.c.o

$(dst)/%.c.o: src/%.c src/%.h
	clang -o $@ -c $< -Wall -I $(VK_INC_WIN) -DWINDOWS

$(dst)/%.cpp.o: src/%.cpp src/%.h
	clang++ -o $@ -c $< -Wall -I $(VK_INC_WIN) -DWINDOWS

app.exe: $(obj)
	clang++ -o $@ $^ -Wall "C:\Program Files (x86)\Windows Kits\10\Lib\10.0.17763.0\um\x64\user32.lib" "C:\Program Files (x86)\Windows Kits\10\Lib\10.0.17763.0\um\x64\Shcore.lib"