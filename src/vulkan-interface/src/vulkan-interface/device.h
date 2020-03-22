#ifndef VULKAN_DEVICE_H
#define VULKAN_DEVICE_H

#include <vulkan/vulkan.h>

VkPhysicalDevice pickPhysicalDevice(VkInstance instance); 
bool isDeviceSuitable(VkInstance instace, VkPhysicalDevice device);

#endif