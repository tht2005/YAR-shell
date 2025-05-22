add_executable (test_environ test_environ.c)
target_link_libraries (test_environ PRIVATE libs)
target_include_directories(test_environ PRIVATE ${PROJECT_SOURCE_DIR})

add_test (NAME test_environ COMMAND test_environ)
