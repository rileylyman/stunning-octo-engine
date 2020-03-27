#!/bin/bash

glslc ${PWD}/shader.vert -o ${PWD}/spir-v/vert.spv
glslc ${PWD}/shader.frag -o ${PWD}/spir-v/frag.spv