include_directories("${TEST_SOURCE_DIR}/include")

add_executable(Test test.c)

target_link_libraries(Test language unity)

add_test(NAME Language COMMAND Test)