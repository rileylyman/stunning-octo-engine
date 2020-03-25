#include <language/fileops.h>
#include "vulkan-interface/pipeline.h"
#include "log.h"

#define VERT_SHADER ("./src/vulkan-interface/shaders/spir-v/vert.spv")
#define FRAG_SHADER ("./src/vulkan-interface/shaders/spir-v/frag.spv")

//
// Creates a shader module for the logical device from the given bytecode buffer
//
VkShaderModule create_shader_module(VkDevice device, uint8_t *bytecode_buffer, size_t buffer_size) {
    VkShaderModuleCreateInfo ci = {};
    ci.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    ci.codeSize = buffer_size;
    ci.pCode = (uint32_t *)bytecode_buffer;

    VkShaderModule module;
    if (vkCreateShaderModule(device, &ci, NULL, &module) != VK_SUCCESS) {
        log_fatal("Failed to create shader module\n");
        exit(EXIT_FAILURE);
    }

    return module;
}

VkRenderPass create_render_pass(VkDevice device, VkFormat image_format) {
    
    VkAttachmentDescription color_attachment = {};
    color_attachment.format = image_format;
    color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
    color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference attach_ref = {};
    attach_ref.attachment = 0;
    attach_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &attach_ref;

    VkRenderPassCreateInfo renderpass_ci = {};
    renderpass_ci.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderpass_ci.attachmentCount = 1;
    renderpass_ci.pAttachments = &color_attachment;
    renderpass_ci.subpassCount = 1;
    renderpass_ci.pSubpasses = &subpass;

    VkRenderPass renderpass;
    if (vkCreateRenderPass(device, &renderpass_ci, NULL, &renderpass) != VK_SUCCESS) {
        log_fatal("Failed to create render pass");
        exit(EXIT_FAILURE);
    }

    return renderpass;
}

//
// Creates a graphics pipeline for the given logical device
//
VkPipeline create_graphics_pipeline(VkDevice device, VkExtent2D sc_extent, VkRenderPass renderpass, VkPipelineLayout *layout) {
    size_t vert_size, frag_size;
    uint8_t *vert = read_binary_file_FREE(VERT_SHADER, &vert_size);
    uint8_t *frag = read_binary_file_FREE(FRAG_SHADER, &frag_size);

    log_trace("Size of vertex buffer: %u\n", vert_size);
    log_trace("Size of frag buffer: %u\n", frag_size);

    VkShaderModule vertex_module = create_shader_module(device, vert, vert_size);
    VkShaderModule fragment_module = create_shader_module(device, frag, frag_size);

    free(vert);
    free(frag);

    //
    // Vertex shader programmable stage
    //
    VkPipelineShaderStageCreateInfo vertex_shader_stage_ci = {};
    vertex_shader_stage_ci.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertex_shader_stage_ci.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertex_shader_stage_ci.pName = "main";
    vertex_shader_stage_ci.module = vertex_module;
    vertex_shader_stage_ci.pSpecializationInfo = VK_NULL_HANDLE; // TODO: This can be used to supply globals to shaders

    //
    // Fragment shader programmable stage
    //
    VkPipelineShaderStageCreateInfo fragment_shader_stage_ci = {};
    fragment_shader_stage_ci.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragment_shader_stage_ci.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragment_shader_stage_ci.pName = "main";
    fragment_shader_stage_ci.module = fragment_module;

    //
    // Vertex input data specification 
    //
    VkPipelineVertexInputStateCreateInfo vertex_input_ci = {};
    vertex_input_ci.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertex_input_ci.vertexAttributeDescriptionCount = 0;
    vertex_input_ci.vertexBindingDescriptionCount = 0;

    //
    // Vertex input assembly fixed pipeline stage
    //
    VkPipelineInputAssemblyStateCreateInfo input_assembly_ci = {};
    input_assembly_ci.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    input_assembly_ci.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    input_assembly_ci.primitiveRestartEnable = VK_FALSE;

    //
    // Rasterization fixed pipeline stage
    //
    VkPipelineRasterizationStateCreateInfo rast_ci = {};
    rast_ci.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rast_ci.depthClampEnable = VK_FALSE; // TODO: This is good for shadow mapping
    rast_ci.rasterizerDiscardEnable = VK_FALSE;
    rast_ci.polygonMode = VK_POLYGON_MODE_FILL;
    rast_ci.lineWidth = 1.0f;
    rast_ci.cullMode = VK_CULL_MODE_BACK_BIT;
    rast_ci.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rast_ci.depthBiasEnable = VK_FALSE;
    //
    // Other depth bias vars are snipped. TODO: implement this for shadow mapping
    //

    //
    // Multisampling settings
    //
    VkPipelineMultisampleStateCreateInfo ms_ci = {};
    ms_ci.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    ms_ci.sampleShadingEnable = VK_FALSE;
    ms_ci.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    ms_ci.minSampleShading = 1.0f; // Optional
    ms_ci.pSampleMask = NULL; // Optional
    ms_ci.alphaToCoverageEnable = VK_FALSE; // Optional
    ms_ci.alphaToOneEnable = VK_FALSE; // Optional

    //
    // TODO: Depth and stencil testing
    //

    //
    // Specify how to color blend our only color attachment (the framebuffer)
    // For now, we will use __over__ operator blending (I think)
    //
    VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
    colorBlendAttachment.colorWriteMask = 
          VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
          VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT ;
    colorBlendAttachment.blendEnable = VK_TRUE;
    colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
    colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

    //
    // List the blend state for each attachment
    //
    VkPipelineColorBlendStateCreateInfo colorBlending = {};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;
    colorBlending.blendConstants[0] = 0.0f; // Optional
    colorBlending.blendConstants[1] = 0.0f; // Optional
    colorBlending.blendConstants[2] = 0.0f; // Optional
    colorBlending.blendConstants[3] = 0.0f; // Optional

    //
    // Create pipeline layout. This is how push constants and
    // uniforms can be sent through to the shader
    //
    VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 0; // Optional
    pipelineLayoutInfo.pSetLayouts = NULL; // Optional
    pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
    pipelineLayoutInfo.pPushConstantRanges = NULL; // Optional

    if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, NULL, layout) != VK_SUCCESS) {
        log_fatal("Failed to create pipeline layout!\n");
        exit(EXIT_FAILURE);
    }


    //
    // Specify the viewport
    //
    VkViewport viewport = {
        .x = 0.0f,
        .y = 0.0f,
        .width = (float)sc_extent.width,
        .height = (float)sc_extent.height,
        .minDepth = 0.0f,
        .maxDepth = 1.0f,
    };

    //
    // Specify the scissor
    //
    VkRect2D scissor = {
        .offset = {0, 0},
        .extent = sc_extent
    };

    //
    // Create the viewport 
    //
    VkPipelineViewportStateCreateInfo viewport_ci = {};
    viewport_ci.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewport_ci.viewportCount = 1;
    viewport_ci.pViewports = &viewport;
    viewport_ci.scissorCount = 1;
    viewport_ci.pScissors = &scissor;

    //
    // Put all of this together to create the pipeline!
    //
    VkGraphicsPipelineCreateInfo pipeline_ci = {};
    pipeline_ci.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipeline_ci.stageCount = 2;
    pipeline_ci.pStages = (VkPipelineShaderStageCreateInfo[]){vertex_shader_stage_ci, fragment_shader_stage_ci};
    pipeline_ci.pVertexInputState = &vertex_input_ci;
    pipeline_ci.pInputAssemblyState = &input_assembly_ci;
    pipeline_ci.pViewportState = &viewport_ci;
    pipeline_ci.pRasterizationState = &rast_ci;
    pipeline_ci.pMultisampleState = &ms_ci;
    pipeline_ci.pDepthStencilState = NULL;
    pipeline_ci.pColorBlendState = &colorBlending;
    pipeline_ci.pDynamicState = NULL;
    pipeline_ci.layout = *layout;
    pipeline_ci.renderPass = renderpass;
    pipeline_ci.subpass = 0;
    pipeline_ci.basePipelineHandle = VK_NULL_HANDLE; // TODO: if you want to use this feature, enable the flag 
    pipeline_ci.basePipelineIndex = -1;

    VkPipeline pipeline;
    if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipeline_ci, NULL, &pipeline) != VK_SUCCESS) {
        log_fatal("Failed to create pipeline!\n");
        exit(EXIT_FAILURE);
    }

    //
    // We no longer need the shader modules
    //
    vkDestroyShaderModule(device, vertex_module, NULL);
    vkDestroyShaderModule(device, fragment_module, NULL);

    log_trace("Created graphics pipeline!\n");
    return pipeline;
}