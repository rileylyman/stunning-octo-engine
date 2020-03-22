#ifndef VULKAN_DEVICE_H
#define VULKAN_DEVICE_H

#include <vulkan/vulkan.h>
#include <language/optional.h>

struct QueueFamilyIndices {
   struct OptionalIndex graphics_family_index; 
};

struct QueueFamilyIndices queue_family_indices_get_indices(VkPhysicalDevice device); 

VkPhysicalDevice pick_physical_device(VkInstance instance); 
bool is_device_suitable(VkPhysicalDevice device);
bool queue_family_indices_is_complete(struct QueueFamilyIndices *indices);

#endif