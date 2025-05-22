cmake_minimum_required(VERSION 3.16)

project(yar VERSION 1.0)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_compile_definitions(STRING_INIT_BUF=16)
add_compile_definitions(_GNU_SOURCE)

find_package (PkgConfig REQUIRED)
pkg_check_modules (READLINE REQUIRED readline)

set(SRC_FILES   yar.c 
                yar_parser.c
                yar_error.c
                yar_env.c
                yar_exec.c
                yar_job.c
                yar_shell.c
                data_structure/string.c
                data_structure/environ.c
            )

add_executable(yar ${SRC_FILES})

target_link_libraries (
    yar
    ${READLINE_LIBRARIES}
)
target_include_directories(
    yar PUBLIC
    ${PROJECT_SOURCE_DIR}
    ${READLINE_INCLUDE_DIRS}
)
target_compile_options (
    yar PUBLIC
    ${READLINE_CFLAGS}
    ${READLINE_LDFLAGS}
)

# configure_file(
#     include/config.h.in
#     include/config.h
#     @ONLY
# )

# Testing
add_subdirectory (tests)
set(LIB_FILES   ${SRC_FILES})
add_library (libs STATIC ${LIB_FILES})
enable_testing ()
