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

//
// Creates a graphics pipeline for the given logical device
//
VkPipeline create_graphics_pipeline(VkDevice device, VkExtent2D sc_extent, VkPipelineLayout *layout) {
    size_t vert_size, frag_size;
    uint8_t *vert = read_binary_file_FREE(VERT_SHADER, &vert_size);
    uint8_t *frag = read_binary_file_FREE(FRAG_SHADER, &frag_size);

    log_trace("Size of vertex buffer: %u", vert_size);
    log_trace("Size of frag buffer: %u", frag_size);

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
    vertex_shader_stage_ci.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertex_shader_stage_ci.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    vertex_shader_stage_ci.pName = "main";
    vertex_shader_stage_ci.module = fragment_module;

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
    rast_ci.depthClampEnable = VK_TRUE; // TODO: this should error since depth clamp feature not enabled
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


    


    vkDestroyShaderModule(device, vertex_module, NULL);
    vkDestroyShaderModule(device, fragment_module, NULL);
}