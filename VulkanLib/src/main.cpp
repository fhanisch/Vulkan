#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <dlfcn.h>
#include "VulkanSetup.h"

#ifdef ANDROID
#include "android_native_app_glue.h"
#define LOGFILE "/storage/emulated/0/Dokumente/VulkanApp.log.txt"
static bool initialized_ = false;
#else
#define LOGFILE "VulkanApp.log.txt"
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

static FILE* file = NULL;
static bool key[256];

class App
{
    const char *appName = "VulkanApp";
	const char *engineName = "MyVulkanEngine";
    void* window;
    uint32_t framecount = 0;
	uint32_t fps = 0;
    timespec tStart, tEnd;
    clock_t start_t;
#ifdef DYNAMIC
    VulkanSetup* (*create_object)(const char *_appNAme, const char *_engineName, FILE* _file);
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
        create_object = (VulkanSetup*(*)(const char *_appNAme, const char *_engineName, FILE* _file))dlsym(libVulkan, "create_object");
        if (!create_object)
        {
            PRINT("Find Symbol create_object failed!\n")
            exit(1);
        }
        vkSetup = create_object(appName, engineName, file);
#else
        vkSetup = new VulkanSetup(appName, engineName, file);
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

    void init(void* _window)
    {
        window = _window;
        memset(key, 0, sizeof(key));
        vkSetup->init(window);
        renderScene = new RenderScene(vkSetup, key);
        //key[VK_SPACE] = true;
        key[0x4a] = true;
        key[0x53] = true;
        start_t = clock();
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &tStart);
    }

    void draw()
    {
        framecount++;
        renderScene->updateUniformBuffers();
		renderScene->camMotion();
		renderScene->drawFrame();
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &tEnd);
        while (((tEnd.tv_sec - tStart.tv_sec) * (long)1e9 + (tEnd.tv_nsec - tStart.tv_nsec)) < (long)(1000000000/60)) clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &tEnd);
        if ((clock() - start_t) > CLOCKS_PER_SEC)
        {
            fps = framecount;
            start_t = clock();
			framecount = 0;
        }
        renderScene->updateTextOverlay(fps);
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &tStart);
    }
};

#ifdef ANDROID

// Process the next main command.
void handle_cmd(android_app* a_app, int32_t cmd) {
  switch (cmd) {
    case APP_CMD_INIT_WINDOW:
      // The window is being shown, get it ready.
      ((App*)a_app->userData)->init(a_app->window);
      initialized_ = true;
      break;
    case APP_CMD_TERM_WINDOW:
      // The window is being hidden or closed, clean it up.
      initialized_ = false;
      break;
    default:
      PRINT("Event not handled: %d\n", cmd)
  }
}

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
    a_app->userData = app;

    // Set the callback to process system events
    a_app->onAppCmd = handle_cmd;

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

    } while (a_app->destroyRequested == 0);

    delete app;

#else
    app->init();
    app->run();
    delete app;
    return 0;
#endif
}