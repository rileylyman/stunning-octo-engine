add_library(
    language
    src/language/optional.h
    src/language/optional.c
)

target_include_directories(language PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")