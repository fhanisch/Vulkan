/*
	Vulkan App 2.0
	19.08.2018
	FH

	Build:
		clang-cl /W4 main.cpp Window.cpp Vulkan.cpp /I C:\VulkanSDK\1.1.82.0\Include /I C:\Home\Entwicklung\inc /I C:\Home\Entwicklung\stb /link user32.lib Shcore.lib vulkan-1.lib mathlib.lib /out:VulkanApp2a.exe /LIBPATH:C:\VulkanSDK\1.1.82.0\Lib /LIBPATH:C:\Home\Entwicklung\lib
		cl /nologo /EHsc /W4 main.cpp Window.cpp Vulkan.cpp /I C:\VulkanSDK\1.1.82.0\Include /I C:\Home\Entwicklung\inc /I C:\Home\Entwicklung\stb /link user32.lib Shcore.lib vulkan-1.lib mathlib.lib /out:VulkanApp2b.exe /LIBPATH:C:\VulkanSDK\1.1.82.0\Lib /LIBPATH:C:\Home\Entwicklung\lib

	Links:
		https://vulkan-tutorial.com
		http://www.songho.ca
*/

#include <iostream>
#include "Window.h"
#include "Vulkan.h"

class App {
	// Main Application Class
	Window *window;
	VulkanSetup *vulkanSetup;
	RenderScene *renderScene;
public:
	App() {
		std::cout << "***** HalliHalllo *****" << std::endl;
		window = new Window("Vulkan App 2", 1800, 1800);
		window->createWindow();
		vulkanSetup = new VulkanSetup("VulkanApp","MyVulkanEngine",window);
		renderScene = new RenderScene(vulkanSetup, 1);
	}
	~App() {
		std::cout << "Servus!" << std::endl;
		delete vulkanSetup;
		delete window;
	}
	void run() {
		//bool *key = window->getKey(); --> später zur Steuerung notwendig

		window->showWindow();
		while (!window->checkMessage()) {
			renderScene->updateUniformBuffer();
			renderScene->drawFrame();
		}
	}
};

//Entry Function
int main(int argc, char *argv[]) {
	std::cout << argv[0] << std::endl;
	if (argc > 1) std::cout << argv[1] << std::endl;
	App app;
	app.run();
	return 0;
}