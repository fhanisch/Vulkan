#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <dlfcn.h>
#include "VulkanSetup.h"

#ifdef ANDROID
#include "android_native_app_glue.h"
#endif

class App
{
#ifdef DYNAMIC
    VulkanSetup* (*create_object)();
#endif
    VulkanSetup *vkSetup;
public:
    App()
    {
        printf("Start App.\n");
#ifdef DYNAMIC
        void* libVulkan = dlopen("./libVulkan.so", RTLD_LAZY);
        if (!libVulkan)
        {
            printf("Loading libVulkan.so failed!\n");
            exit(1);
        }
        create_object = (VulkanSetup*(*)())dlsym(libVulkan, "create_object");
        if (!create_object)
        {
            printf("Find Symbol create_object failed!\n");
            exit(1);
        }
        vkSetup = create_object();
#else
        vkSetup = new VulkanSetup();
#endif
    }

    ~App()
    {
        delete vkSetup;
        printf("Close App.\n");
        
    }

    void init()
    {
        vkSetup->init();
    }

    void run()
    {
        printf("Run App ...\n");
    }
};

#ifdef ANDROID
void android_main(android_app* app)
#else
int main(int argc, char **argv)
#endif
{
    time_t current_time;
    char* c_time_string;

    current_time = time(NULL);
    c_time_string = ctime(&current_time);

    printf("\n==================\n*** Vulkan App ***\n==================\n\n");
    printf("%s\n", c_time_string);

    App app;
    app.init();
    app.run();
#ifndef ANDROID
    return 0;
#endif
}