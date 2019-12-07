#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <dlfcn.h>
#include "VulkanSetup.h"

#ifdef ANDROID
#include "android_native_app_glue.h"
#define LOGFILE "/storage/emulated/0/Dokumente/VulkanApp.log.txt"
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
static int initialized_ = 0;

class App
{
#ifdef DYNAMIC
    VulkanSetup* (*create_object)(FILE* file_);
#endif
    VulkanSetup *vkSetup;
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
        create_object = (VulkanSetup*(*)(FILE* file_))dlsym(libVulkan, "create_object");
        if (!create_object)
        {
            PRINT("Find Symbol create_object failed!\n")
            exit(1);
        }
        vkSetup = create_object(file);
#else
        vkSetup = new VulkanSetup(file);
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

    void init()
    {
        vkSetup->init();
    }

    void run()
    {
        PRINT("Run App ...\n")
    }
};

#ifdef ANDROID

// Process the next main command.
void handle_cmd(android_app* a_app, int32_t cmd) {
  switch (cmd) {
    case APP_CMD_INIT_WINDOW:
      // The window is being shown, get it ready.
      ((App*)a_app->userData)->init();
      ((App*)a_app->userData)->run();
      initialized_ = 1;
      break;
    case APP_CMD_TERM_WINDOW:
      // The window is being hidden or closed, clean it up.
      //delete (App*)a_app->userData;
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
        /*if (IsVulkanReady()) {
            VulkanDrawFrame();
        }*/
    } while (a_app->destroyRequested == 0);

    delete app;

#ifndef ANDROID
    app->init();
    app->run();
    delete app;
    return 0;
#endif
}