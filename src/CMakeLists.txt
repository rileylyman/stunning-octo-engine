find_package(glfw3 REQUIRED FATAL_ERROR)

add_executable(VulkanTest
    main.c
)

set_target_properties(VulkanTest PROPERTIES LINKER_LANGUAGE C)
set_property(TARGET VulkanTest PROPERTY C_STANDARD 11)

add_subdirectory(language)
add_subdirectory(vulkan-interface)

set(COMPILE_FLAGS "-g -DLOG_USE_COLOR -std=c11 ${COMPILE_FLAGS}")

set(CMAKE_BUILD_TYPE Debug)

target_link_libraries(VulkanTest PRIVATE glfw3 rt dl m X11 pthread xcb Xau Xdmcp cglm log vulkan-interface language)