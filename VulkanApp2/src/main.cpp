/*
	Vulkan App 2.0
	19.08.2018
	FH

	Build:
		clang-cl /W4 main.cpp Window.cpp Vulkan.cpp Models.cpp /I C:\VulkanSDK\1.1.82.0\Include /I C:\Home\Entwicklung\inc /I C:\Home\Entwicklung\stb /link user32.lib Shcore.lib vulkan-1.lib mathlib.lib /out:VulkanApp2a.exe /LIBPATH:C:\VulkanSDK\1.1.82.0\Lib /LIBPATH:C:\Home\Entwicklung\lib
		
		oder:
		
		cl /nologo /EHsc /W4 main.cpp Window.cpp Vulkan.cpp Models.cpp /I C:\VulkanSDK\1.1.82.0\Include /I C:\Home\Entwicklung\inc /I C:\Home\Entwicklung\stb /link user32.lib Shcore.lib vulkan-1.lib mathlib.lib /out:VulkanApp2b.exe /LIBPATH:C:\VulkanSDK\1.1.82.0\Lib /LIBPATH:C:\Home\Entwicklung\lib

	Links:
		https://vulkan-tutorial.com
		http://www.songho.ca
*/

#include <iostream>
#include "Window.h"
#include "Vulkan.h"
#include <time.h>

char getNextDelimiter(char **src, const char *delimiter)
{
	while (**src != 0)
	{
		for (uint32_t i = 0; i < strlen(delimiter); i++)
		{
			if (**src == delimiter[i])
			{
				(*src)++;
				return delimiter[i];
			}
		}
		(*src)++;
	}
	return 0;
}

class App
{
	// Main Application Class
	const char *windowName = "Vulkan App 2";
	int windowWidth = 3840;
	int windowHeight = 2160;
	const char *appDir;
	const char *appName = "VulkanApp";
	const char *engineName = "MyVulkanEngine";
	Window *window;
	VulkanSetup *vulkanSetup;
	RenderScene *renderScene;
	clock_t start_t, sync_t;
	uint32_t framecount = 0;
	uint32_t fps = 0;
public:
	App(const char *_appDir)
	{
		appDir = _appDir;
		std::cout << "***** HalliHalllo *****" << std::endl;
		window = new Window(windowName, windowWidth, windowHeight);
		window->createWindow();
		vulkanSetup = new VulkanSetup(appName, engineName, window);
		renderScene = new RenderScene(vulkanSetup, window->getKey());
	}
	~App()
	{
		std::cout << "Servus!" << std::endl;
		delete vulkanSetup;
		delete window;
	}
	void run()
	{
		window->showWindow();
		start_t = clock(); //FPS
		sync_t = start_t;
		while (!window->checkMessage())
		{
			renderScene->updateUniformBuffers();
			renderScene->camMotion();
			renderScene->drawFrame();
			while ((clock() - sync_t) * 125 < CLOCKS_PER_SEC); // Achtung: Auflösung von <1ms scheinbar nicht möglich --> nicht jede FPS-Vorgabe funktioniert daher genau
			if ((clock() - start_t) >= CLOCKS_PER_SEC)
			{
				fps = framecount;
				start_t = clock();
				framecount = 0;
			}
			renderScene->updateTextOverlay(fps);
			sync_t = clock();
			framecount++;
		}
	}
};

//Entry Function
int main(int argc, char *argv[])
{
	char *str = argv[0];
	char *ptr = str;
	while (getNextDelimiter(&ptr, "\\") != 0) str = ptr;
	*str = 0;
	std::cout << argv[0] << std::endl;
	if (argc > 1) std::cout << argv[1] << std::endl;
	App app(argv[0]);
	app.run();
	return 0;
}