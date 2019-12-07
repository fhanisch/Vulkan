#ifndef VULKAN_SETUP_H
#define VULKAN_SETUP_H

class VulkanSetup
{
public:
    VulkanSetup();
    /* 'virtual' ermöglicht dynamisches Laden der Klasse in einem shared object */
    virtual ~VulkanSetup();
    virtual void init();
};

#endif /* VULKAN_SETUP_H */