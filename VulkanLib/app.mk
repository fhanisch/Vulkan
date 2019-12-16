VK_INC=/home/felix/Entwicklung/vulkan-sdk/1.1.126.0/source/Vulkan-Headers/include
dst=build/linux
obj=$(dst)/main.cpp.o $(dst)/Window.cpp.o $(dst)/VulkanSetup.cpp.o $(dst)/Models.cpp.o $(dst)/matrix.c.o

$(dst):
	mkdir -p $(dst)
	touch src/main.h

$(dst)/%.c.o: src/%.c src/%.h
	clang -o $@ -c $< -Wall -I $(VK_INC) -DLINUX

$(dst)/%.cpp.o: src/%.cpp src/%.h
	clang++ -o $@ -c $< -Wall -I $(VK_INC) -DLINUX

app: $(obj)
	clang++ -o app $^ -Wall -ldl -lX11 -lpthread

dir: $(dst)