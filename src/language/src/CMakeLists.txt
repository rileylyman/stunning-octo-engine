add_library(
    language
    language/optional.h
    language/optional.c
    language/raw_vector.h
    language/raw_vector.c
)

target_include_directories(language PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

target_link_libraries(language log)