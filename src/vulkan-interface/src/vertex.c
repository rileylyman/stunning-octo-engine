#include <string.h>
#include "vulkan-interface/vertex.h"

struct Vertex quad_vertices[NUM_QUAD_VERTICES] = {
    {{0.0f, -0.5f, 0.0f}, {0.3f, 0.3f, 0.0f}},
    {{0.5f, 0.5f, 0.0f}, {0.0f, 0.4f, 0.4f}},
    {{-0.5f, 0.5f, 0.0f}, {0.7f, 0.1f, 0.3f}}
};

VkBuffer create_vertex_buffer(VkDevice device) {
    VkBufferCreateInfo ci = {};
    ci.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    ci.size = sizeof(struct Vertex) * NUM_QUAD_VERTICES;
    ci.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    ci.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VkBuffer buffer;
    if (vkCreateBuffer(device, &ci, NULL, &buffer) != VK_SUCCESS) {
        log_fatal("Failed to create vertex buffer\n");
        exit(EXIT_FAILURE);
    }

    return buffer;
}

VkVertexInputBindingDescription get_binding_description() {
    VkVertexInputBindingDescription bd = {};
    bd.binding = 0;
    bd.stride = sizeof(struct Vertex);
    bd.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    return bd;
}

struct RawVector get_attribute_description() {
    struct RawVector attr_dscrps = raw_vector_create(sizeof(VkVertexInputAttributeDescription), 2);

    VkVertexInputAttributeDescription ad = {};
    ad.binding = 0;
    ad.location = 0;
    ad.format = VK_FORMAT_R32G32B32_SFLOAT;
    ad.offset = offsetof(struct Vertex, position);
    raw_vector_push_back(&attr_dscrps, &ad);

    ad.binding = 0;
    ad.location = 1;
    ad.format = VK_FORMAT_R32G32B32_SFLOAT;
    ad.offset = offsetof(struct Vertex, color);
    raw_vector_push_back(&attr_dscrps, &ad);
    return attr_dscrps;
}

VkDeviceMemory allocate_and_bind_and_fill_vertex_buffer_memory(VkPhysicalDevice physical_device, VkDevice device, VkBuffer vertex_buffer) {

    //
    // The mem_reqs.memoryTypeBits has bit i set if mem_props.memoryTypes[i] is supported for
    // the device. Therefore, for our desired properties, we must check that the given
    // memory type includes our desired properties AND that it is supported by our
    // logical device.
    //
    VkMemoryRequirements mem_reqs;
    vkGetBufferMemoryRequirements(device, vertex_buffer, &mem_reqs);

    VkPhysicalDeviceMemoryProperties mem_props;
    vkGetPhysicalDeviceMemoryProperties(physical_device, &mem_props);

    //
    // Select a memory type to allocate for our buffer
    //
    uint32_t desired_properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    uint32_t selected_memory_type_index = UINT32_MAX;

    for (uint32_t i = 0; i < mem_props.memoryTypeCount; i++) {
        if (mem_reqs.memoryTypeBits & (1 << i) && ((mem_props.memoryTypes[i].propertyFlags & desired_properties) == desired_properties)) {
            selected_memory_type_index = i;
            break;
        }
    }
    if (selected_memory_type_index == UINT32_MAX) {
        log_fatal("Failed to select memory type!\n");
        exit(EXIT_FAILURE);
    }

    //
    // Allocate the memory
    //
    VkMemoryAllocateInfo alloc_info = {0};
    alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    alloc_info.allocationSize = mem_reqs.size;
    alloc_info.memoryTypeIndex = selected_memory_type_index;

    VkDeviceMemory memory;
    if (vkAllocateMemory(device, &alloc_info, NULL, &memory) != VK_SUCCESS) {
        log_fatal("Failed to allocate memory!\n");
        exit(EXIT_FAILURE);
    }

    //
    // Now bind this memory to the vertex buffer and return it
    //
    vkBindBufferMemory(device, vertex_buffer, memory, 0);

    //
    // Write data to the newly allocated memory
    //
    void *data;
    vkMapMemory(device, memory, 0, sizeof(struct Vertex) * NUM_QUAD_VERTICES, 0, &data);
    memcpy(data, quad_vertices, sizeof(struct Vertex) * NUM_QUAD_VERTICES);
    vkUnmapMemory(device, memory);

    return memory;
}
