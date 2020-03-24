add_library(
    vulkan-interface

    src/debug.c
    src/device.c
    src/extension.c
    src/init.c
    src/interface-vk.c
    src/swapchain.c

    include/vulkan-interface/debug.h
    include/vulkan-interface/device.h
    include/vulkan-interface/extension.h
    include/vulkan-interface/init.h
    include/vulkan-interface/interface-vk.h
    include/vulkan-interface/swapchain.h
)

set (CMAKE_BUILD_TYPE Debug)

target_include_directories(vulkan-interface PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

target_include_directories(vulkan-interface PRIVATE "${PROJECT_SOURCE_DIR}/vulkansdk/x86_64/include")
target_include_directories(vulkan-interface PRIVATE "/usr/local/include")
target_link_directories(vulkan-interface PRIVATE "${PROJECT_SOURCE_DIR}/vulkansdk/x86_64/lib")
target_link_directories(vulkan-interface PRIVATE "/usr/local/include")

target_link_libraries(vulkan-interface PRIVATE glfw3 rt dl m X11 pthread xcb Xau Xdmcp vulkan language cglm log)