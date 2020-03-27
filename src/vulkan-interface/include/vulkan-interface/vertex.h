#include <vulkan/vulkan.h>
#include <cglm/vec3.h>
#include <language/raw_vector.h>

struct Vertex {
    vec3 position;
    vec3 color;
};

#define NUM_QUAD_VERTICES 6
struct Vertex quad_vertices[NUM_QUAD_VERTICES];

VkVertexInputBindingDescription get_binding_description(); 
struct RawVector get_attribute_description(); 
VkBuffer create_vertex_buffer(VkDevice device); 

VkDeviceMemory allocate_and_bind_and_fill_vertex_buffer_memory(VkPhysicalDevice physical_device, VkDevice device, VkBuffer vertex_buffer); 
