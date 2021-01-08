VK_INC_WIN = C:/VulkanSDK/1.2.162.0/Include

all: exe shared dynamic

exe: app

shared: app2

dynamic: app3

main: src/main.cpp
	mkdir -p build/linux
	clang++ -o build/linux/main.o -c src/main.cpp -Wall -I $(VK_INC) -DLINUX

main_dl: src/main.cpp
	mkdir -p build/linux
	clang++ -o build/linux/main_dl.o -c src/main.cpp -DDYNAMIC -Wall -DLOG

Models: src/Models.cpp src/Models.h
	mkdir -p build/linux
	clang++ -o build/linux/Models.o -c src/Models.cpp -Wall -I $(VK_INC)

matrix: src/matrix.c src/matrix.h
	mkdir -p build/linux
	clang -o build/linux/matrix.o -c src/matrix.c -Wall

libVulkan: VulkanSetup
	clang++ -shared -o libVulkan.so build/linux/VulkanSetup.o -Wall

app2: main libVulkan
	clang++ -o app2 build/linux/main.o ./libVulkan.so -Wall

app3: main_dl libVulkan
	clang++ -o app3 build/linux/main_dl.o -ldl -Wall

# *** Android ***

# Achtung! Wenn Android.mk inkludiert ist funktioniert die Kompilierung für Windows nicht mehr! --> TODO: checken!
include Android.mk

# *** Windows ***

include app_win.mk

#TODO: Warnings auf W4 umstellen
app_win:
	cl /nologo /W3 /c src/matrix.c
	cl /nologo /EHsc /W3 /c src/WindowsMain.cpp src/Window.cpp src/VulkanSetup.cpp src/Models.cpp /I$(VK_INC_WIN) /DWINDOWS
	link /nologo /out:app.exe *.obj user32.lib Shcore.lib

# *** Linux ***
include app.mk

# *** Shader ***
include shader.mk

clean:
	del /Q /S build lib *.apk