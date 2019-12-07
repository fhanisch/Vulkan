#ifndef VULKAN_SETUP_H
#define VULKAN_SETUP_H

#include <stdio.h>

#define VK_NO_PROTOTYPES
#define VK_USE_PLATFORM_ANDROID_KHR
#include <vulkan/vulkan.h>

class VulkanSetup
{
protected:
    void printSupportedApiVersion();

public:
    VulkanSetup(FILE* file_);
    /* 'virtual' ermöglicht dynamisches Laden der Klasse in einem shared object */
    virtual ~VulkanSetup();
    virtual void init();
};

extern PFN_vkEnumerateInstanceVersion vkEnumerateInstanceVersion;

#endif /* VULKAN_SETUP_H */