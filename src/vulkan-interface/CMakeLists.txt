add_library(
    vulkan-interface
    src/vulkan-interface/debug.h
    src/vulkan-interface/debug.c
    src/vulkan-interface/device.h
    src/vulkan-interface/device.c
    src/vulkan-interface/extension.h
    src/vulkan-interface/extension.c
    src/vulkan-interface/init.h
    src/vulkan-interface/init.c
    src/vulkan-interface/interface-vk.h
    src/vulkan-interface/interface-vk.c
)

target_include_directories(vulkan-interface PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")

target_include_directories(vulkan-interface PRIVATE "${PROJECT_SOURCE_DIR}/vulkansdk/x86_64/include")
target_include_directories(vulkan-interface PRIVATE "/usr/local/include")
target_link_directories(vulkan-interface PRIVATE "${PROJECT_SOURCE_DIR}/vulkansdk/x86_64/lib")
target_link_directories(vulkan-interface PRIVATE "/usr/local/include")

target_link_libraries(vulkan-interface PRIVATE glfw3 rt dl m X11 pthread xcb Xau Xdmcp vulkan language cglm log)