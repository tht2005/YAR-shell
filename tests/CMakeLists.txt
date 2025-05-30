# add_executable (test_environ test_environ.c)
# target_link_libraries (test_environ PRIVATE libs)
# target_include_directories(test_environ PRIVATE ${PROJECT_SOURCE_DIR})
# add_test (NAME test_environ COMMAND test_environ)

function(create_test target)
    message(STATUS "Creating test: ${target}")
    add_executable(${target} "${target}.c")
    target_link_libraries(${target} parent_libs)
    target_include_directories(${target} PRIVATE ${PROJECT_SOURCE_DIR})
    add_test(NAME ${target} COMMAND ${target})
endfunction()

create_test(test_environ)
create_test(test_brace_expansion)
create_test(test_glob)

