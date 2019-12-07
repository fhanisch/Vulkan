#include <stdio.h>
#include "VulkanSetup.h"

VulkanSetup::VulkanSetup()
{
    printf("Init Vulkan.\n");
}

VulkanSetup::~VulkanSetup()
{
    printf("Close Vulkan.\n");
}

void VulkanSetup::init()
{
    printf("Run VulkanSetup.\n");
}

/* Funktionfür dynamisches Laden */
extern "C" VulkanSetup* create_object()
{
    return new VulkanSetup();
}