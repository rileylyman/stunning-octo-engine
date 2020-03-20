add_library(
    log
    src/log/log.c
    src/log/log.h
)

target_include_directories(log PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")