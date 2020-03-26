#include <stdlib.h>
#include "vulkan-interface/command.h"

//
// Creates a command pool for the given queue family index on
// the specified device
//
VkCommandPool create_command_pool(VkDevice device, uint32_t qf_idx) {

    VkCommandPoolCreateInfo pool_ci = {};
    pool_ci.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    pool_ci.queueFamilyIndex = qf_idx;
    pool_ci.flags = 0;

    VkCommandPool pool;
    if (vkCreateCommandPool(device, &pool_ci, NULL, &pool) != VK_SUCCESS) {
        log_fatal("Failed to create command pool\n");
        exit(EXIT_FAILURE);
    }

    return pool;
}

struct RawVector create_command_buffers(
    VkDevice device, 
    VkCommandPool pool, 
    VkRenderPass renderpass, 
    VkPipeline pipeline,
    struct RawVector *rvec_VkFramebuffer, 
    VkExtent2D extent) {

    //
    // Allocation info to allocate each command buffer from
    // the command buffer pool. We want to create primary 
    //
    VkCommandBufferAllocateInfo alloc_info = {};
    alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc_info.commandPool = pool;
    alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    alloc_info.commandBufferCount = raw_vector_size(rvec_VkFramebuffer);

    VkCommandBuffer cbs[alloc_info.commandBufferCount];
    if (vkAllocateCommandBuffers(device, &alloc_info, cbs) != VK_SUCCESS) {
        log_fatal("Could not allocate command buffers\n");
        exit(EXIT_FAILURE);
    }
    struct RawVector rvec_VkCommandBuffer = raw_vector_create(sizeof(VkCommandBuffer), alloc_info.commandBufferCount);
    raw_vector_extend_back(&rvec_VkCommandBuffer, cbs, alloc_info.commandBufferCount);

    for (int i = 0; i < raw_vector_size(&rvec_VkCommandBuffer); i++) {
        VkCommandBuffer current_command_buffer = *(VkCommandBuffer *)raw_vector_get_ptr(&rvec_VkCommandBuffer, i);

        VkCommandBufferBeginInfo begin_info = {};
        begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        begin_info.flags = 0;
        begin_info.pInheritanceInfo = NULL;

        if (vkBeginCommandBuffer(current_command_buffer, &begin_info) != VK_SUCCESS) {
            log_fatal("Could not begin command buffer\n");
            exit(EXIT_FAILURE);
        }

        VkRenderPassBeginInfo rpb_info = {};
        rpb_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        rpb_info.renderPass = renderpass;
        rpb_info.framebuffer = *(VkFramebuffer *)raw_vector_get_ptr(rvec_VkFramebuffer, i);
        rpb_info.renderArea.offset = (VkOffset2D){0,0};
        rpb_info.renderArea.extent = extent;

        VkClearValue clear_color = {0.0f, 0.0f, 0.0f, 1.0f};
        rpb_info.clearValueCount = 1;
        rpb_info.pClearValues = &clear_color;

        vkCmdBeginRenderPass(current_command_buffer, &rpb_info, VK_SUBPASS_CONTENTS_INLINE);
        vkCmdBindPipeline(current_command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
        vkCmdDraw(current_command_buffer, 3, 1, 0, 0);
        vkCmdEndRenderPass(current_command_buffer);

        if (vkEndCommandBuffer(current_command_buffer) != VK_SUCCESS) {
            log_fatal("Failed to record command buffer\n");
            exit(EXIT_FAILURE);
        }

        log_trace("Recorded command buffer %i\n", i);
    }
    return rvec_VkCommandBuffer;
}