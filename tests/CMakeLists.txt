function(create_test target)
    message(STATUS "Creating test: ${target}")
    add_executable(${target} "${target}.c")
    target_link_libraries(${target} parent_libs)
    target_include_directories(${target} PRIVATE ${PROJECT_SOURCE_DIR})
    add_test(NAME ${target} COMMAND ${target})
endfunction()

file(GLOB TEST_SOURCES "*.c")
foreach(test_file ${TEST_SOURCES})
    get_filename_component(test_name ${test_file} NAME_WE)
    create_test(${test_name})
endforeach()

