ANDROID_JAR=/home/felix/Android/Sdk/platforms/android-28/android.jar
VK_INC=/home/felix/Entwicklung/vulkan-sdk/1.1.126.0/source/Vulkan-Headers/include

all: exe shared dynamic

exe: app

shared: app2

dynamic: app3

main: src/main.cpp
	mkdir -p build/linux
	clang++ -o build/linux/main.o -c src/main.cpp -Wall -I $(VK_INC)

main_android: src/main.cpp
	mkdir -p build/android
	aarch64-linux-android28-clang++ -o build/android/main.o -c src/main.cpp -Wall -fpic -DANDROID -DLOG

android_native_app_glue: src/android_native_app_glue.c
	mkdir -p build/android
	aarch64-linux-android28-clang -o build/android/android_native_app_glue.o -c src/android_native_app_glue.c -Wall -fpic -DANDROID -DLOG

main_dl: src/main.cpp
	mkdir -p build/linux
	clang++ -o build/linux/main_dl.o -c src/main.cpp -DDYNAMIC -Wall -DLOG

VulkanSetup: src/VulkanSetup.cpp src/VulkanSetup.h
	mkdir -p build/linux
	clang++ -o build/linux/VulkanSetup.o -c src/VulkanSetup.cpp -Wall -fpic -I $(VK_INC)

VulkanSetup_android: src/VulkanSetup.cpp src/VulkanSetup.h
	mkdir -p build/android
	aarch64-linux-android28-clang++ -o build/android/VulkanSetup.o -c src/VulkanSetup.cpp -Wall -fpic -DANDROID -DLOG

Models: src/Models.cpp src/Models.h
	mkdir -p build/linux
	clang++ -o build/linux/Models.o -c src/Models.cpp -Wall -I $(VK_INC)

Models_android: src/Models.cpp src/Models.h
	mkdir -p build/android
	aarch64-linux-android28-clang++ -o build/android/Models.o -c src/Models.cpp -Wall -fpic -DANDROID -DLOG

matrix: src/matrix.c src/matrix.h
	mkdir -p build/linux
	clang -o build/linux/matrix.o -c src/matrix.c -Wall

matrix_android: src/matrix.c src/matrix.h
	mkdir -p build/android
	aarch64-linux-android28-clang -o build/android/matrix.o -c src/matrix.c -Wall -fpic

libVulkan: VulkanSetup
	clang++ -shared -o libVulkan.so build/linux/VulkanSetup.o -Wall

app: main VulkanSetup matrix Models
	clang++ -o app build/linux/main.o build/linux/VulkanSetup.o build/linux/Models.o build/linux/matrix.o -Wall -ldl -lX11

app2: main libVulkan
	clang++ -o app2 build/linux/main.o ./libVulkan.so -Wall

app3: main_dl libVulkan
	clang++ -o app3 build/linux/main_dl.o -ldl -Wall

libVulkanApp: main_android android_native_app_glue VulkanSetup_android Models_android matrix_android
	mkdir -p lib/arm64-v8a
	aarch64-linux-android28-clang++ -shared -o lib/arm64-v8a/libVulkanApp.so build/android/main.o build/android/android_native_app_glue.o build/android/VulkanSetup.o build/android/Models.o build/android/matrix.o -landroid -llog -ldl -Wall

output.apk: libVulkanApp AndroidManifest.xml
	aapt2 link -o output.apk -I $(ANDROID_JAR) --manifest AndroidManifest.xml
	7z a output.apk lib/

VulkanApp_unsigned-aligned.apk: output.apk
	rm -f VulkanApp_unsigned-aligned.apk
	zipalign -v -p 4 output.apk VulkanApp_unsigned-aligned.apk

VulkanApp: VulkanApp_unsigned-aligned.apk
	apksigner sign --ks my-release-key.jks --out VulkanApp.apk VulkanApp_unsigned-aligned.apk

clean:
	rm -f -r build lib *.apk