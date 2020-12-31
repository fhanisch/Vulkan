#define _CRT_SECURE_NO_WARNINGS /* als Compiler-Argument aufrufen */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#if defined(LINUX) || defined(ANDROID)
#include <dlfcn.h>
#endif
#include "Window.h"
#include "VulkanSetup.h"
#ifdef ANDROID
#include "android_native_app_glue.h"
void exit(int status); // --> nicht erforderlich für Kompilierung, aber VS zeigt sonst Fehler an
#endif

#define APP_NAME "VulkanApp"
#define ENGINE_NAME "MyVulkanEngine"
#define WINDOW_NAME "My Vulkan App"

#ifdef ANDROID
#define WND_WIDTH 2560
#define WND_HEIGHT 1600
#define LOGFILE "/storage/emulated/0/Dokumente/VulkanApp.log.txt"
#define RESOURCES_PATH "/storage/emulated/0/Dokumente/Resources"
#define LIB_NAME "libvulkan.so"
static bool initialized_ = false;
#elif LINUX
#define WND_WIDTH 1920 //3840
#define WND_HEIGHT 1080 //2160
#define LOGFILE "VulkanApp.log.txt"
#define RESOURCES_PATH "/home/felix/Entwicklung/Vulkan/VulkanLib/res"
#define LIB_NAME "libvulkan.so.1"
#define FULLSCREEN true
#elif WINDOWS
#define WND_WIDTH 1920 //3840
#define WND_HEIGHT 1080 //2160
#define LOGFILE "VulkanApp.log.txt"
#define RESOURCES_PATH "C:/Home/Entwicklung/Vulkan/VulkanLib/res"
#define LIB_NAME "vulkan-1.dll"
#define FULLSCREEN true
#endif

#ifdef LOG
static char buf[128];
#define PRINT(...) \
sprintf(buf, __VA_ARGS__); \
fwrite(buf, strlen(buf), 1, file);
#else
#define PRINT(...) \
printf(__VA_ARGS__);
#endif

// TODO: zu userdata hinzufügen
static FILE* file = NULL;
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
	timespec tStart, tEnd;
	clock_t start_t;

#ifdef DYNAMIC
	VulkanSetup* (*create_object)(const char* _appName, const char* _engineName, const char* _libName, FILE* _file);
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
		create_object = (VulkanSetup*(*)(const char* _appName, const char* _engineName, const char* _libName, FILE* _file))dlsym(libVulkan, "create_object");
		if (!create_object)
		{
			PRINT("Find Symbol create_object failed!\n")
			exit(1);
		}
		vkSetup = create_object(appName, engineName, libName, file);
#else
		vkSetup = new VulkanSetup(appName, engineName, libName, file);
#endif
	}

	~App()
	{
		delete vkSetup;
		PRINT("Close App.\n")
#ifdef LOG
		fclose(file);
#endif
	}

	void init(MyWindow _window)
	{
		window = _window;
		vkSetup->init(window);
		renderScene = new RenderScene(vkSetup, key, motionPos, resourcesPath);
		start_t = clock();
#ifndef WINDOWS
		clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &tStart);
#endif
	}

	void draw()
	{
		framecount++;
#ifdef WINDOWS
		GetCursorPos((POINT*)(&motionPos->xScreen)); // absolute gesamte Bildschirmposition
#endif
		renderScene->updateUniformBuffers();
		renderScene->camMotion();
		renderScene->drawFrame();
#ifndef WINDOWS
		clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &tEnd);
		while (((tEnd.tv_sec - tStart.tv_sec) * (long)1e9 + (tEnd.tv_nsec - tStart.tv_nsec)) < (long)(1000000000/60)) clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &tEnd);
#endif        
		if ((clock() - start_t) > CLOCKS_PER_SEC)
		{
			fps = framecount;
			start_t = clock();
			framecount = 0;
		}
		renderScene->updateTextOverlay(fps, motionPos->xScreen, motionPos->yScreen);
#ifndef WINDOWS
		clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &tStart);
#endif
	}
};

#ifdef ANDROID
static int32_t handle_input(struct android_app* app, AInputEvent* event)
{
	if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION) {
		
		motionPos->xScreen = AMotionEvent_getX(event, 0);
		motionPos->yScreen = AMotionEvent_getY(event, 0);

		if (motionPos->xScreen < 200) key[KEY_LEFT] = true; else key[KEY_LEFT] = false;
		if (motionPos->xScreen > 2000) key[KEY_RIGHT] = true; else key[KEY_RIGHT] = false;

		if (motionPos->yScreen < 200) key[KEY_UP] = true; else key[KEY_UP] = false;
		if (motionPos->yScreen > 1300) key[KEY_DOWN] = true; else key[KEY_DOWN] = false;
		
		PRINT("Position: %d,%d\n", motionPos->xScreen, motionPos->yScreen);
		return 1;
	}
	return 0;
}

// Process the next main command.
void handle_cmd(android_app* a_app, int32_t cmd)
{
	switch (cmd)
	{
		case APP_CMD_INIT_WINDOW:
			// The window is being shown, get it ready.
			((App*)a_app->userData)->init((MyWindow)a_app->window);
			initialized_ = true;
			key[KEY_W] = true;
			break;
		case APP_CMD_TERM_WINDOW:
			// The window is being hidden or closed, clean it up.
			PRINT("Window terminated.\n")
			fclose(file);
			initialized_ = false;			
			a_app->destroyRequested = 1;
			break;
		default:
			PRINT("Event not handled: %d\n", cmd)
	}
}
#endif

#ifdef ANDROID
void android_main(struct android_app* a_app)
#else
int main(int argc, char **argv)
#endif
{
	App* app;
	time_t current_time;
	char* c_time_string;

	current_time = time(NULL);
	c_time_string = ctime(&current_time);

#ifdef LOG
	file = fopen(LOGFILE, "w");
	if (file == NULL) exit(1);
#endif

	PRINT("\n==================\n*** Vulkan App ***\n==================\n\n")
	PRINT("%s\n", c_time_string)

	app = new App();

#ifdef ANDROID
	key = new bool[256]; /* --> wird hier alles mit 0 initialisiert?
	--> Alternativ: memset(key, 0, sizeof(bool)*256);*/
	motionPos = new MotionPos;

	a_app->userData = app;

	// Set the callback to process system events
	a_app->onAppCmd = handle_cmd;
	a_app->onInputEvent = handle_input;

	// Used to poll the events in the main loop
	int events;
	android_poll_source* source;

	// Main loop
	do {
		if (ALooper_pollAll(initialized_ ? 1 : 0, nullptr, &events, (void**)&source) >= 0) {
			if (source != NULL) source->process(a_app, source);
		}

		// render if vulkan is ready
		if (initialized_) app->draw();

	} while (a_app->destroyRequested == 0 || !initialized_);

	delete app;

#else
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
#endif
}