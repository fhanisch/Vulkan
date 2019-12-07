ANDROID_JAR=/home/felix/Android/Sdk/platforms/android-28/android.jar

all: exe shared dynamic

exe: app

shared: app2

dynamic: app3

main: src/main.cpp
	mkdir -p build/linux
	clang++ -o build/linux/main.o -c src/main.cpp -Wall

main_dl: src/main.cpp
	mkdir -p build/linux
	clang++ -o build/linux/main_dl.o -c src/main.cpp -DDYNAMIC -Wall

VulkanSetup: src/VulkanSetup.cpp src/VulkanSetup.h
	mkdir -p build/linux
	clang++ -o build/linux/VulkanSetup.o -c src/VulkanSetup.cpp -Wall -fpic

libVulkan: VulkanSetup
	clang++ -shared -o libVulkan.so build/linux/VulkanSetup.o -Wall

app: main VulkanSetup
	clang++ -o app build/linux/main.o build/linux/VulkanSetup.o -Wall

app2: main libVulkan
	clang++ -o app2 build/linux/main.o ./libVulkan.so -Wall

app3: main_dl libVulkan
	clang++ -o app3 build/linux/main_dl.o -ldl -Wall

clean:
	rm -f -r build