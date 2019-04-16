#include "Vulkan.h"
#include "RenderScene.h"

#define APP_NAME "BasicVulkanApp"
#define ENGINE_NAME "MyVulkanEngine"
#define WINDOW_NAME "BasicApp"
#define WND_WIDTH 2000
#define WND_HEIGHT 1500
#define IS_FULLSCREEN false

typedef struct MyHandle* TestHandle;

class App
{
	const char* appName = APP_NAME;
	const char* engineName = ENGINE_NAME;
	const char* windowName = WINDOW_NAME;
	int windowWidth = WND_WIDTH;
	int windowHeight = WND_HEIGHT;
	Window* window;
	Vulkan *vulkan;
	RenderScene* renderScene;
public:
	App(int argc, char* argv[])
	{
		for (int i = 1; i < argc; i++)
		{
			std::cout << "arg_" << i << " = " << argv[i] << std::endl;
		}
		std::cout << "\nInit Application..." << std::endl;
		window = new Window(windowName);
		window->createWindow(windowWidth, windowHeight, IS_FULLSCREEN);
		vulkan = new Vulkan(appName, engineName, window);
		renderScene = new RenderScene(vulkan, window->getKey(), window->getMousePos());
	}

	~App()
	{
		vulkan->~Vulkan();
	}

	void run()
	{
		std::cout << "\nRun Application...\n" << std::endl;
		window->showWindow();

		while(!window->checkMessage())
		{
			renderScene->updateUniformBuffers();
			renderScene->camMotion();
			renderScene->drawFrame();
			//std::cout << "xPos = " << window->getMousePos()->x << std::endl;
			//std::cout << "yPos = " << window->getMousePos()->y << std::endl;
		}
	}
};

int main(int argc, char* argv[])
{
	TestHandle handle = nullptr;
	std::cout << "\n*** HalliHallo ***\n" << std::endl;
	std::cout << "Path: " << argv[0] << std::endl << std::endl;

	App app(argc, argv);
	app.run();

	return 0;
}