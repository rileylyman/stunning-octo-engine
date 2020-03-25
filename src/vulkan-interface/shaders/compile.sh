#!/bin/bash

#Run this script from the library folder (src/vulkan-interface)
echo $PWD
glslc ${PWD}/shaders/shader.vert -o ${PWD}/shaders/spir-v/vert.spv
glslc ${PWD}/shaders/shader.frag -o ${PWD}/shaders/spir-v/frag.spv