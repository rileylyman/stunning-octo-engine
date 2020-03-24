add_library(
    language

    src/optional.c
    src/raw_vector.c

    include/language/optional.h
    include/language/raw_vector.h
)

set(CMAKE_BUILD_TYPE Debug)

target_include_directories(language PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(language log)

enable_testing()

add_subdirectory(test)