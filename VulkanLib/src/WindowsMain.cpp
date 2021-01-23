#define _CRT_SECURE_NO_WARNINGS /* TODO: als Compiler-Argument aufrufen */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "defs.h"
#include "Window.h"
#include "VulkanSetup.h"

#define APP_NAME "VulkanApp"
#define ENGINE_NAME "MyVulkanEngine"
#define WINDOW_NAME "My Vulkan App"

#define WND_WIDTH 3840 //1920 //3840
#define WND_HEIGHT 2160 //1080 //2160
#define LOGFILE "VulkanApp.log.txt"
#define RESOURCES_PATH "C:/Home/Entwicklung/Vulkan/VulkanLib/assets"
#define LIB_NAME "vulkan-1.dll"
#define FULLSCREEN true

// TODO: zu userdata hinzufügen
static FILE* logfile = NULL;
bool* key;
MotionPos* motionPos;

class App
{
	const char *appName = APP_NAME;
	const char *engineName = ENGINE_NAME;
	const char *resourcesPath = RESOURCES_PATH;
	const char *libName = LIB_NAME;
	MyWindow window;
	uint32_t framecount = 0;
	uint32_t fps = 0;
	clock_t start_t;

#ifdef DYNAMIC
	VulkanSetup* (*create_object)(const char* _appName, const char* _engineName, const char* _libName, FILE* _logfile);
#endif
	VulkanSetup *vkSetup;
	RenderScene *renderScene;
public:
	App()
	{
		PRINT("Start App.\n")
#ifdef DYNAMIC
		void* libVulkan = dlopen("./libVulkan.so", RTLD_LAZY);
		if (!libVulkan)
		{
			PRINT("Loading libVulkan.so failed!\n")
			exit(1);
		}
		create_object = (VulkanSetup*(*)(const char* _appName, const char* _engineName, const char* _libName, FILE* _logfile))dlsym(libVulkan, "create_object");
		if (!create_object)
		{
			PRINT("Find Symbol create_object failed!\n")
			exit(1);
		}
		vkSetup = create_object(appName, engineName, libName, logfile);
#else
		vkSetup = new VulkanSetup(appName, engineName, libName, logfile);
#endif
	}

	~App()
	{
		delete vkSetup;
		PRINT("Close App.\n")
#ifdef LOG
		fclose(logfile);
#endif
	}

	void init(MyWindow _window)
	{
		window = _window;
		vkSetup->init(window);
		renderScene = new RenderScene(vkSetup, key, motionPos, resourcesPath);
		start_t = clock();
	}

	void draw()
	{
		framecount++;
		GetCursorPos((POINT*)(&motionPos->xScreen)); // absolute gesamte Bildschirmposition
		renderScene->updateUniformBuffers();
		renderScene->camMotion();
		renderScene->drawFrame();     
		if ((clock() - start_t) > CLOCKS_PER_SEC)
		{
			fps = framecount;
			start_t = clock();
			framecount = 0;
		}
		renderScene->updateTextOverlay(fps, motionPos->xScreen, motionPos->yScreen);
	}
};

int main(int argc, char **argv)
{
	App* app;
	time_t current_time;
	char* c_time_string;

	current_time = time(NULL);
	c_time_string = ctime(&current_time);

#ifdef LOG
	logfile = fopen(LOGFILE, "w");
	if (logfile == NULL) exit(1);
#endif

	PRINT("\n==================\n*** Vulkan App ***\n==================\n\n")
	PRINT("%s\n", c_time_string)

	app = new App();
	if (argc>1) PRINT("%s", argv[1])
	Window0* window = new Window0(WINDOW_NAME, WND_WIDTH, WND_HEIGHT, FULLSCREEN);
	window->createWindow();
	key = ((Window0*)window)->getKey();
	motionPos = window->getMotionPosition();
	app->init((MyWindow)window);
	window->showWindow();
	while(!window->checkMessage())
	{
		app->draw();
	}
	delete app;
	delete window;
	return 0;
}